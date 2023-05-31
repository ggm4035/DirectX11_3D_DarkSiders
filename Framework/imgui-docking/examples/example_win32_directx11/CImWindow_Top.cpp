#include "CImWindow_Top.h"

CImWindow_Top::CImWindow_Top()
{
}

HRESULT CImWindow_Top::Initialize(void* pArg)
{
    return S_OK;
}

void CImWindow_Top::Tick(_double TimeDelta)
{
    ImGui::SetNextWindowPos(ImVec2(350, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1250, 80), ImGuiCond_Always);
    ImGui::Begin("Top", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::End();
}

CImWindow_Top* CImWindow_Top::Create(void* pArg)
{
    CImWindow_Top* pInstance = new CImWindow_Top();

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CImWindow_Top");
    }

    return pInstance;
}

void CImWindow_Top::Free()
{
    CImWindow::Free();
}
