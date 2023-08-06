#include "CGameObject.h"

#include "CGameInstance.h"
#include "CObject_Manager.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComposite(pDevice, pContext)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	: CComposite(rhs)
{
}

CGameObject* CGameObject::Get_Parts(const wstring& wstrPartsTag)
{
	auto iter = m_Parts.find(wstrPartsTag);

	if (iter == m_Parts.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Tick(const _double& TimeDelta)
{
#if defined(_USE_IMGUI) || defined(_DEBUG)
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Down(DIK_F1))
		Togle_Render_Debug();

	Safe_Release(pGameInstance);
#endif

	for (auto& Pair : m_Parts)
		Pair.second->Tick(TimeDelta);
}

void CGameObject::Late_Tick(const _double& TimeDelta)
{
	for (auto& Pair : m_Parts)
		Pair.second->Late_Tick(TimeDelta);
}

HRESULT CGameObject::Add_Parts(const _uint& iLevelIndex, const wstring& PrototypeTag, 
	const wstring& ObjectTag, CComponent* pOwner, void* pArg)
{
	if (nullptr != Get_Parts(ObjectTag))
		return E_FAIL;

	CGameObject* pGameObject = CObject_Manager::GetInstance()->
		Clone_GameObject(iLevelIndex, PrototypeTag, ObjectTag, pOwner, pArg);
	if (nullptr == pGameObject)
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
