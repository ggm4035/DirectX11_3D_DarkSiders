#pragma once

#include "CGameObjectUI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCursor final : public CGameObjectUI
{
protected:
	explicit CCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCursor(const CCursor& rhs);
	virtual ~CCursor() = default;

public:
	void On() {
		m_isRender = true;
	}
	void Off() {
		m_isRender = false;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_isRender = { true };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CCursor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCursor* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END