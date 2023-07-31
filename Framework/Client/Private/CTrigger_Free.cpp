#include "CTrigger_Free.h"

#include "CGameInstance.h"

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
}

void CTrigger_Free::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	//cout << "Call" << endl;

	if (Collision.pOtherCollider->Get_Tag() == L"Col_Body")
	{
		/* 내가 가지고 있는 태그값을 누구한테 전달한다 */
		_float4 vPosition;
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		Collision.pOther->Get_Damaged_Knockback(vPosition);
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

#ifdef _DEBUG
	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer", L"Com_Renderer",
		(CComponent**)&m_pRenderer, this)))
		return E_FAIL;
#endif

	return S_OK;
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
	CTrigger::Free();
}
