#pragma once

#include "CBase.h"

#define	MAX_CHANNEL		32

BEGIN(Engine)

class CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)

public:
	enum SOUNDCHANNEL {
		SOUND_BGM,
		SOUND_PLAYER,
		SOUND_ENEMY,
		SOUND_EFFECT_1, SOUND_EFFECT_2,
		SOUND_END
	};

private:
	explicit CSound_Manager() = default;
	virtual ~CSound_Manager() = default;

public:
	// 이거 꼭 Initialize_Engine에 넣어서 초기화 실행해주세요
	HRESULT Initialize();
	// 파일경로에 있는 사운드를 자동으로 파일이름을 태그값으로 입력해줌
	HRESULT Add_Sounds(const _tchar* pSoundFilePath);

public:
	// forceplay 활성화하면 현재 재생중인 사운드 무시하고 실행함.
	// default 매개변수 설정하는것을 추천함.
	HRESULT Play_Sound(const _tchar* pSoundTag, SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay = false);
	HRESULT Play_BGM(const _tchar* pSoundTag, _float fVolume);
	HRESULT Stop_Sound(SOUNDCHANNEL eChannel);
	HRESULT Pause_Sound(SOUNDCHANNEL eChannel);
	HRESULT Restart_Sound(SOUNDCHANNEL eChannel);
	HRESULT Stop_AllSound();
	HRESULT Set_ChannelVolume(SOUNDCHANNEL eChannel, _float fVolume);

private:
	HRESULT Load_SoundFile(const _tchar* pSoundFile);
	FMOD::Sound* Find_Sound(const _tchar* pSoundTag);

private:
	unordered_map<_tchar*, FMOD::Sound*>	m_Sounds;
	FMOD::Channel* m_Channels[SOUND_END] = { nullptr };
	FMOD::System* m_pSystem = { nullptr };

public:
	virtual void Free() override;
};

END