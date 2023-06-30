#pragma once

#include "CVIBuffer.h"
#include "CModel.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
	friend class CModel;

private:
	explicit CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	const _uint& Get_MaterialIndex() const {
		return m_iMaterialsIndex;
	}

	void Get_Matrices(const CModel::BONES& vecBones, OUT _float4x4* pMatrices, _fmatrix PivotMatrix);

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const MESHDATA& MeshData);
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;

private:
	_char m_szName[MAX_PATH] = { "" };
	_uint m_iMaterialsIndex = { 0 };

private:
	_uint m_iNumBones = { 0 };
	vector<_uint> m_vecBoneIndices; /* �𵨿� �ε��س��� ��ü���� �ε���. */

private:
	HRESULT Ready_VertexBuffer_NonAnim(const MESHDATA& MeshData);
	HRESULT Ready_VertexBuffer_Anim(const MESHDATA& MeshData);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const MESHDATA& MeshData);
	virtual CMesh* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END