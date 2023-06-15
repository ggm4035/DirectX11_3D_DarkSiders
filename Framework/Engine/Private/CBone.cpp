#include "CBone.h"

CBone::CBone()
{
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
}

CBone::CBone(const CBone& rhs)
	: m_strName(rhs.m_strName)
	, m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_CombinedTransformationMatrix(rhs.m_CombinedTransformationMatrix)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_iParentIndex(rhs.m_iParentIndex)
	, m_iIndex(rhs.m_iIndex)
{
}

HRESULT CBone::Initialize(const BONEDATA& BoneData, CBone* pParent)
{
	m_strName = BoneData.szName;
	m_TransformationMatrix = BoneData.TransformationMatrix;
	m_OffsetMatrix = BoneData.OffsetMatrix;
	m_iParentIndex = nullptr == pParent ? -1 : pParent->m_iIndex;
	m_iIndex = BoneData.iIndex;

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const CModel::BONES& Bones)
{
	if (-1 == m_iParentIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix;

	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
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

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}
