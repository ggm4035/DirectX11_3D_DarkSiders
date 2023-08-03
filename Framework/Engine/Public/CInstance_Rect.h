#pragma once

#include "CVIBuffer_Instancing.h"

BEGIN(Engine)
/* 아직 변경 안함 */
class ENGINE_DLL CInstance_Rect abstract : public CVIBuffer_Instancing
{
protected:
	explicit CInstance_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInstance_Rect(const CInstance_Rect& rhs);
	virtual ~CInstance_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;

public:
	virtual CInstance_Rect* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END