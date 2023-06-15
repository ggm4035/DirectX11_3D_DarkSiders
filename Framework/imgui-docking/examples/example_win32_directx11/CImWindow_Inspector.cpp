#include "CImWindow_Inspector.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"

#include "CGameObject3D.h"
#include "CDummyObject3D.h"
#include "CGameObjectUI.h"

#include "CTransform.h"

CImWindow_Inspector::CImWindow_Inspector()
{
}

HRESULT CImWindow_Inspector::Initialize(void* pArg)
{
    if (FAILED(CImWindow::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CImWindow_Inspector::Tick(const _double& TimeDelta)
{
    ImGui::SetNextWindowPos(ImVec2(1600, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(320, 760), ImGuiCond_Always);
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoResize);

    Show_Components();

    ImGui::End();
}

void CImWindow_Inspector::Refresh()
{
    CImWindow::Refresh();
}

void CImWindow_Inspector::Show_Components()
{
    if (nullptr == TOOL->m_pCurrentObject)
        return;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);


    ImGui::Text("Tag"); ImGui::SameLine();
    strcpy_s(m_szTag, pGameInstance->wstrToStr(TOOL->m_pCurrentObject->Get_Tag()).c_str());

    if (ImGui::InputText(" ", m_szTag, 256))
    {
        string strTag = m_szTag;
        TOOL->m_pCurrentObject->Set_Tag(pGameInstance->strToWStr(strTag));
    }

    Show_Transform();

   if(TOOL->m_pCurrentObject->Get_Renderer())
       Show_Renderer();
   if(TOOL->m_pCurrentObject->Get_Texture())
       Show_Texture();
   if(TOOL->m_pCurrentObject->Get_Shader())
       Show_Shader();
   if (TOOL->m_pCurrentObject->Get_Buffer())
       Show_Buffer();

    Safe_Release(pGameInstance);
}

void CImWindow_Inspector::Show_Transform()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
    {
        ImGui::Text("Position");

        CTransform* pTransform = TOOL->m_pCurrentObject->Get_Transform();
        _vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);

        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Pos_X", &vPos.m128_f32[0], 0.1f, -10000.f, 10000.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Pos_Y", &vPos.m128_f32[1], 0.1f, -10000.f, 10000.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Pos_Z", &vPos.m128_f32[2], 0.1f, -10000.f, 10000.f, "%.1f");

        pTransform->Set_State(CTransform::STATE_POSITION, vPos);

        _float3 vAngle = pTransform->Get_Angle();

        ImGui::Text("Rotation");
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_X", &vAngle.x, 0.1f, -360.f, 360.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_Y", &vAngle.y, 0.1f, -360.f, 360.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_Z", &vAngle.z, 0.1f, -360.f, 360.f, "%.1f");
        ImGui::Text("Scale");

        pTransform->Rotation(vAngle);

        _float fScale = 0.f;
        _float3 vScale = pTransform->Get_Scaled();

        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_X", &vScale.x, 0.1f, -100.f, 100.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_Y", &vScale.y, 0.1f, -100.f, 100.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_Z", &vScale.z, 0.1f, -100.f, 100.f, "%.1f");
        ImGui::DragFloat("SCL_XYZ",&fScale, 0.1f, -100.f, 100.f, "%.1f");

        _float3 vTemp = _float3(vScale.x + fScale, vScale.y + fScale, vScale.z + fScale);

        pTransform->Scaled(vTemp);

        ImGui::Separator();
    }
}

void CImWindow_Inspector::Show_Renderer()
{
    if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_None))
    {
        _int iRenderType = { 0 };

        iRenderType = TOOL->m_pCurrentObject->Get_RenderGroup();
        ImGui::RadioButton("Priority", &iRenderType, 0);
        ImGui::RadioButton("NonBlend", &iRenderType, 1);
        ImGui::RadioButton("NonLight", &iRenderType, 2);
        ImGui::RadioButton("Blend", &iRenderType, 3);
        ImGui::RadioButton("UI", &iRenderType, 4);

        TOOL->m_pCurrentObject->m_eRenderGroup = (CRenderer::RENDERGROUP)iRenderType;
    }
}

void CImWindow_Inspector::Show_Texture()
{
    if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_None))
    {
    }
}

void CImWindow_Inspector::Show_Buffer()
{
    if (ImGui::CollapsingHeader("Buffer", ImGuiTreeNodeFlags_None))
    {
    }
}

void CImWindow_Inspector::Show_Shader()
{
    if (ImGui::CollapsingHeader("Shader", ImGuiTreeNodeFlags_None))
    {
        
    }
}

CImWindow_Inspector* CImWindow_Inspector::Create(void* pArg)
{
    CImWindow_Inspector* pInstance = new CImWindow_Inspector();

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create CImWindow_Inspector");
    }

    return pInstance;
}

void CImWindow_Inspector::Free()
{
    CImWindow::Free();
}
