#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CSoul final : public CGameObject3D
{
public:
	typedef struct tagSoulDesc : public CTransform::TRASNFORMDESC
	{
		_float4 vAccel;
		_float4 vVelocity;
		_double dGenTime;
		_float4 vPosition;
	}SOULDESC;

private:
	explicit CSoul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSoul(const CSoul& rhs);
	virtual ~CSoul() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render();

public:
	virtual void Dead_Motion(const _double& TimeDelta) override;

private:
	SOULDESC m_tSoulDesc;
	_bool m_isFinish = { false };
	_bool m_isGen = { false };
	_bool m_isFinishFirstStep = { false }; // 첫 번째 행동이 끝났는지
	_float m_fTimeAcc = { 0.f };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

	/* y양의 방향으로 xz랜덤방향으로 이동 */
	void First_Step(const _double& TimeDelta);
	/* 플레이어에게 이동 */
	void Second_Step(const _double& TimeDelta);

public:
	static CSoul* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSoul* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END