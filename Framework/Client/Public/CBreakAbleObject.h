#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
class CHealth;
END

BEGIN(Client)

class CBreakAbleObject final : public CGameObject3D
{
public:
	typedef struct tagStaticModelDesc
	{
		wstring wstrModelTag = { L"" };
		_float4x4 WorldMatrix;
		_float3 vAngle;
		_float3 vExtents;
	}STATICOBJECTDESC;

private:
	explicit CBreakAbleObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBreakAbleObject(const CBreakAbleObject& rhs);
	virtual ~CBreakAbleObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	/* Transform 정보 초기화 하고 싶으면 void*에 값 채우기 */
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Dead_Motion(const _double& TimeDelta) override;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pCollider = { nullptr };
	CHealth* m_pHealth = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Bind_ShaderResources();

public:
	static CBreakAbleObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBreakAbleObject* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END