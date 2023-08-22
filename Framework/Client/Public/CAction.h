#pragma once

#include "CBehavior.h"
#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CAction final : public CBehavior
{
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
	void Bind_SoundTag(const wstring& wstrSoundTag) {
		m_wstrSoundTag = wstrSoundTag;
	}
	void Bind_BgmTag(const wstring& wstrSoundTag) {
		m_wstrBgmTag = wstrSoundTag;
	}
	void Set_CoolTime(const _float& fTime) {
		m_fLimit = fTime;
	}
	/* �� �ѹ��� �����ϴ� �׼����� ���� */
	void Just_One_Time_Action() {
		m_isOneTimeAction = true;
	}
	void NotLerp() {
		m_isLerp = false;
	}

private:
	// ��ü���� ������ TimeAcc�� �����ϱ� ���� ����
	// ��Ÿ�� üũ�뵵�ε� ��� ������ �� 
	_float m_fPreTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

	string m_strAnimationTag = { "" };
	wstring m_wstrSoundTag = { L"" };
	wstring m_wstrBgmTag = { L"" };

	_bool m_isFinishBehaviors = { false };
	_bool m_isOneTimeAction = { false };
	_bool m_isFirstAction = { true };
	_bool m_isFirst = { true };
	_bool m_isLerp = { true };
	_bool m_isStopBehavior = { false };

private:
	CModel* m_pModel = { nullptr };

public:
	static CAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END