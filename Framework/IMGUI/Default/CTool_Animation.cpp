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

void CTool_Animation::Tick(CGameInstance* pGameInstance, list<class CDummyObject3D*>* pObjectList)
{
    if (nullptr == m_pModel)
        Make_New_Model(pGameInstance, pObjectList);

    else
    {
        static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable("Table_Animation", 4, flags, ImVec2(0, 300), 0.f))
        {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, 1);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Loop", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Follow Animation", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);

            ImGui::TableHeadersRow();

            /* Animation Data Table 출력 */
            _uint iIndex = 0;
            for (auto& Data : m_vecAnimationDatas)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable(std::to_string(iIndex).c_str(), m_iCurrentAnimationIndex == iIndex))
                {
                    m_iCurrentAnimationIndex = iIndex;
                    m_pModel->Get_Model()->Change_Animation(m_vecAnimationDatas[iIndex].szName);
                    TOOL->m_pAnimationWindow->Refresh_Animation();
                }

                ImGui::TableSetColumnIndex(1);
                string label = "##AnimationTag" + std::to_string(iIndex);
                ImGui::SetNextItemWidth(300);
                ImGui::InputText(label.c_str(), Data.szName, 256);


                // 3번째 열 인덱스로 이동한다는 뜻
                ImGui::TableSetColumnIndex(2);
                string strLoop = "";
                strLoop = Data.bIsLoop ? "True" : "False";
                string button_label = strLoop + "##" + std::to_string(iIndex);
                if (ImGui::Button(button_label.c_str()))
                    Data.bIsLoop = !Data.bIsLoop;

                ImGui::TableSetColumnIndex(3);
                strLoop = Data.bIsFollowAnimation ? "True" : "False";
                button_label = strLoop + "##" + string("Follow") + std::to_string(iIndex);
                if (ImGui::Button(button_label.c_str()))
                    Data.bIsFollowAnimation = !Data.bIsFollowAnimation;

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
                string strTag = m_vecAnimationDatas[m_iCurrentAnimationIndex].szName;
                m_pModel->Get_Model()->Delete_Animation(strTag);
                m_iCurrentAnimationIndex = 0 > --m_iCurrentAnimationIndex ? 0 : m_iCurrentAnimationIndex;
                
                m_pModel->Get_Model()->Change_Animation(m_vecAnimationDatas[m_iCurrentAnimationIndex].szName);
                Release_Animation_Data();
                m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
                Refresh_TimeRanges();
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
                m_pModel->Get_Model()->Set_Animation(m_vecAnimationDatas[i].szName, m_vecAnimationDatas[i]);
        }

        /* KeyFrameSetting */
        KeyFrameSetting(pGameInstance);

        ImGui::Separator();

        /* Notify */
        NotifySetting(pGameInstance);

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

void CTool_Animation::Make_New_Model(CGameInstance* pGameInstance, list<class CDummyObject3D*>* pObjectList)
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
            string strFilePath;
            MODEL_BINARYDATA Data;
            pGameInstance->ReadModel(m_strFilePath, strFilePath, Data);

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
            for (_uint i = 0; i < CImWindow::LAYER_END; ++i)
            {
                for (auto& iter = pObjectList[i].begin(); iter != pObjectList[i].end(); ++iter)
                {
                    if ((*iter)->Get_Tag() == wstrName)
                        pObject = *iter;
                }
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
            for (_uint i = 1; i < Data.iNumAnimations; ++i)
            {
                m_pModel->Get_Model()->Add_Animation(Data.pAnimations[i]);
            }
            m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();

            Refresh_TimeRanges();

            Safe_Delete_BinaryData(Data);
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

    Refresh_TimeRanges();
}

void CTool_Animation::Export_Animation(CGameInstance* pGameInstance)
{
    //// display
    //if (ImGuiFileDialog::Instance()->Display("ChooseSaveFileKey"))
    //{
    //    // action if OK
    //    if (ImGuiFileDialog::Instance()->IsOk())
    //    {
    //        string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
    //        string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
    //        // action

    //        _ulong dwByte = { 0 };
    //        /* Model 저장 */
    //        HANDLE hFile = CreateFileA(filePathName.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    //        if (INVALID_HANDLE_VALUE == hFile)
    //        {
    //            MSG_BOX("Failed Open Save File");
    //            return;
    //        }

    //        CloseHandle(hFile);

    //        MSG_BOX("Success Save Files");
    //    }
    //    // close
    //    ImGuiFileDialog::Instance()->Close();
    //}

    list<string> FilePathList;
    FilePathList.push_back(m_strFilePath);

    vector<MODEL_BINARYDATA> Temp;
    MODEL_BINARYDATA Data = m_pModel->Get_Model_BinaryData();

    Data.pAnimations = nullptr;

    ANIMATIONDATA* pAnimations = New ANIMATIONDATA[m_vecAnimationDatas.size()];

    /* 따로 저장한 Notify 데이터를 작업한 데이터에 옮기기 */
    Store_NotifyDatas();

    /* 저장 할 데이터에 작업한 데이터를 대입 */
    _uint iNumAnimation = 0;
    for (auto& AnimData : m_vecAnimationDatas)
    {
        pAnimations[iNumAnimation] = AnimData;
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

void CTool_Animation::KeyFrameSetting(CGameInstance* pGameInstance)
{
    ImGui::SeparatorText("KeyFrame Setting");
    _float3 vTranslation;
    _float fTime = { 0.f };
    _float fOffset = { 0 };

    _uint iRootFrameIndex = TOOL->m_pAnimationWindow->m_iCurrentRootFrame;
    _uint iMaxFrameIndex = TOOL->m_pAnimationWindow->m_iCurrentFrame;
    vTranslation = m_pModel->Get_Model()->Get_Translation(iRootFrameIndex);
    fTime = m_pModel->Get_Model()->Get_KeyFrameTime(iMaxFrameIndex);

    _float arr[3] = { vTranslation.x, vTranslation.y, vTranslation.z };

    _float arrOffsets[3] = { 0.f, 0.f, 0.f };

    ImGui::DragFloat3("Translation", arr, 0.01f, -100.f, 100.f, "%.2f");
    if (ImGui::DragFloat3("Offsets##Translation", arrOffsets, 0.01f, -100.f, 100.f, "%.2f"))
    {
        for (_uint i = 0; i < m_pModel->Get_Model()->Get_MaxRootKeyFrame(); ++i)
        {
            vTranslation = m_pModel->Get_Model()->Get_Translation(i);
            vTranslation.x += arrOffsets[0];
            vTranslation.y += arrOffsets[1];
            vTranslation.z += arrOffsets[2];

            m_pModel->Get_Model()->Set_Translation(i, vTranslation);
        }
    }

    else
    {
        vTranslation = _float3(arr[0], arr[1], arr[2]);
        m_pModel->Get_Model()->Set_Translation(iRootFrameIndex, vTranslation);
    }

    ImGui::DragFloat("Time", &fTime, 0.001f, 0.f, 500.f);

    if (ImGui::DragFloat("Offset", &fOffset, 0.001f, -1.f, 1.f))
    {
        for (_uint i = 0; i < m_pModel->Get_Model()->Get_MaxKeyFrame(); ++i)
        {
            fTime = m_pModel->Get_Model()->Get_KeyFrameTime(i);

            fTime = (fTime + fOffset < 0.f) ? 0.f : fTime + fOffset;

            m_pModel->Get_Model()->Set_KeyFrameTime(i, fTime);
        }
    }

    else
        m_pModel->Get_Model()->Set_KeyFrameTime(iMaxFrameIndex, fTime);

    _float fDuration = m_pModel->Get_Model()->Get_Duration();
    ImGui::DragFloat("Duration", &fDuration, 0.001f, 0.f, 1000.f);
    m_pModel->Get_Model()->Set_Duration(fDuration);

    _float fTickPerSec = m_pModel->Get_Model()->Get_TickPerSec();
    ImGui::DragFloat("TickPerSec", &fTickPerSec, 0.001f, 0.f, 1000.f);
    m_pModel->Get_Model()->Set_TickPerSec(fTickPerSec);

    if(ImGui::Button("Apply##Animation Datas"))
    {
        Release_Animation_Data();
        m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();

        _uint iAnimIndex = 0;
        for (auto& Data : m_vecAnimationDatas)
        {
            vector<TIMERANGE> vecTimeRanges;

            for(_uint i = 0; i < Data.iNumRanges; ++i)
                vecTimeRanges.push_back(Data.pTimeRanges[i]);

            m_vecRanges.push_back(vecTimeRanges);
        }
        TOOL->m_pAnimationWindow->Refresh_Animation();
    }
}

void CTool_Animation::NotifySetting(CGameInstance* pGameInstance)
{
    ImGui::SeparatorText("Notify Setting");

    if (ImGui::Button("Create Time Range"))
    {
        TIMERANGE Data;
        m_vecRanges[m_iCurrentAnimationIndex].push_back(Data);
    }

    if (0 < m_vecRanges[m_iCurrentAnimationIndex].size())
    {
        ImGui::SetNextItemWidth(50.f);

        static _uint iSelected = { 0 };
        string* pItems = New string[m_vecRanges[m_iCurrentAnimationIndex].size()];
        const _char** ppItem = New const _char * [m_vecRanges[m_iCurrentAnimationIndex].size()];
        for (_uint i = 0; i < m_vecRanges[m_iCurrentAnimationIndex].size(); ++i)
        {
            pItems[i] = to_string(i);
            ppItem[i] = pItems[i].c_str();
        }

        if (iSelected >= m_vecRanges[m_iCurrentAnimationIndex].size())
            iSelected = 0;
        
        if (ImGui::BeginCombo("##Ranges", ppItem[iSelected]))
        {
            for (_uint i = 0; i < m_vecRanges[m_iCurrentAnimationIndex].size(); ++i)
            {
                const bool is_selected = (iSelected == i);
                if (ImGui::Selectable(pItems[i].c_str(), is_selected))
                    iSelected = i;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            m_vecRanges[m_iCurrentAnimationIndex][0];
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        string strLabel;
        strLabel = "StartPoint##" + to_string(m_iCurrentAnimationIndex) + to_string(iSelected);
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat(strLabel.c_str(), &m_vecRanges[m_iCurrentAnimationIndex][iSelected].fStartPoint, 0.1f, 0.f, 1000.f, "%.1f");

        ImGui::SameLine();
        strLabel = "EndPoint##" + to_string(m_iCurrentAnimationIndex) + to_string(0);
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat(strLabel.c_str(), &m_vecRanges[m_iCurrentAnimationIndex][iSelected].fEndPoint, 0.1f, 0.f, 1000.f, "%.1f");

        Safe_Delete_Array(pItems);
        Safe_Delete_Array(ppItem);

        ImGui::Separator();

        static _uint iSelectObsever = { 0 };

        if(ImGui::Button("Add_Observer"))
            ImGui::OpenPopup("Add Observer");

        if (ImGui::BeginPopupContextItem("Add Observer"))
        {
            if (ImGui::Selectable("Animation"))
            {
                if (OBSERVERTYPE::TYPE_ANIMATION != m_vecRanges[m_iCurrentAnimationIndex][iSelected].arrTypes[OBSERVERTYPE::TYPE_ANIMATION])
                    m_vecRanges[m_iCurrentAnimationIndex][iSelected].arrTypes[OBSERVERTYPE::TYPE_ANIMATION] = OBSERVERTYPE::TYPE_ANIMATION;
            }
            if (ImGui::Selectable("Collider"))
            {
                if (OBSERVERTYPE::TYPE_COLLIDER != m_vecRanges[m_iCurrentAnimationIndex][iSelected].arrTypes[OBSERVERTYPE::TYPE_COLLIDER])
                    m_vecRanges[m_iCurrentAnimationIndex][iSelected].arrTypes[OBSERVERTYPE::TYPE_COLLIDER] = OBSERVERTYPE::TYPE_COLLIDER;
            }
            if (ImGui::Selectable("Sound"))
            {
                if (OBSERVERTYPE::TYPE_SOUND != m_vecRanges[m_iCurrentAnimationIndex][iSelected].arrTypes[OBSERVERTYPE::TYPE_SOUND])
                    m_vecRanges[m_iCurrentAnimationIndex][iSelected].arrTypes[OBSERVERTYPE::TYPE_SOUND] = OBSERVERTYPE::TYPE_SOUND;
            }
            ImGui::EndPopup();
        }
        ImGui::Button("Delete##Observer");

        ImGui::SameLine(200);

        if (ImGui::BeginChild("##Observer", ImVec2(100, 100)))
        {
            ImGui::Text("Observers");

            _uint i = 0;
            for (auto& Type : m_vecRanges[m_iCurrentAnimationIndex][iSelected].arrTypes)
            {
                if (i == Type)
                {
                    switch (Type)
                    {
                    case OBSERVERTYPE::TYPE_ANIMATION:
                        if (ImGui::Selectable("Animation", iSelectObsever == i))
                            iSelectObsever = i;
                        break;

                    case OBSERVERTYPE::TYPE_COLLIDER:
                        if (ImGui::Selectable("Collider", iSelectObsever == i))
                            iSelectObsever = i;
                        break;

                    case OBSERVERTYPE::TYPE_SOUND:
                        if (ImGui::Selectable("Sound", iSelectObsever == i))
                            iSelectObsever = i;
                        break;
                    }
                }
                ++i;
            }
            ImGui::EndChild();
        }
    }
}

void CTool_Animation::Release_Animation_Data()
{
    for (auto& pAnimation : m_vecAnimationDatas)
    {
        Safe_Delete_Array(pAnimation.pTimeRanges);

        for (_uint i = 0; i < pAnimation.iNumChannels; ++i)
        {
            Safe_Delete_Array(pAnimation.pChannels[i].pKeyFrames);
        }
        Safe_Delete_Array(pAnimation.pChannels);
    }
    m_vecAnimationDatas.clear();
    m_vecRanges.clear();
}

void CTool_Animation::Store_NotifyDatas()
{
    for (auto& Data : m_vecAnimationDatas)
    {
        Safe_Delete_Array(Data.pTimeRanges);
    }

    _uint iIndex = { 0 };
    for (auto& Data : m_vecAnimationDatas)
    {
        Data.iNumRanges = m_vecRanges[iIndex].size();

        if(0 < Data.iNumRanges)
            Data.pTimeRanges = new TIMERANGE[Data.iNumRanges];

        for (_uint iRangeIndex = 0; iRangeIndex < Data.iNumRanges; ++iRangeIndex)
        {
            Data.pTimeRanges[iRangeIndex].fStartPoint = m_vecRanges[iIndex][iRangeIndex].fStartPoint;
            Data.pTimeRanges[iRangeIndex].fEndPoint = m_vecRanges[iIndex][iRangeIndex].fEndPoint;

            for (_uint iTypeIndex = 0; iTypeIndex < OBSERVERTYPE::TYPE_END; ++iTypeIndex)
                Data.pTimeRanges[iRangeIndex].arrTypes[iTypeIndex] = m_vecRanges[iIndex][iRangeIndex].arrTypes[iTypeIndex];
        }
        ++iIndex;
    }
}

void CTool_Animation::Refresh_TimeRanges()
{
    for (auto& pAnimation : m_vecAnimationDatas)
    {
        vector<TIMERANGE> vecTimeRange;
        for (_uint j = 0; j < pAnimation.iNumRanges; ++j)
        {
            TIMERANGE TimeRange;
            TimeRange.fStartPoint = pAnimation.pTimeRanges[j].fStartPoint;
            TimeRange.fEndPoint = pAnimation.pTimeRanges[j].fEndPoint;

            for (_uint k = 0; k < OBSERVERTYPE::TYPE_END; ++k)
                TimeRange.arrTypes[k] = pAnimation.pTimeRanges[j].arrTypes[k];

            vecTimeRange.push_back(TimeRange);
        }
        m_vecRanges.push_back(vecTimeRange);
    }
}

void CTool_Animation::Free()
{
    Release_Animation_Data();
    Safe_Release(m_pModel);
}
