#include "CTarget_Manager.h"

#include "CRenderTarget.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

HRESULT CTarget_Manager::Add_RenderTarget(const wstring& wstrTargetTag, ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	const _uint& iSizeX, const _uint& iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(wstrTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pContext, iSizeX, iSizeY, eFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_umapRenderTargets.emplace(wstrTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring& wstrMRTTag, const wstring& wstrTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(wstrTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	auto pTargetList = Find_MRT(wstrMRTTag);
	if (nullptr == pTargetList)
	{
		list<CRenderTarget*> TargetList;
		TargetList.push_back(pRenderTarget);
		m_umapMRTs.emplace(wstrMRTTag, TargetList);
	}
	else
		pTargetList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext* pContext, wstring wstrMRTTag)
{
	auto pMRT = Find_MRT(wstrMRTTag);
	if (pMRT == nullptr)
	{
		MSG_BOX("MRT is nullptr");
		return E_FAIL;
	}

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRTVs[8] = { nullptr };

	_uint iNumRTV = { 0 };
	for (auto& pRenderTarget : *pMRT)
	{
		pRenderTarget->Clear();
		pRTVs[iNumRTV++] = pRenderTarget->Get_RTV();
	}

	if(iNumRTV <= 8)
		pContext->OMSetRenderTargets(iNumRTV, pRTVs, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_BaseMRT(ID3D11DeviceContext* pContext, wstring wstrMRTTag)
{
	auto pMRT = Find_MRT(wstrMRTTag);
	if (pMRT == nullptr)
	{
		MSG_BOX("MRT is nullptr");
		return E_FAIL;
	}
	
	pContext->OMGetRenderTargets(1, &m_pBaseRenderTargetView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRTVs[8] = { nullptr };

	_uint iNumRTV = { 0 };
	for (auto& pRenderTarget : *pMRT)
	{
		pRenderTarget->Clear();
		pRTVs[iNumRTV++] = pRenderTarget->Get_RTV();
	}

	if (iNumRTV <= 8)
		pContext->OMSetRenderTargets(iNumRTV, pRTVs, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* pContext)
{
	ID3D11RenderTargetView* pRTVs[8] = { m_pBackBufferView };

	pContext->OMSetRenderTargets(8, pRTVs, m_pDepthStencilView);

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::End_BaseMRT(ID3D11DeviceContext* pContext)
{
	ID3D11RenderTargetView* pRTVs[8] = { m_pBaseRenderTargetView };

	pContext->OMSetRenderTargets(8, pRTVs, m_pDepthStencilView);

	Safe_Release(m_pBaseRenderTargetView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResourceView(const wstring& wstrTargetTag, CShader* pShader, const string& wstrConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(wstrTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResourceView(pShader, wstrConstantName);
}

HRESULT CTarget_Manager::Clear_RenderTargetView(ID3D11DeviceContext* pContext, const wstring& wstrTargetTag, const _float4& vClearColor)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(wstrTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	pContext->ClearRenderTargetView(pRenderTarget->Get_RTV(), (_float*)(&vClearColor));

	return S_OK;
}

CRenderTarget* CTarget_Manager::Find_RenderTarget(const wstring& wstrTargetTag)
{
	auto iter = m_umapRenderTargets.find(wstrTargetTag);

	if (iter == m_umapRenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring& wstrMRTTag)
{
	auto iter = m_umapMRTs.find(wstrMRTTag);

	if (iter == m_umapMRTs.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_umapRenderTargets)
		Safe_Release(Pair.second);
	m_umapRenderTargets.clear();

	for (auto& Pair : m_umapMRTs)
	{
		for (auto& Target : Pair.second)
			Safe_Release(Target);
	}
	m_umapMRTs.clear();
}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const wstring& wstrTargetTag, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(wstrTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render(const wstring& wstrMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto Pair : m_umapMRTs)
	{
		for (auto pRenderTarget : Pair.second)
		{
			if (FAILED(pRenderTarget->Render(pShader, pVIBuffer)))
				return E_FAIL;
		}
	}

	return S_OK;
}
#endif
