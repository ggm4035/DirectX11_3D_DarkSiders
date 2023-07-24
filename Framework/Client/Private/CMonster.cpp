
#include "CMonster.h"

#include "CRoot.h"
#include "CGameInstance.h"
#include "CUI_HpBar.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject3D(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	:CGameObject3D(rhs)
{
}

HRESULT CMonster::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CGameObject3D::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (FAILED(CMonster::Add_Components()))
		return E_FAIL;

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVector3TransformCoord(vPosition, CPipeLine::GetInstance()->Get_Transform_Matrix(CPipeLine::STATE_VIEW));
	vPosition = XMVector3TransformCoord(vPosition, CPipeLine::GetInstance()->Get_Transform_Matrix(CPipeLine::STATE_PROJ));

	D3D11_VIEWPORT ViewPortDesc;
	_uint iNumView = { 1 };

	m_pContext->RSGetViewports(&iNumView, &ViewPortDesc);

	CUI_HpBar::UIHPBARDESC UIDesc;
	UIDesc.m_fX = ((XMVectorGetX(vPosition) + 1.0f) * 0.5f) * ViewPortDesc.Width;
	UIDesc.m_fY = ((-XMVectorGetY(vPosition) + 1.0f) * 0.5f) * ViewPortDesc.Height;
	UIDesc.m_fDepth = XMVectorGetZ(vPosition);
	UIDesc.m_fSizeX = 50.f;
	UIDesc.m_fSizeY = 10.f;
	UIDesc.wstrTextureTag = L"Texture_UI_HpBar_Monster";
	UIDesc.iTextureLevelIndex = LEVEL_GAMEPLAY;
	UIDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	UIDesc.vOffset = _float3(0.f, 2.f, 0.f);
	UIDesc.pMaxHp = &m_Status.iMaxHP;
	UIDesc.pHp = &m_Status.iHP;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pGameObject = pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, L"UI_HpBar", L"UI_HealthBar", this, &UIDesc);
	m_pHealthBar = dynamic_cast<CUI_HpBar*>(pGameObject);
	if (nullptr == m_pHealthBar)
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (nullptr != pArg)
	{
		m_pTransformCom->Set_Matrix(reinterpret_cast<MONSTERDESC*>(pArg)->WorldMatrix);
		m_pTransformCom->Set_Angle(reinterpret_cast<MONSTERDESC*>(pArg)->vAngle);

		CNavigation::NAVIGATIONDESC NaviDesc;
		NaviDesc.iCurrentIndex = reinterpret_cast<MONSTERDESC*>(pArg)->iNavigationIndex;
		if (FAILED(Add_Component(LEVEL_GAMEPLAY, L"Navigation", L"Com_Navigation",
			(CComponent**)&m_pNavigationCom, this, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Bind_Navigation(m_pNavigationCom);
	}

	return S_OK;
}

void CMonster::Tick(const _double& TimeDelta)
{
	On_Colisions(TimeDelta);

	CGameObject3D::Tick(TimeDelta);

	m_pRoot->Tick(TimeDelta); /* 객체가 움직여 애니메이션 변경 */

	m_pModelCom->Play_Animation(TimeDelta, m_pNavigationCom);

	m_pTransformCom->Animation_Movement(m_pModelCom, TimeDelta); /* 객체가 진짜 움직임 */

	m_pHealthBar->Tick(TimeDelta);

	Tick_Colliders(m_pTransformCom->Get_WorldMatrix());
}

void CMonster::AfterFrustumTick(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (FAILED(Add_Colliders_To_Manager()))
		{
			MSG_BOX("Failed to Add Colliders To Manager");
			Safe_Release(pGameInstance);
			return;
		}

		for (auto Pair : m_Parts)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, static_cast<CGameObject*>(Pair.second));

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

		if (nullptr != m_pHealthBar &&
			m_Status.iHP < m_Status.iMaxHP)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, m_pHealthBar);

#ifdef _DEBUG
		if (true == m_isRender && FAILED(Add_Colliders_Debug_Render_Group(m_pRendererCom)))
			return;
#endif
	}

	Safe_Release(pGameInstance);
}

void CMonster::Late_Tick(const _double& TimeDelta)
{
}
/* PassNum
0 : Default
1 : Hit
*/
HRESULT CMonster::Render(/*const _uint& iPassIndex*/)
{
	if (FAILED(Set_Shader_Resources()))
		return E_FAIL;

	_uint iPassNum = { 0 };

	if (NONE != m_eCurHitState)
		iPassNum = 1;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint iMeshIndex = 0; iMeshIndex < iNumMeshes; ++iMeshIndex)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iMeshIndex);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, TYPE_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", iMeshIndex, TYPE_NORMALS);

		if (FAILED(m_pShaderCom->Begin(iPassNum)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(iMeshIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CMonster::OnCollisionEnter(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pOtherCollider->Get_Tag() == L"Col_Body" &&
		Collision.pMyCollider->Get_Tag() == L"Col_Body")
	{
		_vector vOtherPosition = Collision.pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vOtherPosition);
		m_pTransformCom->Repersive(vDir, TimeDelta);
	}
}

void CMonster::OnCollisionStay(CCollider::COLLISION Collision, const _double& TimeDelta)
{
	if (Collision.pOtherCollider->Get_Tag() == L"Col_Body" &&
		Collision.pMyCollider->Get_Tag() == L"Col_Body")
	{
		_vector vOtherPosition = Collision.pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vOtherPosition);
		m_pTransformCom->Repersive(vDir, TimeDelta);
	}
}

void CMonster::OnCollisionExit(CCollider::COLLISION Collision, const _double& TimeDelta)
{
}

HRESULT CMonster::Add_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, L"Shader_AnimMesh",
		L"Com_Shader", (CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, L"Renderer",
		L"Com_Renderer", (CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Set_Shader_Resources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fHitTimeAcc, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMonster::Free()
{
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRoot);

	Safe_Release(m_pHealthBar);

	CGameObject3D::Free();
}
