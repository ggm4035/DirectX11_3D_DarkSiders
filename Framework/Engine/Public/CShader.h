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
	HRESULT Initialize_Prototype(const _tchar * pShaderFilePath);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin();

private:
	ID3DX11Effect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

public:
	static CShader* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END