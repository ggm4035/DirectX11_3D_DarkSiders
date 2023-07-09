#include "CCollider_Manager.h"

#include "CCollider.h"

IMPLEMENT_SINGLETON(CCollider_Manager)

HRESULT CCollider_Manager::Add_Collider(COLLIDERGROUP eGroupID, CCollider* pCollider)
{
	if (0 > eGroupID || COL_END <= eGroupID ||
		nullptr == pCollider)
		return E_FAIL;

	m_ColliderList[eGroupID].push_back(pCollider);
	Safe_AddRef(pCollider);

	return S_OK;
}

void CCollider_Manager::Tick()
{
	/* 콜라이더 충돌 체크 */
	Collision(COL_PLAYER, COL_ENEMY);
}

void CCollider_Manager::Clear_ColliderList()
{
	for (_uint i = 0; i < COL_END; ++i)
	{
		for (auto& pCollider : m_ColliderList[i])
			Safe_Release(pCollider);
		m_ColliderList[i].clear();
	}
}

/* 이때 울리는 시점이 게임 오브젝트의 tick 중에 호출됨 그런데 이때는 콜라이더 없는데 */
void CCollider_Manager::Update_Observer(NOTIFYDESC* pNotifyDesc)
{
	/* 애니메이션이 특정 구간에 진입한 경우 */
	NOTIFYCOLLIDERDESC Desc = *static_cast<NOTIFYCOLLIDERDESC*>(pNotifyDesc);

}

void CCollider_Manager::Collision(COLLIDERGROUP SrcGroup, COLLIDERGROUP DestGroup)
{
	for (auto& pSrcCollider : m_ColliderList[SrcGroup])
	{
		for (auto& pDestCollider : m_ColliderList[DestGroup])
		{
			if (true == pSrcCollider->Intersect(pDestCollider))
				pSrcCollider->Push_Collision_Message(pDestCollider);
		}
	}
}

void CCollider_Manager::Free()
{
	for (_uint i = 0; i < COL_END; ++i)
	{
		for (auto& pCollider : m_ColliderList[i])
			Safe_Release(pCollider);
		m_ColliderList->clear();
	}
}
