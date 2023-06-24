#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	explicit CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	HRESULT Initialize_Prototype(const wstring& pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pInputElementsDesc, const _uint& iNumElements);
	virtual HRESULT Initialize(const _uint& iLayerIndex, CComponent * pOwner, void* pArg) override;

public:
	HRESULT Bind_RawValue(const string& Typename, const void* pValue, const _uint& iLength);
	HRESULT Bind_Float4x4(const string& Typename, const _float4x4 * pMatrix);
	HRESULT Bind_Float4x4_Array(const string& Typename, const _float4x4 * pMatrix, const _uint& iNumMatrices);
	HRESULT Bind_Rasterizer(const string& Typename, const _uint& iIndex, ID3D11RasterizerState * _pRasterizer);
	HRESULT Bind_ShaderResource(const string& Typename, ID3D11ShaderResourceView * pSRV);
	HRESULT Bind_ShaderResources(const string& Typename, ID3D11ShaderResourceView * *ppSRV, const _uint& iNumTextures);

public:
	HRESULT Begin(const _uint& iNumIndex);

private:
	ID3DX11Effect* m_pEffect = { nullptr };
	_uint m_iNumPasses = { 0 };
	vector<ID3D11InputLayout*> m_InputLayouts;

public:
	static CShader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& ShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pInputElementsDesc, const _uint& iNumElements);
	virtual CComponent* Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END