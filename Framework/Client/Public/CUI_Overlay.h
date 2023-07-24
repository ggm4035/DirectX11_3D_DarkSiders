#pragma once

#include "CGameObjectUI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
class CTransform;
END

BEGIN(Client)

class CUI_Overlay final : public CGameObjectUI
{
public:
	typedef struct tagUIRectDesc : public CGameObjectUI::UIDESC
	{
		_float3 vOffset;
		const _float4x4* pParentMatrix = { nullptr };
		_uint iTextureLevelIndex = { 0 };
		wstring wstrTextureTag = { L"" };
	}UIOVERLAYDESC;

protected:
	explicit CUI_Overlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Overlay(const CUI_Overlay& rhs);
	virtual ~CUI_Overlay() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	const _float4x4* m_pParentMatrix = { nullptr };
	_float3 m_vOffset;

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CUI_Overlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObjectUI* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END