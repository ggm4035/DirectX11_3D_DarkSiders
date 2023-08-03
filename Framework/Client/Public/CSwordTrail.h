#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Trail;
END

BEGIN(Client)

class CSwordTrail : public CGameObject3D
{
public:
	typedef struct tagSwordTrail
	{
		_uint iNumRect = { 0 };
		_float3 vOffsetLow;
		_float3 vOffsetHigh;
		wstring wstrTextureTag = { L"" };
		_uint iLevelIndex = { 0 };
	}SWORDTRAILDESC;

private:
	explicit CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSwordTrail(const CSwordTrail& rhs);
	virtual ~CSwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _float4x4& WorldMatrix);
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render();

private:
	_float3 m_vLowPosition;
	_float3 m_vHighPosition;
	_float3 m_vOffsetLow;
	_float3 m_vOffsetHigh;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Trail* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_ShaderResources();

public:
	static CSwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSwordTrail* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Free() override;
};

END