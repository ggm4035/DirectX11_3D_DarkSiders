#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CStatic_Object final : public CGameObject3D
{
public:
	typedef struct tagStaticModelDesc
	{
		wstring wstrModelTag = { L"" };
		_float4x4 WorldMatrix;
		_float3 vAngle;
	}STATICOBJECTDESC;

private:
	explicit CStatic_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CStatic_Object(const CStatic_Object& rhs);
	virtual ~CStatic_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	/* Transform 정보 초기화 하고 싶으면 void*에 값 채우기 */
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Bind_ShaderResources();

public:
	static CStatic_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CStatic_Object* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END