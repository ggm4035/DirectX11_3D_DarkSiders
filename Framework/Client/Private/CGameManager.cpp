#include "CGameManager.h"

#include "CGameInstance.h"
#include "CCursor.h"

#include "CGameInstance.h"
IMPLEMENT_SINGLETON(CGameManager)

void CGameManager::Set_Cursor(CCursor* pCursor)
{
	m_pCursor = pCursor;
	Safe_AddRef(m_pCursor);
}

void CGameManager::Tick(const _double& TimeDelta)
{
	if (false == m_isFirst)
		return;

	_bool isPlaying = { true };
	CGameInstance::GetInstance()->isPlaying(CSound_Manager::SOUND_SUB_BGM, isPlaying);

	if (true == m_isSubBossDead && false == isPlaying)
	{
		if (true == m_isFinalBossDead)
		{
			
			CGameInstance::GetInstance()->Play_BGM(L"mus_level02_ambient.ogg", 0.6f);
			m_isFirst = false;
		}
		else
		{
			CGameInstance::GetInstance()->Play_BGM(L"mus_hell_low.ogg", 0.6f);
			m_isFirst = false;
		}
	}
}

void CGameManager::Free()
{
	Safe_Release(m_pCursor);
}
