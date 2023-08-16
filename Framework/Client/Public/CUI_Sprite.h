#pragma once

#include "CGameObjectUI.h"
#include "Client_Defines.h"
#include "CViBuffer_Sprite.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_Sprite final : public CGameObjectUI
{
public:
	typedef struct tagUISpriteDesc : public CGameObjectUI::UIDESC
	{
		CVIBuffer_Sprite::SPRITEDESC SpriteDesc;

		_float3 vOffset;
		const _float4x4* pParentMatrix = { nullptr };
		_uint iTextureLevel = { 0 };
		wstring wstrTextureTag = { L"" };
	}UISPRITEDESC;

protected:
	explicit CUI_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Sprite(const CUI_Sprite& rhs);
	virtual ~CUI_Sprite() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	const _float4x4* m_pParentMatrix = { nullptr };
	_float3 m_vOffset;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Sprite* m_pBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CUI_Sprite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObjectUI* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END