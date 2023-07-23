#include "CDummyUI.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CModel.h"

CDummyUI::CDummyUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObjectUI(pDevice, pContext)
{
}

CDummyUI::CDummyUI(const CDummyUI& rhs)
    : CGameObjectUI(rhs)
{
}

HRESULT CDummyUI::Add_Texture(const wstring PrototypeTag)
{
    if (FAILED(Add_Component(LEVEL_TOOL, PrototypeTag, L"Com_Texture",
        (CComponent**)&m_pTextureCom, this)))
        return E_FAIL;

    return S_OK;
}

void CDummyUI::Set_RasterizerState(const D3D11_RASTERIZER_DESC& RasterizerDesc)
{
    Safe_Release(m_pRasterizer);

    m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizer);
}

HRESULT CDummyUI::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
    if (FAILED(CGameObjectUI::Initialize(iLevelIndex, pOwner, pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;

}

void CDummyUI::Tick(const _double& TimeDelta)
{
    if (false == m_isUpdate)
        return;
}

void CDummyUI::Late_Tick(const _double& TimeDelta)
{
    if (nullptr != m_pRenderer)
        m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CDummyUI::Render()
{
    if (false == m_isUpdate)
        return S_OK;

    if (FAILED(Set_Shader_Resources()))
        return E_FAIL;

    if (nullptr != m_pShaderCom)
        m_pShaderCom->Begin(m_iPassNum);

    if (nullptr != m_pBufferCom)
        m_pBufferCom->Render();

    return S_OK;
}

HRESULT CDummyUI::Add_Components()
{
    if (FAILED(Add_Component(LEVEL_TOOL, L"Renderer",
        L"Com_Renderer", (CComponent**)&m_pRenderer, this)))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_TOOL, L"Shader_VtxTex",
        L"Com_Shader", (CComponent**)&m_pShaderCom, this)))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_TOOL, L"VIBuffer_Rect",
        L"Com_Buffer", (CComponent**)&m_pBufferCom, this)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDummyUI::Set_Shader_Resources()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (nullptr != m_pShaderCom)
    {
        if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &m_WorldMatrix)))
            return E_FAIL;

        _float4x4 InputMatrix = pGameInstance->Get_UI_View_Float4x4();
        if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &InputMatrix)))
            return E_FAIL;

        InputMatrix = pGameInstance->Get_UI_Proj_Float4x4(g_iWinSizeX, g_iWinSizeY);
        if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &InputMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fDetail", &m_fDetail, sizeof(_float))))
            return E_FAIL;

        if (nullptr != m_pTextureCom)
        {
            if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
                return E_FAIL;
        }
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

CDummyUI* CDummyUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDummyUI* pInstance = New CDummyUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CDummyUI");
    }

    return pInstance;
}

CDummyUI* CDummyUI::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
    CDummyUI* pInstance = New CDummyUI(*this);

    if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned CDummyUI");
    }

    return pInstance;
}

void CDummyUI::Free()
{
    Safe_Release(m_pRasterizer);
    Safe_Release(m_pRenderer);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pBufferCom);
    CGameObjectUI::Free();
}
