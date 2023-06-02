#include "CImWindow_Base.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"

#include "CImWindow_Inspector.h"
#include "CImWindow_Create.h"

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
    if (FAILED(CImWindow::Initialize(pArg)))
        return E_FAIL;

    m_pCreateWindow = dynamic_cast<CImWindow_Create*>(WINDOWMGR->Get_ImWindow(L"ImWindow_Create"));

    return S_OK;
}

void CImWindow_Base::Tick(const _double& TimeDelta)
{
    ImVec2 pos = ImGui::GetWindowPos();
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
        CGameInstance* pGameInstance = CGameInstance::GetInstance();
        Safe_AddRef(pGameInstance);

        string pObjectTag = "";
        _uint iID = 0;

        for (auto& iter : m_GameObjectList)
        {
            string strTag = pGameInstance->wstrToStr(iter->Get_Tag());
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            ImGui::TreeNodeEx((void*)iID, node_flags, strTag.c_str());
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                CImWindow* pInspector = WINDOWMGR->Get_ImWindow(L"ImWindow_Inspector");
                if (nullptr == pInspector)
                    return;

                wstring wstrTag = pGameInstance->strToWStr(strTag);
                static_cast<CImWindow_Inspector*>(pInspector)->Bind_GameObject(wstrTag);
            }
        }

        if (ImGui::BeginPopupContextItem("Create"))
        {
            if (ImGui::Selectable("3D Object"))
            {
                Create_3D_Object();
            }
            if (ImGui::Selectable("UI"))
            {

            }
            if (ImGui::Selectable("Camera"))
            {

            }
            ImGui::EndPopup();
        }

        if (ImGui::Button("Create new Object"))
            ImGui::OpenPopup("Create");

        Safe_Release(pGameInstance);
        ImGui::TreePop();
    }
}
// 파일 여는 코드임
//if (ImGui::Button("Open File Dialog"))
//ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp", ".");
//
//// display
//if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
//{
//    // action if OK
//    if (ImGuiFileDialog::Instance()->IsOk())
//    {
//        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
//        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
//        // action
//    }
//
//    // close
//    ImGuiFileDialog::Instance()->Close();
//}

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

void CImWindow_Base::Create_3D_Object()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    m_pCreateWindow->m_bIsOpen = true;
    /*if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
        L"Default_3D", L"Layer_Tool")))
        return;*/

    //WINDOWMGR->Refresh_All_Window();

    Safe_Release(pGameInstance);
}

void CImWindow_Base::Refresh()
{
    CImWindow::Refresh();
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
