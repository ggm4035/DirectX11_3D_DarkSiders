#include "CSpawns.h"

#include "CGameInstance.h"
#include "CBlackBoard.h"
#include "CMonster.h"

CSpawns::CSpawns(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CSpawns::CSpawns(const CSpawns& rhs)
	: CBehavior(rhs)
{
}

HRESULT CSpawns::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpawns::Tick(const _double& TimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	m_fTimeAcc += TimeDelta;
	if (0.1f > m_fTimeAcc)
		return BEHAVIOR_RUNNING;

	_float4x4 WorldMatrix;
	_float4 vPosition = _float4(GetRandomFloat(m_fMinX, m_fMaxX), m_fY, GetRandomFloat(m_fMinZ, m_fMaxZ), 1.f);
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	memcpy(&WorldMatrix._41, &vPosition, sizeof(_float4));

	CMonster::MONSTERDESC MonsterDesc;
	MonsterDesc.WorldMatrix = WorldMatrix;
	MonsterDesc.vAngle = _float3();
	MonsterDesc.SpeedPerSec = 3.f;
	MonsterDesc.RotationPerSec = XMConvertToRadians(90.f);
	MonsterDesc.iNavigationIndex = m_iNavigationIndex;
	MonsterDesc.isSpawn = true;

	_int iRandIndex = rand() % m_ModelTags.size();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(LEVEL_GAMEPLAY, 
		m_ModelTags[iRandIndex], m_ModelTags[iRandIndex], L"Layer_Monster", &MonsterDesc)))
		return E_FAIL;

	++m_iCount;
	m_fTimeAcc = 0.f;

	if (m_iNumMonster == m_iCount)
	{
		m_iCount = 0;
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

CSpawns* CSpawns::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpawns* pInstance = new CSpawns(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSpawns");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpawns* CSpawns::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CSpawns* pInstance = new CSpawns(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSpawns");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpawns::Free()
{
	CBehavior::Free();
}
