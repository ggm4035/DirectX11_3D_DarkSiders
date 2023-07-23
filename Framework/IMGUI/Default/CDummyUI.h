#pragma once

#include "IMGUI_Defines.h"
#include "CGameObjectUI.h"
#include "CRenderer.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer;
class CNavigation;
class CCollider;
class CModel;
END

BEGIN(Imgui)

class CDummyUI final : public CGameObjectUI
{
private:
    explicit CDummyUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CDummyUI(const CDummyUI& rhs);
    virtual ~CDummyUI() = default;

public:
    HRESULT Add_Texture(const wstring PrototypeTag);

public:
    void Set_RasterizerState(const D3D11_RASTERIZER_DESC& RasterizerDesc);
    unordered_map<wstring, CComponent*>* Get_Components() {
        return &m_umapComponents;
    }
    CVIBuffer_Rect* Get_Buffer() {
        return m_pBufferCom;
    }
    CShader* Get_Shader() {
        return m_pShaderCom;
    }
    CTexture* Get_Texture() {
        return m_pTextureCom;
    }
    CRenderer* Get_Renderer() {
        return m_pRenderer;
    }
    void Updata_Toggle() {
        m_isUpdate = m_isUpdate ? false : true;
    }
    void Set_PassNum(const _uint& iPassIndex) {
        m_iPassNum = iPassIndex;
    }

public:
    virtual HRESULT Initialize_Prototype() override { return S_OK; }
    virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Late_Tick(const _double& TimeDelta) override;
    virtual HRESULT Render() override;

private:
    CRenderer* m_pRenderer = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Rect* m_pBufferCom = { nullptr };

    ID3D11RasterizerState* m_pRasterizer = { nullptr };

private:
    _bool m_isUpdate = { true };
    _uint m_iPassNum = { 0 };
    _float m_fDetail = { 1.f };

private:
    virtual HRESULT Add_Components() override;
    HRESULT Set_Shader_Resources();

public:
    static CDummyUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CDummyUI* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
    virtual void Free() override;
};

END
