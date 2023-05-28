#pragma once

#include "Tool_Defines.h"
#include "CGameObject3D.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Coordnate;
END

class CCoordnate_Axis final : public CGameObject3D
{
private:
	explicit CCoordnate_Axis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCoordnate_Axis(const CCoordnate_Axis& rhs);
	virtual ~CCoordnate_Axis() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override {}
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Coordnate* m_pBufferCom = { nullptr };

private:
	_bool m_bIsRender = { true };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_ShaderResources();

public:
	static CCoordnate_Axis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCoordnate_Axis* Clone(void* pArg) override;
	virtual void Free() override;
};
