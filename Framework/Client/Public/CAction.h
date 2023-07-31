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
	/* 단 한번만 실행하는 액션으로 설정 */
	void Just_One_Time_Action() {
		m_isOneTimeAction = true;
	}
	void NotLerp() {
		m_isLerp = false;
	}

private:
	string m_strAnimationTag = { "" };

	_bool m_isFinishBehaviors = { false };
	_bool m_isOneTimeAction = { false };
	_bool m_isFirstAction = { true };
	_bool m_isFirst = { true };
	_bool m_isLerp = { true };

private:
	CModel* m_pModel = { nullptr };

public:
	static CAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END