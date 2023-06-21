#pragma once

#include "CBase.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };

private:
	explicit CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

#ifdef _DEBUG
public:
	HRESULT Render();

private:
	class CVIBuffer_Cell* m_pBuffer = { nullptr };
#endif // DEBUG

public:
	_vector Get_Point(POINT ePointID) {
		return XMLoadFloat3(&m_arrPoints[ePointID]);
	}
	void SetUp_Neighbor(NEIGHBOR eNeighborID, CCell* pNeighbor) {
		m_arrNeighborIndex[eNeighborID] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, const _int& iIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_bool is_In(_fvector vPosition); // 이거는 내가 나중에 작업해야게슴

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	_int m_iIndex = { 0 };
	_float3 m_arrPoints[POINT_END];
	_float3 m_arrNormals[NEIGHBOR_END];
	_int m_arrNeighborIndex[NEIGHBOR_END] = { -1, -1, -1 };

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int& iIndex);
	virtual void Free() override;
};

END