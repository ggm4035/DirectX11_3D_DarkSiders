#pragma once

#include "Client_Defines.h"
#include "CGameObject3D.h"

BEGIN(Engine)
class CNavigation;
class CRenderer;
class CShader;
class CModel;
class CRoot;
END

BEGIN(Client)

class CMonster : public CGameObject3D
{
public:
	typedef struct tagMonsterDesc : public CTransform::TRASNFORMDESC
	{
		_float4x4 WorldMatrix;
		_float3 vAngle;
	}MONSTERDESC;

protected:
	explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() = 0;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) = 0;
	virtual void AfterFrustumTick(const _double& TimeDelta) = 0;
	virtual void Late_Tick(const _double& TimeDelta) = 0;
	virtual HRESULT Render(const _uint& iPassIndex);

public:
	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) override;

protected:
	CRoot* m_pRoot = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

protected:
	_bool m_isRangeInPlayer = { false };
	_bool m_isSpawn = { false };
	_bool m_isSpawnEnd = { false };

protected:
	virtual HRESULT Add_Components() override;
	HRESULT Set_Shader_Resources();

public:
	virtual void Free() override;
};

END