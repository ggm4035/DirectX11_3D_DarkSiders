#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class CCubeCol : public CVIBuffer
{
private:
	explicit CCubeCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeCol(const CCubeCol& rhs);
	virtual ~CCubeCol();

public:
	HRESULT		Ready_Buffer(void);

public:
	static CCubeCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);

};

END