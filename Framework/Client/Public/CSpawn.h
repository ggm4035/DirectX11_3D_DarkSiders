#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSpawn final : public CGameObject3D
{
private:
	explicit CSpawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSpawn(const CSpawn& rhs);
	virtual ~CSpawn() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _float4x4& WorldMatrix, const _double& TimeDelta);
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render();

private:
	_bool m_isFirst = { true };
	_float4 m_vPosition;
	_float m_fTimeAcc = { 0.f };
	_float m_fScale = { 0.f };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_ShaderResources();

public:
	static CSpawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpawn* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END