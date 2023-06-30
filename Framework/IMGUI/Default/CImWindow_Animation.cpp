#include "CImWindow_Animation.h"

#include "CGameInstance.h"
#include "CDummyObject3D.h"

IMPLEMENT_SINGLETON(CImWindow_Animation)

void CImWindow_Animation::Set_Model(CDummyObject3D* pModel)
{
    m_pModel = pModel;

    if (nullptr == pModel)
        return;

    m_pModelCom = pModel->Get_Model();

    Refresh_Animation();
}

void CImWindow_Animation::Refresh_Animation()
{
    m_iMaxKeyFrames = m_pModelCom->Get_MaxKeyFrame();
    m_iMaxRootKeyFrames = m_pModelCom->Get_MaxRootKeyFrame();
    m_iCurrentFrame = 0;
    m_iCurrentRootFrame = 0;
}

HRESULT CImWindow_Animation::Initialize()
{
    if (FAILED(CImWindow::Initialize()))
        return E_FAIL;

    return S_OK;
}

void CImWindow_Animation::Tick(const _double& TimeDelta)
{
    if (false == m_bIsOpen)
        return;

	if (nullptr == m_pModel)
		return;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImVec2 pos = ImGui::GetWindowPos();
    pos.x -= 407;
    pos.y += 600;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(2000, 300), ImGuiCond_Always);
    ImGui::Begin("Animation##ImWindow", nullptr, ImGuiWindowFlags_NoResize);

    Show_Animation_KeyFrame(pGameInstance);

    ImGui::End();

    Safe_Release(pGameInstance);
}

void CImWindow_Animation::Refresh()
{
    CImWindow::Refresh();
}

void CImWindow_Animation::Show_Animation_KeyFrame(CGameInstance* pGameInstance)
{
    ImTextureID playTexture;

	ImGui::SameLine(900);
    if (ImGui::Button("Play", ImVec2(50, 50)))
        m_pModelCom->RePlay_Animation();
        ImGui::SameLine();
    if (ImGui::Button("Pause", ImVec2(50, 50)))
        m_pModelCom->Pause_Animation();
    ImGui::SameLine();
    if (ImGui::Button("Stop", ImVec2(50, 50)))
        ;

    m_iCurrentFrame = m_pModelCom->Get_CurrentKeyFrameIndex();
    m_iCurrentRootFrame = m_pModelCom->Get_CurrentRootKeyFrameIndex();

    if (ImGui::SliderInt("Animation Frame", &m_iCurrentFrame, 0, m_iMaxKeyFrames - 1))
    {
        m_pModelCom->Set_KeyFrame(m_iCurrentFrame);
    }

    if (ImGui::SliderInt("Animation Root Frame", &m_iCurrentRootFrame, 0, m_iMaxRootKeyFrames - 1))
    {
        m_pModelCom->Set_RootKeyFrame(m_iCurrentRootFrame);
    }
}

void CImWindow_Animation::Free()
{
    CImWindow::Free();
}
