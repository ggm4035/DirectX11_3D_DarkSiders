#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sprite final : public CVIBuffer
{
public:
	typedef struct tagSpriteDesc
	{
		_bool bRepeat;		// 반복재생 유무
		_float fFrameSpeed; // 초 단위로 넣으면 된다.
		_uint iNumWidth;
		_uint iNumHeight;
	}SPRITEDESC;

private:
	explicit CVIBuffer_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Sprite(const CVIBuffer_Sprite& rhs);
	virtual ~CVIBuffer_Sprite() = default;

public:
	_bool isPlay() const {
		return m_isPlay;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual HRESULT Render() override;

	void Tick(const _double& TimeDelta);

public:
	HRESULT Bind_LengthTexelU(class CShader* pShader, const string& strConstantName);
	HRESULT Bind_LengthTexelV(class CShader* pShader, const string& strConstantName);

private:
	_bool m_isRepeat = { true };
	_bool m_isPlay = { true };
	_float m_fFrameTick = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_uint m_iNumWidth = { 0 };
	_uint m_iNumHeight = { 0 };
	_uint m_iCurKeyFrame = { 0 };

public:
	static CVIBuffer_Sprite* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CVIBuffer_Sprite* Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg) override;
	virtual void Free() override;
};

END