#include "CNavigation.h"
\
#if defined(_USE_IMGUI) || defined(_DEBUG)
#include "CShader.h"
#include "CBounding_Sphere.h"
#include "CPipeLine.h"
#endif // _USE_IMGUI

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_vecCells(rhs.m_vecCells)
#if defined(_USE_IMGUI) || defined(_DEBUG)
	, m_pSpherePrototype(rhs.m_pSpherePrototype)
	, m_pCurrentCell(rhs.m_pCurrentCell)
	, m_pShader(rhs.m_pShader)
#endif // _USE_IMGUI
{
	for (auto& pCell : m_vecCells)
		Safe_AddRef(pCell);

#if defined(_USE_IMGUI) || defined(_DEBUG)
	Safe_AddRef(m_pSpherePrototype);
	Safe_AddRef(m_pCurrentCell);
	Safe_AddRef(m_pShader);
#endif // _USE_IMGUI
}

HRESULT CNavigation::Initialize_Prototype(const wstring& wstrCellFilePath)
{
	if (0 < wstrCellFilePath.size())
	{
		HANDLE hFile = CreateFile(wstrCellFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		_ulong dwByte = { 0 };
		_float3 vPoints[CCell::POINT_END];
		CCell::OPTION eOption;

		while (true)
		{
			ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

			ReadFile(hFile, &eOption, sizeof eOption, &dwByte, nullptr);
			if (0 == dwByte)
				break;

#if defined(_USE_IMGUI) || defined(_DEBUG)
			CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_vecCells.size(), eOption, m_pSpherePrototype);
			if (nullptr == pCell)
				return E_FAIL;
#else
			CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_vecCells.size(), eOption);
			if (nullptr == pCell)
				return E_FAIL;
#endif

			m_vecCells.push_back(pCell);
		}

		CloseHandle(hFile);

		if (FAILED(SetUp_Neighbors()))
			return E_FAIL;
	}

#if defined(_USE_IMGUI) || defined(_DEBUG)

	m_pSpherePrototype = CBounding_Sphere::Create(m_pDevice, m_pContext);

	_float3 Data{};
	m_pCurrentCell = CCell::Create(m_pDevice, m_pContext, &Data, m_vecCells.size(), CCell::OPT_NORMAL, m_pSpherePrototype);

	m_pShader = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_Navigation.hlsl",
		VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memmove(&m_NavigationDesc, pArg, sizeof(NAVIGATIONDESC));

	return S_OK;
}

CNavigation::RETURNDESC CNavigation::is_Move(_fvector vPosition)
{
	RETURNDESC NaviRetDesc;
	NaviRetDesc.eMoveType = TYPE_MOVE;
	NaviRetDesc.eOption = m_vecCells[m_NavigationDesc.iCurrentIndex]->Get_Optioin();

	CCell::RETURNDESC CellRetDesc = m_vecCells[m_NavigationDesc.iCurrentIndex]->is_In(vPosition);

	/* 셀 안에 있는 경우 */
	if (0 == CellRetDesc.vecNeighborIndex.size())
		return NaviRetDesc;

	/* 셀 밖에 나감 그런데 이웃 있음 */
	if (-1 != CellRetDesc.vecNeighborIndex.front())
	{
		while (true) /* 진자읾ㅇ!@#!>?!@ */
		{
			if (-1 == CellRetDesc.vecNeighborIndex.front()) /* 구라임 이웃 없음 ㅋ */
			{
				NaviRetDesc.eMoveType = TYPE_STOP;
				NaviRetDesc.vSlide = CellRetDesc.vSlide;
				return NaviRetDesc;
			}

			m_NavigationDesc.iCurrentIndex = CellRetDesc.vecNeighborIndex.front();
			NaviRetDesc.eOption = m_vecCells[m_NavigationDesc.iCurrentIndex]->Get_Optioin();

			CellRetDesc = m_vecCells[CellRetDesc.vecNeighborIndex.front()]->is_In(vPosition);

			if (0 == CellRetDesc.vecNeighborIndex.size())
				break;
		}

		return NaviRetDesc;
	}
	/* 이웃 없음 그런데 나간 라인은 두개임 */
	else if (2 == CellRetDesc.vecNeighborIndex.size())
	{
		/* 진자 두개나간거 마증ㅁㄻ? ?@?@!@? */
		while (true)
		{
			if (0 == CellRetDesc.vecNeighborIndex.size())
			{
				NaviRetDesc.eMoveType = TYPE_MOVE;
				return NaviRetDesc;
			}

			if (2 > CellRetDesc.vecNeighborIndex.size() &&
				0 < CellRetDesc.vecNeighborIndex.size() &&
				-1 == CellRetDesc.vecNeighborIndex.front()) /* 구라임 ㅋ*/
			{
				NaviRetDesc.eMoveType = TYPE_STOP;
				return NaviRetDesc;
			}

			if (-1 != CellRetDesc.vecNeighborIndex.back())
			{
				m_NavigationDesc.iCurrentIndex = CellRetDesc.vecNeighborIndex.back();
				CellRetDesc = m_vecCells[CellRetDesc.vecNeighborIndex.back()]->is_In(vPosition);
			}

			if (2 == CellRetDesc.vecNeighborIndex.size()) 
				break;
		}

		NaviRetDesc.eMoveType = TYPE_STOP;

		return NaviRetDesc;
	}

	NaviRetDesc.eMoveType = TYPE_SLIDING;
	NaviRetDesc.vSlide = CellRetDesc.vSlide;

	return NaviRetDesc;
}

_float CNavigation::is_OnNavigation(_fvector vPosition)
{
	return m_vecCells[m_NavigationDesc.iCurrentIndex]->is_On(vPosition);
}

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_vecCells)
	{
		for (auto& pDestCell : m_vecCells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_AB, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_BC, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

_bool CNavigation::isEqual(_fvector vSourPoint, _fvector vDestPoint)
{
	for (_uint i = 0; i < 3; ++i)
	{
		if (0.2f < fabs(vSourPoint.m128_f32[i] - vDestPoint.m128_f32[i]))
			return false;
	}

	return true;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrCellFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrCellFilePath)))
	{
		MSG_BOX("Failed to Created CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CNavigation* CNavigation::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
#if defined(_USE_IMGUI) || defined(_DEBUG)
	Safe_Release(m_pSpherePrototype);
	Safe_Release(m_pCurrentCell);
	Safe_Release(m_pShader);
#endif // _USE_IMGUI

	for (auto& pCell : m_vecCells)
		Safe_Release(pCell);

	CComponent::Free();
}

#if defined(_USE_IMGUI) || defined(_DEBUG)

_float3& CNavigation::Get_CellPoint(const _uint& iCellIndex, const _uint& iPointIndex)
{
	return m_vecCells[iCellIndex]->Get_Point_Tool(CCell::POINT(iPointIndex));
}

void CNavigation::Set_CellPosition(const _uint& iCellIndex, const _uint& iPointIndex, const _float3& vPosition)
{
	m_vecCells[iCellIndex]->Set_Position((CCell::POINT)iPointIndex, vPosition);
}

HRESULT CNavigation::Render()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	XMStoreFloat4x4(&ViewMatrix, pPipeLine->Get_Transform_Matrix(CPipeLine::STATE_VIEW));
	XMStoreFloat4x4(&ProjMatrix, pPipeLine->Get_Transform_Matrix(CPipeLine::STATE_PROJ));

	Safe_Release(pPipeLine);

	if (FAILED(m_pShader->Bind_Float4x4("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Float4x4("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_float4		vColor = _float4(0.f, 1.f, 0.f, 1.f);

	if (FAILED(m_pShader->Bind_Float4x4("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	m_pShader->Begin(0);

	for (auto& pCell : m_vecCells)
		pCell->Render();

	if (-1 != m_NavigationDesc.iCurrentIndex)
	{
		vColor = _float4(1.f, 0.f, 0.f, 1.f);

		WorldMatrix._42 = 0.01f;
		if (FAILED(m_pShader->Bind_Float4x4("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (_uint i = 0; i < 3; ++i)
		{
			_float3 vPosition = m_vecCells[m_NavigationDesc.iCurrentIndex]->Get_Point_Tool(CCell::POINT(i));
			m_pCurrentCell->Set_Position(CCell::POINT(i), vPosition);
		}
		m_pCurrentCell->Render();
	}

	for (auto& pCell : m_vecCells)
		pCell->Render_Sphere();

	/* 볼때만 주석 해제하셈 */
	for (auto& pCell : m_vecCells)
		pCell->Render_Font();

	return S_OK;
}

void CNavigation::Add_Cell(const TRIANGLE& Triangle, CCell::OPTION eOption)
{
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, Triangle.vDot, m_vecCells.size(), eOption, m_pSpherePrototype);
	if (nullptr == pCell)
		return;

	m_vecCells.push_back(pCell);
}

void CNavigation::Sort()
{
	for (auto& pCell : m_vecCells)
		pCell->Sort();
}

_float3 CNavigation::Pick_Sphere(_fvector vOrigin, _fvector vDirection)
{
	_float3 fPosition = _float3(-1.f, -1.f, -1.f);
	_int iSphereIndex;

	for (auto& pCell : m_vecCells)
	{
		if (true == pCell->Picking_Spheres(vOrigin, vDirection, fPosition, iSphereIndex))
			return fPosition;
	}

	return fPosition;
}

vector<pair<_uint, _int>> CNavigation::Pick_Spheres(_fvector vOrigin, _fvector vDirection)
{
	_float fDistance = { 0.f };
	_float3 fPosition;
	vector<pair<_uint, _int>> RetData;
	_int iSphereIndex = { 0 };

	_uint iCellIndex = 0;
	for (auto& pCell : m_vecCells)
	{
		if (true == pCell->Picking_Spheres(vOrigin, vDirection, fPosition, iSphereIndex))
			RetData.push_back({ iCellIndex, iSphereIndex });

		++iCellIndex;
	}

	return RetData;
}

void CNavigation::Remove_Cell(const _uint& iCellIndex)
{
	if (m_vecCells.size() <= iCellIndex)
		return;

	auto iter = m_vecCells.begin();

	for (_uint i = 0; i < iCellIndex; ++i)
		++iter;

	Safe_Release(m_vecCells[iCellIndex]);
	m_vecCells.erase(iter);

}

void CNavigation::Remove_All_Cell()
{
	for (auto& pCell : m_vecCells)
		Safe_Release(pCell);

	m_vecCells.clear();
}

#endif // _USE_IMGUI