#pragma once

#include "CBehavior.h"
#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)

class CPattern final : public CBehavior
{
public:
	enum STATE { STATE_NONE, STATE_START, STATE_RUNING, STATE_FINISH, STATE_END };

private:
	explicit CPattern(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPattern(const CPattern& rhs);
	virtual ~CPattern() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Bind_AnimationTag(const string& strStartAnimTag, const string& strRunningAnimTag, const string& strFinishAnimTag) {
		m_strStartAnimTag = strStartAnimTag;
		m_strRunningAnimTag = strRunningAnimTag;
		m_strFinishAnimTag = strFinishAnimTag;
	}

private:
	string m_strStartAnimTag = { "" };
	string m_strRunningAnimTag = { "" };
	string m_strFinishAnimTag = { "" };
	_bool m_isFirst = { true };
	STATE m_eCurState = { STATE_NONE };

	CModel* m_pModel = { nullptr };
	CTransform* m_pTransform = { nullptr };

public:
	static CPattern* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPattern* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END