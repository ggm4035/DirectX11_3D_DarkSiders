#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)

class CPlayerAction final : public CBehavior
{
	friend class CPlayerController;

public:
	enum STATE { STATE_IDLE, STATE_RUN, STATE_DASH,
		STATE_LATK_1, STATE_LATK_2, STATE_LATK_3, STATE_LATK_4,
		STATE_HATK_1, STATE_HATK_2, STATE_HATK_3, 
		STATE_JUMP, STATE_DOUBLE_JUMP, STATE_JUMP_LAND, STATE_END };

private:
	explicit CPlayerAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerAction(const CPlayerAction& rhs);
	virtual ~CPlayerAction() = default;

public:
	void Set_State(STATE eState);
	STATE Get_State() const {
		return m_eCurState;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	STATE m_eCurState = { STATE_END };
	STATE m_ePreState = { STATE_END };

private:
	CModel* m_pModelCom = { nullptr };

	class CMoveAction* m_pMoveAction = { nullptr };
	class CJumpAction* m_pJumpAction = { nullptr };
	class CAttackAction* m_pAttackAction = { nullptr };

private:
	HRESULT AssembleBehaviors();

public:
	static CPlayerAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayerAction* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END