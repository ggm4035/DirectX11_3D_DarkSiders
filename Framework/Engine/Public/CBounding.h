#pragma once

#include "DebugDraw.h"
#include "CBase.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3 vPosition;
	}BOUNDINGDESC;

protected:
	explicit CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding(const CBounding& rhs);
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) = 0;
	virtual void Tick(_fmatrix WorldMatrix) = 0;
	virtual HRESULT Render() = 0;

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

protected:
	_bool m_isColl = { false };

protected:
	HRESULT Begin_Batch();
	void End_Batch() {
		m_pBatch->End();
	}

private:
	_bool m_isCloned = { false };

public:
	virtual CBounding* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END