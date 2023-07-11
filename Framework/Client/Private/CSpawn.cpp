#include "stdafx.h"
#include "CSpawn.h"

#include "CGameObject3D.h"
#include "CBlackBoard.h"
#include "CTransform.h"
#include "CModel.h"

CSpawn::CSpawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CSpawn::CSpawn(const CSpawn& rhs)
	: CBehavior(rhs)
{
}

HRESULT CSpawn::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameObject3D* pGameObject = dynamic_cast<CGameObject3D*>(m_pOwner);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_pTransform = pGameObject->Get_Transform();
	if (nullptr == m_pTransform)
		return E_FAIL;
	Safe_AddRef(m_pTransform);

	m_pModel = dynamic_cast<CModel*>(pGameObject->Get_Component(L"Com_Model"));
	if (nullptr == m_pModel)
		return E_FAIL;
	Safe_AddRef(m_pModel);

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawnEnd = { nullptr };

			pBlackBoard->Get_Type(L"isSpawnEnd", pIsSpawnEnd);

			return !(*pIsSpawnEnd);
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsSpawn = { nullptr };

			pBlackBoard->Get_Type(L"isSpawn", pIsSpawn);

			return *pIsSpawn;
		});

	return S_OK;
}

HRESULT CSpawn::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	_vector vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);

	if (true == m_isFirst)
	{
		vPosition.m128_f32[1] -= 3.f;
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
		m_isFirst = false;
		m_pModel->Change_Animation("Spawn");
	}

	m_pTransform->LeapJump(0.7f, TimeDelta);

	if (true == m_pModel->isFinishedAnimation())
	{
		_bool* pIsSpawnEnd = { nullptr };

		m_pBlackBoard->Get_Type(L"isSpawnEnd", pIsSpawnEnd);
		m_pModel->Change_Animation("Idle");

		*pIsSpawnEnd = true;
		m_isFirst = true;
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CSpawn* CSpawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpawn* pInstance = new CSpawn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSpawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpawn* CSpawn::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSpawn* pInstance = new CSpawn(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSpawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpawn::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pTransform);
		Safe_Release(m_pModel);
	}
	CBehavior::Free();
}
