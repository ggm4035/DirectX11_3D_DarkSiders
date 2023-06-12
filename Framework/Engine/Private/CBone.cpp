#include "..\Public\CBone.h"

CBone::CBone()
{
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
}

HRESULT CBone::Initialize(const BONEDATA& BoneData, CBone* pParent)
{
	m_strName = BoneData.szName;
	m_TransformationMatrix = BoneData.TransformationMatrix;
	m_OffsetMatrix = BoneData.OffsetMatrix;
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

CBone* CBone::Create(const BONEDATA& BoneData, CBone* pParent)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(BoneData, pParent)))
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
