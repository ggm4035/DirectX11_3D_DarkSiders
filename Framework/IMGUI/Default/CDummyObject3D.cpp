#include "CDummyObject3D.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CModel.h"

CDummyObject3D::CDummyObject3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject3D(pDevice, pContext)
{
}

CDummyObject3D::CDummyObject3D(const CDummyObject3D& rhs)
    :CGameObject3D(rhs)
{
}

MODEL_BINARYDATA CDummyObject3D::Get_Model_BinaryData()
{
    vector<MODEL_BINARYDATA>* pModels = TOOL->m_pAnimModelDatas;

    MODEL_BINARYDATA retData;
    ZeroMemory(&retData, sizeof(MODEL_BINARYDATA));
    
    for (auto& Data : *pModels)
    {
        wstring wstrTag = { L"Model_" };
        wstrTag += Data.szTag;

        if (wstrTag == m_pModelCom->Get_Tag())
        {
            retData = Data;
            lstrcpy(retData.szTag, wstrTag.c_str());
            return retData;
        }
    }

    pModels = TOOL->m_pModelDatas;
    ZeroMemory(&retData, sizeof(MODEL_BINARYDATA));

    for (auto& Data : *pModels)
    {
        wstring wstrTag = { L"Model_" };
        wstrTag += Data.szTag;

        if (wstrTag == m_pModelCom->Get_Tag())
        {
            retData = Data;
            lstrcpy(retData.szTag, wstrTag.c_str());
            return retData;
        }
    }

    return MODEL_BINARYDATA();
}

HRESULT CDummyObject3D::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDummyObject3D::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
    if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;

}

void CDummyObject3D::Tick(const _double& TimeDelta)
{
    if (false == m_isUpdate)
        return;

    if (nullptr != m_pModelCom &&
        CModel::TYPE_ANIM == m_pModelCom->Get_Type())
    {
        m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta);

        m_pModelCom->Play_Animation(TimeDelta);
    }

    if (nullptr != m_pColliderCom)
        m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CDummyObject3D::Late_Tick(const _double& TimeDelta)
{
    if (nullptr != m_pRenderer)
        m_pRenderer->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CDummyObject3D::Render()
{
    if (false == m_isUpdate)
        return S_OK;

    if (FAILED(Set_Shader_Resources()))
        return E_FAIL;

    if (nullptr != m_pModelCom)
    {
        _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (_uint i = 0; i < iNumMeshes; ++i)
        {
            if(m_pModelCom->Get_Type() == CModel::TYPE_ANIM)
                m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

            m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TYPE_DIFFUSE);

            if (nullptr != m_pShaderCom)
                m_pShaderCom->Begin(m_iPassNum);

            m_pModelCom->Render(i);
        }
    }
    else
    {
        if (nullptr != m_pShaderCom)
            m_pShaderCom->Begin(m_iPassNum);

        if (nullptr != m_pBufferCom)
            m_pBufferCom->Render();
    }

    if (nullptr != m_pColliderCom)
        m_pColliderCom->Render();


    return S_OK;
}

HRESULT CDummyObject3D::Add_Texture(const wstring PrototypeTag)
{
    if (0 == PrototypeTag.size())
        return S_OK;

    if (FAILED(Add_Component(LEVEL_TOOL, PrototypeTag, L"Com_Texture",
        (CComponent**)&m_pTextureCom, this)))
        return E_FAIL;

    m_pTextureCom->Set_Tag(PrototypeTag);

    return S_OK;
}

HRESULT CDummyObject3D::Add_Shader(const wstring PrototypeTag)
{
    if (0 == PrototypeTag.size())
        return S_OK;

    if (FAILED(Add_Component(LEVEL_TOOL, PrototypeTag, L"Com_Shader",
        (CComponent**)&m_pShaderCom, this)))
        return E_FAIL;

    m_pShaderCom->Set_Tag(PrototypeTag);

    return S_OK;
}

HRESULT CDummyObject3D::Add_Buffer(const wstring PrototypeTag)
{
    if (0 == PrototypeTag.size())
        return S_OK;

    if (FAILED(Add_Component(LEVEL_TOOL, PrototypeTag, L"Com_Buffer",
        (CComponent**)&m_pBufferCom, this)))
        return E_FAIL;

    m_pBufferCom->Set_Tag(PrototypeTag);

    return S_OK;
}

HRESULT CDummyObject3D::Add_Model(const wstring PrototypeTag)
{
    if (0 == PrototypeTag.size())
        return S_OK;

    if (FAILED(Add_Component(LEVEL_TOOL, PrototypeTag, L"Com_Model",
        (CComponent**)&m_pModelCom, this)))
        return E_FAIL;

    m_pModelCom->Set_Tag(PrototypeTag);

    if (PrototypeTag == L"Model_Warrior")
    {
        /* Collider */
        CBounding_AABB::AABBDESC AABBDesc;
        AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
        AABBDesc.vPosition = _float3(0.f, 1.f, 0.f);

        if (FAILED(Add_Component(LEVEL_TOOL, L"Collider_AABB", L"Com_Collider_AABB",
            (CComponent**)&m_pColliderCom, this, &AABBDesc)))
            return E_FAIL;
    }

    return S_OK;
}

void CDummyObject3D::Set_RasterizerState(const D3D11_RASTERIZER_DESC& RasterizerDesc)
{
    Safe_Release(m_pRasterizer);

    m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizer);
}

HRESULT CDummyObject3D::Add_Components()
{
    if(FAILED(Add_Component(LEVEL_TOOL, L"Renderer",
        L"Com_Renderer", (CComponent**)&m_pRenderer, this)))
        return E_FAIL;

    if(FAILED(Add_Component(LEVEL_TOOL, L"Navigation",
        L"Com_Navigation", (CComponent**)&m_pNavigationCom, this)))
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

        if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ))))
            return E_FAIL;

        if (nullptr != m_pTextureCom)
        {
            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
                return E_FAIL;
        }

        if (m_pShaderCom->Get_Tag() == L"Shader_VtxNorTex" ||
            m_pShaderCom->Get_Tag() == L"Shader_AnimMesh" ||
            m_pShaderCom->Get_Tag() == L"Shader_Mesh")
        {
            CLight::LIGHTDESC LightDesc = *pGameInstance->Get_LightDesc(0);
            if (FAILED(m_pShaderCom->Bind_RawValue("g_LightDirection",
                &LightDesc.vDirection, sizeof(_float4))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_LightDiffuse",
                &LightDesc.vDiffuse, sizeof(_float4))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_LightSpecular",
                &LightDesc.vSpecular, sizeof(_float4))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_LightAmbient",
                &LightDesc.vAmbient, sizeof(_float4))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_CameraPosition",
                &pGameInstance->Get_Camera_Position(), sizeof(_float4))))
                return E_FAIL;
        }

        if(nullptr != dynamic_cast<CVIBuffer_Terrain*>(m_pBufferCom))
            m_pNavigationCom->Render_Navigation();

    }
    Safe_Release(pGameInstance);

    return S_OK;
}

CDummyObject3D* CDummyObject3D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDummyObject3D* pInstance = New CDummyObject3D(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CDummyObject3D");
    }

    return pInstance;
}

CGameObject3D* CDummyObject3D::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
    CDummyObject3D* pInstance = New CDummyObject3D(*this);

    if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned CDummyObject3D");
    }

    return pInstance;
}

void CDummyObject3D::Free()
{
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pRasterizer);
    Safe_Release(m_pRenderer);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pBufferCom);
    CGameObject3D::Free();
}
