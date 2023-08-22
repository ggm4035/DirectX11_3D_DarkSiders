#include "CMonoEffect.h"

#include "CGameInstance.h"
#include "CBlackBoard.h"
#include "CGameObject3D.h"

CMonoEffect::CMonoEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CMonoEffect::CMonoEffect(const CMonoEffect& rhs)
	: CBehavior(rhs)
{
}

HRESULT CMonoEffect::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	return S_OK;
}

HRESULT CMonoEffect::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	m_pEffect->Tick(TimeDelta);
	m_pEffect->AfterFrustumTick(TimeDelta);

	return BEHAVIOR_SUCCESS;
}

CMonoEffect* CMonoEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonoEffect* pInstance = new CMonoEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMonoEffect* CMonoEffect::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CMonoEffect* pInstance = new CMonoEffect(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonoEffect::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pEffect);
	}

	CBehavior::Free();
}
