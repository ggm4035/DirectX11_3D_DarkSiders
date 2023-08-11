#include "CTrigger_Free.h"

#include "CGameInstance.h"

#include "CPlayer.h"
#include "CMonster.h"
#include "CCamera_Free.h"

CTrigger_Free::CTrigger_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTrigger(pDevice, pContext)
{
}

CTrigger_Free::CTrigger_Free(const CTrigger_Free& rhs)
	: CTrigger(rhs)
{
}

HRESULT CTrigger_Free::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CTrigger::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CTrigger_Free::Tick(const _double& TimeDelta)
{
	CTrigger::Tick(TimeDelta);
}

void CTrigger_Free::Late_Tick(const _double& TimeDelta)
{
	CTrigger::Late_Tick(TimeDelta);

//#ifdef _DEBUG
//	if (FAILED(m_pRenderer->Add_DebugGroup(m_pColliderCom)))
//		return;
//#endif // _DEBUG
}

void CTrigger_Free::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (true == m_isCall)
		return;

	if (Collision.pOtherCollider->Get_Collider_Group() == CCollider::COL_PLAYER &&
		Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (wstring::npos != m_wstrTag.find(L"Final"))
			Final_Boss(pGameInstance);

		if (wstring::npos != m_wstrTag.find(L"Mid"))
			Sub_Boss(pGameInstance);

		Safe_Release(pGameInstance);
	}

	m_isCall = true;
}

HRESULT CTrigger_Free::Add_Components()
{
	CBounding_AABB::AABBDESC Desc;
	Desc.vExtents = m_TriggerDesc.vExtents;
	Desc.eGroup = CCollider::COL_TRIGGER;
	Desc.vPosition = _float3(0.f, 0.f, 0.f);
	Desc.vOffset = _float3(0.f, m_TriggerDesc.vExtents.y * 0.5f, 0.f);

	if (FAILED(Add_Component(LEVEL_STATIC, L"Collider_AABB", L"Col_Trigger",
		(CComponent**)&m_pColliderCom, this, &Desc)))
		return E_FAIL;

//#ifdef _DEBUG
//	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
//		(CComponent**)&m_pRenderer, this)))
//		return E_FAIL;
//#endif

	return S_OK;
}

void CTrigger_Free::Sub_Boss(CGameInstance* pGameInstance)
{
	CGameObject* pGameObject = { nullptr };

	pGameObject = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, L"Layer_Monster", L"SteamRoller");
	if (nullptr == pGameObject)
		return;

	static_cast<CMonster*>(pGameObject)->Spawn();
	pGameObject = static_cast<CPlayer*>(pGameInstance->Get_Player())->Get_Parts(L"Camera_Free");
	if (nullptr == pGameObject)
		return;

	static_cast<CCamera_Free*>(pGameObject)->Set_CamState(CCamera_Free::CAM_SUBBOSS);

	pGameInstance->Stop_Sound(CSound_Manager::SOUND_BGM);
	pGameInstance->Play_Sound(L"mus_level02_boss_intro.ogg", CSound_Manager::SOUND_SUB_BGM, 0.5f);
	pGameInstance->Play_BGM(L"mus_level02_boss.ogg", 0.5f);
}

void CTrigger_Free::Final_Boss(CGameInstance* pGameInstance)
{
	CGameObject* pGameObject = { nullptr };

	pGameObject = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, L"Layer_Monster", L"HollowLord");
	if (nullptr == pGameObject)
		return;

	static_cast<CMonster*>(pGameObject)->Spawn();
	pGameObject = static_cast<CPlayer*>(pGameInstance->Get_Player())->Get_Parts(L"Camera_Free");
	if (nullptr == pGameObject)
		return;

	static_cast<CCamera_Free*>(pGameObject)->Set_CamState(CCamera_Free::CAM_FINALBOSS);

	pGameInstance->Stop_Sound(CSound_Manager::SOUND_BGM);
	pGameInstance->Play_Sound(L"mus_level01_hollowlord_intro.ogg", CSound_Manager::SOUND_SUB_BGM, 0.5f);
	pGameInstance->Play_BGM(L"mus_level01_hollowlord.ogg", 0.5f);
}

CTrigger_Free* CTrigger_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrigger_Free* pInstance = new CTrigger_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTrigger_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CTrigger_Free* CTrigger_Free::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CTrigger_Free* pInstance = new CTrigger_Free(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CTrigger_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrigger_Free::Free()
{
//#ifdef _DEBUG
//	Safe_Release(m_pRenderer);
//#endif // _DEBUG

	CTrigger::Free();
}
