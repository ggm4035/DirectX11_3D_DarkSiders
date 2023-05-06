#pragma once

#include "Base.h"

BEGIN(Engine)

enum VIBUFFER_FLAG
{
	VB_DYNAMIC,
	VB_STATIC,
};

class  CVIBuffer
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	HRESULT		Ready_Buffer(VIBUFFER_FLAG flag = VB_STATIC);
	virtual void Render_Component() override;

	LPDIRECT3DVERTEXBUFFER11 GetVertexBuffer() { return m_pVB; }
	LPDIRECT3DINDEXBUFFER9	GetIndexBuffer() { return m_pIB; }

	// 도저히 답이 없어서 임시방편으로 만들어놓은 코드다.
	// 만약에 VIBuffer에 월드행렬을 먹이고 싶지않다면 이 플래그를 True로 만들어라.
	void		Set_RenderFlag(_bool state = true) { m_bRenderFlag = state; }

protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;

	_ulong		m_dwVtxSize;
	_ulong		m_dwVtxCnt;
	_ulong		m_dwTriCnt;
	_ulong		m_dwFVF;

	_ulong		m_dwIdxSize;
	D3DFORMAT	m_IdxFmt;

	_bool m_bRenderFlag;
protected:
	virtual void Free(void);

};

END