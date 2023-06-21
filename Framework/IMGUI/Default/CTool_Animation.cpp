#include "CTool_Animation.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"

#include "CMainCamera.h"
#include "CDummyObject3D.h"

CTool_Animation::CTool_Animation()
{
}

void CTool_Animation::Tick(CGameInstance* pGameInstance, list<class CDummyObject3D*>& ObjectList)
{
    if (nullptr == m_pModel)
        Make_New_Model(pGameInstance, ObjectList);

    else
    {
        static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable("Table_Animation", 3, flags, ImVec2(0, 300), 0.f))
        {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, 1);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Loop", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);

            ImGui::TableHeadersRow();

            /* Animation Data Table 출력 */
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
                string button_label = strLoop + "##" + std::to_string(iIndex);
                if (ImGui::Button(button_label.c_str()))
                    Data.bIsLoop = !Data.bIsLoop;
                ++iIndex;
            }
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

        /* Export */
        if (ImGui::Button("Export"))
            Export_Animation(pGameInstance);

        ImGui::SameLine();

        /* Exit */
        if (ImGui::Button("Exit"))
        {
            pGameInstance->Remove_GameObject(m_pModel->Get_Tag());
            m_pModel = nullptr;
            TOOL->m_pCamera->Set_OriginView();
            WINDOWMGR->Refresh_All_Window();
            Release_Animation_Data();
        }
    }
}

void CTool_Animation::Make_New_Model(CGameInstance* pGameInstance, list<class CDummyObject3D*>& ObjectList)
{
    if (ImGui::Button("Select Model", ImVec2(100, 20)))
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileModelKey", "Choose File", ".dat", ".");

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

            CDummyObject3D* pObject = { nullptr };
            for (auto& iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
            {
                if ((*iter)->Get_Tag() == wstrName)
                    pObject = *iter;
            }
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

void CTool_Animation::Add_Animation(CGameInstance* pGameInstance, const string& strTag)
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

void CTool_Animation::Export_Animation(CGameInstance* pGameInstance)
{
    m_FilePathList.push_back(m_strFilePath);
    m_vecBinData.push_back(m_pModel->Get_Model_BinaryData());

    _char szFullPath[MAX_PATH] = { "" };
    GetCurrentDirectoryA(MAX_PATH, szFullPath);
    string strFullPath = { "" };
    strFullPath = strFullPath + szFullPath + "\\Out\\";

    pGameInstance->WriteModels(strFullPath, m_FilePathList, m_vecBinData);
}

void CTool_Animation::Release_Animation_Data()
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

void CTool_Animation::Release_Export_Data()
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

void CTool_Animation::Free()
{
    Release_Export_Data();
    Release_Animation_Data();

    Safe_Release(m_pModel);
}
