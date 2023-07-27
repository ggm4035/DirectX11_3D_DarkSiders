#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CGameObject3D
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };
	typedef struct tagPlayerDesc : public CTransform::TRASNFORMDESC
	{
		_float4x4 WorldMatrix;
		_float3 vAngle;
	}PLAYERDESC;

public:
	CCollider* Get_Collider(const wstring& wstrColliderTag);
	HITSTATE Get_CurHitState() const {
		return m_eCurHitState;
	}
	void Set_CurHitState(HITSTATE eState) {
		m_eCurHitState = eState;
	}

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

public:
	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) override;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	class CPlayerAction* m_pActionCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Add_Parts();
	HRESULT Set_Shader_Resources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayer* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END