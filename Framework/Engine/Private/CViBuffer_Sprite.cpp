#include "CVIBuffer_Sprite.h"

#include "CTexture.h"
#include "CShader.h"

CVIBuffer_Sprite::CVIBuffer_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Sprite::CVIBuffer_Sprite(const CVIBuffer_Sprite& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Sprite::Initialize_Prototype()
{
	m_iVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSTEX) };
	m_iNumVertices = 4; // 사각형이니까 버텍스 개수는 4개읾
	m_iIndexStride = { sizeof(_ushort) };
	m_iNumIndices = 6;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexCoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexCoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexCoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexCoord = _float2(0.f, 1.f);

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
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };

	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ushort* pIndices = new _ushort[m_iNumIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(CVIBuffer::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Sprite::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(CVIBuffer::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	SPRITEDESC Desc = *static_cast<SPRITEDESC*>(pArg);

	m_isRepeat = Desc.bRepeat;
	m_fFrameTick = Desc.fFrameSpeed / _float(Desc.iNumHeight * Desc.iNumWidth);
	m_iNumHeight = Desc.iNumHeight;
	m_iNumWidth = Desc.iNumWidth;

	return S_OK;
}

HRESULT CVIBuffer_Sprite::Render()
{
	if (false == m_isPlay)
		return S_OK;

	if (FAILED(CVIBuffer::Render()))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Sprite::Tick(const _double& TimeDelta)
{
	if (false == m_isPlay)
		return;

	/* 여기에 텍스처 uv의 좌표 4개를 만듦 */

	m_fTimeAcc += TimeDelta;
	if (m_fFrameTick < m_fTimeAcc)
	{
		++m_iCurKeyFrame;

		_uint iNumSprite = m_iNumHeight * m_iNumWidth;

		if (m_iCurKeyFrame >= iNumSprite)
		{
			if (false == m_isRepeat)
				m_isPlay = false;

			m_iCurKeyFrame = 0;
		}

		m_fTimeAcc = 0.f;
	}

	_uint iHeightIndex = m_iCurKeyFrame / m_iNumWidth;
	_uint iWidthIndex = m_iCurKeyFrame % m_iNumWidth;
	_float fLengthTexelU = 1.f / m_iNumWidth;
	_float fLengthTexelV = 1.f / m_iNumHeight;

	VTXPOSTEX* pVertex = { nullptr };
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &m_MappedSubResource);
	pVertex = reinterpret_cast<VTXPOSTEX*>(m_MappedSubResource.pData);

	/* 여기서 텍스처의 uv좌표를 0 ~ 1 사이값 중에 현재 스프라이트 위치에 맞게 변경 */
	pVertex[0].vTexCoord = _float2(fLengthTexelU * iWidthIndex, fLengthTexelV * iHeightIndex);
	pVertex[1].vTexCoord = _float2(fLengthTexelU * iWidthIndex + fLengthTexelU, fLengthTexelV * iHeightIndex);
	pVertex[2].vTexCoord = _float2(fLengthTexelU * iWidthIndex + fLengthTexelU, fLengthTexelV * iHeightIndex + fLengthTexelV);
	pVertex[3].vTexCoord = _float2(fLengthTexelU * iWidthIndex, fLengthTexelV * iHeightIndex + fLengthTexelV);

	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Sprite* CVIBuffer_Sprite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Sprite* pInstance = new CVIBuffer_Sprite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CVIBuffer_Sprite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Sprite* CVIBuffer_Sprite::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CVIBuffer_Sprite* pInstance = new CVIBuffer_Sprite(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Sprite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Sprite::Free()
{
	CVIBuffer::Free();
}
