#pragma once

#include "CBase.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	explicit CBone();
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

	void Set_OffsetMatrix(const _float4x4& OffsetMatrix) {
		m_OffsetMatrix = OffsetMatrix;
	}

public:
	HRESULT Initialize(aiNode* pAINode, CBone* pParent);
	void Invalidate_CombinedTransformationMatrix();

private:
	string m_strName = { "" };
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix;
	_float4x4 m_OffsetMatrix;
	CBone* m_pParent = { nullptr };

public:
	static CBone* Create(aiNode* pAINode, CBone* pParent);
	virtual void Free() override;
};

END