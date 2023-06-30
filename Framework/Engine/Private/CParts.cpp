#include "CParts.h"

CParts::CParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
{
}

CParts::CParts(const CGameObject3D& rhs)
	:CGameObject3D(rhs)
{
}

HRESULT CParts::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	PARENTDESC Desc = *reinterpret_cast<PARENTDESC*>(pArg);

	m_OffsetMatrix = Desc.OffsetMatrix;
	m_PivotMatrix = Desc.PivotMatrix;
	m_pCombinedTransformationMatrix = Desc.pCombinedTransformationMatrix;
	m_pNotMoveCombinedTransformationMatrix = Desc.pNotMoveCombinedTransformationMatrix;
	m_pParentWorldMatrix = Desc.pWorldMatrix;

	return S_OK;
}

void CParts::Free()
{
	CGameObject3D::Free();
}
