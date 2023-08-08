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
		if (nullptr != pLight)
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

_float4x4 CLight_Manager::Get_LightViewFloat4x4(const _uint& iIndex)
{
	if (0 == m_vecLights.size())
		return _float4x4();

	const CLight::LIGHTDESC* pLightDesc = m_vecLights[iIndex]->Get_LightDesc();
	if (nullptr == pLightDesc)
		return _float4x4();

	_vector vEye = XMLoadFloat4(&pLightDesc->vPosition);
	_vector vAt = vEye + XMLoadFloat4(&pLightDesc->vDirection);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_float4x4 LightViewFloat4x4;
	XMStoreFloat4x4(&LightViewFloat4x4, XMMatrixLookAtLH(vEye, vAt, vUp));

	return LightViewFloat4x4;
}

_float4x4 CLight_Manager::Get_LightProjFloat4x4(ID3D11DeviceContext* pContext)
{
	_uint		iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	_matrix LightProjMatrix = XMMatrixPerspectiveFovLH(120.f, ViewportDesc.Width / ViewportDesc.Height, 0.3f, 1000.f);

	_float4x4 LightProjFloat4x4;
	XMStoreFloat4x4(&LightProjFloat4x4, LightProjMatrix);

	return LightProjFloat4x4;
}

void CLight_Manager::Free()
{
	for (auto& iter : m_vecLights)
		Safe_Release(iter);

	m_vecLights.clear();
}
