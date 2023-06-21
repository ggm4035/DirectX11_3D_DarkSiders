#include "CImWindow.h"

#include "CGameInstance.h"
#include "CDummyObject3D.h"

CImWindow::CImWindow()
{
}

HRESULT CImWindow::Initialize()
{
    Refresh();
    return S_OK;
}

void CImWindow::Refresh()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    m_GameObjectList.clear();

    list<CGameObject*> pObjects = pGameInstance->Get_All_GameObject();

    for (auto& pObject : pObjects)
    {
        if (dynamic_cast<CDummyObject3D*>(pObject))
            m_GameObjectList.push_back(static_cast<CDummyObject3D*>(pObject));
    }
    m_iNumGameObjects = m_GameObjectList.size();

    Safe_Release(pGameInstance);
}

CDummyObject3D* CImWindow::Find_GameObject(const wstring& GameObjectTag)
{
    for (auto& iter = m_GameObjectList.begin(); iter != m_GameObjectList.end(); ++iter)
    {
        if ((*iter)->Get_Tag() == GameObjectTag)
            return *iter;
    }

    return nullptr;
}

CDummyUI* CImWindow::Find_UI(const wstring& UITag)
{
    return nullptr;
}

void CImWindow::Free(void)
{
}
