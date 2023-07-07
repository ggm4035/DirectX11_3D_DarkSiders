#include "CFont_Manager.h"

#include "CFont.h"

IMPLEMENT_SINGLETON(CFont_Manager)

HRESULT CFont_Manager::Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrFontTag, const wstring& wstrFontFilePath)
{
	if (nullptr != Find_Font(wstrFontTag))
		return E_FAIL;

	CFont* pFont = CFont::Create(pDevice, pContext, wstrFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_umapFonts.emplace(wstrFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render_Font(const wstring& wstrFontTag, const wstring& wstrText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
	CFont* pFont = Find_Font(wstrFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(wstrText, vPosition, vColor, fRotation, vOrigin, fScale);
}

CFont* CFont_Manager::Find_Font(const wstring& wstrFontTag)
{
	auto iter = m_umapFonts.find(wstrFontTag);

	if (iter == m_umapFonts.end())
		return nullptr;

	return iter->second;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_umapFonts)
		Safe_Release(Pair.second);
}
