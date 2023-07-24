#pragma once

#include "CParts.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon : public CParts
{
public:
	typedef struct tagWeaponDesc : public PARENTDESC
	{
		_uint iModelLevel = { 0 };
		wstring wstrModelTag = { L"" };
	}WEAPONDESC;

private:
	explicit CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Bind_Collider(const _uint& iLevelIndex, const wstring& wstrPrototypeTag, const wstring& wstrColliderTag, void* pArg = nullptr);

	virtual void OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta) override;
	virtual void OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta) override;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_Shader_Resources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END