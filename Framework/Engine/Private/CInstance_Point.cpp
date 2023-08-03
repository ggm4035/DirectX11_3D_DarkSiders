#include "CInstance_Point.h"

#include "CShader.h"

CInstance_Point::CInstance_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CInstance_Point::CInstance_Point(const CInstance_Point& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CInstance_Point::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CVIBuffer_Instancing::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		SPRITEDESC Desc = *reinterpret_cast<SPRITEDESC*>(pArg);

		m_fFrameTick = Desc.fFrameSpeed / _float(Desc.iNumHeight * Desc.iNumWidth);
		m_iNumHeight = Desc.iNumHeight;
		m_iNumWidth = Desc.iNumWidth;

		m_fLengthTexelU = 1.f / m_iNumWidth;
		m_fLengthTexelV = 1.f / m_iNumHeight;
	}

	m_iIndexCountPerInstance = 1;
	m_iVertexBuffers = { 2 };
	m_iStride = { sizeof(VTXPOINTTEX) };
	m_iNumVertices = 1;
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { m_iIndexCountPerInstance * m_iNumInstance };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	/*=================*/
	//  Vertex_Buffer  //
	/*=================*/

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };

	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOINTTEX* pVertices = new VTXPOINTTEX;
	ZeroMemory(pVertices, sizeof pVertices);

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);
	pVertices->vPSize = _float2(1.f, 1.f);
	pVertices->vTexCoord = _float2(0.f, 0.f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	/*================*/
	//  Index_Buffer  //
	/*================*/

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.StructureByteStride = { 0 };

	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

void CInstance_Point::Tick(vector<_float4x4>& vecMatrices, const _double& TimeDelta)
{
	CVIBuffer_Instancing::Tick(vecMatrices);

	/* 여기에 텍스처 uv의 좌표 4개를 만듦 */

	m_fTimeAcc += TimeDelta;
	if (m_fFrameTick < m_fTimeAcc)
	{
		++m_iCurKeyFrame;

		_uint iNumSprite = m_iNumHeight * m_iNumWidth;

		if (m_iCurKeyFrame >= iNumSprite)
			m_iCurKeyFrame = 0;

		m_fTimeAcc = 0.f;
	}

	_uint iHeightIndex = m_iCurKeyFrame / m_iNumWidth;
	_uint iWidthIndex = m_iCurKeyFrame % m_iNumWidth;

	VTXPOINTTEX* pVertex = { nullptr };
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &m_MappedSubResource);
	pVertex = reinterpret_cast<VTXPOINTTEX*>(m_MappedSubResource.pData);

	/* 여기서 텍스처의 uv좌표중 좌 상단의 uv좌표를 저장한다. */
	pVertex->vTexCoord = _float2(m_fLengthTexelU * iWidthIndex, m_fLengthTexelV * iHeightIndex);

	m_pContext->Unmap(m_pVB, 0);
}

HRESULT CInstance_Point::Bind_LengthTexelU(CShader* pShader, const string& strConstantName)
{
	if (FAILED(pShader->Bind_RawValue(strConstantName, &m_fLengthTexelU, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstance_Point::Bind_LengthTexelV(CShader* pShader, const string& strConstantName)
{
	if (FAILED(pShader->Bind_RawValue(strConstantName, &m_fLengthTexelV, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CInstance_Point* CInstance_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstance_Point* pInstance = new CInstance_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CInstance_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CInstance_Point* CInstance_Point::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CInstance_Point* pInstance = new CInstance_Point(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CInstance_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInstance_Point::Free()
{
	CVIBuffer_Instancing::Free();
}
