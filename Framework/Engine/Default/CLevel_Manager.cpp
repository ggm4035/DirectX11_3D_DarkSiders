#include "CLevel_Manager.h"
#include "CLevel.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Initialize()
{
	return S_OK;
}

void CLevel_Manager::Tick(_double TimeDelta)
{
}

HRESULT CLevel_Manager::Render()
{
	return S_OK;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
