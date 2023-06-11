#include "CShader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_InputLayouts(rhs.m_InputLayouts)
	, m_iNumPasses(rhs.m_iNumPasses)
{
	for(auto& InputLayout : m_InputLayouts)
		Safe_AddRef(InputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const wstring& ShaderFilePath, 
	const D3D11_INPUT_ELEMENT_DESC* pInputElementsDesc, 
	const _uint& iNumElements)
{
	_uint iHlslFlag = { 0 };
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif
	if (FAILED(D3DX11CompileEffectFromFile(ShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);

	pTechnique->GetDesc(&TechniqueDesc);
	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; ++i)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);
		pPass->GetDesc(&PassDesc);
		HRESULT hr = 0;

		if (FAILED(hr = m_pDevice->CreateInputLayout(pInputElementsDesc, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader::Bind_RawValue(const string& Typename, void* pValue, const _uint& iLength)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(Typename.c_str());
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pValue, 0, iLength);
}

HRESULT CShader::Bind_Float4x4(const string& Typename, const _float4x4* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	/* 해당하는 이름의 전역변수에 해당하는 컴객체를 얻어온다. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(Typename.c_str());
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pVariableMatrix = pVariable->AsMatrix();
	if (nullptr == pVariableMatrix)
		return E_FAIL;

	/* 해당 컴객체로 변수에 값을 던진다. */
	return pVariableMatrix->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Float4x4_Array(const string& Typename, const _float4x4* pMatrix, const _uint& iNumMatrices)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(Typename.c_str());
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pVariableMatrix = pVariable->AsMatrix();
	if (nullptr == pVariableMatrix)
		return E_FAIL;

	return pVariableMatrix->SetMatrixArray((_float*)pMatrix, 0, iNumMatrices);
}

HRESULT CShader::Bind_Rasterizer(const string& Typename, const _uint& _iIndex, ID3D11RasterizerState* _pRasterizer)
{
	if (nullptr == m_pEffect || nullptr == _pRasterizer)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(Typename.c_str());
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectRasterizerVariable* pRasterizer = pVariable->AsRasterizer();
	if (nullptr == pRasterizer)
		return E_FAIL;

	return pRasterizer->SetRasterizerState(_iIndex, _pRasterizer);
}

HRESULT CShader::Bind_ShaderResource(const string& Typename, ID3D11ShaderResourceView* pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(Typename.c_str());
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariableShaderResource = pVariable->AsShaderResource();
	if (nullptr == pVariableShaderResource)
		return E_FAIL;

	return pVariableShaderResource->SetResource(pSRV);
}

HRESULT CShader::Bind_ShaderResources(const string& Typename, ID3D11ShaderResourceView** ppSRV, const _uint& iNumTextures)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(Typename.c_str());
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariableShaderResource = pVariable->AsShaderResource();
	if (nullptr == pVariableShaderResource)
		return E_FAIL;

	return pVariableShaderResource->SetResourceArray(ppSRV, 0, iNumTextures);
}

HRESULT CShader::Begin(const _uint& iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(iPassIndex);
	if (nullptr == pTechnique)
		return E_FAIL;

	pTechnique->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
	const wstring& pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pInputElementsDesc, const _uint& iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pInputElementsDesc, iNumElements)))
	{
		MSG_BOX("Failed to Created CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(CComponent* pOwner, void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	CComponent::Free();

	for (auto& InputLayout : m_InputLayouts)
		Safe_Release(InputLayout);

	Safe_Release(m_pEffect);
}
