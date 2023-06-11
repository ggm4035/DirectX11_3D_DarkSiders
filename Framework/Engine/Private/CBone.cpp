#include "..\Public\CBone.h"

CBone::CBone()
{
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
}

HRESULT CBone::Initialize(aiNode* pAINode, CBone* pParent)
{
	m_strName = pAINode->mName.data;
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombinedTransformationMatrix = m_TransformationMatrix;

	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	}
}

CBone* CBone::Create(aiNode* pAINode, CBone* pParent)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, pParent)))
	{
		MSG_BOX("Failed to Created CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
	Safe_Release(m_pParent);
}
