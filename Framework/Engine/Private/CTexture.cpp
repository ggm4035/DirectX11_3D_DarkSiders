#include "CTexture.h"

#include "CShader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_Textures(rhs.m_Textures)
{
	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_AddRef(m_Textures[i]);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	_tchar szTextureFilePath[MAX_PATH] = TEXT("");

	m_iNumTextures = iNumTextures;

	m_Textures.reserve(iNumTextures);

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };

		wsprintf(szTextureFilePath, pTextureFilePath, i);

		_tchar			szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, 256);

		HRESULT			hr = { 0 };

		if (false == lstrcmp(szExt, TEXT(".dds")))
		{
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}
		else if (false == lstrcmp(szExt, TEXT(".tga")))
		{
		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.push_back(pSRV);
	}
	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pTypename, _uint iTextureIndex)
{
	if (nullptr == pShader || nullptr == pTypename ||
		iTextureIndex < 0 || iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_ShaderResource(pTypename, m_Textures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pTypename)
{
	if (nullptr == pShader || nullptr == pTypename)
		return E_FAIL;

	return pShader->Bind_ShaderResources(pTypename, m_Textures.data(), m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	CComponent::Free();

	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_Release(m_Textures[i]);

}
