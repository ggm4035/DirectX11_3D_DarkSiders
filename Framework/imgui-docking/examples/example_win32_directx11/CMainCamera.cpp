#include "CMainCamera.h"

#include "CGameInstance.h"
#include "CToolInstance.h"

BEGIN(Imgui)

CMainCamera::CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CMainCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainCamera::Initialize(CComponent* pOwner, void* pArg)
{
	if (FAILED(CCamera::Initialize(pOwner, pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CMainCamera::Tick(const _double& TimeDelta)
{
	CCamera::Tick(TimeDelta);
	KeyInput(TimeDelta);
}

void CMainCamera::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CMainCamera::Render()
{
	return S_OK;
}

HRESULT CMainCamera::Add_Components()
{
	return S_OK;
}

void CMainCamera::KeyInput(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_SPACE))
		m_pTransformCom->Go_Up(TimeDelta);

    if (ImGui::IsMousePosValid() && pGameInstance->Mouse_Pressing(CInput_Device::DIM_RB))
    {
        _long dwMouseMove = 0;

        if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (dwMouseMove)*TimeDelta / 10.f);
        }

        if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (dwMouseMove)*TimeDelta / 10.f);
        }
    }

	Safe_Release(pGameInstance);
}

CMainCamera* CMainCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainCamera* pInstance = new CMainCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created CMainCamera");
	}
	return pInstance;
}

CMainCamera* CMainCamera::Clone(CComponent* pOwner, void* pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned CMainCamera");
	}
	return pInstance;
}

void CMainCamera::Free()
{
	CCamera::Free();
}

END
