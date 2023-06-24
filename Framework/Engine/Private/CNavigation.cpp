#include "CNavigation.h"

#include "CCell.h"

#ifdef _DEBUG
#include "CShader.h"
#include "CPipeLine.h"
#endif // DEBUG

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_vecCells(rhs.m_vecCells)
	, m_NavigationDesc(rhs.m_NavigationDesc)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // DEBUG
{
	for (auto& pCell : m_vecCells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // DEBUG
}

#ifdef _DEBUG

HRESULT CNavigation::Render_Navigation()
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

	if (FAILED(m_pShader->Bind_Float4x4("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Float4x4("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Float4x4("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	m_pShader->Begin(0);

	for (auto& pCell : m_vecCells)
		pCell->Render();

	return S_OK;
}

#endif // DEBUG

HRESULT CNavigation::Initialize_Prototype(const wstring& wstrCellFilePath)
{
	HANDLE hFile = CreateFile(wstrCellFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = { 0 };
	_float3 vPoints[CCell::POINT_END];
	ZeroMemory(vPoints, sizeof(_float3) * CCell::POINT_END);

	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_vecCells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_vecCells.push_back(pCell);
	}

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_Navigation.hlsl", 
		VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLayerIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memmove(&m_NavigationDesc, pArg, sizeof(NAVIGATIONDESC));

	return S_OK;
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

CNavigation* CNavigation::Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(iLayerIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // DEBUG

	for (auto& pCell : m_vecCells)
		Safe_Release(pCell);

	CComponent::Free();
}
