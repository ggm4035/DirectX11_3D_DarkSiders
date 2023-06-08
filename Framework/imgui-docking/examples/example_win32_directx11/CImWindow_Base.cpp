#include "CImWindow_Base.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"

#include "CImWindow_Inspector.h"
#include "CImWindow_Create.h"

#include "CDummyObject3D.h"
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
    m_pInspectorWindow = dynamic_cast<CImWindow_Inspector*>(WINDOWMGR->Get_ImWindow(L"ImWindow_Inspector"));

    if (nullptr == m_pCreateWindow)
        return E_FAIL;

    if (nullptr == m_pInspectorWindow)
        return E_FAIL;

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
        if (ImGui::BeginTabItem("HeightMap"))
        {
            HeightMap();
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
        _uint i = 0;

        for (auto& iter : m_GameObjectList)
        {
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            pObjectTag = pGameInstance->wstrToStr(iter->Get_Tag());

            if (pObjectTag == m_strCurNode)
                node_flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, pObjectTag.c_str());
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                wstring wstrTag = pGameInstance->strToWStr(pObjectTag);
                m_pInspectorWindow->Bind_GameObject(wstrTag);
                m_strCurNode = pObjectTag;
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

        if (ImGui::Button("Delete"))
        {
            pGameInstance->Remove_GameObject(pGameInstance->strToWStr(pObjectTag));
            m_pInspectorWindow->m_pCurGameObject = nullptr;
            WINDOWMGR->Refresh_All_Window();
        }
        Safe_Release(pGameInstance);
        ImGui::TreePop();
    }
}

void CImWindow_Base::HeightMap()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (nullptr == m_pTerrain)
    {
        if (ImGui::BeginPopupContextItem("Terrain"))
        {
            _uint iNumTerrains = m_GameObjectList.size();

            auto iter = m_GameObjectList.begin();

            string* pstrTags = new string[iNumTerrains];
            
            _uint iIndex = 0;
            for (auto iter = m_GameObjectList.begin(); iter != m_GameObjectList.end(); ++iter)
            {
                pstrTags[iIndex] = pGameInstance->wstrToStr((*iter)->Get_Tag());

                if (string::npos != pstrTags[iIndex].find("Terrain"))
                    ++iIndex;
            }

            const _char** ppiTems = new const _char * [iIndex];

            for (_uint j = 0; j < iIndex; ++j)
                ppiTems[j] = pstrTags[j].c_str();

            static int iPick = 0;
            ImGui::ListBox("Terrain", &iPick, ppiTems, iIndex);

            if (ImGui::Button("Select"))
            {
                if (iPick < 0 || iPick >= iIndex)
                {
                    Safe_Release(pGameInstance);
                    return;
                }
                m_pTerrain = dynamic_cast<CDummyObject3D*>(Find_GameObject(pGameInstance->strToWStr(ppiTems[iPick])));
                ImGui::CloseCurrentPopup();
            }

            Safe_Delete_Array(pstrTags);
            Safe_Delete_Array(ppiTems);

            ImGui::EndPopup();
        }

        if (ImGui::Button("Select Terrain"))
            ImGui::OpenPopup("Terrain");

        Safe_Release(pGameInstance);
        return;
    }

    if (ImGui::Button("Reset All"))
        m_pTerrain = nullptr;

    Safe_Release(pGameInstance);
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

void CImWindow_Base::Create_3D_Object()
{
    m_pCreateWindow->m_bIsOpen = true;
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
