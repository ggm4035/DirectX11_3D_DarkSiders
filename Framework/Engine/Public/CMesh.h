#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	explicit CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(CComponent* pOwner, void* pArg) override;

private:
	string m_strName;
	_uint m_iMaterialIndex = { 0 };

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	virtual CMesh* Clone(CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END