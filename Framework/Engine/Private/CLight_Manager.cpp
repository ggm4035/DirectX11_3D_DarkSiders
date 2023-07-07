#include "CLight_Manager.h"

IMPLEMENT_SINGLETON(CLight_Manager)

HRESULT CLight_Manager::Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC& LightDesc)
{
	CLight* pLight = CLight::Create(pDevice, pContext, LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_vecLights.push_back(pLight);

	return S_OK;
}

const CLight::LIGHTDESC* CLight_Manager::Get_LightDesc(const _uint& iIndex)
{
	if (0 > iIndex || iIndex >= m_vecLights.size())
		return nullptr;

	return m_vecLights[iIndex]->Get_LightDesc();
}

void CLight_Manager::Free()
{
	for (auto& iter : m_vecLights)
		Safe_Release(iter);

	m_vecLights.clear();
}
