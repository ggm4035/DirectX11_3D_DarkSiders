#pragma once

#include "CBehavior.h"
#include "CGameObject3D.h"
#include "Client_Defines.h"
#include "CSound_Manager.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CAction final : public CBehavior
{
public:
	typedef struct tagSoundDesc
	{
		_bool isPlaySound;
		_float fTime;
		wstring wstrSoundTag;
		CSound_Manager::SOUNDCHANNEL eChennel;
		_float fVolum;
	}SOUNDDESC;

private:
	explicit CAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAction(const CAction& rhs);
	virtual ~CAction() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Bind_AnimationTag(const string& strAnimationTag) {
		m_strAnimationTag = strAnimationTag;
	}
	void Add_Sound(const SOUNDDESC& SoundDesc) {
		m_Sounds.push_back(SoundDesc);
	}
	void Bind_BgmTag(const wstring& wstrSoundTag) {
		m_wstrBgmTag = wstrSoundTag;
	}
	void Set_CoolTime(const _float& fTime) {
		m_fLimit = fTime;
	}
	/* 단 한번만 실행하는 액션으로 설정 */
	void Just_One_Time_Action() {
		m_isOneTimeAction = true;
	}
	void NotLerp() {
		m_isLerp = false;
	}

private:
	// 객체에서 가져온 TimeAcc를 저장하기 위한 변수
	// 쿨타임 체크용도로도 사용 가능할 듯 
	_float m_fPreTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };
	_float m_fTimeAcc = { 0.f };

	string m_strAnimationTag = { "" };
	vector<SOUNDDESC> m_Sounds;
	wstring m_wstrBgmTag = { L"" };

	_bool m_isFinishBehaviors = { false };
	_bool m_isOneTimeAction = { false };
	_bool m_isFirstAction = { true };
	_bool m_isFirst = { true };
	_bool m_isLerp = { true };
	_bool m_isStopBehavior = { false };

private:
	CModel* m_pModel = { nullptr };

private:
	void PlaySounds(const _double& TimeDelta);

public:
	static CAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END