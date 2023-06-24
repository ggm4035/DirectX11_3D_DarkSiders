#pragma once

#include "IMGUI_Defines.h"
#include "CGameObject3D.h"
#include "CRenderer.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer;
class CModel;
END

BEGIN(Imgui)

class CDummyObject3D final : public CGameObject3D
{
private:
    explicit CDummyObject3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CDummyObject3D(const CDummyObject3D& rhs);
    virtual ~CDummyObject3D() = default;

public:
    unordered_map<wstring, CComponent*>* Get_Components() { return &m_Components; }
    CTransform* Get_Transform() { return m_pTransformCom; }
    CVIBuffer* Get_Buffer() { return m_pBufferCom; }
    CShader* Get_Shader() { return m_pShaderCom; }
    CTexture* Get_Texture() { return m_pTextureCom; }
    CRenderer* Get_Renderer() { return m_pRenderer; }
    CModel* Get_Model() { return m_pModelCom; }
    CRenderer::RENDERGROUP Get_RenderGroup() { return m_eRenderGroup; }
    void Updata_Toggle() { m_isUpdate = m_isUpdate ? false : true; }
    MODEL_BINARYDATA Get_Model_BinaryData();

public:
    virtual HRESULT Initialize_Prototype() override;
    /* Transform 정보 초기화 하고 싶으면 void*에 값 채우기 */
    virtual HRESULT Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Late_Tick(const _double& TimeDelta) override;
    virtual HRESULT Render() override;

public:
    _uint m_iPassNum = { 0 };
    _float m_fDetail = { 1.f };
    ID3D11RasterizerState* m_pRasterizer = { nullptr };
    CRenderer::RENDERGROUP m_eRenderGroup = { CRenderer::RENDER_PRIORITY };

public:
    HRESULT Add_Texture(const wstring PrototypeTag);
    HRESULT Add_Shader(const wstring PrototypeTag);
    HRESULT Add_Buffer(const wstring PrototypeTag);
    HRESULT Add_Model(const wstring PrototypeTag);

public:
    void Set_RasterizerState(const D3D11_RASTERIZER_DESC& RasterizerDesc);

private:
    CRenderer* m_pRenderer = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer* m_pBufferCom = { nullptr };
    CModel* m_pModelCom = { nullptr };

private:
    _bool m_isUpdate = { true };

private:
    virtual HRESULT Add_Components() override;
    HRESULT Set_Shader_Resources();

public:
    static CDummyObject3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject3D* Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg);
    virtual void Free() override;
};

END
