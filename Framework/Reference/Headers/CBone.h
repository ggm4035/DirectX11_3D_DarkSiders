#pragma once

#include "CModel.h"

BEGIN(Engine)

class CBone final : public CBase
{
	friend class CModel;

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

	_matrix Get_NotMoveCombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_NotMoveCombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationMatrixPtr() const {
		return &m_CombinedTransformationMatrix;
	}

	const _float4x4* Get_NotMoveCombinedTransformationMatrixPtr() const {
		return &m_NotMoveCombinedTransformationMatrix;
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	void Set_NotMoveTransformationMatrix(_fmatrix NotMoveTransformationMatrix) {
		XMStoreFloat4x4(&m_NotMoveTransformationMatrix, NotMoveTransformationMatrix);
	}

	void Set_CombinedTransformationMatrix(_fmatrix CombinedTransformationMatrix) {
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedTransformationMatrix);
	}

public:
	HRESULT Initialize(const BONEDATA& BoneData, CBone* pParent);
	void Invalidate_CombinedTransformationMatrix(const CModel::BONES& Bones);

private:
	string m_strName = { "" };
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix; /* 객체가 이동할 때 참조할 상태행렬 */

	_float4x4 m_NotMoveTransformationMatrix;
	_float4x4 m_NotMoveCombinedTransformationMatrix; /* 애니메이션의 이동값을 없앤 상태행렬, 이제 애니메이션은 이 행렬만 이용한다. */

	_float4x4 m_OffsetMatrix;
	_int m_iParentIndex = { -1 };
	_uint m_iIndex = { 0 };

public:
	static CBone* Create(const BONEDATA& BoneData, CBone* pParent);
	CBone* Clone();
	virtual void Free() override;
};

END