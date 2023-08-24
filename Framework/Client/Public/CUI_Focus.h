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

class CUI_Focus final : public CGameObjectUI
{
protected:
	explicit CUI_Focus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Focus(const CUI_Focus& rhs);
	virtual ~CUI_Focus() = default;

public:
	void On_Focus() {
		m_bSwitch = true;
	}
	void Off_Focus() {
		m_bSwitch = false;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bSwitch = { false };

private:
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CUI_Focus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObjectUI* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END