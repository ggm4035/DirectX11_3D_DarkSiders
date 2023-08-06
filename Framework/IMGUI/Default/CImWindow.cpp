#include "CImWindow.h"

#include "CGameInstance.h"
#include "CDummyObject3D.h"
#include "CDummyTrigger.h"

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

    for(_uint i = 0; i < LAYER_END; ++i)
        m_GameObjectList[i].clear();
    m_TriggerList.clear();

    /* Refresh Static */
    list<CGameObject*> pObjects = pGameInstance->Get_GameObject_Layer(L"Layer_Tool");

    for (auto& pObject : pObjects)
    {
        if (dynamic_cast<CDummyObject3D*>(pObject))
            m_GameObjectList[LAYER_STATIC].push_back(static_cast<CDummyObject3D*>(pObject));
    }

    m_iNumGameObjects[LAYER_STATIC] = m_GameObjectList[LAYER_STATIC].size();

    /* Refresh BreakAble */
    pObjects = pGameInstance->Get_GameObject_Layer(L"Layer_BreakAble");

    for (auto& pObject : pObjects)
    {
        if (dynamic_cast<CDummyObject3D*>(pObject))
            m_GameObjectList[LAYER_BREAKABLE].push_back(static_cast<CDummyObject3D*>(pObject));
    }

    m_iNumGameObjects[LAYER_BREAKABLE] = m_GameObjectList[LAYER_BREAKABLE].size();

    /* Refresh Monster */
    pObjects = pGameInstance->Get_GameObject_Layer(L"Layer_Monster");

    for (auto& pObject : pObjects)
    {
        if (dynamic_cast<CDummyObject3D*>(pObject))
            m_GameObjectList[LAYER_MONSTER].push_back(static_cast<CDummyObject3D*>(pObject));
    }

    m_iNumGameObjects[LAYER_MONSTER] = m_GameObjectList[LAYER_MONSTER].size();

    /* Refresh Trigger */
    pObjects = pGameInstance->Get_GameObject_Layer(L"Layer_Trigger");

    for (auto& pTrigger : pObjects)
    {
        m_TriggerList.push_back(static_cast<CDummyTrigger*>(pTrigger));
    }

    m_iNumTriggers = m_TriggerList.size();

    Safe_Release(pGameInstance);
}

CDummyObject3D* CImWindow::Find_GameObject(const wstring& GameObjectTag, LAYERTYPE eType)
{
    for (auto& iter = m_GameObjectList[eType].begin(); iter != m_GameObjectList[eType].end(); ++iter)
    {
        if ((*iter)->Get_Tag() == GameObjectTag)
            return *iter;
    }

    return nullptr;
}

CDummyTrigger* CImWindow::Find_Trigger(const wstring& TriggerTag)
{
    for (auto& iter = m_TriggerList.begin(); iter != m_TriggerList.end(); ++iter)
    {
        if ((*iter)->Get_Tag() == TriggerTag)
            return *iter;
    }

    return nullptr;
}

void CImWindow::Free(void)
{
}
