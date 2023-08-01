#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTransform;
END

BEGIN(Client)

class CPlayerAction final : public CBehavior
{
	friend class CPlayerController;

public:
	enum STATE { STATE_IDLE, STATE_RUN, STATE_DASH, STATE_HIT, STATE_KNOCKBACK,
		STATE_LATK_1, STATE_LATK_2, STATE_LATK_3, STATE_LATK_4,
		STATE_JUMP, STATE_DOUBLE_JUMP, STATE_JUMP_LAND, 
		STATE_WHEEL, STATE_LEAP, STATE_END };

private:
	explicit CPlayerAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerAction(const CPlayerAction& rhs);
	virtual ~CPlayerAction() = default;

public:
	void Set_State(STATE eState);
	STATE Get_State() const {
		return m_eCurState;
	}
	void On_SuperArmor() {
		m_isSuperArmor = true;
	}
	const _float* Get_LeapCoolTimePtr() const;
	const _float* Get_WheelCoolTimePtr() const;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	HRESULT AssembleBehaviors();
	void Reset_Wheel();
	void Reset_Leap();
	void Reset_Jump();

private:
	STATE m_eCurState = { STATE_END };
	STATE m_ePreState = { STATE_END };

	_float m_fSuperArmor = { 3.f };
	_float m_fTimeAcc = { 0.f };
	_bool m_isSuperArmor = { false };

private:
	class CPlayer* m_pPlayer = { nullptr };
	CModel* m_pModelCom = { nullptr };

	class CPlayerHit* m_pHitAction = { nullptr };
	class CPlayerMove* m_pMoveAction = { nullptr };
	class CPlayerJump* m_pJumpAction = { nullptr };
	class CPlayerAttack* m_pAttackAction = { nullptr };
	class CPlayerKnockback* m_pKnockbackAction = { nullptr };

	class CWheelWind* m_pSkillWheelWind = { nullptr };
	class CLeapAttack* m_pSkillLeapAttack = { nullptr };

public:
	static CPlayerAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayerAction* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END