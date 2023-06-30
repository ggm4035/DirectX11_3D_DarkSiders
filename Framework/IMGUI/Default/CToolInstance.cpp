#include "CToolInstance.h"

#include "CImWindow_Base.h"
#include "CImWindow_Create.h"
#include "CImWindow_Inspector.h"
#include "CImWindow_Animation.h"
#include "CImWindow_Manager.h"
#include "CImWindow_Top.h"

IMPLEMENT_SINGLETON(CToolInstance)

CToolInstance::CToolInstance()
    : m_pBaseWindow(CImWindow_Base::GetInstance())
    , m_pInspectorWindow(CImWindow_Inspector::GetInstance())
    , m_pCreateWindow(CImWindow_Create::GetInstance())
    , m_pTopWindow(CImWindow_Top::GetInstance())
    , m_pAnimationWindow(CImWindow_Animation::GetInstance())
{
}

void CToolInstance::Free()
{
    Safe_Release(m_pBaseWindow);
    Safe_Release(m_pInspectorWindow);
    Safe_Release(m_pCreateWindow);
    Safe_Release(m_pTopWindow);
    Safe_Release(m_pAnimationWindow);
}
