#include "CCollider_Manager.h"

#include "CCollider.h"

IMPLEMENT_SINGLETON(CCollider_Manager)

HRESULT CCollider_Manager::Add_Collider(CCollider* pCollider)
{
	if (nullptr == pCollider)
		return E_FAIL;

	CCollider::COLLIDERGROUP eGroupID = pCollider->Get_Collider_Group();

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
	Collision(CCollider::COL_PLAYER, CCollider::COL_ENEMY);
	Collision(CCollider::COL_PLAYER, CCollider::COL_ITEM);
	Collision(CCollider::COL_PLAYER, CCollider::COL_ENEMY_ATK);
	Collision(CCollider::COL_PLAYER_ATK, CCollider::COL_ENEMY);
	Collision(CCollider::COL_PLAYER_ATK, CCollider::COL_BOSS);
	Collision(CCollider::COL_PLAYER_ATK, CCollider::COL_STATIC);
	Collision(CCollider::COL_PLAYER, CCollider::COL_STATIC);

	Collision(CCollider::COL_ENEMY, CCollider::COL_PLAYER);
	Collision(CCollider::COL_ENEMY, CCollider::COL_ENEMY);
	Collision(CCollider::COL_ENEMY_ATK, CCollider::COL_PLAYER);
	Collision(CCollider::COL_ENEMY_RANGE, CCollider::COL_PLAYER);
	Collision(CCollider::COL_ENEMY_MELEE_RANGE, CCollider::COL_PLAYER);
	Collision(CCollider::COL_ENEMY, CCollider::COL_STATIC);

	Collision(CCollider::COL_TRIGGER, CCollider::COL_PLAYER);
	Collision(CCollider::COL_ITEM, CCollider::COL_PLAYER);

	Collision(CCollider::COL_BOSS_ATK, CCollider::COL_PLAYER);
}

void CCollider_Manager::Clear_ColliderList()
{
	for (_uint i = 0; i < CCollider::COL_END; ++i)
	{
		for (auto& pCollider : m_ColliderList[i])
			Safe_Release(pCollider);
		m_ColliderList[i].clear();
	}
}

void CCollider_Manager::Collision(CCollider::COLLIDERGROUP SrcGroup, CCollider::COLLIDERGROUP DestGroup)
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
				if (pSrcCollider->Get_Tag() == L"Col_Attack")
					;//cout << "진입" << endl;

				pSrcCollider->Push_Collision_Message(pDestCollider);
			}
		}
	}
}

void CCollider_Manager::Free()
{
	for (_uint i = 0; i < CCollider::COL_END; ++i)
	{
		for (auto& pCollider : m_ColliderList[i])
			Safe_Release(pCollider);
		m_ColliderList->clear();
	}
}