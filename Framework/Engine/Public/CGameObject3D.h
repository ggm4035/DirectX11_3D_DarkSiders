#pragma once

#include "CGameObject.h"
#include "CTransform.h"

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
	virtual void OnCollisionEnter(const _double& TimeDelta) {}
	virtual void OnCollisionStay(const _double& TimeDelta) {}
	virtual void OnCollisionExit(const _double& TimeDelta) {}

protected:
	CTransform* m_pTransformCom = { nullptr };

protected:
	virtual HRESULT Add_Components() = 0;

public:
	virtual CGameObject3D* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END