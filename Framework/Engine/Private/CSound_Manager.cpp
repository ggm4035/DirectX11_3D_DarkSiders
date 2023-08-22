#include "CSound_Manager.h"

#include <io.h>

IMPLEMENT_SINGLETON(CSound_Manager)

HRESULT CSound_Manager::isPlaying(SOUNDCHANNEL eChannel, OUT _bool& Out)
{
	return m_Channels[eChannel]->isPlaying(&Out);
}

HRESULT CSound_Manager::Initialize()
{
	// fmod system 객체 생성
	if (FMOD_OK != FMOD::System_Create(&m_pSystem))
	{
		MSG_BOX("Failed Create fmod System");
		return E_FAIL;
	}
	// 시스템 초기화
	if (FMOD_OK != m_pSystem->init(MAX_CHANNEL, FMOD_INIT_NORMAL, NULL))
	{
		MSG_BOX("Failed Initialize FMOD");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSound_Manager::Add_Sounds(const _tchar* pSoundFilePath)
{
	// 사운드 파일 저장
	if (FAILED(Load_SoundFile(pSoundFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Play_Sound(const _tchar* pSoundTag, SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay)
{
	FMOD::Sound* pSound = Find_Sound(pSoundTag);

	if (nullptr == pSound)
		return E_FAIL;

	if (0 > eChannel ||
		SOUND_END <= eChannel)
		return E_FAIL;

	_bool isPlay = { false };

	//  forcePlay가 true면 강제로 들어온 사운드 재생
	if (true == bForcePlay ||
		nullptr == m_Channels[eChannel])
	{
		m_pSystem->playSound(pSound, nullptr, false, &m_Channels[eChannel]);
	}
	else
	{
		// false일때 재생중이면 패스, 재생중이 아니면 들어온 사운드 재생.
		m_Channels[eChannel]->isPlaying(&isPlay);

		if (false == isPlay)
			m_pSystem->playSound(pSound, nullptr, false, &m_Channels[eChannel]);
		else
			return S_OK;
	}

	// 예외처리
	if (nullptr == m_Channels[eChannel])
	{
		MSG_BOX("Failed Set Channel");
		return E_FAIL;
	}

	if (FMOD_OK != m_Channels[eChannel]->setVolume(fVolume))
		return E_FAIL;

	if (FMOD_OK != m_pSystem->update())
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Play_BGM(const _tchar* pSoundTag, _float fVolume, SOUNDCHANNEL eChannel)
{
	FMOD::Sound* pSound = Find_Sound(pSoundTag);

	if (nullptr == pSound)
		return E_FAIL;

	if (FMOD_OK != m_pSystem->playSound(pSound, nullptr, false, &m_Channels[eChannel]))
		return E_FAIL;

	m_Channels[eChannel]->setMode(FMOD_LOOP_NORMAL);
	m_Channels[eChannel]->setVolume(fVolume);
	m_pSystem->update();

	return S_OK;
}

HRESULT CSound_Manager::Stop_Sound(SOUNDCHANNEL eChannel)
{
	if (0 > eChannel ||
		SOUND_END <= eChannel)
		return E_FAIL;

	m_Channels[eChannel]->stop();

	return S_OK;
}

HRESULT CSound_Manager::Pause_Sound(SOUNDCHANNEL eChannel)
{
	if (0 > eChannel ||
		SOUND_END <= eChannel)
		return E_FAIL;

	m_Channels[eChannel]->setPaused(true);

	return S_OK;
}

HRESULT CSound_Manager::Restart_Sound(SOUNDCHANNEL eChannel)
{
	if (0 > eChannel ||
		SOUND_END <= eChannel)
		return E_FAIL;

	m_Channels[eChannel]->setPaused(false);

	return S_OK;
}

HRESULT CSound_Manager::Stop_AllSound()
{
	for (auto& pChannel : m_Channels)
		pChannel->stop();

	return S_OK;
}

HRESULT CSound_Manager::Set_ChannelVolume(SOUNDCHANNEL eChannel, _float fVolume)
{
	if (0 > eChannel ||
		SOUND_END <= eChannel)
		return E_FAIL;

	m_Channels[eChannel]->setVolume(fVolume);
	m_pSystem->update();

	return S_OK;
}

HRESULT CSound_Manager::Load_SoundFile(const _tchar* pSoundFile)
{
	_wfinddata_t FindDataValue;

	_tchar szAllFiles[MAX_PATH] = TEXT("");
	lstrcpy(szAllFiles, pSoundFile);
	lstrcat(szAllFiles, TEXT("*.*"));

	INT64 handle = _wfindfirst(szAllFiles, &FindDataValue);

	if (-1 == handle)
		return E_FAIL;

	_int iResult = { 0 };

	_tchar szFullPath[MAX_PATH] = TEXT("");

	// 파일이 없을떄까지 순회.
	while (-1 != iResult)
	{
		lstrcpy(szFullPath, pSoundFile);
		lstrcat(szFullPath, FindDataValue.name);

		// system file
		if (!lstrcmp(FindDataValue.name, TEXT(".")) ||
			!lstrcmp(FindDataValue.name, TEXT("..")))
		{
			iResult = _wfindnext(handle, &FindDataValue);
			continue;
		}

		FMOD::Sound* pSound = { nullptr };

		_char szFmodPath[MAX_PATH] = "";
		WideCharToMultiByte(CP_ACP, 0, szFullPath, -1, szFmodPath, MAX_PATH, nullptr, nullptr);

		if (FMOD_OK == m_pSystem->createSound(szFmodPath, FMOD_DEFAULT, nullptr, &pSound))
		{
			_int iLength = lstrlen(FindDataValue.name) + 1;

			_tchar* szSoundKey = new _tchar[iLength];
			ZeroMemory(szSoundKey, sizeof(_tchar) * iLength);

			lstrcpy(szSoundKey, FindDataValue.name);

			m_Sounds.emplace(szSoundKey, pSound);
		}
		else
		{
			MSG_BOX("Failed Create Sound");
			return E_FAIL;
		}

		iResult = _wfindnext(handle, &FindDataValue);
	}

	if (FMOD_OK != m_pSystem->update())
	{
		MSG_BOX("Failed update FMOD");
		_findclose(handle);
		return E_FAIL;
	}

	_findclose(handle);

	return S_OK;
}

FMOD::Sound* CSound_Manager::Find_Sound(const _tchar* pSoundTag)
{
	auto iter = find_if(m_Sounds.begin(), m_Sounds.end(), CTag_Finder(pSoundTag));

	if (iter == m_Sounds.end())
		return nullptr;

	return iter->second;
}

void CSound_Manager::Free()
{
	for (auto& pair : m_Sounds)
	{
		delete[] pair.first;
		pair.second->release();
	}
	m_Sounds.clear();

	m_pSystem->release();
	m_pSystem->close();
}