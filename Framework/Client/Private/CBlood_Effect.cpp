#include "CBlood_Effect.h"

#include "CGameInstance.h"

CBlood_Effect::CBlood_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CBlood_Effect::CBlood_Effect(const CBlood_Effect& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CBlood_Effect::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBlood_Effect::Tick(const _double& TimeDelta)
{
	vector<_float4x4> ParticleMatrices;

	for (auto& Particle : m_vecParticles)
	{
		Particle.dAge += TimeDelta;

		if (Particle.dAge > Particle.dLifeTime)
			Particle.isAlive = false;

		if (false == Particle.isAlive)
		{
			++m_iCount;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			ParticleMatrices.push_back(WorldMatrix);
			continue;
		}

		_float4 vPos;

		memcpy(&vPos, Particle.WorldMatrix.m[3], sizeof(_float4));
		_vector vVelocity = XMLoadFloat4(&Particle.vVelocity);

		vVelocity += XMLoadFloat4(&Particle.vAccel) * TimeDelta;

		XMStoreFloat4(&Particle.vVelocity, vVelocity);

		XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + vVelocity * _float(TimeDelta));

		_float fScale = Particle.fScale - Particle.dAge * 0.6f;
		fScale = fScale < 0.f ? 0.f : fScale;
		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(fScale, fScale, fScale) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

		ParticleMatrices.push_back(Particle.WorldMatrix);
	}

	if (m_iNumParticles == m_iCount)
		m_isRender = false;
	else
		m_isRender = true;
	
	m_iCount = 0;

	m_pBufferCom->Tick(ParticleMatrices, TimeDelta);
}

void CBlood_Effect::AfterFrustumTick(const _double& TimeDelta)
{
	if (false == m_isRender)
		return;

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
}

HRESULT CBlood_Effect::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);

	m_pBufferCom->Render();

	return S_OK;
}

void CBlood_Effect::Render_Effect(_fvector vEffectPos)
{
	for (auto& Particle : m_vecParticles)
		Reset_Particle(Particle);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
}

void CBlood_Effect::Reset_Particle(STONEPARTICLE& Particle)
{
	Particle.dAge = 0.f;
	Particle.dLifeTime = 1.f;
	Particle.isAlive = true;
	Particle.vVelocity = _float4(GetRandomFloat(-2.f, 2.f), 2.f, GetRandomFloat(-2.f, 2.f), 0.f);
	Particle.vAccel = _float4(0.f, -10.f, 0.f, 0.f);
	XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixIdentity());
	Particle.WorldMatrix._42 += 1.f;
	Particle.fScale = 0.3f;
}

HRESULT CBlood_Effect::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRenderer, this)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_Particle", L"Com_Texture",
		(CComponent**)&m_pTextureCom, this)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_PointInstance", L"Com_Shader",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	m_vecParticles.resize(30);
	m_iNumParticles = 30;

	CVIBuffer_Point_Instance::POINTINSTDESC Desc;
	Desc.iNumInstance = m_iNumParticles;
	Desc.iNumWidth = 0;
	Desc.iNumHeight = 0;
	Desc.fFrameSpeed = 1.f;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Instance_Point", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, this, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlood_Effect::SetUp_ShaderResources()
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

	_float3 vData = _float3(1.f, 0.f, 0.f);
	if(FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vData, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pBufferCom->Bind_LengthTexelU(m_pShaderCom, "g_fLengthTexelU")))
		return E_FAIL;
	if (FAILED(m_pBufferCom->Bind_LengthTexelV(m_pShaderCom, "g_fLengthTexelV")))
		return E_FAIL;

	return S_OK;
}

CBlood_Effect* CBlood_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlood_Effect* pInstance = new CBlood_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBlood_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBlood_Effect* CBlood_Effect::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CBlood_Effect* pInstance = new CBlood_Effect(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CBlood_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlood_Effect::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBufferCom);

	CGameObject3D::Free();
}
