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

HRESULT CTexture::Initialize_Prototype(wstring TextureFilePath, const _uint& iNumTextures)
{
	_tchar szTextureFilePath[MAX_PATH] = TEXT("");

	m_iNumTextures = iNumTextures;

	m_Textures.reserve(iNumTextures);

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };

		wsprintf(szTextureFilePath, TextureFilePath.c_str(), i);

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

HRESULT CTexture::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, string Typename, const _uint& iTextureIndex)
{
	if (nullptr == pShader || iTextureIndex < 0 || iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_ShaderResource(Typename, m_Textures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, string Typename)
{
	if (nullptr == pShader)
		return E_FAIL;

	return pShader->Bind_ShaderResources(Typename, m_Textures.data(), m_iNumTextures);
}

const _float2 CTexture::Get_TextureSize(_uint iTextureIndex)
{
	_float2 vSize = _float2(0.f, 0.f);

	// Index except
	if (iTextureIndex >= m_Textures.size())
		return vSize;

	ID3D11Resource* pResource = { nullptr };

	m_Textures[iTextureIndex]->GetResource(&pResource);

	// Resource except
	if (nullptr == pResource)
		return vSize;

	ID3D11Texture2D* pTexture2D = { nullptr };

	// HRESULT except
	if (FAILED(pResource->QueryInterface(&pTexture2D)))
		return vSize;

	// NULL except
	if (nullptr == pTexture2D)
		return vSize;

	D3D11_TEXTURE2D_DESC TexDesc;
	pTexture2D->GetDesc(&TexDesc);

	vSize.x = static_cast<_float>(TexDesc.Width);
	vSize.y = static_cast<_float>(TexDesc.Height);

	// Get할때 내부적으로 레퍼런스 카운트 증가하여 삭제.
	Safe_Release(pTexture2D);
	Safe_Release(pResource);

	return vSize;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring TextureFilePath, const _uint& iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(TextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
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
