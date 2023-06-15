#pragma once

#include "CModel.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	explicit CBone();
	explicit CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	const string& Get_Name() const {
		return m_strName;
	}

	_matrix Get_OffsetMatrix() const {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(const BONEDATA& BoneData, CBone* pParent);
	void Invalidate_CombinedTransformationMatrix(const CModel::BONES& Bones);

private:
	string m_strName = { "" };
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix;
	_float4x4 m_OffsetMatrix;
	_int m_iParentIndex = { -1 };
	_uint m_iIndex = { 0 };

public:
	static CBone* Create(const BONEDATA& BoneData, CBone* pParent);
	CBone* Clone();
	virtual void Free() override;
};

END