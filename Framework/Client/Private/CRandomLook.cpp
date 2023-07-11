#include "stdafx.h"
#include "CRandomLook.h"

#include "CBlackBoard.h"

CRandomLook::CRandomLook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CRandomLook::CRandomLook(const CRandomLook& rhs)
	: CBehavior(rhs)
{
}

HRESULT CRandomLook::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CBehavior::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRandomLook::Tick(const _double& TimeDelta)
{
	_float3 vDirection;

	m_pBlackBoard->Get_Type<_float3>(L"vDirection", vDirection);

	vDirection.x = GetRandomFloat(-1.f, 1.f);
	vDirection.z = GetRandomFloat(-1.f, 1.f);

	XMStoreFloat3(&vDirection, XMVector3Normalize(XMLoadFloat3(&vDirection)));

	any data = vDirection;

	m_pBlackBoard->Set_Type(L"vDirection", data);

	return BEHAVIOR_SUCCESS;
}

CRandomLook* CRandomLook::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRandomLook* pInstance = new CRandomLook(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRandomLook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRandomLook* CRandomLook::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CRandomLook* pInstance = new CRandomLook(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CRandomLook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandomLook::Free()
{
	CBehavior::Free();
}
