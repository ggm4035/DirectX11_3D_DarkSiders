#include "CCollider_Manager.h"

#include "CCollider.h"

IMPLEMENT_SINGLETON(CCollider_Manager)

HRESULT CCollider_Manager::Add_Collider(COLLIDERGROUP eGroupID, CCollider* pCollider)
{
	if (0 > eGroupID || COL_END <= eGroupID ||
		nullptr == pCollider)
		return E_FAIL;

	if (true == pCollider->isEnable())
	{
		m_ColliderList[eGroupID].push_back(pCollider);
		Safe_AddRef(pCollider);
	}

	return S_OK;
}

void CCollider_Manager::Tick()
{
	/* 콜라이더 충돌 체크 */
	Collision(COL_PLAYERATK, COL_ENEMY);
	Collision(COL_ENEMY, COL_PLAYER);
	Collision(COL_ENEMY, COL_ENEMY);
	Collision(COL_ENEMYATK, COL_PLAYER);
	Collision(COL_ENEMYRANGE, COL_PLAYER);
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

void CCollider_Manager::Collision(COLLIDERGROUP SrcGroup, COLLIDERGROUP DestGroup)
{
	for (auto& pSrcCollider : m_ColliderList[SrcGroup])
	{
		for (auto& pDestCollider : m_ColliderList[DestGroup])
		{
			if (SrcGroup == DestGroup &&
				pSrcCollider == pDestCollider)
				continue;

			if (true == pSrcCollider->Intersect(pDestCollider))
			{
				pSrcCollider->Push_Collision_Message(pDestCollider);
			}
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