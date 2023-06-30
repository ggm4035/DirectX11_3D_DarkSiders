#include "CTool_Animation.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"
#include "CImWindow_Animation.h"

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
                    TOOL->m_pAnimationWindow->Refresh_Animation();
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
                ImGui::BeginChild("Name", ImVec2(220, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
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

                ImGui::EndChild();
                /* Add Animation */
                if (ImGui::Button("Add Animation"))
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
                m_pModel->Get_Model()->Delete_Animation(iSelected);
                iSelected = 0 > --iSelected ? 0 : iSelected;
                m_pModel->Get_Model()->Set_AnimIndex(iSelected);
                Release_Animation_Data();
                m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
                TOOL->m_pAnimationWindow->Refresh_Animation();
            }

            ImGui::SameLine();

            /* 위치 조정하는건데 나중에 고치면 지울거임 */
            if (ImGui::Button("Origin"))
            {
                _float4x4 WorldMatrix;
                XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
                m_pModel->Get_Transform()->Set_Matrix(WorldMatrix);
            }
        }
        /* Apply Animation */
        if (ImGui::Button("Apply"))
        {
            for (_uint i = 0; i < m_vecAnimationDatas.size(); ++i)
                m_pModel->Get_Model()->Set_Animation(i, m_vecAnimationDatas[i]);
        }

        /* Control Transformation */
        Transformation(pGameInstance);

        ImGui::Separator();

        /* Export */
        if (ImGui::Button("Export"))
            Export_Animation(pGameInstance);

        ImGui::SameLine();

        /* Exit */
        if (ImGui::Button("Exit"))
        {
            pGameInstance->Remove_GameObject(m_pModel->Get_Tag());
            m_pModel = nullptr;
            TOOL->m_pAnimationWindow->Set_Model(nullptr);
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

            /* Import Model Data */
            list<string> FilePath;
            vector<MODEL_BINARYDATA> vecData;
            pGameInstance->ReadModels(m_strFilePath, FilePath, vecData);

            /* Create Dummy Model */
            _char szName[MAX_PATH] = { "" };
            _splitpath_s(m_strFilePath.c_str(), nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);
            string strName = szName;
            wstring wstrName = pGameInstance->strToWStr(strName);

            if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
                wstrName, L"Layer_Tool")))
                return;

            /* Refresh List */
            WINDOWMGR->Refresh_All_Window();

            /* Bind Model Pointer */
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

            TOOL->m_pCamera->Set_AnimationView();
            TOOL->m_pAnimationWindow->Set_Model(pObject);

            /* Add Animation */
            for (_uint i = 1; i < vecData[0].iNumAnimations; ++i)
            {
                m_pModel->Get_Model()->Add_Animation(vecData[0].pAnimations[i]);
            }
            m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();

            Safe_Delete_BinaryData(vecData[0]);
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
    list<string> FilePathList;
    FilePathList.push_back(m_strFilePath);

    vector<MODEL_BINARYDATA> Temp;
    MODEL_BINARYDATA Data = m_pModel->Get_Model_BinaryData();

    Data.pAnimations = nullptr;

    ANIMATIONDATA* pAnimations = New ANIMATIONDATA[m_vecAnimationDatas.size()];

    _uint iNumAnimation = 0;
    for (auto& AnimData : m_vecAnimationDatas)
    {
        pAnimations[iNumAnimation] = m_vecAnimationDatas[iNumAnimation];
        ++iNumAnimation;
    }
    Data.iNumAnimations = iNumAnimation;
    Data.pAnimations = pAnimations;

    Temp.push_back(Data);

    _char szFullPath[MAX_PATH] = { "" };
    GetCurrentDirectoryA(MAX_PATH, szFullPath);
    string strFullPath = { "" };
    strFullPath = strFullPath + szFullPath + "\\Out\\";

    /* Export Model Data */
    pGameInstance->WriteModels(strFullPath, FilePathList, Temp);

    /* Remove Datas */
    pGameInstance->Remove_GameObject(m_pModel->Get_Tag());
    m_pModel = nullptr;
    TOOL->m_pAnimationWindow->Set_Model(nullptr);
    TOOL->m_pCamera->Set_OriginView();
    WINDOWMGR->Refresh_All_Window();
    Release_Animation_Data();

    Safe_Delete_Array(pAnimations);

    MSG_BOX("Success to Export");
}

void CTool_Animation::Transformation(CGameInstance* pGameInstance)
{
    ImGui::SeparatorText("Transformation");
    _float3 vTranslation;
    _float fOffset = { 0 };

    _uint iRootFrameIndex = TOOL->m_pAnimationWindow->m_iCurrentRootFrame;
    vTranslation = m_pModel->Get_Model()->Get_Translation(iRootFrameIndex);

    _float arr[3] = { vTranslation.x, vTranslation.y, vTranslation.z };

    ImGui::InputFloat3("Translation", arr);

    if (ImGui::DragFloat("Offset", &fOffset, 0.01f, -100.f, 100.f, "%.2f"))
    {
        for (_uint i = 0; i < m_pModel->Get_Model()->Get_MaxRootKeyFrame(); ++i)
        {
            vTranslation = m_pModel->Get_Model()->Get_Translation(i);

            vTranslation.x += fOffset;

            m_pModel->Get_Model()->Set_Translation(i, vTranslation);
        }
    }
    else
    {
        vTranslation = _float3(arr[0], arr[1], arr[2]);

        m_pModel->Get_Model()->Set_Translation(iRootFrameIndex, vTranslation);
    }
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

void CTool_Animation::Free()
{
    Release_Animation_Data();
    Safe_Release(m_pModel);
}
