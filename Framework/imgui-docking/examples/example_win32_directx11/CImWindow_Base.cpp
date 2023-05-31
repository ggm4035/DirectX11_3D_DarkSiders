#include "CImWindow_Base.h"

#include "CToolInstance.h"
#include "CTerrain.h"
#include "CCoordnate_Axis.h"
#include "CFileInfo.h"

CImWindow_Base::CImWindow_Base()
{
    ZeroMemory(&m_vTerrain_Position, sizeof m_vTerrain_Position);
    ZeroMemory(&m_vTerrain_Rotation, sizeof m_vTerrain_Rotation);
}

HRESULT CImWindow_Base::Initialize(void* pArg)
{
    return S_OK;
}

void CImWindow_Base::Tick(_double TimeDelta)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(350, 760), ImGuiCond_Always);
    ImGui::Begin("Hierarchy ", nullptr, ImGuiWindowFlags_NoResize);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Tabs", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Hierarchy"))
        {
            Hierarchy();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Terrain"))
        {
            Tab_Terrain();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void CImWindow_Base::Hierarchy()
{
    if (ImGui::TreeNode("Scene"))
    {
        _char* pObjectTag = "";

        for (_uint i = 0; i < 1/* 이거는 현재 씬의 오브젝트 개수 임시로 1 설정 */; ++i)
        {
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            ImGui::TreeNodeEx((void*)i, node_flags, "Terrain");
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                pObjectTag = "Terrain";
        }
        string str;
        size_t i = str.max_size();
        ImGui::TreePop();
    }
}

void CImWindow_Base::Tab_Terrain()
{
    D3D11_RASTERIZER_DESC RasterizerDesc;
    ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

    RasterizerDesc.CullMode = { D3D11_CULL_BACK };
    RasterizerDesc.FrontCounterClockwise = { false };

    ImGui::Text("RenderState");
    ImGui::SameLine(150.f);
    ImGui::Text("RenderCoordnate");
    static int state = 0, coordnate = 0;
    if (ImGui::RadioButton("WireFrame", &state, 0))
    {
        RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };
        TOOL->m_pTerrain->Reset_Rasterizer(RasterizerDesc);
    }
    ImGui::SameLine(150.f);
    if (ImGui::RadioButton("Render_Coordnate", &coordnate, 0))
        TOOL->m_pCoordnate->RenderOn();

    if (ImGui::RadioButton("Solid", &state, 1))
    {
        RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
        TOOL->m_pTerrain->Reset_Rasterizer(RasterizerDesc);
    }
    ImGui::SameLine(150.f);
    if (ImGui::RadioButton("Render_None", &coordnate, 1))
        TOOL->m_pCoordnate->RenderOff();

    ImGui::Separator();
    ImGui::Text("VertexState");
    ImGui::Separator();

    static _int iXCount = 0, iZCount = 0;
    static _float fInterval = 0.f, fDetail = 0.f;
    ImGui::DragInt("XCount", &iXCount, 1.f, 0, 1025, "%d", ImGuiSliderFlags_None);
    ImGui::DragInt("ZCount", &iZCount, 1.f, 0, 1025, "%d", ImGuiSliderFlags_None);
    ImGui::DragFloat("Interval", &fInterval, 0.1f, 0.f, 100.f, "%.1f", ImGuiSliderFlags_None);
    ImGui::DragFloat("Detail", &fDetail, 0.1f, 0.f, 100.f, "%.1f", ImGuiSliderFlags_None);
    if (ImGui::Button("APPLY", ImVec2(70, 20)))
    {
        TOOL->m_pTerrain->Reset_Data(iXCount, iZCount, fInterval, fDetail);
    }

    ImGui::Separator();
    ImGui::Text("Position");
    ImGui::Separator();
    ImGui::DragFloat("Position_X", &m_vTerrain_Position.x, 0.01f, -10000.f, 10000.f, "%.3f", ImGuiSliderFlags_None);
    ImGui::DragFloat("Position_Y", &m_vTerrain_Position.y, 0.01f, -10000.f, 10000.f, "%.3f", ImGuiSliderFlags_None);
    ImGui::DragFloat("Position_Z", &m_vTerrain_Position.z, 0.01f, -10000.f, 10000.f, "%.3f", ImGuiSliderFlags_None);

    TOOL->m_pTerrain->Set_Position(_float4(m_vTerrain_Position.x, m_vTerrain_Position.y, m_vTerrain_Position.z, 1.f));

    ImGui::Separator();
    ImGui::Text("Rotation");
    ImGui::Separator();
    ImGui::DragFloat("Rotation_X", &m_vTerrain_Rotation.x, 0.01f, -360.f, 360.f, "%.3f", ImGuiSliderFlags_None);
    ImGui::DragFloat("Rotation_Y", &m_vTerrain_Rotation.y, 0.01f, -360.f, 360.f, "%.3f", ImGuiSliderFlags_None);
    ImGui::DragFloat("Rotation_Z", &m_vTerrain_Rotation.z, 0.01f, -360.f, 360.f, "%.3f", ImGuiSliderFlags_None);

    TOOL->m_pTerrain->Set_Rotation(_float3(m_vTerrain_Rotation.x, m_vTerrain_Rotation.y, m_vTerrain_Rotation.z));

    ImGui::Separator();
    ImGui::Text("Texture");
    ImGui::Separator();
}

CImWindow_Base* CImWindow_Base::Create(void* pArg)
{
    CImWindow_Base* pInstance = new CImWindow_Base();

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CImWindow_Base");
    }

    return pInstance;
}

void CImWindow_Base::Free()
{
    CImWindow::Free();
}
