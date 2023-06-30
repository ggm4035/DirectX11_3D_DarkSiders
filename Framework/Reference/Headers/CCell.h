#pragma once

#include "CBase.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
	enum OPTION { OPT_NORMAL, OPT_CLIFF, OPT_END };

	typedef struct tagReturnDesc
	{
		vector<_int> vecNeighborIndex;
		_float3 vSlide;
	}RETURNDESC;

private:
	explicit CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePointID) {
		return XMLoadFloat3(&m_arrPoints[ePointID]);
	}
	void SetUp_Neighbor(NEIGHBOR eNeighborID, CCell* pNeighbor) {
		m_arrNeighborIndex[eNeighborID] = pNeighbor->m_iIndex;
	}
	OPTION Get_Optioin() const {
		return m_eOption;
	}

public:
	HRESULT Initialize(const _float3* pPoints, const _int& iIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	RETURNDESC is_In(_fvector vPosition);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	_int m_iIndex = { 0 };
	OPTION m_eOption = { OPT_END };
	_float3 m_arrPoints[POINT_END];
	_float3 m_arrNormals[NEIGHBOR_END];
	_int m_arrNeighborIndex[NEIGHBOR_END] = { -1, -1, -1 };

private:
	_bool isEqual(_fvector vSourPoint, _fvector vDestPoint);

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int& iIndex);
	virtual void Free() override;

#if defined(_USE_IMGUI) || defined(_DEBUG)
public:
	_float3& Get_Point_Tool(POINT ePoint) {
		return m_arrPoints[ePoint];
	}
	void Set_Position(POINT ePoint, const _float3& vPosition);
	
public:
	HRESULT Initialize(const _float3* pPoints, const _int& iIndex, class CBounding_Sphere* pPrototype);
	HRESULT Render();
	HRESULT Render_Sphere();
	_bool Picking_Spheres(_fvector vOrigin, _fvector vDirection, OUT _float3& vPosition, OUT _int& iSphereIndex);
	void Sort();

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int& iIndex, class CBounding_Sphere* pPrototype);

private:
	class CVIBuffer_Cell* m_pBuffer = { nullptr };
#endif // _USE_IMGUI
};

END