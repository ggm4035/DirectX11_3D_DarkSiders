#include "CTerrain.h"

#include "CPlayer.h"
#include "CGameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	:CGameObject3D(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

 	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	TERRAINDESC* pTerrainDesc = reinterpret_cast<TERRAINDESC*>(pArg);
	wstring wstrTextureTag = pTerrainDesc->wstrTextureTag;
	_uint iXCount = pTerrainDesc->iXCount;
	_uint iZCount = pTerrainDesc->iZCount;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, wstrTextureTag.c_str(),
		L"Com_Texture_Terrain", (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	m_pBufferCom->Load_Terrain(iXCount, iZCount, pTerrainDesc->pPositions);

	return S_OK;
}

void CTerrain::Tick(const _double& TimeDelta)
{
	m_fTimeAcc += TimeDelta;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_Player());
	if (nullptr == pPlayer)
		return;

	CTransform* pPlayerTransform = pPlayer->Get_Transform();
	if (nullptr == pPlayerTransform)
		return;

	CNavigation* pPlayerNavigation = dynamic_cast<CNavigation*>(pPlayer->Get_Component(L"Com_Navigation"));
	if (nullptr == pPlayerNavigation)
		return;

	if (CCell::OPT_LAVA == pPlayerNavigation->Get_Cur_Cell_Option() && 
		m_fTimeAcc > 0.1f && false == pPlayerTransform->isJump())
	{
		m_fTimeAcc = 0.f;
		pPlayer->Get_Damaged(m_pAttack);
	}

	Safe_Release(pGameInstance);
}

void CTerrain::AfterFrustumTick(const _double& TimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
		if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
			return;
#endif
	}
}

void CTerrain::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CTerrain::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	
	m_pShaderCom->Begin(0);

	m_pBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	if (FAILED(Add_Component(m_iLevelIndex, L"VIBuffer_Terrain",
		L"Com_Buffer_Terrain", (CComponent**)&m_pBufferCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(m_iLevelIndex, L"Texture_NMTerrain",
		L"Com_Texture_NMTerrain", (CComponent**)&m_pNmTextureCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_VtxNorTex",
		L"Com_Shader_VtxNorTex", (CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer",
		L"Com_Renderer", (CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(m_iLevelIndex, L"Navigation",
		L"Com_Navigation", (CComponent**)&m_pNavigationCom, this)))
		return E_FAIL;

	CAttack::ATTACKDESC AttackDesc;
	AttackDesc.iDamage = 1;
	AttackDesc.isIgnoreDeffence = true;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Status_Attack", L"Com_Attack",
		(CComponent**)&m_pAttack, this, &AttackDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 InputMatrix = m_pTransformCom->Get_WorldFloat4x4();
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_WorldMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_VIEW);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ViewMatrix", &InputMatrix)))
		return E_FAIL;

	InputMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::STATE_PROJ);
	if (FAILED(m_pShaderCom->Bind_Float4x4("g_ProjMatrix", &InputMatrix)))
		return E_FAIL;

	_float4 InputFloat4 = pGameInstance->Get_Camera_Position();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &InputFloat4, sizeof(_float4))))
		return E_FAIL;

	_float3 InputFloat3 = _float3(1.f, 0.3f, 0.3f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vFogColor", &InputFloat3, sizeof(_float3))))
		return E_FAIL;

	_float InputFloat = 250.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFogRange", &InputFloat, sizeof(_float))))
		return E_FAIL;

	InputFloat = 0.5f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFogDensity", &InputFloat, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pNmTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject3D* CTerrain::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNmTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pAttack);

	CGameObject3D::Free();
}
