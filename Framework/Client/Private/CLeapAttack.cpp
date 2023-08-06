
#include "CLeapAttack.h"

#include "CGameInstance.h"
#include "CPlayerAction.h"
#include "CStone_Effect.h"
#include "CStatic_Object.h"
#include "CBlackBoard.h"
#include "CPlayer.h"
#include "CWeapon.h"

CLeapAttack::CLeapAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CLeapAttack::CLeapAttack(const CLeapAttack& rhs)
	: CBehavior(rhs)
{
}

HRESULT CLeapAttack::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CStatic_Object::STATICOBJECTDESC Desc;
	Desc.vAngle = _float3(0.f, 0.f, 0.f);
	XMStoreFloat4x4(&Desc.WorldMatrix, XMMatrixIdentity());
	Desc.wstrModelTag = L"Model_Rocks_Circle";
	
	m_pRocks_Circle = dynamic_cast<CStatic_Object*>(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"Static_Object", L"Rocks_Circle", m_pOwner, &Desc));
	if (nullptr == m_pRocks_Circle)
		return E_FAIL;

	m_pEffect = dynamic_cast<CStone_Effect*>(pGameInstance->Clone_Component(LEVEL_GAMEPLAY, L"Stone_Effect", m_pOwner));
	if (nullptr == m_pEffect)
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLeapAttack::Tick(const _double& TimeDelta)
{
	m_fCoolTime += TimeDelta;

	if (false == m_isPlay)
		return S_OK;

	m_pEffect->Tick(TimeDelta);

	CModel* pModel = { nullptr };
	CRenderer* pRenderer = { nullptr };
	CTransform* pTransform = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type(L"pModel", pModel)))
		return E_FAIL;
	if (FAILED(m_pBlackBoard->Get_Type(L"pRenderer", pRenderer)))
		return E_FAIL;
	if (FAILED(m_pBlackBoard->Get_Type(L"pTransform", pTransform)))
		return E_FAIL;

	if (true == m_isFirst)
	{
		dynamic_cast<CWeapon*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Parts(L"Weapon"))->On_SwordTrail();
		pTransform->Set_On_Navigation(false);
		pModel->Change_Animation("Atk_Heavy_03");
		CGameInstance::GetInstance()->Play_Sound(L"char_war_attack_heavy_3.ogg", CSound_Manager::SOUND_PLAYER, 0.5f, true);
		m_isFirst = false;
		m_fTimeAcc = 0.f;
	}

	m_fTimeAcc += TimeDelta;

	if (true == pModel->isFinishedAnimation() ||
		true == pModel->isAbleChangeAnimation())
	{
		dynamic_cast<CWeapon*>(dynamic_cast<CPlayer*>(m_pOwner)->Get_Parts(L"Weapon"))->Off_SwordTrail();
		Reset();
	}

	if (1.0f <= m_fTimeAcc)
	{
		if (false == m_isRenderEffect)
		{
			m_pRocks_Circle->Get_Transform()->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
			m_pRocks_Circle->Get_Transform()->Set_State(CTransform::STATE_POSITION, pTransform->Get_State(CTransform::STATE_POSITION));
			m_pEffect->Render_Effect(pTransform->Get_State(CTransform::STATE_POSITION));
			m_isRenderEffect = true;
		}

		pRenderer->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, m_pEffect);
		pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, m_pRocks_Circle);
	}

	return S_OK;
}

void CLeapAttack::Reset()
{
	CTransform* pTransform = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type(L"pTransform", pTransform)))
		return;

	CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

	pAction->Set_State(CPlayerAction::STATE_IDLE);
	dynamic_cast<CPlayer*>(m_pOwner)->Get_Collider(L"Col_WheelWind")->Set_Enable(false);
	dynamic_cast<CPlayer*>(m_pOwner)->Get_Collider(L"Col_Attack")->Set_Enable(false);
	pTransform->Set_On_Navigation(true);
	m_isPlay = false;
	m_isFirst = true;
	m_isRenderEffect = false;
	m_fCoolTime = 0.f;
	m_fTimeAcc = 0.f;
}

void CLeapAttack::Play()
{
	if (false == m_isPlay && 3.f <= m_fCoolTime)
	{
		CPlayerAction* pAction = dynamic_cast<CPlayerAction*>(m_pParentBehavior);

		pAction->Set_State(CPlayerAction::STATE_LEAP);

		m_isPlay = true;
	}
}

CLeapAttack* CLeapAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLeapAttack* pInstance = new CLeapAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CLeapAttack* CLeapAttack::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CLeapAttack* pInstance = new CLeapAttack(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CAttackAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLeapAttack::Free()
{
	if (true == m_isCloned)
	{
		Safe_Release(m_pRocks_Circle);
		Safe_Release(m_pEffect);
	}

	CBehavior::Free();
}
