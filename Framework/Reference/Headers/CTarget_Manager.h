#pragma once

#include "CBase.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)

private:
	explicit CTarget_Manager() = default;
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(const wstring& wstrTargetTag, ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const _uint& iSizeX, const _uint& iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& wstrMRTTag, const wstring& wstrTargetTag);
	/* pMRTTag에 추가되어있는 렌더타겟들을 장치에 바인딩한다. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, wstring wstrMRTTag);
	/* 원래상태로 복구한다. (0번째에 백버퍼가 바인딩 된 상태로 돌려준다. */
	HRESULT End_MRT(ID3D11DeviceContext* pContext);
	HRESULT Bind_ShaderResourceView(const wstring& wstrTargetTag, class CShader* pShader, const string& wstrConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& wstrTargetTag, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	HRESULT Render(const wstring& wstrMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	unordered_map<wstring, class CRenderTarget*> m_umapRenderTargets;
	unordered_map<wstring, list<class CRenderTarget*>> m_umapMRTs;

private:
	ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

private:
	class CRenderTarget* Find_RenderTarget(const wstring& wstrTargetTag);
	list<class CRenderTarget*>* Find_MRT(const wstring& wstrMRTTag);

public:
	virtual void Free() override;
};

END