#include "CLevel.h"

#include "CGameInstance.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CLevel::~CLevel()
{
}

void CLevel::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
