#include "CImWindow_Top.h"

#include "CImWindow_Base.h"
#include "CImWindow_Create.h"
#include "CDummyObject3D.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"

#include "CTool_HeightMap.h"

IMPLEMENT_SINGLETON(CImWindow_Top)

HRESULT CImWindow_Top::Initialize()
{
    if (FAILED(CImWindow::Initialize()))
        return E_FAIL;

    m_strPickMode = "Picking OFF";

    return S_OK;
}

void CImWindow_Top::Tick(const _double& TimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImVec2 pos = ImGui::GetWindowPos();
    pos.x -= 57;
    pos.y -= 140;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
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

            for (_uint i = 0; i < LAYER_END; ++i)
            {
                for (auto& pObject : m_GameObjectList[i])
                    pObject->Set_RasterizerState(RasterizerDesc);
            }

            m_isPickSolidMode = false;
            m_isSolidMode = true;
        }
        if (ImGui::RadioButton("Pick Solid the rest WireFrame", &iMode, 1))
        {
            RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };
            for (_uint i = 0; i < LAYER_END; ++i)
            {
                for (auto& pObject : m_GameObjectList[i])
                    pObject->Set_RasterizerState(RasterizerDesc);
            }

            RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
            if (nullptr != TOOL->m_pCurrentObject)
                TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);

            m_isPickSolidMode = true;
            m_isSolidMode = false;
        }
        if (ImGui::RadioButton("All WireFrame", &iMode, 2))
        {
            RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };
            for (_uint i = 0; i < LAYER_END; ++i)
            {
                for (auto& pObject : m_GameObjectList[i])
                    pObject->Set_RasterizerState(RasterizerDesc);
            }

            m_isPickSolidMode = false;
            m_isSolidMode = false;
        }
        ImGui::EndPopup();
    }

    if (ImGui::Button("Render Mode", ImVec2(100, 40)))
        ImGui::OpenPopup("Render Mode");
}

void CImWindow_Top::PickingMode(CGameInstance* pGameInstance)
{
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

            _ulong dwByte = { 0 };
            /* Model 저장 */
            HANDLE hFile = CreateFileA(filePathName.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (INVALID_HANDLE_VALUE == hFile)
            {
                MSG_BOX("Failed Open Save File");
                return;
            }

            auto iter = m_GameObjectList[LAYER_STATIC].begin();

            /* 1. Terrain을 저장한다. */
            _uint iXCount = TOOL->m_pBaseWindow->m_pToolHeightMap->m_iXCount;
            _uint iZCount = TOOL->m_pBaseWindow->m_pToolHeightMap->m_iZCount;

            /* Write iXCount */
            WriteFile(hFile, &iXCount, sizeof(_uint), &dwByte, nullptr);

            /* Write iZCount */
            WriteFile(hFile, &iZCount, sizeof(_uint), &dwByte, nullptr);

            /* Write pPositions */
            D3D11_MAPPED_SUBRESOURCE MappedSubResource;
            ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

            (*iter)->Get_Buffer()->Begin(MappedSubResource);
            VTXPOSNORTEX* pVertices = reinterpret_cast<VTXPOSNORTEX*>(MappedSubResource.pData);

            for (_uint i = 0; i < iXCount * iZCount; ++i)
                WriteFile(hFile, &pVertices[i].vPosition, sizeof(_float3), &dwByte, nullptr);

            (*iter)->Get_Buffer()->End();

            ++iter;

            /* 2. 전체 3D 게임오브젝트들을 저장한다. */
            _uint iNumObjects = m_GameObjectList[LAYER_STATIC].size() - 2;
            WriteFile(hFile, &iNumObjects, sizeof(_uint), &dwByte, nullptr);

            for (_uint i = 1; i < m_GameObjectList[LAYER_STATIC].size() - 1; ++i, ++iter)
            {
                /* Write szObjectTag */
                _uint iTaglength = lstrlen((*iter)->Get_Model()->Get_Tag().c_str()) + 1;
                WriteFile(hFile, &iTaglength, sizeof(_uint), &dwByte, nullptr);
                WriteFile(hFile, (*iter)->Get_Tag().c_str(), sizeof(_tchar) * iTaglength, &dwByte, nullptr);

                /* Write TransformMatrix */
                WriteFile(hFile, &(*iter)->Get_Transform()->Get_WorldFloat4x4(), sizeof(_float4x4), &dwByte, nullptr);

                /* Write vAngle */
                WriteFile(hFile, &(*iter)->Get_Transform()->Get_Angle(), sizeof(_float3), &dwByte, nullptr);

                /* Write Navigation Index */
                WriteFile(hFile, &(*iter)->m_iNavigationIndex, sizeof(_uint), &dwByte, nullptr);

                /* Write Binary Datas */
                Write_BinData(hFile, (*iter)->Get_Model_BinaryData(), dwByte);
            }

            auto iterMonster = m_GameObjectList[LAYER_MONSTER].begin();

            /* 3. Monster 저장 */
            iNumObjects = m_GameObjectList[LAYER_MONSTER].size();
            WriteFile(hFile, &iNumObjects, sizeof(_uint), &dwByte, nullptr);

            for (_uint i = 0; i < iNumObjects; ++i, ++iterMonster)
            {
                /* Write szObjectTag */
                _uint iTaglength = lstrlen((*iterMonster)->Get_Model()->Get_Tag().c_str()) + 1;
                WriteFile(hFile, &iTaglength, sizeof(_uint), &dwByte, nullptr);
                WriteFile(hFile, (*iterMonster)->Get_Tag().c_str(), sizeof(_tchar) * iTaglength, &dwByte, nullptr);

                /* Write TransformMatrix */
                WriteFile(hFile, &(*iterMonster)->Get_Transform()->Get_WorldFloat4x4(), sizeof(_float4x4), &dwByte, nullptr);

                /* Write vAngle */
                WriteFile(hFile, &(*iterMonster)->Get_Transform()->Get_Angle(), sizeof(_float3), &dwByte, nullptr);

                /* Write Navigation Index */
                WriteFile(hFile, &(*iterMonster)->m_iNavigationIndex, sizeof(_uint), &dwByte, nullptr);

                /* Write Binary Datas */
                Write_BinData(hFile, (*iterMonster)->Get_Model_BinaryData(), dwByte);
            }

            /* 4. Player 저장 */
            WriteFile(hFile, &(*iter)->Get_Transform()->Get_WorldFloat4x4(), sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &(*iter)->Get_Transform()->Get_Angle(), sizeof(_float3), &dwByte, nullptr);

            /* 5. 카메라를 저장한다. */

            /* 6. UI를 저장한다. */

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

            for (_uint i = 0; i < LAYER_END; ++i)
            {
                for (auto& pObject : m_GameObjectList[i])
                    pGameInstance->Remove_GameObject(pObject->Get_Tag());
            }

            FILEDATA FileData;

            if (FAILED(pGameInstance->Load(filePathName, FileData)))
            {
                MSG_BOX("Failed Load Files");
                return;
            }
            else
                MSG_BOX("Success Load Files");

            /* Load Terrain */
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
                L"Terrain", L"Layer_Tool")))
                return;

            WINDOWMGR->Refresh_All_Window();

            CDummyObject3D* pObject = Find_GameObject(L"Terrain");
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
            pObject->Add_Buffer(L"VIBuffer_Terrain");
            pObject->Add_Shader(L"Shader_VtxNorTex");
            pObject->Add_Texture(L"Texture_Terrain"); // 이거는 바뀔 수 있음

            /* Position Setting */
            dynamic_cast<CVIBuffer_Terrain*>(pObject->Get_Buffer())->Load_Terrain(FileData.iXCount, FileData.iZCount, FileData.pPositions);

            TOOL->m_pBaseWindow->m_pToolHeightMap->m_iXCount = FileData.iXCount;
            TOOL->m_pBaseWindow->m_pToolHeightMap->m_iZCount = FileData.iZCount;

            pObject->Set_Tag(L"Terrain");

            TOOL->m_pBaseWindow->m_pToolHeightMap->Set_Terrain(pObject);

            TOOL->m_pCurrentObject = pObject;

            /* Load Static GameObjects */
            _uint idx = { 0 };
            for (auto& Data : FileData.vecModelData)
            {
                _char szNum[8] = { "" };
                _char szObjName[256] = { "" };
                strcpy_s(szObjName, "Static_Object");
                _itoa_s(idx, szNum, 10);
                strcat_s(szObjName, szNum);

                if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
                    pGameInstance->strToWStr(szObjName), L"Layer_Tool")))
                    return;

                WINDOWMGR->Refresh_All_Window();

                pObject = Find_GameObject(pGameInstance->strToWStr(szObjName));
                if (nullptr == pObject)
                    return;

                pObject->Get_Transform()->Set_Matrix(Data.TransformMatrix);
                pObject->Get_Transform()->Set_Angle(Data.vAngle);
                pObject->m_iNavigationIndex = Data.iNavigationIndex;

                /* Rasterizer */
                RasterizerDesc.CullMode = { D3D11_CULL_BACK };
                RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
                RasterizerDesc.FrontCounterClockwise = { false };

                pObject->Set_RasterizerState(RasterizerDesc);

                /* Components */
                pObject->Add_Shader(L"Shader_Mesh");
                pObject->Add_Model(Data.BinaryData.szTag);

                pObject->Set_Tag(pGameInstance->strToWStr(szObjName));
                ++idx;
            }

            /* Load Monsters */
            idx = 0;
            for (auto& Data : FileData.vecMonsterData)
            {
                _char szNum[8] = { "" };
                _char szObjName[256] = { "" };
                string strObjName = "Monster";
                strcpy_s(szObjName, strObjName.c_str());
                _itoa_s(idx, szNum, 10);
                strcat_s(szObjName, szNum);

                if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Dummy3D",
                    pGameInstance->strToWStr(szObjName), L"Layer_Monster")))
                    return;

                WINDOWMGR->Refresh_All_Window();

                pObject = Find_GameObject(pGameInstance->strToWStr(szObjName), LAYER_MONSTER);
                if (nullptr == pObject)
                    return;

                pObject->Get_Transform()->Set_Matrix(Data.TransformMatrix);
                pObject->Get_Transform()->Set_Angle(Data.vAngle);
                pObject->m_iNavigationIndex = Data.iNavigationIndex;

                /* Rasterizer */
                RasterizerDesc.CullMode = { D3D11_CULL_BACK };
                RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
                RasterizerDesc.FrontCounterClockwise = { false };

                pObject->Set_RasterizerState(RasterizerDesc);

                /* Components */
                pObject->Add_Shader(L"Shader_AnimMesh");
                pObject->Add_Model(Data.BinaryData.szTag);

                pObject->Set_Tag(pGameInstance->strToWStr(szObjName));
                ++idx;
            }

            for (auto& Data : FileData.vecModelData)
                Safe_Delete_BinaryData(Data.BinaryData);

            for (auto& Data : FileData.vecMonsterData)
                Safe_Delete_BinaryData(Data.BinaryData);

            Safe_Delete_Array(FileData.pPositions);
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void CImWindow_Top::Write_BinData(HANDLE hFile, MODEL_BINARYDATA& Data, _ulong dwByte)
{
    /* Write szTag */
    _uint iPathLength = { 0 };
    iPathLength = lstrlen(Data.szTag) + 1;
    WriteFile(hFile, &iPathLength, sizeof(_uint), &dwByte, nullptr);
    WriteFile(hFile, Data.szTag, sizeof(_tchar) * iPathLength, &dwByte, nullptr);

    /* Write iNumMeshes */
    WriteFile(hFile, &Data.iNumMeshes, sizeof(_uint), &dwByte, nullptr);

    /* Write iNumMaterials */
    WriteFile(hFile, &Data.iNumMaterials, sizeof(_uint), &dwByte, nullptr);

    /* Write szMaterialTexturePath */
    _uint iTexturePathlength = { 0 };
    for (_uint iMaterialIndex = 0; iMaterialIndex < Data.iNumMaterials; ++iMaterialIndex)
    {
        for (_uint iTextureIndex = 0; iTextureIndex < 21; ++iTextureIndex)
        {
            iTexturePathlength = lstrlen(Data.pMaterialPaths[iMaterialIndex].szMaterialTexturePath[iTextureIndex]) + 1;
            WriteFile(hFile, &iTexturePathlength, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, Data.pMaterialPaths[iMaterialIndex].szMaterialTexturePath[iTextureIndex],
                sizeof(_tchar) * iTexturePathlength, &dwByte, nullptr);
        }
    }

    /* Write iNumBones */
    WriteFile(hFile, &Data.iNumBones, sizeof(_uint), &dwByte, nullptr);

    /*===== BONE DATAS ======*/
    if (0 < Data.iNumBones)
    {
        for (_uint iBoneIndex = 0; iBoneIndex < Data.iNumBones; ++iBoneIndex)
        {
            /* Write szName_Bone */
            _uint iNameLength = strlen(Data.pBoneDatas[iBoneIndex].szName) + 1;
            WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, Data.pBoneDatas[iBoneIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

            /* Write iParentIdx */
            WriteFile(hFile, &Data.pBoneDatas[iBoneIndex].iParentIdx, sizeof(_uint), &dwByte, nullptr);

            /* Write iIndex */
            WriteFile(hFile, &Data.pBoneDatas[iBoneIndex].iIndex, sizeof(_uint), &dwByte, nullptr);

            /* Write TransformationMatrix */
            WriteFile(hFile, &Data.pBoneDatas[iBoneIndex].TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

            /* Write OffsetMatrix */
            WriteFile(hFile, &Data.pBoneDatas[iBoneIndex].OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);
        }
    }

    /* Write iNumAnimations */
    WriteFile(hFile, &Data.iNumAnimations, sizeof(_uint), &dwByte, nullptr);

    /*===== ANIMATION DATAS ======*/
    for (auto& AnimationData : Data.vecAnimations)
    {
        /* Write szName */
        _uint iNameLength = strlen(AnimationData.szName) + 1;
        WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
        WriteFile(hFile, AnimationData.szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

        /* Write Duration */
        WriteFile(hFile, &AnimationData.Duration, sizeof(_double), &dwByte, nullptr);

        /* Write TickPerSec */
        WriteFile(hFile, &AnimationData.TickPerSec, sizeof(_double), &dwByte, nullptr);

        /* Write iNumChannels */
        WriteFile(hFile, &AnimationData.iNumChannels, sizeof(_uint), &dwByte, nullptr);

        /* Write bIsLoop */
        WriteFile(hFile, &AnimationData.bIsLoop, sizeof(_bool), &dwByte, nullptr);

        /* Write bIsFollowAnimation */
        WriteFile(hFile, &AnimationData.bIsFollowAnimation, sizeof(_bool), &dwByte, nullptr);

        /* Write iNumRanges */
        WriteFile(hFile, &AnimationData.iNumPoints, sizeof(_uint), &dwByte, nullptr);

        /* Write NotifyDesc */
        for (auto& Notify : AnimationData.vecNotifyDesc)
        {
            WriteFile(hFile, &Notify.fPoint, sizeof(_float), &dwByte, nullptr);

            _uint iNumTags = { 0 };
            WriteFile(hFile, &iNumTags, sizeof(_uint), &dwByte, nullptr);
            for (auto& Tags : Notify.vecNotifyTags)
            {
                _uint iTagLength = Tags.size() + 1;
                WriteFile(hFile, &iTagLength, sizeof(_uint), &dwByte, nullptr);
                WriteFile(hFile, Tags.c_str(), sizeof(_tchar) * iTagLength, &dwByte, nullptr);
            }
        }

        /*===== CHANNEL DATAS ======*/
        for (auto& ChannelData : AnimationData.vecChannels)
        {
            /* Write szName */
            iNameLength = strlen(ChannelData.szName) + 1;
            WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, ChannelData.szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

            /* Write iNumKeyFrames */
            WriteFile(hFile, &ChannelData.iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

            /* Write KeyFrame */
            for (auto& KeyFrameData : ChannelData.vecKeyFrames)
                WriteFile(hFile, &KeyFrameData, sizeof(KEYFRAME), &dwByte, nullptr);
        }
    }

    /*===== MESH DATAS ======*/
    for (_uint iMeshIndex = 0; iMeshIndex < Data.iNumMeshes; ++iMeshIndex)
    {
        /* Write szName_Mesh */
        _uint iNameLength = strlen(Data.pMeshDatas[iMeshIndex].szName) + 1;
        WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
        WriteFile(hFile, Data.pMeshDatas[iMeshIndex].szName, sizeof(_char) * iNameLength, &dwByte, nullptr);

        /* Write iMaterialIndex */
        WriteFile(hFile, &Data.pMeshDatas[iMeshIndex].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

        /* Write iNumNonAnimVertices */
        WriteFile(hFile, &Data.pMeshDatas[iMeshIndex].iNumNonAnimVertices, sizeof(_uint), &dwByte, nullptr);

        /* Write iNumAnimVertices */
        WriteFile(hFile, &Data.pMeshDatas[iMeshIndex].iNumAnimVertices, sizeof(_uint), &dwByte, nullptr);

        /* Write iNumIndices */
        WriteFile(hFile, &Data.pMeshDatas[iMeshIndex].iNumIndices, sizeof(_uint), &dwByte, nullptr);

        /* Write iNumMeshBones */
        WriteFile(hFile, &Data.pMeshDatas[iMeshIndex].iNumMeshBones, sizeof(_uint), &dwByte, nullptr);

        /* Write pBoneIndices */
        WriteFile(hFile, Data.pMeshDatas[iMeshIndex].pBoneIndices, sizeof(_uint) *
            Data.pMeshDatas[iMeshIndex].iNumMeshBones, &dwByte, nullptr);

        /* Write pNonAnimVertices */
        if (0 < Data.pMeshDatas[iMeshIndex].iNumNonAnimVertices)
        {
            WriteFile(hFile, Data.pMeshDatas[iMeshIndex].pNonAnimVertices, sizeof(VTXMESH) *
                Data.pMeshDatas[iMeshIndex].iNumNonAnimVertices, &dwByte, nullptr);
        }

        /* Write pAnimVertices */
        if (0 < Data.pMeshDatas[iMeshIndex].iNumAnimVertices)
        {
            WriteFile(hFile, Data.pMeshDatas[iMeshIndex].pAnimVertices, sizeof(VTXANIMMESH) *
                Data.pMeshDatas[iMeshIndex].iNumAnimVertices, &dwByte, nullptr);
        }

        /* Write pIndices */
        WriteFile(hFile, Data.pMeshDatas[iMeshIndex].pIndices, sizeof(_ulong) *
            Data.pMeshDatas[iMeshIndex].iNumIndices, &dwByte, nullptr);
    }
}

void CImWindow_Top::Free()
{
    CImWindow::Free();
}
