#include "CInstance_Point.h"

#include "CShader.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Point_Instance::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(__super::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		POINTINSTDESC Desc = *static_cast<POINTINSTDESC*>(pArg);

		m_fFrameTick = Desc.fFrameSpeed / _float(Desc.iNumHeight * Desc.iNumWidth);
		m_iNumHeight = Desc.iNumHeight;
		m_iNumWidth = Desc.iNumWidth;

		m_fLengthTexelU = 1.f / m_iNumWidth;
		m_fLengthTexelV = 1.f / m_iNumHeight;
	}

	m_iIndexCountPerInstance = 1;
	m_iVertexBuffers = { 2 };
	m_iStride = { sizeof(VTXPOINTTEX) };
	m_iNumVertices = { 1 };
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = { m_iIndexCountPerInstance * m_iNumInstance };
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	VTXPOINTTEX* pVertices = new VTXPOINTTEX;
	ZeroMemory(pVertices, sizeof(VTXPOINTTEX));

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);
	pVertices->vPSize = _float2(1.f, 1.f);
	pVertices->vTexCoord = _float2(0.f, 0.f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

void CVIBuffer_Point_Instance::Tick(const vector<_float4x4>& vecMatrices, const _double& TimeDelta)
{
	CVIBuffer_Instancing::Tick(vecMatrices);

	if (0 == m_iNumWidth)
		return;

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
	
	pVertex->vTexCoord = _float2(m_fLengthTexelU * iWidthIndex, m_fLengthTexelV * iHeightIndex);

	m_pContext->Unmap(m_pVB, 0);
}

HRESULT CVIBuffer_Point_Instance::Bind_LengthTexelU(CShader* pShader, const string& strConstantName)
{
	if(FAILED(pShader->Bind_RawValue(strConstantName, &m_fLengthTexelU, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Bind_LengthTexelV(CShader* pShader, const string& strConstantName)
{
	if (FAILED(pShader->Bind_RawValue(strConstantName, &m_fLengthTexelV, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

}
