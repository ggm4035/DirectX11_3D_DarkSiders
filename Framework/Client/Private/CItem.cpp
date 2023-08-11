#include "CItem.h"

#include "CGameInstance.h"
#include "CInven.h"
#include "CCurrency.h"

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

	if (nullptr != pArg)
	{
		ITEMDESC Desc = *static_cast<ITEMDESC*>(pArg);

		if (FAILED(Add_Component(LEVEL_GAMEPLAY, Desc.wstrTextureTag, L"Com_Texture",
			(CComponent**)&m_pTextureCom, this)))
			return E_FAIL;
	}

	return S_OK;
}

void CItem::Tick(const _double& TimeDelta)
{
	m_pTransformCom->BillBoard(TimeDelta);

	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CItem::AfterFrustumTick(const _double& TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

void CItem::Late_Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);
}

HRESULT CItem::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(9);

	m_pBufferCom->Render();

	return S_OK;
}

void CItem::Dead_Motion(const _double& TimeDelta)
{
	m_isRemove = true;
}

void CItem::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	/* 여기는 애초에 플레이어랑만 충돌하게 해서 들어온 상황의 other객체는 무조건 플레이어임 */

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	Safe_Release(pGameInstance);
}

HRESULT CItem::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxTex", L"Com_Shader",
		(CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"VIBuffer_Rect", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	CBounding_Sphere::SPHEREDESC SphereDesc;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);
	SphereDesc.vOffset = _float3(0.f, 0.f, 0.f);
	SphereDesc.eGroup = CCollider::COL_ITEM;
	SphereDesc.fRadius = 1.f;
	SphereDesc.isEnable = true;
	if (FAILED(Add_Collider(LEVEL_STATIC, L"Collider_Sphere", L"Col_Sphere", &SphereDesc)))
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

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture")))
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
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);

	CGameObject3D::Free();
}
