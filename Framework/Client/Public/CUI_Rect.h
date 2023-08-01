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

class CUI_Rect final : public CGameObjectUI
{
public:
	typedef struct tagUIRectDesc : public CGameObjectUI::UIDESC
	{
		const _int* pMaxHp = { nullptr };
		const _int* pHp = { nullptr };
		const _float* pCoolTime = { nullptr };
		_uint iTextureLevelIndex = { 0 };
		wstring wstrTextureTag = { L"" };
		_uint iPassNum = { 2 };
	}UIRECTDESC;

protected:
	explicit CUI_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Rect(const CUI_Rect& rhs);
	virtual ~CUI_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fTimeAcc = { 0.f };
	_uint m_iPassNum = { 0 };
	const _int* m_pMaxHp = { nullptr };
	const _int* m_pHp = { nullptr };
	const _float* m_pCoolTime = { nullptr };

private:
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CUI_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObjectUI* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END