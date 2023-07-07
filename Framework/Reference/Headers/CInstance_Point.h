#pragma once

#include "CVIBuffer_Instancing.h"

BEGIN(Engine)

class CInstance_Point abstract : public CVIBuffer_Instancing
{
protected:
	explicit CInstance_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInstance_Point(const CInstance_Point& rhs);
	virtual ~CInstance_Point() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCEDESC* pInstanceDesc) override;
	virtual	HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;

public:
	virtual CInstance_Point* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override = 0;
	virtual void Free() override;
};

END