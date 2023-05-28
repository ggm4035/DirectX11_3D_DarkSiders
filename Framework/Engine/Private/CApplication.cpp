#include "CApplication.h"

IMPLEMENT_SINGLETON(CApplication)

_uint CApplication::m_iWinSizeX = 0;
_uint CApplication::m_iWinSizeY = 0;

CApplication::CApplication()
{
}

void CApplication::Free()
{
}
