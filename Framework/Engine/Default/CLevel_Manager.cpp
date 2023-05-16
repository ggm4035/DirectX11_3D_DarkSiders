#include "CLevel_Manager.h"
#include "CLevel.h"
#include "CGameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, class CLevel* pNewLevel)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	// ���� ������ �ִ� �ڿ��� �����.
	if(nullptr != m_pCurrentLevel)
		pGameInstance->Clear_LevelResources(m_iLevelIndex);

	// ������ ������ �����Ѵ�.
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;
	m_iLevelIndex = iLevelIndex;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Manager::Tick(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(TimeDelta);
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
