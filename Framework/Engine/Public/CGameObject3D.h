#pragma once

#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject3D abstract : public CGameObject
{
protected:
	explicit CGameObject3D(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObject3D(const CGameObject3D& rhs);
	virtual ~CGameObject3D() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	/* Transform 정보 초기화 하고 싶으면 void*에 값 채우기 */
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	class CTransform* m_pTransformCom = { nullptr };

protected:
	virtual HRESULT Add_Components() = 0;

public:
	virtual CGameObject3D* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END