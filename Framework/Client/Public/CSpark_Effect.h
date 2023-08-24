#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Point_Instance;
class CCollider;
END

BEGIN(Client)

class CSpark_Effect final : public CGameObject3D
{
public:
	typedef struct tagStoneParticle
	{
		_bool		isAlive;
		_float4     vAccel;
		_float4     vVelocity;
		_float4x4	WorldMatrix;
		_double     dAge;
		_double     dLifeTime;
		//_double	dGenTime;
	}STONEPARTICLE;

private:
	explicit CSpark_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSpark_Effect(const CSpark_Effect& rhs);
	virtual ~CSpark_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void AfterFrustumTick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override {}
	virtual HRESULT Render();

public:
	void Reset_Effects();
	void Reset_Particle(STONEPARTICLE& Particle);
	void Render_Effect(_fvector vEffectPos);

private:
	vector<STONEPARTICLE> m_vecParticles;
	_uint m_iNumParticles = { 0 };
	_uint m_iCount = { 0 };
	_bool m_isFinishEffect = { false };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Point_Instance* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CSpark_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpark_Effect* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END