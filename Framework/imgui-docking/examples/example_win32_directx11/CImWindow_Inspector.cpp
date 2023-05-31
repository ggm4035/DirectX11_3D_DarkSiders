#include "CImWindow_Inspector.h"

CImWindow_Inspector::CImWindow_Inspector()
{
}

HRESULT CImWindow_Inspector::Initialize(void* pArg)
{
    return S_OK;
}

void CImWindow_Inspector::Tick(_double TimeDelta)
{
    ImGui::SetNextWindowPos(ImVec2(1600, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(320, 1035), ImGuiCond_Always);
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::End();
}

CImWindow_Inspector* CImWindow_Inspector::Create(void* pArg)
{
    CImWindow_Inspector* pInstance = new CImWindow_Inspector();

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CImWindow_Inspector");
    }

    return pInstance;
}

void CImWindow_Inspector::Free()
{
    CImWindow::Free();
}
