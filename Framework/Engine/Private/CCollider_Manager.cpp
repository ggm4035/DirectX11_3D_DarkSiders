#include "CCollider_Manager.h"

#include "CCollider.h"

IMPLEMENT_SINGLETON(CCollider_Manager)

HRESULT CCollider_Manager::Add_Collider(COLLIDERGROUP eGroupID, CCollider* pCollider)
{
	if (0 > eGroupID || COL_END <= eGroupID ||
		nullptr == pCollider)
		return E_FAIL;

	if (string::npos != pCollider->Get_Tag().find(m_wstrEnableTag))
		pCollider->Switch_On();

	m_ColliderList[eGroupID].push_back(pCollider);
	Safe_AddRef(pCollider);

	return S_OK;
}

void CCollider_Manager::Tick()
{
	/* �ݶ��̴� �浹 üũ */
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
	m_wstrEnableTag = L"^-^";
}

/* �̶� �︮�� ������ ���� ������Ʈ�� tick �߿� ȣ��� �׷��� �̶��� �ݶ��̴� ���µ� */
void CCollider_Manager::Update_Observer(NOTIFYDESC* pNotifyDesc)
{
	/* �ִϸ��̼��� Ư�� ������ ������ ��� */
	NOTIFYCOLLIDERDESC Desc = *static_cast<NOTIFYCOLLIDERDESC*>(pNotifyDesc);
	
	m_wstrEnableTag = Desc.wstrTag;
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
