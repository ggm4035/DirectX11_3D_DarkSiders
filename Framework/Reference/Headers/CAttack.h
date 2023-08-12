#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CAttack final : public CComposite
{
public:
	typedef struct tagAttackDesc
	{
		_int iDamage;
		_int iSkillDamage;
		_bool isIgnoreDeffence;
	}ATTACKDESC;

private:
	explicit CAttack(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CAttack(const CAttack& rhs);
	virtual ~CAttack() = default;

public:
	_bool isIgnoreDeffence() const {
		return m_isIgnoreDeffence;
	}
	const _int& Get_Damage() const {
		return m_iDamage;
	}
	const _int& Get_Skill_Damage() const {
		return m_iSkillDamage;
	}
	void Set_Damage(const _int& iDamage) {
		m_iDamage = iDamage;
	}
	void Set_Skill_Damage(const _int& iDamage) {
		m_iSkillDamage = iDamage;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;

private:
	_int m_iDamage = { 0 };
	_int m_iSkillDamage = { 0 };
	_bool m_isIgnoreDeffence = { false };

public:
	static CAttack* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CAttack* Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg);
	virtual void Free() override;
};

END