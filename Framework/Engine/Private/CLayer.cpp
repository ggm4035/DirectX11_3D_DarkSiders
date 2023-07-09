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

void CLayer::Tick(const _double& TimeDelta)
{
	for (auto& iter : m_pGameObjects)
		iter->Tick(TimeDelta);
}

void CLayer::AfterFrustumTick(const _double& TimeDelta)
{
	for (auto& iter : m_pGameObjects)
		iter->AfterFrustumTick(TimeDelta);
}

void CLayer::Late_Tick(const _double& TimeDelta)
{
	for (auto& iter : m_pGameObjects)
		iter->Late_Tick(TimeDelta);
}

void CLayer::Free()
{
	for (auto& pGameObject : m_pGameObjects)
		Safe_Release(pGameObject);

	m_pGameObjects.clear();
}
