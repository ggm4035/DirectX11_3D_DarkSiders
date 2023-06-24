#include "CObject_Manager.h"
#include "CGameObject.h"
#include "CLayer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Containers(const _uint& iNumLevels)
{
	if (0 == iNumLevels)
		return E_FAIL;

	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring& PrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(PrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(PrototypeTag, pPrototype);
	pPrototype->Set_Tag(PrototypeTag);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(const _uint& iLayerIndex, const wstring& PrototypeTag, const wstring& GameObjectTag, const wstring& LayerTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(PrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(iLayerIndex, nullptr, pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLayerIndex, LayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		m_pLayers[iLayerIndex].emplace(LayerTag, pLayer);

		pLayer->Add_GameObject(pGameObject);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	pGameObject->Set_Tag(GameObjectTag);

	return S_OK;
}

void CObject_Manager::Tick(const _double& TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Tick(TimeDelta);
	}
}

void CObject_Manager::Late_Tick(const _double& TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Tick(TimeDelta);
	}
}

void CObject_Manager::Clear_LevelResources(const _uint& iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

list<CGameObject*> CObject_Manager::Get_All_GameObject()
{
	list<CGameObject*> retList;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			for (auto& GameObject : Pair.second->m_pGameObjects)
			{
				retList.push_back(GameObject);
			}
		}
	}

	return retList;
}

HRESULT CObject_Manager::Remove_GameObject(const wstring& GameObjectTag)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			for (auto& iter = Pair.second->m_pGameObjects.begin(); iter != Pair.second->m_pGameObjects.end(); ++iter)
			{
				if (GameObjectTag == (*iter)->Get_Tag())
				{
					Safe_Release(*iter);
					Pair.second->m_pGameObjects.erase(iter);
					return S_OK;
				}
			}
		}
	}

	return E_FAIL;
}

CGameObject* CObject_Manager::Find_Prototype(const wstring& PrototypeTag)
{
	auto& iter = m_Prototypes.find(PrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(const _uint& iLayerIndex, const wstring& LayerTag)
{
	auto& iter = m_pLayers[iLayerIndex].find(LayerTag);

	if (iter == m_pLayers[iLayerIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
	{
		Safe_Release(Pair.second);
	}
	m_Prototypes.clear();
}
