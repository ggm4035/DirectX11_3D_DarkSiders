#include "CImWindow.h"

#include "CGameInstance.h"

CImWindow::CImWindow()
{
}

HRESULT CImWindow::Initialize(void* pArg)
{
    Refresh();
    return S_OK;
}

void CImWindow::Refresh()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    m_GameObjectList = pGameInstance->Get_All_GameObject();
    m_iNumGameObjects = m_GameObjectList.size();

    Safe_Release(pGameInstance);
}

CGameObject* CImWindow::Find_GameObject(const wstring& GameObjectTag)
{
    for (auto& iter = m_GameObjectList.begin(); iter != m_GameObjectList.end(); ++iter)
    {
        if ((*iter)->Get_Tag() == GameObjectTag)
            return *iter;
    }

    return nullptr;
}

void CImWindow::Free(void)
{
}
