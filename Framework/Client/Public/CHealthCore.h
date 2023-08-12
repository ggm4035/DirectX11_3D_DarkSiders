#pragma once

#include "CGameObjectUI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CHealth;
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CHealthCore final : public CGameObjectUI
{
private:
	explicit CHealthCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHealthCore(const CHealthCore& rhs);
	virtual ~CHealthCore() = default;

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Tick(const _float3& _vPosition);
	virtual void Late_Tick(class CInven* pInven, const _double& TimeDelta);
	virtual HRESULT Render();

public:
	void Upgrade(class CInven* pInven, CHealth* pHealth);
	const _uint& Get_NeedSouls();

private:
	_uint m_inumLevel = { 0 };

	class CUI_Rect* m_pFocus = { nullptr };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextures[4] = { nullptr };
	CTexture* m_pCurrentTexture = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components();
	HRESULT Set_ShaderResources();

public:
	static CHealthCore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CHealthCore* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END