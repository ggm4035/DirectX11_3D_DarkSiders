#pragma once

#include "CGameObjectUI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;

class CHealth;
class CAttack;
class CDeffence;
END

BEGIN(Client)

class CInven final : public CGameObjectUI
{
public:
	typedef struct tagInvecDesc
	{
		class CHealth* pHealth;
		class CAttack* pAttack;
		class CDeffence* pDeffence;
	}INVENDESC;

private:
	explicit CInven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInven(const CInven& rhs);
	virtual ~CInven() = default;

public:
	class CCurrency* Get_Currency() {
		return m_pCurrency;
	}
	void ToggleSwitch() {
		m_isEnable = !m_isEnable;
		m_isPopEnd = false;
	}

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override {}
	virtual HRESULT Render();

private:
	_bool m_isEnable = { false };
	_bool m_isPopEnd = { true };
	_float m_vDestX = { 0.f };

private:
	class CCurrency* m_pCurrency = { nullptr };

	CHealth* m_pPlayerHealth = { nullptr };
	CAttack* m_pPlayerAttack = { nullptr };
	CDeffence* m_pPlayerDeffence = { nullptr };

	class CUI_Rect* m_pPlayerIcon = { nullptr };
	class CUI_Rect* m_pCharBox = { nullptr };
	class CUI_Rect* m_pAttackDS = { nullptr };
	class CUI_Rect* m_pDeffenceDS = { nullptr };
	class CUI_Rect* m_pHealthDS = { nullptr };

	class CSlot* m_pMajor = { nullptr };
	class CSlot* m_pMinors[3] = { nullptr };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components();
	HRESULT Set_ShaderResources();
	
	void Tick_UI(const _double& TimeDelta);
	void Activation(const _double& TimeDelta);

public:
	static CInven* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CInven* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END