#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pGameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	for (auto& iter : m_pGameObjects)
		iter->Tick(TimeDelta);
}

void CLayer::Late_Tick(_double TimeDelta)
{
	for (auto& iter : m_pGameObjects)
		iter->Late_Tick(TimeDelta);
}

void CLayer::Free()
{
	for (auto& iter : m_pGameObjects)
		Safe_Release(iter);

	m_pGameObjects.clear();
}
