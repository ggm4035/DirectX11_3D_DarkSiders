#include "CImWindow_Create.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"
#include "CDummyObject3D.h"

#include "CImWindow_Top.h"

IMPLEMENT_SINGLETON(CImWindow_Create)

CImWindow_Create::CImWindow_Create()
{
}

HRESULT CImWindow_Create::Initialize()
{
    if (FAILED(CImWindow::Initialize()))
        return E_FAIL;

    m_Types[TEXTURE] = "Texture";
    m_Types[SHADER] = "Shader";
    m_Types[BUFFER] = "VIBuffer";
    m_Types[MODEL] = "Model";

    return S_OK;
}

void CImWindow_Create::Tick(const _double& TimeDelta)
{
    if (false == m_bIsOpen)
        return;

    ImGui::Begin("Create");

    Create_Object();

    ImGui::End();
}

void CImWindow_Create::Refresh()
{
    CImWindow::Refresh();
}

void CImWindow_Create::Create_Object()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImGui::Text("Object Name");
    ImGui::Separator();
    ImGui::SetNextItemWidth(300.f);
    ImGui::InputText("    ", m_szObjectName, 256);

    for (_uint i = 0; i < TYPE_END; ++i)
    {
        ImGui::Checkbox(m_Types[i], &m_bTypes[i]);

        if (true == m_bTypes[i])
        {
            _char szButtonName[256] = { "Select Prototype " };
            _char szDialogTag[256] = { "Prototype " };

            ImGui::Text(m_Types[i]);
            ImGui::Separator();
            ImGui::Text("PrototypeTag");
            ImGui::SetNextItemWidth(500.f);
            ImGui::InputText("  ", m_szPrototypeTag[i], 256, ImGuiInputTextFlags_ReadOnly);

            strcat_s(szDialogTag, m_Types[i]);
            if (ImGui::BeginPopupContextItem(szDialogTag))
            {
                ImGui::InputText("Search", m_szSearchTag, 256);
                /* string 소문자로 변환 */
                string strSearchTag = m_szSearchTag;
                transform(strSearchTag.begin(), strSearchTag.end(), strSearchTag.begin(), [](_char c) {
                        return tolower(c);
                });

                list<CComponent*> PrototypeList = pGameInstance->Get_All_Prototypes();
                _uint iNumPrototypes = PrototypeList.size();

                auto iter = PrototypeList.begin();

                string* pstrTags = New string[iNumPrototypes];

                _uint iIndex = 0;
                for (auto iter = PrototypeList.begin(); iter != PrototypeList.end(); ++iter)
                {
                    pstrTags[iIndex] = pGameInstance->wstrToStr((*iter)->Get_Tag());

                    string strTemp = pstrTags[iIndex];
                    transform(strTemp.begin(), strTemp.end(), strTemp.begin(), [](_char c) {
                        return tolower(c);
                    });

                    if (string::npos != pstrTags[iIndex].find(m_Types[i]) &&
                        ((0 == strSearchTag.size()) || (string::npos != strTemp.find(strSearchTag))))
                    {
                        ++iIndex;
                    }
                }

                const _char** ppiTems = New const _char * [iIndex];

                for (_uint j = 0; j < iIndex; ++j)
                    ppiTems[j] = pstrTags[j].c_str();

                static int iPick = 0;
                ImGui::SetNextItemWidth(500.f);
                ImGui::ListBox("Prototypes", &iPick, ppiTems, iIndex);

                if (ImGui::Button("Select"))
                {
                    strcpy_s(m_szPrototypeTag[i], ppiTems[iPick]);
                    ImGui::CloseCurrentPopup();
                }

                Safe_Delete_Array(pstrTags);
                Safe_Delete_Array(ppiTems);

                ImGui::EndPopup();
            }
            strcat_s(szButtonName, m_Types[i]);
            if (ImGui::Button(szButtonName))
                ImGui::OpenPopup(szDialogTag);

            ImGui::Separator();
        }
    }

    static _int clicked = 0;

    if (ImGui::Button("Create Object"))
    {
        if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
            pGameInstance->strToWStr(m_szObjectName), L"Layer_Tool")))
            return;

        WINDOWMGR->Refresh_All_Window();

        CDummyObject3D* pObject = Find_GameObject(pGameInstance->strToWStr(m_szObjectName));
        if (nullptr == pObject)
            return;

        /* Rasterizer */
        D3D11_RASTERIZER_DESC RasterizerDesc;
        ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

        RasterizerDesc.CullMode = { D3D11_CULL_BACK };
        RasterizerDesc.FrontCounterClockwise = { false };
        RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };

        if (nullptr != TOOL->m_pCurrentObject &&
            false == TOOL->m_pTopWindow->isSolidMode())
            TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);

        RasterizerDesc.FillMode = { D3D11_FILL_SOLID };

        pObject->Set_RasterizerState(RasterizerDesc);

        /* Components */
        pObject->Add_Texture(pGameInstance->strToWStr(m_szPrototypeTag[TEXTURE]));
        pObject->Add_Shader(pGameInstance->strToWStr(m_szPrototypeTag[SHADER]));
        pObject->Add_Buffer(pGameInstance->strToWStr(m_szPrototypeTag[BUFFER]));
        pObject->Add_Model(pGameInstance->strToWStr(m_szPrototypeTag[MODEL]));

        pObject->Set_Tag(pGameInstance->strToWStr(m_szObjectName));

        TOOL->m_pCurrentObject = pObject;

        ZeroMemory(m_szPrototypeTag, sizeof(_char) * 256 * TYPE_END);
        ZeroMemory(m_szObjectName, sizeof(_char) * 256);
        m_bIsOpen = false;
        clicked = { 0 };
        for (_uint i = 0; i < TYPE_END; ++i)
            m_bTypes[i] = false;

        m_bIsPicking = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Clone"))
        ++clicked;

    ImGui::SameLine();
    if (clicked & 1)
    {
        m_bIsPicking = true;
        ImGui::Text("Copy Mode On");
        Create_Object_Pick(pGameInstance);
    }
    else
    {
        m_bIsPicking = false;
        ImGui::Text("Copy Mode Off");
        m_iCloneNum = { 0 };
    }

    if (ImGui::Button("Exit"))
    {
        m_bIsOpen = false;
        ZeroMemory(m_szPrototypeTag, sizeof(_char) * 256 * 3);
        ZeroMemory(m_szObjectName, sizeof(_char) * TYPE_END);
        for (_uint i = 0; i < TYPE_END; ++i)
            m_bTypes[i] = false;
        clicked = { 0 };
        m_iCloneNum = { 0 };

        m_bIsPicking = false;
    }

    Safe_Release(pGameInstance);
}

void CImWindow_Create::Create_Object_Pick(CGameInstance* pGameInstance)
{
    if (g_hWnd == ::GetFocus() && pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
    {
        _char szNum[8] = { "" };
        _char szObjName[256] = { "" };
        strcpy_s(szObjName, m_szObjectName);
        _itoa_s(m_iCloneNum, szNum, 10);
        strcat_s(szObjName, szNum);

        while (nullptr != Find_GameObject(pGameInstance->strToWStr(szObjName)))
        {
            ++m_iCloneNum;
            strcpy_s(szObjName, m_szObjectName);
            _itoa_s(m_iCloneNum, szNum, 10);
            strcat_s(szObjName, szNum);
        }

        if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
            pGameInstance->strToWStr(szObjName), L"Layer_Tool")))
            return;

        WINDOWMGR->Refresh_All_Window();

        CDummyObject3D* pObject = Find_GameObject(pGameInstance->strToWStr(szObjName));
        if (nullptr == pObject)
            return;

        /* Rasterizer */
        D3D11_RASTERIZER_DESC RasterizerDesc;
        ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

        RasterizerDesc.CullMode = { D3D11_CULL_BACK };
        RasterizerDesc.FrontCounterClockwise = { false };
        RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };

        if (nullptr != TOOL->m_pCurrentObject &&
            false == TOOL->m_pTopWindow->isSolidMode())
            TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);

        RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
        pObject->Set_RasterizerState(RasterizerDesc);

        /* Components */
        pObject->Add_Texture(pGameInstance->strToWStr(m_szPrototypeTag[TEXTURE]));
        pObject->Add_Shader(pGameInstance->strToWStr(m_szPrototypeTag[SHADER]));
        pObject->Add_Buffer(pGameInstance->strToWStr(m_szPrototypeTag[BUFFER]));
        pObject->Add_Model(pGameInstance->strToWStr(m_szPrototypeTag[MODEL]));

        pObject->Set_Tag(pGameInstance->strToWStr(szObjName));

        POINT ptMouse;
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        /* 임시 코드임 원래 오브젝트 전체의 버택스를 검사해야됨.. */
        CDummyObject3D* pTerrain = Find_GameObject(L"Terrain");
        if (nullptr == pTerrain)
        {
            pGameInstance->Remove_GameObject(pObject->Get_Tag());
            WINDOWMGR->Refresh_All_Window();
            return;
        }

        _vector vPickPos = pGameInstance->Picking_On_Triangle(ptMouse, pTerrain->Get_Buffer(), pTerrain->Get_Transform());
        /*========================================================*/

        if (0 > vPickPos.m128_f32[3])
        {
            pGameInstance->Remove_GameObject(pObject->Get_Tag());
            WINDOWMGR->Refresh_All_Window();
            return;
        }

        pObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPickPos);

        TOOL->m_pCurrentObject = pObject;

        ++m_iCloneNum;
    }
}

void CImWindow_Create::Free()
{
    CImWindow::Free();
}
