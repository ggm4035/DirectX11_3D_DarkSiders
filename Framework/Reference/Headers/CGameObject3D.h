#pragma once

#include "CGameObject.h"
#include "CTransform.h"
#include "CCollider.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject3D abstract : public CGameObject
{
protected:
	explicit CGameObject3D(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObject3D(const CGameObject3D& rhs);
	virtual ~CGameObject3D() = default;

public:
	CTransform* Get_Transform() const {
		return m_pTransformCom;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	/* Transform 정보 초기화 하고 싶으면 void*에 값 채우기 */
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() { return S_OK; }

public:
	CCollider* Find_Collider(const wstring& wstrColliderTag);

	virtual void Get_Damaged(const class CAttack* pAttack) {}
	virtual void Dead_Motion(const _double& TimeDelta) {}

	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) {}
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) {}
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) {}

protected:
	CTransform* m_pTransformCom = { nullptr };
	unordered_map<wstring, CCollider*> m_umapColliders;

protected: /* Collider */
	HRESULT Add_Collider(const _uint& iLevelIndex, const wstring& wstrPrototypeTag, const wstring& wstrColliderTag, void* pArg);
	HRESULT Add_Colliders_To_Manager();
	void Tick_Colliders(_fmatrix WorldMatrix);
	void On_Colisions(const _double& TimeDelta);

#if defined(_USE_IMGUI) || defined(_DEBUG)
	HRESULT Add_Colliders_Debug_Render_Group(class CRenderer* pRenderer);
#endif // _DEBUG

protected:
	virtual HRESULT Add_Components() = 0;

public:
	virtual CGameObject3D* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END