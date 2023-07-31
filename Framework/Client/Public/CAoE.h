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

class CAoE final : public CGameObject3D
{
public:
	typedef struct tagAOEDesc
	{
		_uint iTextureLevel = { 0 };
		_uint iPassNum = { 0 };
		wstring wstrTextureTag = { L"" };
		string strShaderResourceTag = { "" };
	}AOEDESC;

private:
	explicit CAoE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAoE(const CAoE& rhs);
	virtual ~CAoE() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _float4x4& WorldMatrix);
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render();

private:
	_uint m_iPassNum = { 0 };
	string m_strShaderResourceTag = { "" };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_ShaderResources();

public:
	static CAoE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAoE* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END