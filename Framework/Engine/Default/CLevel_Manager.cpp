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

	// 이전 레벨이 있던 자원을 지운다.
	// 지금 보면 최초로 시작 할 경우 0번의 레벨(현재 클라 기준.STATIC_LEVEL)을 지우는점 주의.
	pGameInstance->Clear_LevelResources(m_iLevelIndex);

	// 기존의 레벨을 삭제한다.
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
