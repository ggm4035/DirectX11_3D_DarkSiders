#include "CImWindow_Base.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"

#include "CImWindow_Inspector.h"
#include "CImWindow_Create.h"
#include "CImWindow_Top.h"

#include "CDummyObject3D.h"
#include "CCoordnate_Axis.h"
#include "CFileInfo.h"
#include "CMainCamera.h"

#include "CAnimation.h"

CImWindow_Base::CImWindow_Base()
{
}

HRESULT CImWindow_Base::Initialize(void* pArg)
{
    if (FAILED(CImWindow::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CImWindow_Base::Tick(const _double& TimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImVec2 pos = ImGui::GetWindowPos();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(350, 760), ImGuiCond_Always);
    ImGui::Begin("Hierarchy ", nullptr, ImGuiWindowFlags_NoResize);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Tabs", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Hierarchy"))
        {
            Hierarchy(pGameInstance);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("UI"))
        {
            UI(pGameInstance);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Animation"))
        {
            Animation(pGameInstance);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();

    Safe_Release(pGameInstance);
}

void CImWindow_Base::Hierarchy(CGameInstance* pGameInstance)
{
    if (true == m_isIntoAnimTab)
    {
        for (auto& pObject : m_GameObjectList)
            pObject->Updata_On();

        m_isIntoAnimTab = false;
    }

    if (ImGui::TreeNode("Scene"))
    {
        wstring pObjectTag = L"";

        _uint i = 0;

        if (true == TOOL->m_pTopWindow->isPickingMode())
            Picking_GameObject(pGameInstance);

        for (auto& iter : m_GameObjectList)
        {
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            pObjectTag = iter->Get_Tag();

            if (nullptr != TOOL->m_pCurrentObject && pObjectTag == TOOL->m_pCurrentObject->Get_Tag())
                node_flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, pGameInstance->wstrToStr(pObjectTag).c_str());
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                if (true == TOOL->m_pTopWindow->isPickSolidMode())
                {
                    D3D11_RASTERIZER_DESC RasterizerDesc;
                    ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

                    RasterizerDesc.CullMode = { D3D11_CULL_BACK };
                    RasterizerDesc.FrontCounterClockwise = { false };
                    RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };

                    if (nullptr != TOOL->m_pCurrentObject)
                        TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);

                    RasterizerDesc.FillMode = { D3D11_FILL_SOLID };

                    TOOL->m_pCurrentObject = Find_GameObject(pObjectTag);

                    if (nullptr != TOOL->m_pCurrentObject)
                        TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);
                }
                else
                    TOOL->m_pCurrentObject = Find_GameObject(pObjectTag);
            }
        }

        if (ImGui::BeginPopupContextItem("Create"))
        {
            if (ImGui::Selectable("3D Object"))
            {
                TOOL->m_pCreateWindow->m_bIsOpen = true;
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
            pGameInstance->Remove_GameObject(TOOL->m_pCurrentObject->Get_Tag());
            TOOL->m_pCurrentObject = nullptr;
            WINDOWMGR->Refresh_All_Window();
        }
        ImGui::TreePop();
    }
}

void CImWindow_Base::UI(CGameInstance* pGameInstance)
{
    if (true == m_isIntoAnimTab)
    {
        for (auto& pObject : m_GameObjectList)
            pObject->Updata_On();

        m_isIntoAnimTab = false;
    }

}

void CImWindow_Base::Picking_GameObject(CGameInstance* pGameInstance)
{
    if (pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
    {
        POINT ptMouse;
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        for (auto& pObject : m_GameObjectList)
        {
            if (nullptr == dynamic_cast<CModel*>(pObject->Get_Model()))
                continue;

            _vector vPosition = pGameInstance->Picking_On_Triangle(ptMouse, pObject->Get_Model(), pObject->Get_Transform());

            if (0 > vPosition.m128_f32[3])
                continue;

            if (true == TOOL->m_pTopWindow->isPickSolidMode())
            {
                D3D11_RASTERIZER_DESC RasterizerDesc;
                ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

                RasterizerDesc.CullMode = { D3D11_CULL_BACK };
                RasterizerDesc.FrontCounterClockwise = { false };
                RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };

                if (nullptr != TOOL->m_pCurrentObject)
                    TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);

                RasterizerDesc.FillMode = { D3D11_FILL_SOLID };

                pObject->Set_RasterizerState(RasterizerDesc);
            }

            TOOL->m_pCurrentObject = pObject;
        }
    }
}

void CImWindow_Base::Animation(CGameInstance* pGameInstance)
{
    if (false == m_isIntoAnimTab)
    {
        for (auto& pObject : m_GameObjectList)
            pObject->Updata_Off();

        m_isIntoAnimTab = true;
    }

    if (nullptr == m_pModel)
    {
        if (ImGui::Button("Select Model", ImVec2(100, 20)))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileModelKey", "Choose File", ".dat"/*".cpp,.h,.hpp"*/, ".");

        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileModelKey"))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                m_strFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action

                _char szName[MAX_PATH] = { "" };
                _splitpath_s(m_strFilePath.c_str(), nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);
                string strName = szName;
                wstring wstrName = pGameInstance->strToWStr(strName);

                if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
                    wstrName, L"Layer_Tool")))
                    return;

                WINDOWMGR->Refresh_All_Window();

                CDummyObject3D* pObject = Find_GameObject(wstrName);
                if (nullptr == pObject)
                    return;

                /* Rasterizer */
                D3D11_RASTERIZER_DESC RasterizerDesc;
                ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

                RasterizerDesc.CullMode = { D3D11_CULL_BACK };
                RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
                RasterizerDesc.FrontCounterClockwise = { false };

                pObject->Set_RasterizerState(RasterizerDesc);

                /* Components */
                wstring wstrTag = L"Model_";
                wstrTag = wstrTag + wstrName;

                pObject->Add_Shader(L"Shader_AnimMesh");
                pObject->Add_Model(wstrTag);

                pObject->Set_Tag(wstrName);
                m_pModel = pObject;
                m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();

                TOOL->m_pCamera->Set_AnimationView();
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
    }

    else
    {
        static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable 
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_SizingFixedFit;

        enum ContentsType { CT_Text, CT_Button, CT_SmallButton, CT_FillButton, CT_Selectable, CT_SelectableSpanRow };
        static int contents_type = CT_SelectableSpanRow;

        static ImVec2 outer_size_value = ImVec2(0.0f, 12);
        static bool outer_size_enabled = true;
        static bool show_wrapped_text = false;

        ImVec2 table_scroll_cur, table_scroll_max;
        const ImDrawList* table_draw_list = NULL;

        if (ImGui::BeginTable("Table_Animation", 3, flags, ImVec2(0, 300), 0.f))
        {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, 1);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Loop", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);

            ImGui::TableHeadersRow();

            static _uint iSelected = 0;
            _uint iIndex = 0;
            for (auto& Data : m_vecAnimationDatas)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable(std::to_string(iIndex).c_str(), iSelected == iIndex))
                {
                    iSelected = iIndex;
                    m_pModel->Get_Model()->Set_AnimIndex(iIndex);
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(Data.szName);

                // 세번째 열 인덱스로 이동한다는 뜻
                ImGui::TableSetColumnIndex(2);
                string strLoop = "";
                strLoop = Data.bIsLoop ? "True" : "False";
                ImGui::Text(strLoop.c_str());
                ++iIndex;
            }

            table_scroll_cur = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
            table_scroll_max = ImVec2(ImGui::GetScrollMaxX(), ImGui::GetScrollMaxY());
            table_draw_list = ImGui::GetWindowDrawList();
            ImGui::EndTable();

            /* Add Animation */
            if (ImGui::Button("Add_Animation"))
                ImGui::OpenPopup("Add_Animation");

            if (ImGui::BeginPopup("Add_Animation"))
            {
                string strTag = pGameInstance->wstrToStr(m_pModel->Get_Tag());
                strTag += "_";

                vector<string> AnimationTagList;
                m_PrototypeList = pGameInstance->Get_All_Prototypes();

                for (auto& Prototype : m_PrototypeList)
                {
                    string strPrototypeTag = pGameInstance->wstrToStr(Prototype->Get_Tag());

                    if (string::npos != strPrototypeTag.find(strTag))
                        AnimationTagList.push_back(strPrototypeTag);
                }

                static _int iTagIndex = 0;
                for (_uint i = 0; i < AnimationTagList.size(); ++i)
                    ImGui::RadioButton(AnimationTagList[i].c_str(), &iTagIndex, i);

                if (ImGui::Button("Apply"))
                {
                    Add_Animation(pGameInstance, AnimationTagList[iTagIndex]);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Exit"))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }

            /* Delete Animation */
            ImGui::SameLine();
            if (ImGui::Button("Delete_Animation"))
            {
                m_pModel->Get_Model()->Set_AnimIndex(0);
                m_pModel->Get_Model()->Delete_Animation(iSelected);
                Release_Animation_Data();
                m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
                iSelected = 0;
            }
        }

        if (ImGui::Button("Export"))
            Export_Animation(pGameInstance);

        ImGui::SameLine();

        if (ImGui::Button("Exit"))
        {
            pGameInstance->Remove_GameObject(m_pModel->Get_Tag());
            m_pModel = nullptr;
            TOOL->m_pCamera->Set_OriginView();
            WINDOWMGR->Refresh_All_Window();

            for (auto& pAnimation : m_vecAnimationDatas)
            {
                for (_uint i = 0; i < pAnimation.iNumChannels; ++i)
                {
                    Safe_Delete_Array(pAnimation.pChannels[i].pKeyFrames);
                }
                Safe_Delete_Array(pAnimation.pChannels);
            }
            m_vecAnimationDatas.clear();
        }
    }
}

void CImWindow_Base::Add_Animation(CGameInstance* pGameInstance, const string& strTag)
{
    wstring wstrTag = pGameInstance->strToWStr(strTag);

    auto& iter = find_if(m_PrototypeList.begin(), m_PrototypeList.end(), [&](CComponent* pPrototype)
        {
            if (wstrTag == pPrototype->Get_Tag())
                return true;
            else
                return false;
        });

    if (iter == m_PrototypeList.end())
        return;

    CModel* pAnimModel = dynamic_cast<CModel*>(*iter);
    if (nullptr == pAnimModel)
        return;

    vector<ANIMATIONDATA> vecAnimData;
    vecAnimData = pAnimModel->Get_AnimationDatas();

    for (auto& Data : m_vecAnimationDatas)
    {
        if (0 == strcmp(Data.szName, vecAnimData[0].szName))
        {
            Release_Animation_Data();
            m_vecAnimationDatas = vecAnimData;
            Release_Animation_Data();
            m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
            return;
        }
    }

    /* 이 부분은 모든 애니메이션 전용 모델은 하나의 애니메이션만 가지고 있다고 가정하고 진행하는 것임. */
    m_pModel->Get_Model()->Add_Animation(vecAnimData[0]);

    Release_Animation_Data();
    m_vecAnimationDatas = vecAnimData;
    Release_Animation_Data();
    m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
}

void CImWindow_Base::Export_Animation(CGameInstance* pGameInstance)
{
    m_FilePathList.push_back(m_strFilePath);
    m_vecBinData.push_back(m_pModel->Get_Model_BinaryData());

    _char szFullPath[MAX_PATH] = { "" };
    GetCurrentDirectoryA(MAX_PATH, szFullPath);
    string strFullPath = { "" };
    strFullPath = strFullPath + szFullPath + "\\Out\\";

    pGameInstance->WriteModels(strFullPath, m_FilePathList, m_vecBinData);
}

void CImWindow_Base::Release_Animation_Data()
{
    for (auto& pAnimation : m_vecAnimationDatas)
    {
        for (_uint i = 0; i < pAnimation.iNumChannels; ++i)
        {
            Safe_Delete_Array(pAnimation.pChannels[i].pKeyFrames);
        }
        Safe_Delete_Array(pAnimation.pChannels);
    }
    m_vecAnimationDatas.clear();
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
    Release_Export_Data();
    Release_Animation_Data();
    CImWindow::Free();
}

void CImWindow_Base::Release_Export_Data()
{
    m_FilePathList.clear();

    for (auto& BinData : m_vecBinData)
    {
        for (_uint i = 0; i < BinData.iNumAnimations; ++i)
        {
            for (_uint j = 0; j < BinData.pAnimations[i].iNumChannels; ++j)
            {
                Safe_Delete_Array(BinData.pAnimations[i].pChannels[j].pKeyFrames);
            }
            Safe_Delete_Array(BinData.pAnimations[i].pChannels);
        }
        Safe_Delete_Array(BinData.pAnimations);
    }
}
