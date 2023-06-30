#include "CImWindow_Base.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"

#include "CImWindow_Animation.h"
#include "CImWindow_Inspector.h"
#include "CImWindow_Create.h"
#include "CImWindow_Top.h"

#include "CDummyObject3D.h"
#include "CCoordnate_Axis.h"
#include "CFileInfo.h"
#include "CMainCamera.h"

#include "CTool_Animation.h"
#include "CTool_HeightMap.h"

IMPLEMENT_SINGLETON(CImWindow_Base)

HRESULT CImWindow_Base::Initialize()
{
    if (FAILED(CImWindow::Initialize()))
        return E_FAIL;

    m_pToolAnimation = New CTool_Animation;
    if (nullptr == m_pToolAnimation)
        return E_FAIL;

    m_pToolHeightMap = New CTool_HeightMap;
    if (nullptr == m_pToolHeightMap)
        return E_FAIL;
    m_pToolHeightMap->Initialize();

    return S_OK;
}

void CImWindow_Base::Tick(const _double& TimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImVec2 pos = ImGui::GetWindowPos();
    pos.x -= 407;
    pos.y -= 140;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(350, 763), ImGuiCond_Always);
    ImGui::Begin("Hierarchy ", nullptr, ImGuiWindowFlags_NoResize);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Tabs", tab_bar_flags))
    {
        if (m_PreAnimTab != m_isIntoAnimTab)
        {
            for (auto& pObject : m_GameObjectList)
                pObject->Updata_Toggle();

            m_PreAnimTab = m_isIntoAnimTab;
        }

        if (ImGui::BeginTabItem("Hierarchy"))
        {
            Hierarchy(pGameInstance);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Terrain"))
        {
            if (nullptr != m_pToolHeightMap)
                m_pToolHeightMap->Tick(pGameInstance);
            HeightMap(pGameInstance);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("UI"))
        {
            UI(pGameInstance);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Animation"))
        {
            m_isIntoAnimTab = true;
            TOOL->m_pAnimationWindow->m_bIsOpen = true;
            if (nullptr != m_pToolAnimation)
                m_pToolAnimation->Tick(pGameInstance, m_GameObjectList);

            ImGui::EndTabItem();
        }
        else
        {
            m_isIntoAnimTab = false;
            TOOL->m_pAnimationWindow->m_bIsOpen = false;
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    Safe_Release(pGameInstance);
}

void CImWindow_Base::Hierarchy(CGameInstance* pGameInstance)
{
    if (nullptr == m_pToolHeightMap->Get_Terrain())
    {
        if (ImGui::Button("Create_Terrain"))
        {
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
                L"Terrain", L"Layer_Tool")))
                return;

            WINDOWMGR->Refresh_All_Window();

            CDummyObject3D* pTerrain = Find_GameObject(L"Terrain");
            if (nullptr == pTerrain)
                return;

            /* Rasterizer */
            D3D11_RASTERIZER_DESC RasterizerDesc;
            ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

            RasterizerDesc.CullMode = { D3D11_CULL_BACK };
            RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };
            RasterizerDesc.FrontCounterClockwise = { false };

            pTerrain->Set_RasterizerState(RasterizerDesc);

            /* Component */
            pTerrain->Add_Buffer(L"VIBuffer_Terrain");
            pTerrain->Add_Texture(L"Texture_Terrain");
            pTerrain->Add_Shader(L"Shader_VtxNorTex");

            pTerrain->Set_Tag(L"Terrain");

            TOOL->m_pCurrentObject = pTerrain;

            m_pToolHeightMap->Set_Terrain(pTerrain);
        }
    }
    else
        Create_Object(pGameInstance);
}

void CImWindow_Base::HeightMap(CGameInstance* pGameInstance)
{
    
}

void CImWindow_Base::UI(CGameInstance* pGameInstance)
{
}

void CImWindow_Base::Create_Object(CGameInstance* pGameInstance)
{
    wstring pObjectTag = L"";

    _uint i = 0;

    if (true == TOOL->m_pTopWindow->isPickingMode())
        Picking_GameObject(pGameInstance);


    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
        | ImGuiTableFlags_SizingFixedFit;

    if (ImGui::BeginTable("Table_Model", 3, flags, ImVec2(0, 300), 0.f))
    {
        ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
        ImGui::TableSetupColumn("ModelTag", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);

        ImGui::TableHeadersRow();

        /* Model Data Table ��� */
        static _uint iSelected = 0;
        _uint iIndex = 0;
        auto iter = m_GameObjectList.begin();
        for (auto& Object : m_GameObjectList)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            if (ImGui::Selectable(std::to_string(iIndex).c_str(), iSelected == iIndex))
            {
                iSelected = iIndex;

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

                    TOOL->m_pCurrentObject = *iter;

                    if (nullptr != TOOL->m_pCurrentObject)
                        TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);
                }
                else
                    TOOL->m_pCurrentObject = *iter;
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(pGameInstance->wstrToStr(Object->Get_Tag()).c_str());

            ImGui::TableSetColumnIndex(2);
            if(nullptr != Object->Get_Model())
                ImGui::TextUnformatted(pGameInstance->wstrToStr(Object->Get_Model()->Get_Tag()).c_str());

            ++iIndex;
            ++iter;
        }
        ImGui::EndTable();
    }

    if (ImGui::Button("Create new Object"))
        ImGui::OpenPopup("Create");

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

    if (ImGui::Button("Delete"))
    {
        if (0 == lstrcmp(L"Terrain", TOOL->m_pCurrentObject->Get_Tag().c_str()))
            m_pToolHeightMap->Set_Terrain(nullptr);

        pGameInstance->Remove_GameObject(TOOL->m_pCurrentObject->Get_Tag());
        TOOL->m_pCurrentObject = nullptr;
        WINDOWMGR->Refresh_All_Window();
    }
}

void CImWindow_Base::Picking_GameObject(CGameInstance* pGameInstance)
{
    if (g_hWnd == ::GetFocus() && pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
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
    else if (g_hWnd == ::GetFocus() && pGameInstance->Mouse_Pressing(CInput_Device::DIM_LB))
    {
        POINT ptMouse;
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        if (nullptr == TOOL->m_pCurrentObject)
            return;

        CDummyObject3D* pTerrain = Find_GameObject(L"Terrain");
        if (nullptr == pTerrain)
            return;

        _vector vPosition = pGameInstance->Picking_On_Triangle(ptMouse, pTerrain->Get_Buffer(), pTerrain->Get_Transform());

        if (0 > vPosition.m128_f32[3])
            return;

        _vector vPrePos = TOOL->m_pCurrentObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        vPosition.m128_f32[1] = vPrePos.m128_f32[1];

        TOOL->m_pCurrentObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPosition);
    }
}

void CImWindow_Base::Refresh()
{
    CImWindow::Refresh();
}

void CImWindow_Base::Free()
{
    Safe_Release(m_pToolAnimation);
    Safe_Release(m_pToolHeightMap);
    CImWindow::Free();
}
