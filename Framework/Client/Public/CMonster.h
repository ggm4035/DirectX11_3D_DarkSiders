#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
class CRenderer;
class CShader;
class CModel;
class CRoot;

class CHealth;
class CAttack;
class CDeffence;
END

BEGIN(Client)

class CMonster : public CGameObject3D
{
public:
	typedef struct tagMonsterDesc : public CTransform::TRASNFORMDESC
	{
		_float4x4 WorldMatrix;
		_float3 vAngle;
		_uint iNavigationIndex = { 0 };
	}MONSTERDESC;

protected:
	explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual void Get_Damaged(const CAttack* pAttack) override;
	void Spawn() {
		m_isSpawn = true;
	}

public:
	virtual HRESULT Initialize_Prototype() = 0;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta);
	virtual void AfterFrustumTick(const _double& TimeDelta);
	virtual void Late_Tick(const _double& TimeDelta);
	virtual HRESULT Render(/*const _uint& iPassIndex = 0*/);

public:
	virtual void Dead_Motion(const _double& TimeDelta) override;
	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) override;

protected:
	CRoot* m_pRoot = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

	class CUI_HpBar* m_pHealthBar = { nullptr };

protected: /* Status */
	CHealth* m_pHealth = { nullptr };
	CAttack* m_pAttack = { nullptr };
	CDeffence* m_pDeffence = { nullptr };

protected:
	_bool m_isSpawn = { false };
	_bool m_isAbleAttack = { false };
	_bool m_isRangeInPlayer = { false };

	_float m_fTimeAcc = { 0.f };

protected:
	virtual HRESULT Add_Components() override;
	HRESULT Set_Shader_Resources();

private:
	_float m_fHitTimeAcc = { 0.f };
	_bool m_isSuperArmor = { false };

public:
	virtual void Free() override;
};

END