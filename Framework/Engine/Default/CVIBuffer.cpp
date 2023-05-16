#include "CVIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	:CComponent(rhs)
{
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	if (FAILED(CComponent::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	if (FAILED(CComponent::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(OUT ID3D11Buffer** ppOut)
{
	if (nullptr == m_pDevice)
		return E_FAIL;
	
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData,ppOut);
}

CComponent* CVIBuffer::Clone(void* pArg)
{
	return nullptr;
}

void CVIBuffer::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	CComponent::Free();
}
