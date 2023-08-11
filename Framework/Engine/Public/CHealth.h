#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CHealth final : public CComposite
{
public:
	enum HITSTATE { HIT_NONE, HIT_ENTER, HIT_STAY, HIT_KNOCKBACK, HIT_END };

	typedef struct tagHealthDesc
	{
		_int iMaxHP;
		_int iHP;
	}HEALTHDESC;

private:
	explicit CHealth(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CHealth(const CHealth& rhs);
	virtual ~CHealth() = default;

public:
	const _bool& isDead() const {
		return m_iHP <= 0 ? true : false;
	}
	HITSTATE Get_Current_HitState() const {
		return m_eCurHitState;
	}
	/* 0.f ~ 1.f 사이의 퍼센트 */
	const _float& Get_Current_HP_Percent() const {
		return _float(m_iHP) / _float(m_iMaxHP);
	}
	const _int& Get_MaxHP() const {
		return m_iMaxHP;
	}
	void Set_HitState(HITSTATE eState) {
		m_eCurHitState = eState;
	}
	void Set_Max_Hp(const _int& iMaxHP) {
		m_iMaxHP = iMaxHP;
	}
	void Set_HP(const _int& iHP) {
		m_iHP = iHP;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;

public:
	void Damaged(const _int& iValue) {
		m_eCurHitState = HIT_ENTER;
		Saturate(m_iHP -= iValue, 0, m_iMaxHP);
	}
	void Damaged_By_Knockback(const _int& iValue) {
		m_eCurHitState = HIT_KNOCKBACK;
		Saturate(m_iHP -= iValue, 0, m_iMaxHP);
	}
	void Heal(const _int& iValue) {
		Saturate(m_iHP += iValue, 0, m_iMaxHP);
	}

private:
	_int m_iMaxHP = { 0 };
	_int m_iHP = { 0 };

private:
	HITSTATE m_eCurHitState = { HIT_NONE };

public:
	static CHealth* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CHealth* Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg);
	virtual void Free() override;
};

END