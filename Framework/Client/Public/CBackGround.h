#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
class CTransform;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
protected:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	_float4x4 m_ViewMatrix, m_ProjMatrix;
	_float m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	virtual HRESULT Add_Components() override;
	HRESULT SetUp_ShaderResources();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END