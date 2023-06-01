#pragma once

#include "Client_Defines.h"
#include "CGameObject3D.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CTexture;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CTerrain final : public CGameObject3D
{
private:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(CComponent* pOwner, void* pArg);
	virtual void Tick(const _double& TimeDelta);
	virtual void Late_Tick(const _double& TimeDelta);
	virtual HRESULT Render();

private:
	CVIBuffer_Terrain* m_pBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject3D* Clone(CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END