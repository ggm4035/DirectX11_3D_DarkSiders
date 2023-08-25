#pragma once

#include "CRandomSelector.h"
#include "Client_Defines.h"
#include "CAction.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CPattern_FollowAtk final : public CSelector
{
public:
	typedef struct tagAttackDesc
	{
		string strAttackAnimTag;
		vector<CAction::SOUNDDESC> Sounds;
	}ATTACKDESC;

private:
	explicit CPattern_FollowAtk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPattern_FollowAtk(const CPattern_FollowAtk& rhs);
	virtual ~CPattern_FollowAtk() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	HRESULT Assemble_Childs();
	void Add_Attack(const ATTACKDESC& AttackDesc) {
		m_Attacks.push_back(AttackDesc);
	}

private:
	vector<ATTACKDESC> m_Attacks;

private:
	CModel* m_pModel = { nullptr };

public:
	static CPattern_FollowAtk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPattern_FollowAtk* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END