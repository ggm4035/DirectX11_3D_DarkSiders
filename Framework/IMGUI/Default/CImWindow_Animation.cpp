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
    m_iMaxKeyFrames = m_pModelCom->Get_MaxNumKeyFrame();
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
    //Demo();

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

    if (ImGui::SliderInt("Animation Frame", &m_iCurrentFrame, 0, m_iMaxKeyFrames - 2))
    {
        m_pModelCom->Set_KeyFrame(m_iCurrentFrame);
    }

    if (ImGui::SliderInt("Animation Root Frame", &m_iCurrentRootFrame, 0, m_iMaxRootKeyFrames - 2))
    {
        m_pModelCom->Set_RootKeyFrame(m_iCurrentRootFrame);
    }
}

void CImWindow_Animation::Demo()
{
    if (ImGui::TreeNode("Plotting"))
    {
        static bool animate = !m_pModelCom->isPause();

        // Plot as lines and plot as histogram
        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));
        ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

        // Fill an array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float
        // and the sizeof() of your structure in the "stride" parameter.
        static float values[90] = {};
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (!animate || refresh_time == 0.0)
            refresh_time = ImGui::GetTime();
        while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
        {
            static float phase = 0.0f;
            values[values_offset] = cosf(phase);
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            phase += 0.10f * values_offset;
            refresh_time += 1.0f / 60.0f;
        }

        // Plots can display overlay texts
        // (in this example, we will display an average value)
        {
            float average = 0.0f;
            for (int n = 0; n < IM_ARRAYSIZE(values); n++)
                average += values[n];
            average /= (float)IM_ARRAYSIZE(values);
            char overlay[32];
            sprintf(overlay, "avg %f", average);
            ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        }

        // Use functions to generate output
        // FIXME: This is actually VERY awkward because current plot API only pass in indices.
        // We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
        };
        static int func_type = 0, display_count = 70;
        ImGui::SeparatorText("Functions");
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
        ImGui::Combo("func", &func_type, "Sin\0Saw\0");
        ImGui::SameLine();
        ImGui::SliderInt("Sample count", &display_count, 1, 400);
        float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        ImGui::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        ImGui::Separator();

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        if (animate)
        {
            progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        }

        // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
        // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");

        ImGui::ProgressBar(progress, ImVec2(0.f, 0.f));
        ImGui::TreePop();
    }
}

void CImWindow_Animation::Free()
{
    CImWindow::Free();
}
