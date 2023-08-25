#include "CSoul.h"

#include "CGameInstance.h"
#include "CInven.h"
#include "CCurrency.h"

CSoul::CSoul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CSoul::CSoul(const CSoul& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CSoul::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_tSoulDesc.vVelocity = _float4(GetRandomFloat(-5.f, 5.f), GetRandomFloat(15.f, 25.f), GetRandomFloat(-5.f, 5.f), 0.f);

	m_tSoulDesc.vAccel = _float4(0.f, GetRandomFloat(-35.f, -45.f), 0.f, 0.f);

	m_tSoulDesc.dGenTime = GetRandomFloat(0.f, 0.3f);

	return S_OK;
}

void CSoul::Tick(const _double& TimeDelta)
{
	if (false == m_isGen)
	{
		m_fTimeAcc += TimeDelta;
		if (m_tSoulDesc.dGenTime < m_fTimeAcc)
		{
			m_isGen = true;
			m_fTimeAcc = 0.f;
			Tick(TimeDelta);
		}
		return;
	}

	if (false == m_isFinishFirstStep)
		First_Step(TimeDelta);
	else
		Second_Step(TimeDelta);

	m_pTransformCom->BillBoard(TimeDelta);
}

void CSoul::AfterFrustumTick(const _double& TimeDelta)
{
	if (false == m_isGen)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
}

void CSoul::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CSoul::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(9);

	m_pBufferCom->Render();

	return S_OK;
}

void CSoul::Dead_Motion(const _double& TimeDelta)
{
	/* 플레이어의 소울 양을 조절(보통 증가하지) 하고 삭제 */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject3D* pPlayer = pGameInstance->Get_Player();
	if (nullptr == pPlayer)
		return;

	CInven* pInven = dynamic_cast<CInven*>(pPlayer->Get_Component(L"Inven"));
	if (nullptr == pInven)
		return;

	CCurrency* pCurrency = pInven->Get_Currency();
	if (nullptr == pCurrency)
		return;

	pCurrency->Add_Currency(100);

	Safe_Release(pGameInstance);

	CGameInstance::GetInstance()->Play_Sound(L"general_soul_collect_01.ogg", CSound_Manager::SOUND_EFFECT_2, 0.1f, true);

	m_isRemove = true;
}

HRESULT CSoul::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Texture_Soul", L"Com_Texture",
		(CComponent**)&m_pTextureCom, this)))
		return E_FAIL;
	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxTex", L"Com_Shader",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"VIBuffer_Rect", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSoul::SetUp_ShaderResources()
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

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture")))
		return E_FAIL;

	return S_OK;
}

void CSoul::First_Step(const _double& TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vVelocity = XMLoadFloat4(&m_tSoulDesc.vVelocity);

	vVelocity += XMLoadFloat4(&m_tSoulDesc.vAccel) * TimeDelta;
	if (0 > XMVectorGetY(vVelocity))
	{
		m_fTimeAcc += TimeDelta;

		if (0.5 < m_fTimeAcc)
		{
			m_isFinishFirstStep = true;
			m_fTimeAcc = 0.f;
		}

		return;
	}

	XMStoreFloat4(&m_tSoulDesc.vVelocity, vVelocity);

	vPosition += vVelocity * _float(TimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CSoul::Second_Step(const _double& TimeDelta)
{
	m_SoulSpeed += TimeDelta;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPosition = pGameInstance->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vPlayerPosition = XMVectorSetY(vPlayerPosition, vPlayerPosition.m128_f32[1] + 1.f);

	_vector vDir = vPlayerPosition - vPosition;
	/* MinDistance 보다 거리가 작은 경우 false */
	m_pTransformCom->Chase(vPlayerPosition, TimeDelta * m_SoulSpeed);
	if(1.f > XMVectorGetX(XMVector3Length(vDir)))
		Dead_Motion(TimeDelta);

	Safe_Release(pGameInstance);
}

CSoul* CSoul::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSoul* pInstance = new CSoul(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSoul");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSoul* CSoul::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSoul* pInstance = new CSoul(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSoul");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSoul::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);

	CGameObject3D::Free();
}
