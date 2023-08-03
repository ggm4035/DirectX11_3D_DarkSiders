#pragma once
#include "CVIBuffer_Instancing.h"

BEGIN(Engine)

class CInstance_Parabola : public CVIBuffer_Instancing
{
public:
	typedef struct tagMeshInstanceDesc : public INSTANCEDESC
	{
		_float4x4 WorldMatrix;
		MESHDATA MeshData;
	}MESHINSTANCEDESC;

private:
	explicit CInstance_Parabola(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInstance_Parabola(const CInstance_Parabola& rhs);
	virtual ~CInstance_Parabola() = default;

public:
	virtual HRESULT Initialize_Prototype(const MESHINSTANCEDESC* pInstanceDesc);
	virtual	HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;

public:
	static CInstance_Parabola* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MESHINSTANCEDESC* pInstanceDesc);
	virtual CInstance_Parabola* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END