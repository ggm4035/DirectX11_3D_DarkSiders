#include "CDummyObject3D.h"

#include "CGameInstance.h"

CDummyObject3D::CDummyObject3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject3D(pDevice, pContext)
{
}

CDummyObject3D::CDummyObject3D(const CDummyObject3D& rhs)
    :CGameObject3D(rhs)
{
}

HRESULT CDummyObject3D::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDummyObject3D::Initialize(CComponent* pOwner, void* pArg)
{
    if (FAILED(CGameObject3D::Initialize(pOwner, pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    D3D11_RASTERIZER_DESC RasterizerDesc;
    ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

    RasterizerDesc.CullMode = { D3D11_CULL_BACK };
    RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
    RasterizerDesc.FrontCounterClockwise = { false };

    m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizer);

    return S_OK;
}

void CDummyObject3D::Tick(const _double& TimeDelta)
{
    if (nullptr != m_pRenderer)
        m_pRenderer->Add_RenderGroup(m_eRenderGroup, this);
}

void CDummyObject3D::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CDummyObject3D::Render()
{
    if (FAILED(Set_Shader_Resources()))
        return E_FAIL;

    if (nullptr != m_pShaderCom)
        m_pShaderCom->Begin(m_iPassNum);

    if (nullptr != m_pBufferCom)
        m_pBufferCom->Render();

    return S_OK;
}

HRESULT CDummyObject3D::Add_Texture(const wstring PrototypeTag)
{
    if (FAILED(Add_Component(LEVEL_TOOL, PrototypeTag, L"Com_Texture", (CComponent**)&m_pTextureCom, this)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDummyObject3D::Add_Shader(const wstring PrototypeTag)
{
    if (FAILED(Add_Component(LEVEL_TOOL, PrototypeTag, L"Com_Shader", (CComponent**)&m_pShaderCom, this)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDummyObject3D::Add_Buffer(const wstring PrototypeTag)
{
    if (FAILED(Add_Component(LEVEL_TOOL, PrototypeTag, L"Com_Buffer", (CComponent**)&m_pBufferCom, this)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDummyObject3D::Add_Components()
{
    if(FAILED(Add_Component(LEVEL_TOOL, L"Prototype_Component_Renderer",
        L"Com_Renderer", (CComponent**)&m_pRenderer, this)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDummyObject3D::Set_Shader_Resources()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (nullptr != m_pShaderCom)
    {
        if (FAILED(m_pShaderCom->Bind_Rasterizer("g_Rasterizer", 0, m_pRasterizer)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ))))
            return E_FAIL;
    }

    if (nullptr != m_pTextureCom)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fDetail", (void*)&m_fDetail, sizeof(_float))))
            return E_FAIL;
    }

    Safe_Release(pGameInstance);
    return S_OK;
}

CDummyObject3D* CDummyObject3D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDummyObject3D* pInstance = new CDummyObject3D(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CDummyObject3D");
    }

    return pInstance;
}

CGameObject3D* CDummyObject3D::Clone(CComponent* pOwner, void* pArg)
{
    CDummyObject3D* pInstance = new CDummyObject3D(*this);

    if (FAILED(pInstance->Initialize(pOwner, pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned CDummyObject3D");
    }

    return pInstance;
}

void CDummyObject3D::Free()
{
    Safe_Release(m_pRasterizer);
    Safe_Release(m_pRenderer);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pBufferCom);
    CGameObject3D::Free();
}
