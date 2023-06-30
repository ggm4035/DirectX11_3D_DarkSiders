#pragma once

#include "CGameObject3D.h"

BEGIN(Engine)

class ENGINE_DLL CParts abstract : public CGameObject3D
{
public:
	typedef struct tagParentDesc : public CTransform::TRASNFORMDESC
	{
		_float4x4 OffsetMatrix;
		_float4x4 PivotMatrix;
		const _float4x4* pCombinedTransformationMatrix;
		const _float4x4* pNotMoveCombinedTransformationMatrix;
		const _float4x4* pWorldMatrix;
	}PARENTDESC;

protected:
	explicit CParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CParts(const CGameObject3D& rhs);
	virtual ~CParts() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	/* Transform 정보 초기화 하고 싶으면 void*에 값 채우기 */
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override {}
	virtual void Late_Tick(const _double& TimeDelta) override {}
	virtual HRESULT Render() override { return S_OK; }

protected:
	_float4x4  m_WorldMatrix;
	_float4x4  m_OffsetMatrix;
	_float4x4  m_PivotMatrix;
	const _float4x4* m_pCombinedTransformationMatrix;
	const _float4x4* m_pNotMoveCombinedTransformationMatrix;
	const _float4x4* m_pParentWorldMatrix;

protected:
	virtual HRESULT Add_Components() = 0;

public:
	virtual CParts* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END