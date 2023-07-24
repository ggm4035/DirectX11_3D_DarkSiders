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

            /* Animation Data Table ��� */
            _uint iIndex = 0;
            for (auto& Data : m_vecAnimationDatas)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable(std::to_string(iIndex).c_str(), m_iCurAnimIndex == iIndex))
                {
                    m_iCurAnimIndex = iIndex;
                    m_pModel->Get_Model()->Change_Animation(m_vecAnimationDatas[iIndex].szName);
                    TOOL->m_pAnimationWindow->Refresh_Animation();
                }

                ImGui::TableSetColumnIndex(1);
                string label = "##AnimationTag" + std::to_string(iIndex);
                ImGui::SetNextItemWidth(300);
                ImGui::InputText(label.c_str(), Data.szName, 256);


                // 3��° �� �ε����� �̵��Ѵٴ� ��
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
                string strTag = m_vecAnimationDatas[m_iCurAnimIndex].szName;
                m_pModel->Get_Model()->Delete_Animation(strTag);
                m_iCurAnimIndex = 0 > --m_iCurAnimIndex ? 0 : m_iCurAnimIndex;

                m_pModel->Get_Model()->Change_Animation(m_vecAnimationDatas[m_iCurAnimIndex].szName);
                m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
                TOOL->m_pAnimationWindow->Refresh_Animation();
            }

            ImGui::SameLine();

            /* ��ġ �����ϴ°ǵ� ���߿� ��ġ�� ������� */
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
            vector<ANIMATIONDATA> vecAnimationData;
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
                m_pModel->Get_Model()->Add_Animation(Data.vecAnimations[i]);
            }
            m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();

            strFilePath = strFilePath.substr(0, strFilePath.find(".dat"));
            strFilePath += "_Notify.dat";
            pGameInstance->Read_Notify_Data(pGameInstance->strToWStr(strFilePath), vecAnimationData);

            _uint iAnimIndex = { 0 };
            for (auto& animation : vecAnimationData)
                m_vecAnimationDatas[iAnimIndex++].vecNotifyDesc = animation.vecNotifyDesc;

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
            m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
            return;
        }
    }

    /* �� �κ��� ��� �ִϸ��̼� ���� ���� �ϳ��� �ִϸ��̼Ǹ� ������ �ִٰ� �����ϰ� �����ϴ� ����. */
    m_pModel->Get_Model()->Add_Animation(vecAnimData[0]);

    m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
}

void CTool_Animation::Export_Animation(CGameInstance* pGameInstance)
{
    list<string> FilePathList;
    FilePathList.push_back(m_strFilePath);

    vector<MODEL_BINARYDATA> Temp;
    MODEL_BINARYDATA Data = m_pModel->Get_Model_BinaryData();

    /* ���� �� �����Ϳ� �۾��� �����͸� ���� */
    Data.iNumAnimations = m_vecAnimationDatas.size();
    Data.vecAnimations = m_vecAnimationDatas;

    Temp.push_back(Data);

    _char szFullPath[MAX_PATH] = { "" };
    GetCurrentDirectoryA(MAX_PATH, szFullPath);
    string strFullPath = { "" };
    strFullPath = strFullPath + szFullPath + "\\Out\\";

    /* Export Model Data */
    pGameInstance->WriteModels(strFullPath, FilePathList, Temp);

    /* Export Notify Data */
    wstring wstrName = Data.szTag;
    wstrName = wstrName.substr(wstrName.find(L"_") + 1, wstrName.size() - 1);

    wstring wstrFilePath = pGameInstance->strToWStr(strFullPath);
    wstrFilePath += wstrName + L"_Notify.dat";

    pGameInstance->Write_Notify_Data(wstrFilePath, m_vecAnimationDatas);

    /* Remove Datas */
    pGameInstance->Remove_GameObject(m_pModel->Get_Tag());
    m_pModel = nullptr;
    m_vecAnimationDatas.clear();
    TOOL->m_pAnimationWindow->Set_Model(nullptr);
    TOOL->m_pCamera->Set_OriginView();
    WINDOWMGR->Refresh_All_Window();

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
    _float arrPer[3] = { 1.f, 1.f, 1.f };

    if (ImGui::DragFloat3("Persentage##Translation", arrPer, 0.01f, -100.f, 100.f, "%.2f"))
    {
        for (_uint i = 0; i < m_pModel->Get_Model()->Get_MaxRootKeyFrame(); ++i)
        {
            vTranslation = m_pModel->Get_Model()->Get_Translation(i);
            vTranslation.x *= arrPer[0];
            vTranslation.y *= arrPer[1];
            vTranslation.z *= arrPer[2];

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

    if (ImGui::Button("Apply##Animation Datas"))
    {
        m_vecAnimationDatas = m_pModel->Get_Model()->Get_AnimationDatas();
        TOOL->m_pAnimationWindow->Refresh_Animation();
    }
}

void CTool_Animation::NotifySetting(CGameInstance* pGameInstance)
{
    ImGui::SeparatorText("Notify Setting");

    if (ImGui::Button("Create Notify Point"))
    {
        NOTIFYDATA NotifyData;
        m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc.push_back(NotifyData);
    }

    ImGui::SameLine();

    if (0 < m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc.size())
    {
        if (ImGui::Button("Add_Notify"))
            m_isAddNotify = true;

        ImGui::SetNextItemWidth(50.f);

        string* pItems = New string[m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc.size()];
        const _char** ppItem = New const _char * [m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc.size()];
        for (_uint i = 0; i < m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc.size(); ++i)
        {
            pItems[i] = to_string(i);
            ppItem[i] = pItems[i].c_str();
        }

        if (m_iCurNotifyIndex >= m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc.size())
            m_iCurNotifyIndex = 0;

        if (ImGui::BeginCombo("##Point", ppItem[m_iCurNotifyIndex]))
        {
            for (_uint i = 0; i < m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc.size(); ++i)
            {
                const bool is_selected = (m_iCurNotifyIndex == i);
                if (ImGui::Selectable(pItems[i].c_str(), is_selected))
                    m_iCurNotifyIndex = i;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        string strLabel;
        strLabel = "NotifyPoint##" + to_string(m_iCurAnimIndex) + to_string(m_iCurNotifyIndex);
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat(strLabel.c_str(), &m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc[m_iCurNotifyIndex].fPoint, 0.1f, 0.f, 1000.f, "%.1f");

        Safe_Delete_Array(pItems);
        Safe_Delete_Array(ppItem);

        ImGui::Separator();

        for (auto& Observer : m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc[m_iCurNotifyIndex].vecObservers)
        {
            string strTag = pGameInstance->wstrToStr(Observer.wstrNotifyTag);
            ImGui::Text(strTag.c_str());
        }
    }

    if (true == m_isAddNotify)
        Add_Notify(pGameInstance);
}

void CTool_Animation::Add_Notify(CGameInstance* pGameInstance)
{
    ImGui::SeparatorText("Add_Notify##text");

    if (ImGui::BeginChild("Add_Notify##Child"))
    {
        if (ImGui::Button("Animation##Add_Notify"))
        {
            wstring wstrTag = pGameInstance->strToWStr(m_vecAnimationDatas[m_iCurAnimIndex].szName);
            if (nullptr == Find_NotifyTag(wstrTag))
            {
                OBSERVERDATA ObserverData;
                ObserverData.iObserverType = 0;
                ObserverData.isEnable = true;
                ObserverData.wstrNotifyTag = wstrTag;
                m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc[m_iCurNotifyIndex].vecObservers.push_back(ObserverData);
            }

            m_isAddNotify = false;
        }

        if (ImGui::Button("Collider##Add_Notify"))
            ImGui::OpenPopup("Add_Collider_Notify");

        ImGui::Button("Sound##Add_Notify");

        if (ImGui::BeginPopup("Add_Collider_Notify"))
        {
            static _bool isEnable = { false };
            ImGui::Text("is Enable Check is True");
            ImGui::Checkbox("isEnable", &isEnable);
            ImGui::InputText("##Input_Collider_Tag", m_szNotifyTag, MAX_PATH);
            if (ImGui::Button("Add_Collider_Notify##Button"))
            {
                OBSERVERDATA ObserverData;
                ObserverData.iObserverType = 1;
                ObserverData.isEnable = isEnable;
                wstring wstrTag = pGameInstance->strToWStr(m_szNotifyTag);
                ObserverData.wstrNotifyTag = wstrTag;
                if (nullptr == Find_NotifyTag(wstrTag))
                    m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc[m_iCurNotifyIndex].vecObservers.push_back(ObserverData);

                strcpy(m_szNotifyTag, " ");
                m_isAddNotify = false;
            }
            ImGui::EndPopup();
        }

        ImGui::EndChild();
    }

    ImGui::Separator();
}

wstring* CTool_Animation::Find_NotifyTag(const wstring& wstrTag)
{
    auto iter = find_if(m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc[m_iCurNotifyIndex].vecObservers.begin(),
        m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc[m_iCurNotifyIndex].vecObservers.end(), [&](const OBSERVERDATA ObserverData)
        {
            if (wstrTag == ObserverData.wstrNotifyTag)
                return true;
            else
                return false;
        });

    if (iter == m_vecAnimationDatas[m_iCurAnimIndex].vecNotifyDesc[m_iCurNotifyIndex].vecObservers.end())
        return nullptr;

    wstring ret = wstrTag;

    return &ret;
}

void CTool_Animation::Free()
{
    Safe_Release(m_pModel);
}
