#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	explicit CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(wstring pTextureFilePath, const _uint& iNumTextures);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent * pOwner, void* pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, string Typename, const _uint & iTextureIndex = 0);
	HRESULT Bind_ShaderResources(class CShader* pShader, string Typename);

private:
	/* ID3D11ShaderResourceView*: 쉐이더에 전달될 수 있는 텍스쳐 타입. */
	vector<ID3D11ShaderResourceView*>	m_Textures;
	_uint								m_iNumTextures = { 0 };

public:
	static CTexture* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, wstring TextureFilePath, const _uint & iNumTextures = 1);
	virtual CComponent* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END