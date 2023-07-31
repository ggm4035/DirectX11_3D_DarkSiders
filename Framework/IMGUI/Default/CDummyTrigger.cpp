#include "CDummyTrigger.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CModel.h"

CDummyTrigger::CDummyTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CTrigger(pDevice, pContext)
{
}

CDummyTrigger::CDummyTrigger(const CDummyTrigger& rhs)
    : CTrigger(rhs)
{
}

HRESULT CDummyTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDummyTrigger::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
    if (FAILED(CTrigger::Initialize(iLevelIndex, pOwner, pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CDummyTrigger::Tick(const _double& TimeDelta)
{
    if (false == m_isUpdate)
        return;

    CTrigger::Tick(TimeDelta);

    Add_RenderDebug();
}

void CDummyTrigger::Late_Tick(const _double& TimeDelta)
{
    CTrigger::Late_Tick(TimeDelta);
}

HRESULT CDummyTrigger::Render()
{
    if (false == m_isUpdate)
        return S_OK;

    return S_OK;
}

HRESULT CDummyTrigger::Add_Components()
{
    CBounding_AABB::AABBDESC Desc;
    Desc.vExtents = m_TriggerDesc.vExtents;
    Desc.eGroup = CCollider::COL_TRIGGER;
    Desc.vPosition = _float3(0.f, 0.f, 0.f);
    Desc.vOffset = _float3(0.f, m_TriggerDesc.vExtents.y * 0.5f, 0.f);

    if (FAILED(Add_Component(LEVEL_TOOL, L"Collider_AABB", L"Col_Trigger",
        (CComponent**)&m_pColliderCom, this, &Desc)))
        return E_FAIL;

    if(FAILED(Add_Component(LEVEL_TOOL, L"Renderer", L"Com_Renderer",
        (CComponent**)&m_pRenderer, this)))
        return E_FAIL;
    return S_OK;
}

CDummyTrigger* CDummyTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDummyTrigger* pInstance = New CDummyTrigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CDummyTrigger");
    }

    return pInstance;
}

CDummyTrigger* CDummyTrigger::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
    CDummyTrigger* pInstance = New CDummyTrigger(*this);

    if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned CDummyTrigger");
    }

    return pInstance;
}

void CDummyTrigger::Free()
{
    CTrigger::Free();
}
