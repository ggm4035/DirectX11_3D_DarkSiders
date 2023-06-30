#include "CGameObject.h"

#include "CObject_Manager.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComposite(pDevice, pContext)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	:CComposite(rhs)
{
}

void CGameObject::Tick(const _double& TimeDelta)
{
	for (auto& Pair : m_Parts)
		Pair.second->Tick(TimeDelta);
}

void CGameObject::Late_Tick(const _double& TimeDelta)
{
	for (auto& Pair : m_Parts)
		Pair.second->Late_Tick(TimeDelta);
}

HRESULT CGameObject::Add_Parts(const _uint& iLevelIndex, const wstring& PrototypeTag, const wstring& ObjectTag, CComponent* pOwner, void* pArg)
{
	CGameObject* pGameObject = CObject_Manager::GetInstance()->
		Clone_GameObject(iLevelIndex, PrototypeTag, ObjectTag, pOwner, pArg);

	if (nullptr == pGameObject)
		return E_FAIL;

	auto& iter = m_Parts.find(ObjectTag);
	if (iter != m_Parts.end())
		return E_FAIL;

	m_Parts.emplace(ObjectTag, pGameObject);

	pGameObject->Set_Tag(ObjectTag);

	return S_OK;
}

void CGameObject::Free()
{
	for (auto Pair : m_Parts)
		Safe_Release(Pair.second);

	CComposite::Free();
}
