#pragma once

#include "CRandomSelector.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CBoss_Attack final : public CSelector
{
private:
	explicit CBoss_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss_Attack(const CBoss_Attack& rhs);
	virtual ~CBoss_Attack() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	HRESULT Assemble_Childs();
	void Set_CoolTime(const _float& fTime) {
		m_fLimit = fTime;
	}
	void Add_Attack_AnimTag(const string& strAnimationTag) {
		m_AttackAnimTagList.push_back(strAnimationTag);
	}

private:
	// 객체에서 가져온 TimeAcc를 저장하기 위한 변수
	// 쿨타임 체크용도로도 사용 가능할 듯 
	_float m_fPreTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };
	list<string> m_AttackAnimTagList;

private:
	CModel* m_pModel = { nullptr };

public:
	static CBoss_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBoss_Attack* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END