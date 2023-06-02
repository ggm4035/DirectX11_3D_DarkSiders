#include "CImWindow_Create.h"

CImWindow_Create::CImWindow_Create()
{
}

HRESULT CImWindow_Create::Initialize(void* pArg)
{
    if (FAILED(CImWindow::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CImWindow_Create::Tick(const _double& TimeDelta)
{
    if (false == m_bIsOpen)
        return;

    ImGui::Begin("Create");

    Create_Object();

    ImGui::End();
}

void CImWindow_Create::Refresh()
{
}

void CImWindow_Create::Create_Object()
{
    if (ImGui::Button("Create"))
        m_bIsOpen = false;
}

CImWindow_Create* CImWindow_Create::Create(void* pArg)
{
    CImWindow_Create* pInstance = new CImWindow_Create();

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CImWindow_Create");
    }

    return pInstance;
}

void CImWindow_Create::Free()
{
    CImWindow::Free();
}
