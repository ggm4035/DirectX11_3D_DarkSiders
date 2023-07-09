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

class CMonster : public CGameObject3D
{
public:
	typedef struct tagMonsterDesc : public CTransform::TRASNFORMDESC
	{
		_float4x4 WorldMatrix;
		_float3 vAngle;
	}MONSTERDESC;

private:
	explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
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
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMonster* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END