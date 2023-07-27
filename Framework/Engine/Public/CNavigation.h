#pragma once

#include "CComponent.h"
#include "CCell.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	enum MOVETYPE { TYPE_MOVE, TYPE_SLIDING, TYPE_STOP, TYPE_END };

	typedef struct tagNavigationDesc
	{
		explicit tagNavigationDesc() = default;
		explicit tagNavigationDesc(const tagNavigationDesc& rhs)
			: iCurrentIndex(rhs.iCurrentIndex) {}

		_int iCurrentIndex = { -1 };
	}NAVIGATIONDESC;

	typedef struct tagReturnDesc
	{
		MOVETYPE eMoveType;
		_float3 vSlide;
		CCell::OPTION eOption;
	}RETURNDESC;

private:
	explicit CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& wstrCellFilePath);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);

public:
	RETURNDESC is_Move(_fvector vPosition);
	_float is_OnNavigation(_fvector vPosition);

private:
	NAVIGATIONDESC m_NavigationDesc;
	vector<CCell*> m_vecCells;

private:
	HRESULT SetUp_Neighbors();
	_bool isEqual(_fvector vSourPoint, _fvector vDestPoint);

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrCellFilePath = L"");
	virtual CNavigation* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;

#if defined(_USE_IMGUI) || defined(_DEBUG)

public:
	_uint Get_NumCells() const {
		return m_vecCells.size();
	}
	CCell::OPTION Get_Option(const _uint& iIndex) {
		return m_vecCells[iIndex]->Get_Optioin();
	}
	void Set_Option(const _uint& iIndex, CCell::OPTION eOption) {
		m_vecCells[iIndex]->Set_Option(eOption);
	}
	void Set_CurrentIndex(const _uint& iIndex) {
		m_NavigationDesc.iCurrentIndex = iIndex;
	}
	_float3& Get_CellPoint(const _uint& iCellIndex, const _uint& iPointIndex);
	void Set_CellPosition(const _uint& iCellIndex, const _uint& iPointIndex, const _float3& vPosition);

	virtual HRESULT Render() override;

	void Add_Cell(const TRIANGLE& Triangle, CCell::OPTION eOption);
	void Sort();
	_float3 Pick_Sphere(_fvector vOrigin, _fvector vDirection);
	vector<pair<_uint, _int>> Pick_Spheres(_fvector vOrigin, _fvector vDirection);

	void Remove_Cell(const _uint& iCellIndex);
	void Remove_All_Cell();

private:
	class CBounding_Sphere* m_pSpherePrototype = { nullptr };
	class CCell* m_pCurrentCell = { nullptr };
	class CShader* m_pShader = { nullptr };

#endif // _USE_IMGUI
};

END