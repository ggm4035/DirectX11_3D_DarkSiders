#pragma once

#include "Client_Defines.h"
#include "CGameObject3D.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CNavigation;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CGameObject3D
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };
	typedef struct tagPlayerDesc
	{
		_float4x4 WorldMatrix;
	}PLAYERDESC;

private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_Shader_Resources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayer* Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END