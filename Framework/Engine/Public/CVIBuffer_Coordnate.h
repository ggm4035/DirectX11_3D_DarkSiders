#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Coordnate final : public CVIBuffer
{
private:
	explicit CVIBuffer_Coordnate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Coordnate(const CVIBuffer_Coordnate& rhs);
	virtual ~CVIBuffer_Coordnate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual HRESULT Render() override;

public:
	static CVIBuffer_Coordnate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CVIBuffer_Coordnate* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END