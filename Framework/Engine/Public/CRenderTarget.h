#pragma once

#include "CBase.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() {
		return m_pRTV;
	}

public:
	HRESULT Initialize(const _uint& iSizeX, const _uint& iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor);
	void Clear();
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const string& wstrConstantName);

public:
	HRESULT Ready_Debug(const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#ifdef _DEBUG
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	ID3D11Texture2D* m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	_float4 m_vClearColor;

private:
	_float4x4 m_WorldMatrix;
#ifdef _DEBUG
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const _uint& iSizeX, const _uint& iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor);
	virtual void Free() override;
};

END