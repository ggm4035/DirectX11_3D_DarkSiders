#pragma once

#include "CVIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CInstance_Point final : public CVIBuffer_Instancing
{
public:
	typedef struct tagSpriteDesc : public CVIBuffer_Instancing::INSTANCEDESC
	{
		_float fFrameSpeed = 0.f; // 초 단위로 넣으면 된다.
		_uint iNumWidth = 1.f;
		_uint iNumHeight = 1.f;
	}SPRITEDESC;

protected:
	explicit CInstance_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInstance_Point(const CInstance_Point& rhs);
	virtual ~CInstance_Point() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual	HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	void Tick(vector<_float4x4>& vecMatrices, const _double& TimeDelta);

public:
	HRESULT Bind_LengthTexelU(class CShader* pShader, const string& strConstantName);
	HRESULT Bind_LengthTexelV(class CShader* pShader, const string& strConstantName);

private:
	_float m_fFrameTick = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_float m_fLengthTexelU = { 1.f };
	_float m_fLengthTexelV = { 1.f };
	_uint m_iNumWidth = { 1 };
	_uint m_iNumHeight = { 1 };
	_uint m_iCurKeyFrame = { 0 };

public:
	static CInstance_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CInstance_Point* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Free() override;
};

END