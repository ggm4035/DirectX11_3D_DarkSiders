#pragma once

#include "CGameObjectUI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CHUD final : public CGameObjectUI
{
private:
	explicit CHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHUD(const CHUD& rhs);
	virtual ~CHUD() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render();

private:
	class CPlayer* m_pPlayer = { nullptr };

	class CUI_Rect* m_pPlayerUI[9] = { nullptr };

private:
	CRenderer* m_pRendererCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;

public:
	static CHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CHUD* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END