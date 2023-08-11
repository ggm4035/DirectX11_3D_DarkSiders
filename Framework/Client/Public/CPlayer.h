#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
class CNavigation;
class CRoot;

class CHealth;
class CAttack;
class CDeffence;
END

BEGIN(Client)

class CPlayer final : public CGameObject3D
{
public:
	typedef struct tagPlayerDesc : public CTransform::TRASNFORMDESC
	{
		_float4x4 WorldMatrix;
		_float3 vAngle;
	}PLAYERDESC;

public:
	virtual void Get_Damaged(const CAttack* pAttack) override;
	void Get_Damaged_Knockback(const _float4& _vPosition, const CAttack* pAttack);
	class CInven* Get_Inven() {
		return m_pInven;
	}
	class CPlayerAction* Get_Action() {
		return m_pActionCom;
	}
	CCollider* Get_Collider(const wstring& wstrColliderTag);

private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) override;

private:
	CNavigation* m_pNavigationCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRoot* m_pRoot = { nullptr };

	class CPlayerAction* m_pActionCom = { nullptr };

private: /* Status */
	CHealth* m_pHealth = { nullptr };
	CAttack* m_pAttack = { nullptr };
	CDeffence* m_pDeffence = { nullptr };

private:
	class CInven* m_pInven = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Add_Parts();
	HRESULT Set_Shader_Resources();
	HRESULT Set_Shader_Shadow_Resources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayer* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END