#include "CGameObject3D.h"

#include "CGameInstance.h"

CGameObject3D::CGameObject3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CGameObject3D::CGameObject3D(const CGameObject3D& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGameObject3D::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject3D::Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComposite::Initialize(iLayerIndex, pOwner, pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pTransformCom = dynamic_cast<CTransform*>(pGameInstance->Clone_Transform(this, pArg));
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject3D::Tick(const _double& TimeDelta)
{
}

void CGameObject3D::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CGameObject3D::Render()
{
	return S_OK;
}

void CGameObject3D::Free()
{
	Safe_Release(m_pTransformCom);

	CComposite::Free();
}
