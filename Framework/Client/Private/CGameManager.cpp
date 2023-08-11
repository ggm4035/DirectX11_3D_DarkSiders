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
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Down(DIK_ESCAPE))
		m_isPop = !m_isPop;

	Safe_Release(pGameInstance);

	if (true == m_isPop)
		PopUpMenu(TimeDelta);

	BossSoundCheck();
}

void CGameManager::BossSoundCheck()
{
	if (false == m_isFirst)
		return;

	_bool isPlaying = { true };
	CGameInstance::GetInstance()->isPlaying(CSound_Manager::SOUND_SUB_BGM, isPlaying);

	if (true == m_isSubBossDead && false == isPlaying)
	{
		if (true == m_isFinalBossDead)
		{
			CGameInstance::GetInstance()->Play_BGM(L"mus_hell_low.ogg", 0.6f);
			m_isFirst = false;
		}
		else
		{
			CGameInstance::GetInstance()->Play_BGM(L"mus_level02_ambient.ogg", 0.6f);
			m_isFirst = false;
		}
	}
}

void CGameManager::PopUpMenu(const _double& TimeDelta)
{
}

void CGameManager::Free()
{
	Safe_Release(m_pCursor);
}
