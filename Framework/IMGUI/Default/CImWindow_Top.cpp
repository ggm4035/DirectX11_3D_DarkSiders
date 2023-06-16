#include "CImWindow_Top.h"

#include "CImWindow_Base.h"
#include "CImWindow_Create.h"
#include "CDummyObject3D.h"

#include "CGameInstance.h"
#include "CImWindow_Manager.h"

CImWindow_Top::CImWindow_Top()
{
}

HRESULT CImWindow_Top::Initialize(void* pArg)
{
    if (FAILED(CImWindow::Initialize(pArg)))
        return E_FAIL;

    m_strPickMode = "Picking OFF";

    return S_OK;
}

void CImWindow_Top::Tick(const _double& TimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImGui::SetNextWindowPos(ImVec2(350, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1250, 80), ImGuiCond_Always);
    ImGui::Begin("Top", nullptr, ImGuiWindowFlags_NoResize);

    RenderMode(pGameInstance);

    ImGui::SameLine();
    PickingMode(pGameInstance);

    ImGui::SameLine();
    Save(pGameInstance);

    ImGui::SameLine();
    Load(pGameInstance);

    ImGui::End();

    Safe_Release(pGameInstance);
}

void CImWindow_Top::Refresh()
{
    CImWindow::Refresh();
}

void CImWindow_Top::RenderMode(CGameInstance* pGameInstance)
{
    if (ImGui::BeginPopupContextItem("Render Mode"))
    {
        D3D11_RASTERIZER_DESC RasterizerDesc;
        ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

        RasterizerDesc.CullMode = { D3D11_CULL_BACK };
        RasterizerDesc.FrontCounterClockwise = { false };

        static _int iMode = { 0 };

        if (ImGui::RadioButton("All Solid", &iMode, 0))
        {
            RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
            for (auto& pObject : m_GameObjectList)
                if (dynamic_cast<CDummyObject3D*>(pObject))
                    dynamic_cast<CDummyObject3D*>(pObject)->Set_RasterizerState(RasterizerDesc);

            m_isPickSolidMode = false;
        }
        if (ImGui::RadioButton("Pick Solid the rest WireFrame", &iMode, 1))
        {
            m_isPickSolidMode = true;
        }
        if (ImGui::RadioButton("All WireFrame", &iMode, 2))
        {
            RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };
            for (auto& pObject : m_GameObjectList)
                if (dynamic_cast<CDummyObject3D*>(pObject))
                    dynamic_cast<CDummyObject3D*>(pObject)->Set_RasterizerState(RasterizerDesc);

            m_isPickSolidMode = false;
        }
        ImGui::EndPopup();
    }

    if (ImGui::Button("Render Mode", ImVec2(100, 40)))
        ImGui::OpenPopup("Render Mode");
}

void CImWindow_Top::PickingMode(CGameInstance* pGameInstance)
{
    /*if (ImGui::BeginPopupContextItem("Picking Mode"))
    {
        static _int iMode = { 0 };

        if (ImGui::RadioButton("None", &iMode, 0))
        {

        }
        if (ImGui::RadioButton("All Object", &iMode, 1))
        {

        }
        ImGui::EndPopup();
    }*/

    if (ImGui::Button(m_strPickMode.c_str(), ImVec2(100, 40)))
    {
        static _uint i = 0;
        if (i & 1)
        {
            m_isPicking = false;
            m_strPickMode = "Picking OFF";
        }
        else
        {
            m_isPicking = true;
            m_strPickMode = "Picking ON";
        }
        ++i;
    }
}

void CImWindow_Top::Save(CGameInstance* pGameInstance)
{
    if (ImGui::Button("SAVE", ImVec2(100, 40)))
        ImGuiFileDialog::Instance()->OpenDialog("ChooseSaveFileKey", "Choose File", ".dat"/*".cpp,.h,.hpp"*/, ".");

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseSaveFileKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action

            HANDLE hFile = CreateFileA(filePathName.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (INVALID_HANDLE_VALUE == hFile)
            {
                MSG_BOX("Failed Open Save File");
                return;
            }

            _ulong dwByte = { 0 };
            /* 1. 전체 3D 게임오브젝트들을 저장한다. */
            /* Write iNumObjects */
            _uint iNumObjects = m_GameObjectList.size();
            WriteFile(hFile, &iNumObjects, sizeof(_uint), &dwByte, nullptr);
            for (auto& pGameObject : m_GameObjectList)
            {
                /* Write szModelTag */
                _uint iTaglength = lstrlen(pGameObject->Get_Model()->Get_Tag().c_str()) + 1;
                WriteFile(hFile, &iTaglength, sizeof(_uint), &dwByte, nullptr);
                WriteFile(hFile, pGameObject->Get_Model()->Get_Tag().c_str(), sizeof(_tchar) * iTaglength, &dwByte, nullptr);

                /* Write TransformMatrix */
                WriteFile(hFile, &pGameObject->Get_Transform()->Get_WorldMatrix(), sizeof(_float4x4), &dwByte, nullptr);
            }

            /* 2. 카메라를 저장한다. */

            /* 3. Terrain을 저장한다. */

            /* 4. UI를 저장한다. */

            CloseHandle(hFile);

            MSG_BOX("Success Save Files");
        }
        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void CImWindow_Top::Load(CGameInstance* pGameInstance)
{
    if (ImGui::Button("LOAD", ImVec2(100, 40)))
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".dat"/*".cpp,.h,.hpp"*/, ".");

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action

            for (auto& pObject : m_GameObjectList)
                pGameInstance->Remove_GameObject(pObject->Get_Tag());

            list<FILEDATA> FileData;
            if (FAILED(pGameInstance->Load(filePathName, FileData)))
            {
                MSG_BOX("Failed Load Files");
                return;
            }
            else
                MSG_BOX("Success Load Files");

            _uint idx = { 0 };
            for (auto& Data : FileData)
            {
                _char szNum[8] = { "" };
                _char szObjName[256] = { "" };
                strcpy_s(szObjName, "Test");
                _itoa_s(idx, szNum, 10);
                strcat_s(szObjName, szNum);

                if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
                    pGameInstance->strToWStr(szObjName), L"Layer_Tool")))
                    return;

                WINDOWMGR->Refresh_All_Window();

                CDummyObject3D* pObject = Find_GameObject(pGameInstance->strToWStr(szObjName));
                if (nullptr == pObject)
                    return;

                pObject->Get_Transform()->Set_Matrix(Data.TransformMatrix);

                /* Rasterizer */
                D3D11_RASTERIZER_DESC RasterizerDesc;
                ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

                RasterizerDesc.CullMode = { D3D11_CULL_BACK };
                RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
                RasterizerDesc.FrontCounterClockwise = { false };

                pObject->Set_RasterizerState(RasterizerDesc);

                /* Components */
                pObject->Add_Shader(L"Shader_Mesh");
                pObject->Add_Model(Data.szModelTag);

                pObject->Set_Tag(pGameInstance->strToWStr(szObjName));
                ++idx;
            }
        }
    
        // close
        ImGuiFileDialog::Instance()->Close();
    }

}

CImWindow_Top* CImWindow_Top::Create(void* pArg)
{
    CImWindow_Top* pInstance = new CImWindow_Top;

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create CImWindow_Top");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImWindow_Top::Free()
{
    CImWindow::Free();
}
