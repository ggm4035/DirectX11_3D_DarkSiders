#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Trail;
END

BEGIN(Client)

class CSwordTrail : public CGameObject3D
{
private:
	explicit CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSwordTrail(const CSwordTrail& rhs);
	virtual ~CSwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render(CShader* pShader, const _uint& iPassNum);

private:

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Trail* m_pBufferCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_ShaderResources();

public:
	static CSwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSwordTrail* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Free() override;
};

END