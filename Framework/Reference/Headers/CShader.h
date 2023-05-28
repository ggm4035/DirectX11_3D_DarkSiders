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
	HRESULT Initialize_Prototype(const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pInputElementsDesc, const _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Float(const _char * pTypename, const _float& rFloat);
	HRESULT Bind_Float4x4(const _char * pTypename, const _float4x4 * pMatrix);
	HRESULT Bind_Rasterizer(const _char * pTypename, _uint iIndex, ID3D11RasterizerState * _pRasterizer);
	HRESULT Bind_ShaderResource(const _char * pTypename, ID3D11ShaderResourceView * pSRV);
	HRESULT Bind_ShaderResources(const _char * pTypename, ID3D11ShaderResourceView * *ppSRV, _uint iNumTextures);

public:
	HRESULT Begin(const _uint iNumIndex);

private:
	ID3DX11Effect* m_pEffect = { nullptr };
	_uint m_iNumPasses = { 0 };
	vector<ID3D11InputLayout*> m_InputLayouts;

public:
	static CShader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pInputElementsDesc, const _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END