#pragma once

#include "CVIBuffer.h"
#include "CModel.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	explicit CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	const _uint& Get_MaterialIndex() const {
		return m_MeshDataDesc.iMaterialIndex;
	}

	void Get_Matrices(const CModel::BONES& vecBones, OUT _float4x4* pMatrices);

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const CModel::BONES& vecBones, const MESHDATA& MeshData, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(CComponent* pOwner, void* pArg) override;

private:
	MESHDATA m_MeshDataDesc;

private:
	_uint m_iNumBones = { 0 };
	vector<_uint> m_BoneIndices; /* ∏µ®ø° ∑ŒµÂ«ÿ≥ı¿∫ ¿¸√ºª¿¿« ¿Œµ¶Ω∫. */

private:
	HRESULT Ready_VertexBuffer_NonAnim(const MESHDATA& MeshData, _fmatrix PivotMatrix);
	//HRESULT Ready_VertexBuffer_Anim(const MESHDATA& MeshData, const CModel::BONES& vecBones);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const CModel::BONES& vecBones, const MESHDATA& MeshData, _fmatrix PivotMatrix);
	virtual CMesh* Clone(CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END