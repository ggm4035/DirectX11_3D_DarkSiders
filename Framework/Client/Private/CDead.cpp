
#include "CDead.h"

#include "CBlackBoard.h"
#include "CGameObject3D.h"
#include "CModel.h"

CDead::CDead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CDead::CDead(const CDead& rhs)
	: CBehavior(rhs)
{
}

HRESULT CDead::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDead::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	static_cast<CGameObject3D*>(m_pOwner)->Dead_Motion(TimeDelta);

	return BEHAVIOR_RUNNING;
}

CDead* CDead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDead* pInstance = new CDead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDead* CDead::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CDead* pInstance = new CDead(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CDead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDead::Free()
{
	CBehavior::Free();
}
