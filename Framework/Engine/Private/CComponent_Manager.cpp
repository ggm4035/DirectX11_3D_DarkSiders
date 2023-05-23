#include "CComponent_Manager.h"
#include "CComponent.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Containers(_uint iNumLevels)
{
	if (0 == iNumLevels)
		return E_FAIL;

	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pComponent = pPrototype->Clone(pArg);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CComponent_Manager::Clear_LevelResources(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels || 0 > iLevelIndex)
		return;

	for (auto& Pair : m_pPrototypes[iLevelIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	auto& iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTag_Finder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}
	Safe_Delete_Array(m_pPrototypes);
}
