#pragma once

#include "CVIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct tagPointInstanceDesc : public CVIBuffer_Instancing::INSTANCEDESC
	{
		_float fFrameSpeed = 0.f; // �� ������ ������ �ȴ�.
		_uint iNumWidth = 1;
		_uint iNumHeight = 1;
	}POINTINSTDESC;

private:
	CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;
	void Tick(const vector<_float4x4>& vecMatrices, const _double& TimeDelta);

public:
	HRESULT Bind_LengthTexelU(class CShader* pShader, const string& strConstantName);
	HRESULT Bind_LengthTexelV(class CShader* pShader, const string& strConstantName);

private:
	_float m_fFrameTick = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_float m_fLengthTexelU = { 0.f };
	_float m_fLengthTexelV = { 0.f };
	_uint m_iNumWidth = { 0 };
	_uint m_iNumHeight = { 0 };
	_uint m_iCurKeyFrame = { 0 };

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual void Free() override;
};

END