#include "CImWindow_Bottom.h"

CImWindow_Bottom::CImWindow_Bottom()
{
}

HRESULT CImWindow_Bottom::Initialize(void* pArg)
{
    return S_OK;
}

void CImWindow_Bottom::Tick(_double TimeDelta)
{
    ImGui::SetNextWindowPos(ImVec2(0, 760), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1600, 270), ImGuiCond_Always);
    ImGui::Begin("Bottom", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::End();
}

CImWindow_Bottom* CImWindow_Bottom::Create(void* pArg)
{
    CImWindow_Bottom* pInstance = new CImWindow_Bottom();

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CImWindow_Bottom");
    }

    return pInstance;
}

void CImWindow_Bottom::Free()
{
    CImWindow::Free();
}
