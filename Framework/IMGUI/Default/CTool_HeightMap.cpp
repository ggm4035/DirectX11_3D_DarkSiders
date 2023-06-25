#include "CTool_HeightMap.h"

#include "CGameInstance.h"
#include "CDummyObject3D.h"

CTool_HeightMap::CTool_HeightMap()
{
    ZeroMemory(&m_vBrushPos, sizeof(_float4));
}

void CTool_HeightMap::Initialize()
{
    CGameInstance* pGaemInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGaemInstance);

    m_pBrushTextureCom = dynamic_cast<CTexture*>(pGaemInstance->Clone_Component(LEVEL_TOOL, L"Texture_Brush", m_pTerrain));
    if (nullptr == m_pBrushTextureCom)
        return;

    Safe_Release(pGaemInstance);
}

void CTool_HeightMap::Tick(CGameInstance* pGameInstance)
{
    if (nullptr == m_pTerrain)
    {
        ImGui::Text("Is Not Terrain");
        return;
    }

    /* Interface */
    static _int iSelected = { 0 };
    ImGui::SeparatorText("Mode");
    ImGui::RadioButton("Height", &iSelected, 0); ImGui::SameLine();
    ImGui::RadioButton("Filter", &iSelected, 1); ImGui::SameLine();
    ImGui::RadioButton("Navigation", &iSelected, 2);
    ImGui::Separator();

    if (2 == iSelected)
        Make_Navigation(pGameInstance);

    else
    {
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Brush Radius", &m_fRadius, 0.1f, 0.f, 50.f, "%.1f");

        /* Brush */
        POINT ptMouse;
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        XMStoreFloat4(&m_vBrushPos, pGameInstance->Picking_On_Triangle(ptMouse, m_pTerrain->Get_Buffer(), m_pTerrain->Get_Transform()));

        if (0 == iSelected)
            Control_Height(pGameInstance);
        else
            Draw_Filter(pGameInstance);

        /* Bind Shader Resources */
        if (FAILED(Bind_Shader_Resources()))
            return;
    }
}

HRESULT CTool_HeightMap::Bind_Shader_Resources()
{
    if (FAILED(m_pBrushTextureCom->Bind_ShaderResource(m_pTerrain->Get_Shader(), "g_BrushTexture")))
        return E_FAIL;

    if (FAILED(m_pTerrain->Get_Shader()->Bind_RawValue("g_vBrushPos", &m_vBrushPos, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTerrain->Get_Shader()->Bind_RawValue("g_fBrushRadius", &m_fRadius, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

void CTool_HeightMap::Control_Height(CGameInstance* pGameInstance)
{
    /* Interface */
    static _int iOption = { 0 };
    ImGui::SeparatorText("Control Height");
    ImGui::SetNextItemWidth(50.f);
    ImGui::DragFloat("Max Depth", &m_fMaxDepth, 0.1f, 0.f, 100.f, "%.1f"); ImGui::SameLine();
    ImGui::SetNextItemWidth(50.f);
    ImGui::DragFloat("Direction", &m_fDir, 0.01f, -1.f, 1.f, "%.2f");
    ImGui::Text("Sharpness");
    ImGui::RadioButton("Sharp", &iOption, 0); ImGui::SameLine();
    ImGui::RadioButton("Flat", &iOption, 1);
    if (1 == iOption)
        ImGui::DragFloat("Standard", &m_fStandard, 0.1f, -100.f, 100.f, "%.1f");
    else
        ImGui::DragFloat("Sharpness", &m_fSharpness, 0.01f, -10.f, 10.f, "%.2f");
    if (ImGui::Button("Apply Normal"))
        dynamic_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Buffer())->Load_Terrain();
    ImGui::Separator();

    /* Function */
    if (g_hWnd == ::GetFocus() && pGameInstance->Mouse_Pressing(CInput_Device::DIM_LB))
    {
        /* 여기서 피킹을 중심으로 반지름 만큼 정규분포표 함수나 곡선 함수를 이용해서 부드럽게 위로 올린다. */

        D3D11_MAPPED_SUBRESOURCE MappedSubResource;
        ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

        dynamic_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Buffer())->Begin(MappedSubResource);

        VTXPOSNORTEX* pVertices = reinterpret_cast<VTXPOSNORTEX*>(MappedSubResource.pData);
        if (nullptr == pVertices)
        {
            dynamic_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Buffer())->End();
            return;
        }

        for (_uint i = 0; i < m_iXCount * m_iZCount; ++i)
        {
            _float4 vSourPosition = _float4(m_vBrushPos.x, 0.f, m_vBrushPos.z, 1.f);
            _float4 vDestPosition = _float4(pVertices[i].vPosition.x, 0.f, pVertices[i].vPosition.z, 1.f);

            _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vSourPosition) - XMLoadFloat4(&vDestPosition)));

            if (m_fRadius < fDistance)
                continue;

            _float fRatio = 0.f;
            if (0 == iOption)
            {
                fRatio = 1.f - (fDistance / m_fRadius);
                fRatio = powf(fRatio, m_fSharpness) * m_fDir;

                if (pVertices[i].vPosition.y >= m_fMaxDepth * -1.f &&
                    pVertices[i].vPosition.y <= m_fMaxDepth)
                {
                    pVertices[i].vPosition.y += fRatio;

                    if (pVertices[i].vPosition.y > m_fMaxDepth)
                        pVertices[i].vPosition.y = m_fMaxDepth;
                    if (pVertices[i].vPosition.y < m_fMaxDepth * -1.f)
                        pVertices[i].vPosition.y = m_fMaxDepth * -1.f;

                    if (0 < i % (m_iXCount - 1))
                    {
                        m_pTerrain->Get_Buffer()->Set_Triangle_Dot(i * 2, 0, pVertices[i].vPosition);
                        m_pTerrain->Get_Buffer()->Set_Triangle_Dot(i * 2 + 1, 0, pVertices[i].vPosition);
                        m_pTerrain->Get_Buffer()->Set_Triangle_Dot((i + m_iXCount - 1) * 2 + 1, 2, pVertices[i].vPosition);
                        m_pTerrain->Get_Buffer()->Set_Triangle_Dot(i * 2 - 2, 1, pVertices[i].vPosition);
                        m_pTerrain->Get_Buffer()->Set_Triangle_Dot((i + m_iXCount - 1) * 2 - 1, 1, pVertices[i].vPosition);
                        m_pTerrain->Get_Buffer()->Set_Triangle_Dot((i + m_iXCount - 1) * 2 - 2, 2, pVertices[i].vPosition);
                    }
                }
            }
            else
            {
                if (pVertices[i].vPosition.y > m_fStandard ||
                    pVertices[i].vPosition.y < m_fStandard)
                    pVertices[i].vPosition.y = m_fStandard;

                if (0 < i % (m_iXCount - 1))
                {
                    m_pTerrain->Get_Buffer()->Set_Triangle_Dot(i * 2, 0, pVertices[i].vPosition);
                    m_pTerrain->Get_Buffer()->Set_Triangle_Dot(i * 2 + 1, 0, pVertices[i].vPosition);
                    m_pTerrain->Get_Buffer()->Set_Triangle_Dot((i + m_iXCount - 1) * 2 + 1, 2, pVertices[i].vPosition);
                    m_pTerrain->Get_Buffer()->Set_Triangle_Dot(i * 2 - 2, 1, pVertices[i].vPosition);
                    m_pTerrain->Get_Buffer()->Set_Triangle_Dot((i + m_iXCount - 1) * 2 - 1, 1, pVertices[i].vPosition);
                    m_pTerrain->Get_Buffer()->Set_Triangle_Dot((i + m_iXCount - 1) * 2 - 2, 2, pVertices[i].vPosition);
                }
            }
        }

        dynamic_cast<CVIBuffer_Terrain*>(m_pTerrain->Get_Buffer())->End();
    }
}

void CTool_HeightMap::Draw_Filter(CGameInstance* pGameInstance)
{
    /* Interface */
    ImGui::SeparatorText("Draw Filter");

    ImGui::Separator();

    /* Function */

}
//
//ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, pGameInstance->wstrToStr(pObjectTag).c_str());
//if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
//{
//    if (true == TOOL->m_pTopWindow->isPickSolidMode())
//    {
//        D3D11_RASTERIZER_DESC RasterizerDesc;
//        ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);
//
//        RasterizerDesc.CullMode = { D3D11_CULL_BACK };
//        RasterizerDesc.FrontCounterClockwise = { false };
//        RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };
//
//        if (nullptr != TOOL->m_pCurrentObject)
//            TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);
//
//        RasterizerDesc.FillMode = { D3D11_FILL_SOLID };
//
//        TOOL->m_pCurrentObject = Find_GameObject(pObjectTag);
//
//        if (nullptr != TOOL->m_pCurrentObject)
//            TOOL->m_pCurrentObject->Set_RasterizerState(RasterizerDesc);
//    }
//    else
//        TOOL->m_pCurrentObject = Find_GameObject(pObjectTag);
//}
/*
Position Setting
_float3 vPosition = m_pTerrain->Get_Navigation()->Get_CellPoint(iCurCellIndex, iCurPointIndex);
_float arrData[3] = { vPosition.x, vPosition.y, vPosition.z };
ImGui::DragFloat3("Position", arrData, 0.01f, 0.01f, 100.f, "%.2f");
vPosition = _float3(arrData[0], arrData[1], arrData[2]);
m_pTerrain->Get_Navigation()->Set_CellPosition(iCurCellIndex, iCurPointIndex, vPosition);
*/

void CTool_HeightMap::Make_Navigation(CGameInstance* pGameInstance)
{
    ImGui::SeparatorText("Navigation");

    /* Navigation List */
    ImGui::BeginChild("Navigations", ImVec2(330, 400), true, ImGuiWindowFlags_HorizontalScrollbar);
    static _int iCur;

    _uint iIndex = m_vecPoints.size();

    if (0 < iIndex)
    {
        const _char** ppiTems = New const _char * [iIndex];

        string* pstrTags = New string[iIndex];

        for (_uint iCellIndex = 0; iCellIndex < m_vecPoints.size(); ++iCellIndex)
        {
            _char szId[16] = { "" };
            _itoa_s(iCellIndex, szId, 10);
            strcat_s(szId, "'s Index");

            pstrTags[iCellIndex] = szId;

            ppiTems[iCellIndex] = pstrTags[iCellIndex].c_str();
        }

        if (ImGui::ListBox("Navigations", &m_iPickCell, ppiTems, iIndex, 20))
        {
            m_pTerrain->Get_Navigation()->Set_CurrentIndex(m_iPickCell);
        }

        /* Sort Navigation Cells */
        if (ImGui::Button("Sort Navigation Cells"))
            m_pTerrain->Get_Navigation()->Sort();

        Safe_Delete_Array(pstrTags);
        Safe_Delete_Array(ppiTems);
    }

    ImGui::EndChild();

    /* Check Option */
    ImGui::RadioButton("Make Navi", &m_iSelectOption, 0); ImGui::SameLine();
    ImGui::RadioButton("Pick Navi", &m_iSelectOption, 1);

    /* Setting Position */

    if (0 < iIndex)
    {
        for (_int iPointIndex = 0; iPointIndex < 3; ++iPointIndex)
        {
            _char szlabel[16];
            _itoa_s(iPointIndex, szlabel, 10);
            strcat_s(szlabel, "##point");
            _float3 vPosition = m_pTerrain->Get_Navigation()->Get_CellPoint(m_iPickCell, iPointIndex);
            _float arrData[3] = { vPosition.x, vPosition.y, vPosition.z };
            ImGui::DragFloat3(szlabel, arrData, 0.01f, 0.01f, 1000.f, "%.2f");
            vPosition = _float3(arrData[0], arrData[1], arrData[2]);
            m_pTerrain->Get_Navigation()->Set_CellPosition(m_iPickCell, iPointIndex, vPosition);
        }
    }

    /* Picking Navigation */
    Pick_Navigation(pGameInstance);

    ImGui::Separator();
}

void CTool_HeightMap::Pick_Navigation(CGameInstance* pGameInstance)
{
    if (g_hWnd == ::GetFocus() && pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
    {
        POINT ptMouse;
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        _vector vPosition = pGameInstance->Picking_On_Spheres(ptMouse, m_pTerrain->Get_Navigation(), m_pTerrain->Get_Transform());

        /* Make Navigation */
        if (0 == m_iSelectOption)
        {
            if (0.f > vPosition.m128_f32[0])
                vPosition = pGameInstance->Picking_On_Triangle(ptMouse, m_pTerrain->Get_Buffer(), m_pTerrain->Get_Transform());

            XMStoreFloat3(&m_Triangle.vDot[m_iCount], vPosition);
            ++m_iCount;

            if (3 == m_iCount)
            {
                m_pTerrain->Get_Navigation()->Add_Cell(m_Triangle);

                m_vecPoints.push_back(m_Triangle);
                ZeroMemory(&m_Triangle, sizeof(TRIANGLE));
                m_iCount = 0;
            }
        }

        /* Pick Sphere */
        if (1 == m_iSelectOption)
        {
            if (0.f > vPosition.m128_f32[0])
                return;

            /* 구체를 찾는다. */
            vector<pair<_uint, _int>> vecSphereInfo = pGameInstance->Pick_Spheres(ptMouse, m_pTerrain->Get_Navigation(), m_pTerrain->Get_Transform());

            if (0 == vecSphereInfo.size())
                return;

            
        }
    }
}

void CTool_HeightMap::Free()
{
    Safe_Release(m_pTerrain);
    Safe_Release(m_pBrushTextureCom);
}
