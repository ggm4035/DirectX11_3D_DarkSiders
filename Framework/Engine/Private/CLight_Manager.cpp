#include "CLight_Manager.h"

IMPLEMENT_SINGLETON(CLight_Manager)

HRESULT CLight_Manager::Add_Light(const CLight::LIGHTDESC& LightDesc)
{
	CLight* pLight = CLight::Create(LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_vecLights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render_Lights(CShader* pShader, CVIBuffer_Rect* m_pVIBuffer)
{
	for (auto& pLight : m_vecLights)
	{
		if(nullptr != pLight)
			pLight->Render(pShader, m_pVIBuffer);
	}

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
