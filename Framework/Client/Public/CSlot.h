#pragma once

#include "CGameObjectUI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSlot final : public CGameObjectUI
{
public:
	typedef struct tagSlotDesc : public UIDESC
	{
		wstring wstrTextureTag = { L"" };
	}SLOTDESC;

private:
	explicit CSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSlot(const CSlot& rhs);
	virtual ~CSlot() = default;
	
public:
	class CItem* Get_Item();
	_bool Set_Item(class CItem* pItem);

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Tick(const _float3& _vPosition);
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render();

private:
	class CItem* m_pItem = { nullptr };
	class CUI_Rect* m_pFocus = { nullptr };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components();
	HRESULT Set_ShaderResources();

public:
	static CSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSlot* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END