#include "CImWindow_Inspector.h"

#include "CGameInstance.h"
#include "CImWindow_Manager.h"

#include "CGameObject3D.h"
#include "CGameObjectUI.h"

CImWindow_Inspector::CImWindow_Inspector()
{
}

HRESULT CImWindow_Inspector::Bind_GameObject(const wstring& wstrGameObjectTag)
{
    m_pCurGameObject = Find_GameObject(wstrGameObjectTag);
    if (nullptr == m_pCurGameObject)
        return E_FAIL;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    string strTag = pGameInstance->wstrToStr(m_pCurGameObject->Get_Tag());
    strcpy(m_szTag, strTag.c_str());

    Safe_Release(pGameInstance);

    return S_OK;
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
    ImGui::SetNextWindowSize(ImVec2(320, 1035), ImGuiCond_Always);
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
    if (nullptr == m_pCurGameObject)
        return;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    ImGui::Text("Tag"); ImGui::SameLine();
    if (ImGui::InputText(" ", m_szTag, 256))
    {
        string strTag = m_szTag;
        m_pCurGameObject->Set_Tag(pGameInstance->strToWStr(strTag));
    }
    ImGui::SameLine();
    ImGui::Button("Apply");
    ImGui::Separator();

    if (dynamic_cast<CGameObject3D*>(m_pCurGameObject))
        Show_Transform();

    Add_Component();

    Safe_Release(pGameInstance);
}

void CImWindow_Inspector::Show_Transform()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
    {
        ImGui::Text("Position");
        float f = 0.f;



        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Pos_X", &f, 0.1f, -10000.f, 10000.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Pos_Y", &f, 0.1f, -10000.f, 10000.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Pos_Z", &f, 0.1f, -10000.f, 10000.f, "%.1f");
        
        ImGui::Text("Rotation");
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_X", &f, 0.1f, -360.f, 360.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_Y", &f, 0.1f, -360.f, 360.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rot_Z", &f, 0.1f, -360.f, 360.f, "%.1f");
        ImGui::Text("Scale");

        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_X", &f, 0.1f, -100.f, 100.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_Y", &f, 0.1f, -100.f, 100.f, "%.1f"); ImGui::SameLine();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("SCL_Z", &f, 0.1f, -100.f, 100.f, "%.1f");

        ImGui::Separator();
    }
}

void CImWindow_Inspector::Add_Component()
{
    if (ImGui::BeginPopupContextItem("Create"))
    {
        CGameInstance* pGameInstance = CGameInstance::GetInstance();
        Safe_AddRef(pGameInstance);

        list<CComponent*> PrototypeList = pGameInstance->Get_All_Prototypes();
        _uint iNumPrototypes = PrototypeList.size();
        static int i = 3;

        auto iter = PrototypeList.begin();

        string* pstrTags = new string[iNumPrototypes];
        const _char** ppiTems = new const _char * [iNumPrototypes];

        for (_uint i = 0; i < iNumPrototypes; ++i)
        {
            pstrTags[i] = pGameInstance->wstrToStr((*iter)->Get_Tag());
            ppiTems[i] = pstrTags[i].c_str();
            ++iter;
        }

        ImGui::SetNextItemWidth(300.f);
        ImGui::ListBox("Prototypes", &i, ppiTems, iNumPrototypes);
        if (ImGui::Button("Create"))
        {

        }

        Safe_Delete_Array(pstrTags);
        Safe_Delete_Array(ppiTems);

        Safe_Release(pGameInstance);
        ImGui::EndPopup();
    }

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("Create");

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
