#include "CVIBuffer_Terrain.h"

#include "CFrustum.h"
#include "CQuardTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
	ZeroMemory(&m_MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_iXCount(rhs.m_iXCount)
	, m_iZCount(rhs.m_iZCount)
	, m_fInterval(rhs.m_fInterval)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _uint& iXCount, const _uint& iZCount, const _float& fInterval)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	m_iXCount = iXCount;
	m_iZCount = iZCount;
	m_fInterval = fInterval;

	m_iVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = { m_iXCount * m_iZCount };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { (m_iXCount - 1) * (m_iZCount - 1) * 6 };
	m_eFormat = { DXGI_FORMAT_R32_UINT };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iZCount; ++i)
	{
		for (_uint j = 0; j < m_iXCount; ++j)
		{
			_uint iIndex = i * m_iXCount + j;

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3(j, 0.f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexCoord = _float2(j / (m_iXCount - 1.f), i / (m_iZCount - 1.f));
		}
	}

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

	TRIANGLE TriangleDesc;
	ZeroMemory(&TriangleDesc, sizeof TriangleDesc);

	_uint iNumIndices = { 0 };

	for (_uint i = 0; i < m_iZCount - 1; ++i)
	{
		for (_uint j = 0; j < m_iXCount - 1; ++j)
		{
			_uint iIndex = i * m_iXCount + j;

			_uint iIndices[4] = {
				iIndex + m_iXCount,
				iIndex + m_iXCount + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			TriangleDesc.vDot[0] = pVertices[iIndices[0]].vPosition;
			TriangleDesc.vDot[1] = pVertices[iIndices[1]].vPosition;
			TriangleDesc.vDot[2] = pVertices[iIndices[2]].vPosition;
			m_vecTriangle.push_back(TriangleDesc);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);

			TriangleDesc.vDot[0] = pVertices[iIndices[0]].vPosition;
			TriangleDesc.vDot[1] = pVertices[iIndices[2]].vPosition;
			TriangleDesc.vDot[2] = pVertices[iIndices[3]].vPosition;

			m_vecTriangle.push_back(TriangleDesc);
		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
#if defined(_USE_IMGUI) || defined(_DEBUG)
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
#else
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.CPUAccessFlags = { 0 };
#endif
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.MiscFlags = { 0 };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.StructureByteStride = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	Safe_Release(m_pQuadTree);

	m_pQuadTree = CQuadTree::Create(m_iXCount * m_iZCount - m_iXCount,
		m_iXCount * m_iZCount - 1,
		m_iXCount - 1,
		0);

	if (FAILED(m_pQuadTree->Make_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CVIBuffer::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Render()
{
	if (FAILED(CVIBuffer::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Load_Terrain(const _uint iXCount, const _uint iZCount, const _float3* pPositions)
{
	_uint iTriangleIndex = { 0 };

	Safe_Delete_Array(m_pVerticesPos);
	Safe_Delete_Array(m_pIndices);

	Initialize_Prototype(iXCount, iZCount, 1.f);

	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	VTXPOSNORTEX* pVertices = reinterpret_cast<VTXPOSNORTEX*>(MappedSubResource.pData);

	for (_uint i = 0; i < iZCount; ++i)
	{
		for (_uint j = 0; j < iXCount; ++j)
		{
			_uint iIndex = i * iXCount + j;

			pVertices[iIndex].vPosition = pPositions[iIndex];
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexCoord = _float2(j / (iXCount - 1.f), i / (iZCount - 1.f));
		}
	}

	for (_uint i = 0; i < iZCount - 1; ++i)
	{
		for (_uint j = 0; j < iXCount - 1; ++j)
		{
			_uint iIndex = i * iXCount + j;

			_uint iIndices[4] = {
				iIndex + iXCount,
				iIndex + iXCount + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			m_vecTriangle[iTriangleIndex].vDot[0] = pVertices[iIndices[0]].vPosition;
			m_vecTriangle[iTriangleIndex].vDot[1] = pVertices[iIndices[1]].vPosition;
			m_vecTriangle[iTriangleIndex].vDot[2] = pVertices[iIndices[2]].vPosition;
			++iTriangleIndex;

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);

			m_vecTriangle[iTriangleIndex].vDot[0] = pVertices[iIndices[0]].vPosition;
			m_vecTriangle[iTriangleIndex].vDot[1] = pVertices[iIndices[2]].vPosition;
			m_vecTriangle[iTriangleIndex].vDot[2] = pVertices[iIndices[3]].vPosition;
			++iTriangleIndex;
		}
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Load_Terrain()
{
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	VTXPOSNORTEX* pVertices = reinterpret_cast<VTXPOSNORTEX*>(MappedSubResource.pData);

	for (_uint i = 0; i < m_iXCount * m_iZCount; ++i)
		pVertices[i].vNormal = _float3(0.f, 0.f, 0.f);

	for (_uint i = 0; i < m_iZCount - 1; ++i)
	{
		for (_uint j = 0; j < m_iXCount - 1; ++j)
		{
			_uint iIndex = i * m_iXCount + j;

			_uint iIndices[4] = {
				iIndex + m_iXCount,
				iIndex + m_iXCount + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	CFrustum* pFrustum = CFrustum::GetInstance();
	Safe_AddRef(pFrustum);

	pFrustum->Transform_To_LocalSpace(WorldMatrix);

	_uint iNumIndices = { 0 };

	m_pQuadTree->Culling(pFrustum, m_pVerticesPos, m_pIndices, &iNumIndices);

	D3D11_MAPPED_SUBRESOURCE SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pIndices, sizeof(_ulong) * iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;

	Safe_Release(pFrustum);
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	const _uint& iXCount, const _uint& iZCount, const _float& fInterval)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iXCount, iZCount, fInterval)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	Safe_Release(m_pQuadTree);
	Safe_Delete_Array(m_pIndices);

	CVIBuffer::Free();
}
