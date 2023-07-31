#include "CTrigger.h"

#include "CGameInstance.h"
#include "CCollider_Manager.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CTrigger::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_TriggerDesc = *reinterpret_cast<TRIGGERDESC*>(pArg);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_TriggerDesc.vPosition));
	}

	return S_OK;
}

void CTrigger::Tick(const _double& TimeDelta)
{
	if (true == m_isCall)
		return;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 콜라이더 매니저에 콜라이더 추가 */
	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0))
	{
		CCollider_Manager* pColliderManager = CCollider_Manager::GetInstance();
		Safe_AddRef(pColliderManager);
		if (FAILED(pColliderManager->Add_Collider(m_pColliderCom)))
		{
			MSG_BOX("Failed to Add Colliders To Manager");
			Safe_Release(pGameInstance);
			return;
		}
		Safe_Release(pColliderManager);
	}

	Safe_Release(pGameInstance);
}

void CTrigger::Late_Tick(const _double& TimeDelta)
{
	if (true == m_isCall)
		return;

	m_pColliderCom->On_Collision(this, TimeDelta);
}

void CTrigger::Free()
{
	Safe_Release(m_pColliderCom);

#if defined(_USE_IMGUI) || defined(_DEBUG)
	Safe_Release(m_pRenderer);
#endif
	CGameObject3D::Free();
}

#if defined(_USE_IMGUI) || defined(_DEBUG)
HRESULT CTrigger::Add_RenderDebug()
{
	if (FAILED(m_pRenderer->Add_DebugGroup(m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}
#endif
