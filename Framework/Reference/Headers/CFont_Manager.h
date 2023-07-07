#pragma once

#include "CBase.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager)

private:
	explicit CFont_Manager() = default;
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const wstring& wstrFontTag, const wstring& wstrFontFilePath);

	HRESULT Render_Font(const wstring& wstrFontTag, const wstring& wstrText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale);

private:
	unordered_map<wstring, class CFont*> m_umapFonts;

private:
	class CFont* Find_Font(const wstring& wstrFontTag);

public:
	virtual void Free() override;
};

END