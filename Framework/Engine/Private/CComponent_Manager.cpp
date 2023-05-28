#include "CComponent_Manager.h"
#include "CComponent.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Containers(_uint iNumLevels, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr != m_pPrototypes || 0 == iNumLevels ||
		nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];
	if (nullptr == m_pPrototypes)
		return E_FAIL;

	m_pPrototype_Transform = CTransform::Create(pDevice, pContext);
	if (nullptr == m_pPrototype_Transform)
		return E_FAIL;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	if (nullptr != dynamic_cast<CTransform*>(pPrototype))
	{
		MSG_BOX("Transform Prototype is already in Engine");
		return E_FAIL;
	}

	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
	{
		MSG_BOX("Prototype is already in Level");
		return E_FAIL;
	}

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

CComponent* CComponent_Manager::Clone_Transform(void* pArg)
{
	if (nullptr == m_pPrototype_Transform)
		return nullptr;

	CComponent* pComponent = m_pPrototype_Transform->Clone(pArg);
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
	Safe_Release(m_pPrototype_Transform);
}
