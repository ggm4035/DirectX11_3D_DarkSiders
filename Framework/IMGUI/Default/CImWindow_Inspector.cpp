#include "CImWindow_Inspector.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"

#include "CGameObject3D.h"
#include "CDummyObject3D.h"
#include "CGameObjectUI.h"

#include "CTransform.h"

IMPLEMENT_SINGLETON(CImWindow_Inspector)

HRESULT CImWindow_Inspector::Initialize()
{
    if (FAILED(CImWindow::Initialize()))
        return E_FAIL;

    return S_OK;
}

void CImWindow_Inspector::Tick(const _double& TimeDelta)
{
    ImVec2 pos = ImGui::GetWindowPos();
    pos.x += 1193;
    pos.y -= 140;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(320, 763), ImGuiCond_Always);
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

    if (TOOL->m_pCurrentObject->Get_Buffer())
        Show_Buffer();
    if (TOOL->m_pCurrentObject->Get_Texture())
        Show_Texture();
    if (TOOL->m_pCurrentObject->Get_Shader())
        Show_Shader();

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
        ImGui::DragFloat("Pos_X", &vPos.m128_f32[0], 0.01f, -10000.f, 10000.f, "%.2f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Pos_Y", &vPos.m128_f32[1], 0.01f, -10000.f, 10000.f, "%.2f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Pos_Z", &vPos.m128_f32[2], 0.01f, -10000.f, 10000.f, "%.2f");

        pTransform->Set_State(CTransform::STATE_POSITION, vPos);

        _float3 vAngle = pTransform->Get_Angle();

        ImGui::Text("Rotation");
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_X", &vAngle.x, 0.01f, -360.f, 360.f, "%.2f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_Y", &vAngle.y, 0.01f, -360.f, 360.f, "%.2f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_Z", &vAngle.z, 0.01f, -360.f, 360.f, "%.2f");
        ImGui::Text("Scale");

        pTransform->Rotation(vAngle);

        _float fScale = 0.f;
        _float3 vScale = pTransform->Get_Scaled();

        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_X", &vScale.x, 0.01f, -100.f, 100.f, "%.2f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_Y", &vScale.y, 0.01f, -100.f, 100.f, "%.2f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_Z", &vScale.z, 0.01f, -100.f, 100.f, "%.2f");
        ImGui::DragFloat("SCL_XYZ",&fScale, 0.01f, -100.f, 100.f, "%.2f");

        _float3 vTemp = _float3(vScale.x + fScale, vScale.y + fScale, vScale.z + fScale);

        pTransform->Scaled(vTemp);

        ImGui::Separator();
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

void CImWindow_Inspector::Free()
{
    CImWindow::Free();
}
