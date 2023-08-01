#include "CSwordTrail.h"

#include "CGameInstance.h"

CSwordTrail::CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
{
}

CSwordTrail::CSwordTrail(const CSwordTrail& rhs)
	: CGameObject3D(rhs)
{
}

HRESULT CSwordTrail::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CSwordTrail::Tick(const _double& TimeDelta)
{
	m_pBufferCom->Tick(TimeDelta);
}

void CSwordTrail::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CSwordTrail::Render(CShader* pShader, const _uint& iPassNum)
{
	if (FAILED(Set_ShaderResources()))
		return E_FAIL;

	pShader->Begin(iPassNum);

	m_pBufferCom->Render();
	
	return S_OK;
}

HRESULT CSwordTrail::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"VIBuffer_Trail", L"Com_Buffer",
		(CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	/* Texture */

	return S_OK;
}

HRESULT CSwordTrail::Set_ShaderResources()
{

	return S_OK;
}

CSwordTrail* CSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwordTrail* pInstance = new CSwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSwordTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CSwordTrail* CSwordTrail::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CSwordTrail* pInstance = new CSwordTrail(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CSwordTrail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSwordTrail::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);

	CGameObject3D::Free();
}
