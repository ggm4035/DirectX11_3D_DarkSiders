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
}

void CImWindow_Base::Picking_GameObject(CGameInstance* pGameInstance)
{
    if(pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
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
    if (true == m_isFirst)
    {
        for (auto& pObject : m_GameObjectList)
            pObject->Updata_Off();

        m_isFirst = false;
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
                string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action

                _char szName[MAX_PATH] = { "" };
                _splitpath_s(filePathName.c_str(), nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);
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

                TOOL->m_pCamera->Set_AnimationView();
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
    }

    else if (ImGui::Button("Exit"))
    {
        pGameInstance->Remove_GameObject(m_pModel->Get_Tag());
        m_pModel = nullptr;
        TOOL->m_pCamera->Set_OriginView();
        WINDOWMGR->Refresh_All_Window();
    }
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
