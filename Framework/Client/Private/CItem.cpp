#include "CItem.h"

#include "CGameInstance.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CItem::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CItem::Tick(const _double& TimeDelta)
{
	vector<_float4x4>		ParticleMatrices;

	for (auto& Particle : m_vecParticles)
	{
		Particle.dAge += TimeDelta;

		if (Particle.dAge > Particle.dLifeTime)
			Particle.isAlive = false;

		_float4 vPos;

		memcpy(&vPos, Particle.WorldMatrix.m[3], sizeof(_float4));
		_vector vVelocity = XMLoadFloat4(&Particle.vVelocity);

		vVelocity += XMLoadFloat4(&Particle.vAccel) * TimeDelta;

		XMStoreFloat4(&Particle.vVelocity, vVelocity);

		XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + vVelocity * _float(TimeDelta));

		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

		ParticleMatrices.push_back(Particle.WorldMatrix);
	}

	m_pBufferCom->Tick(ParticleMatrices, TimeDelta);
}

void CItem::AfterFrustumTick(const _double& TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

void CItem::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CItem::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

void CItem::Reset_Effects()
{
	for (auto& particle : m_vecParticles)
	{
		Reset_Particle(particle);
	}
}

void CItem::Reset_Particle(STONEPARTICLE& Particle)
{
	Particle.dAge = 0.f;
	Particle.dLifeTime = 2.f;
	Particle.isAlive = true;

	Particle.vVelocity = _float4(GetRandomFloat(-5.f, 5.f), GetRandomFloat(15.f, 25.f), GetRandomFloat(-5.f, 5.f), 0.f);

	Particle.vAccel = _float4(0.f, GetRandomFloat(-35.f, -45.f), 0.f, 0.f);

	XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixIdentity());
}

void CItem::Render_Effect(_fvector vEffectPos)
{
	Reset_Effects();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
}

HRESULT CItem::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRenderer, this)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_RockChip", L"Com_Texture",
		(CComponent**)&m_pTextureCom, this)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_PointInstance", L"Com_Shader",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	m_vecParticles.resize(20);
	m_iNumParticles = 20;

	CVIBuffer_Point_Instance::POINTINSTDESC Desc;
	Desc.iNumInstance = m_iNumParticles;
	Desc.iNumHeight = 4;
	Desc.iNumWidth = 4;
	Desc.fFrameSpeed = 1.f;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Instance_Point", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, this, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4Ptr())))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &InputMatrix)))
		return E_FAIL;
	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &InputMatrix)))
		return E_FAIL;
	_float4 InputFloat = pGameInstance->Get_Camera_Position();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &InputFloat, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pBufferCom->Bind_LengthTexelU(m_pShaderCom, "g_fLengthTexelU")))
		return E_FAIL;
	if (FAILED(m_pBufferCom->Bind_LengthTexelV(m_pShaderCom, "g_fLengthTexelV")))
		return E_FAIL;

	return S_OK;
}

CItem* CItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem* pInstance = new CItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CItem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CItem* CItem::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CItem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBufferCom);

	CGameObject3D::Free();
}
