#pragma once

#include "CGameObject3D.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)

class CLava final : public CGameObject3D
{
private:
	explicit CLava(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLava(const CLava& rhs);
	virtual ~CLava() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_ftimeAcc = { 0.f };

private:
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CLava* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLava* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END