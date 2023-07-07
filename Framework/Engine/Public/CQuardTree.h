#pragma once

#include "CBase.h"

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };

private:
	explicit CQuadTree() = default;
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	HRESULT Make_Neighbors();
	void Culling(class CFrustum* pFrustum, const _float3* pVerticesPos, OUT _ulong* pIndices, OUT _uint* pNumIndices);
	_bool isDraw(const _float3* pVerticesPos);

private:
	_uint m_iCorners[CORNER_END] = { 0 };
	_uint m_iCenter = { 0 };

	CQuadTree* m_pChilds[CHILD_END] = { nullptr };
	CQuadTree* m_pNeighbors[NEIGHBOR_END] = { nullptr };

public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

END