#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	HRESULT Set_Matrix(const char* pTypename, const _matrix * pMatrix);

public:
	HRESULT Initialize_Prototype(const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC * pInputElementsDesc, const _uint iNumElements);
	HRESULT Initialize(void* pArg) override;

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