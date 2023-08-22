#include "CRoll_Effect.h"

#include "CGameInstance.h"

CRoll_Effect::CRoll_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CRoll_Effect::CRoll_Effect(const CRoll_Effect& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CRoll_Effect::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CRoll_Effect::Tick(const _double& TimeDelta)
{
	CTransform* pTransform = dynamic_cast<CGameObject3D*>(m_pOwner)->Get_Transform();
	_vector vPosition = pTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	vector<_float4x4> ParticleMatrices;

	for (auto& Particle : m_vecParticles)
	{
		Particle.dAge += TimeDelta;

		if (Particle.dAge < Particle.dGenTime)
		{
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
			ParticleMatrices.push_back(WorldMatrix);
			continue;
		}

		if (Particle.dAge > Particle.dLifeTime + Particle.dGenTime)
		{
			Reset_Particle(Particle, pTransform->Get_State(CTransform::STATE_LOOK));
		}

		_float4 vPos;

		memcpy(&vPos, Particle.WorldMatrix.m[3], sizeof(_float4));
		_vector vVelocity = XMLoadFloat4(&Particle.vVelocity);

		vVelocity += XMLoadFloat4(&Particle.vAccel) * TimeDelta;

		XMStoreFloat4(&Particle.vVelocity, vVelocity);

		XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + vVelocity * _float(TimeDelta));

		XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

		ParticleMatrices.push_back(Particle.WorldMatrix);
	}

	if (m_iNumParticles)
		m_isFinishEffect = true;

	m_pBufferCom->Tick(ParticleMatrices, TimeDelta);
}

void CRoll_Effect::AfterFrustumTick(const _double& TimeDelta)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CRoll_Effect::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pBufferCom->Render();

	return S_OK;
}

void CRoll_Effect::Reset_Particle(STONEPARTICLE& Particle, _fvector _vLook)
{
	Particle.dGenTime = GetRandomFloat(0.f, 1.f);
	Particle.dAge = 0.f;
	Particle.dLifeTime = 1.f;
	Particle.isAlive = true;

	Particle.vVelocity = _float4(-_vLook.m128_f32[0] * 10.f, GetRandomFloat(15.f, 20.f), -_vLook.m128_f32[2] * 10.f, 0.f);

	Particle.vAccel = _float4(0.f, GetRandomFloat(-35.f, -45.f), 0.f, 0.f);

	CTransform* pTransform = dynamic_cast<CGameObject3D*>(m_pOwner)->Get_Transform();
	_vector vLook = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_LOOK)) * 3.6f;
	_vector vRight = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_RIGHT));
	vLook += vRight * GetRandomFloat(-2.f, 2.f);
	XMStoreFloat4x4(&Particle.WorldMatrix, XMMatrixIdentity());
	_float4 vPosition = _float4(vLook.m128_f32[0], vLook.m128_f32[1], vLook.m128_f32[2], 1.f);
	memcpy(&Particle.WorldMatrix._41, &vPosition, sizeof(_float4));
}

HRESULT CRoll_Effect::Add_Components()
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

	m_vecParticles.resize(30);
	m_iNumParticles = 30;

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

HRESULT CRoll_Effect::SetUp_ShaderResources()
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

CRoll_Effect* CRoll_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoll_Effect* pInstance = new CRoll_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRoll_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CRoll_Effect* CRoll_Effect::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CRoll_Effect* pInstance = new CRoll_Effect(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CRoll_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRoll_Effect::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBufferCom);

	CGameObject3D::Free();
}
