#include "CGameObjectUI.h"

#include "CApplication.h"

CGameObjectUI::CGameObjectUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

CGameObjectUI::CGameObjectUI(const CGameObjectUI& rhs)
	: CGameObject(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

HRESULT CGameObjectUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObjectUI::Initialize(CComponent* pOwner, void* pArg)
{
	if (FAILED(CComposite::Initialize(pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		UIDESC UIDesc;
		memmove(&UIDesc, pArg, sizeof UIDesc);

		Set_Scale(_float2(UIDesc.m_fSizeX, UIDesc.m_fSizeY));
		Set_Position(_float2(UIDesc.m_fX, UIDesc.m_fY));
	}

	return S_OK;
}

void CGameObjectUI::Tick(const _double& TimeDelta)
{
}

void CGameObjectUI::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CGameObjectUI::Render()
{
	return S_OK;
}

_float2 CGameObjectUI::Get_Scale()
{
	return _float2(XMVector2Length(XMLoadFloat4x4(&m_WorldMatrix).r[0]).m128_f32[0], 
		XMVector2Length(XMLoadFloat4x4(&m_WorldMatrix).r[1]).m128_f32[0]);
}

void CGameObjectUI::Set_Scale(const _float2& _vScale)
{
	_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	WorldMatrix.r[0] = XMVector2Normalize(WorldMatrix.r[0]) * _vScale.x;
	WorldMatrix.r[1] = XMVector2Normalize(WorldMatrix.r[1]) * _vScale.y;
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CGameObjectUI::Set_Position(const _float2& _vPosition)
{
	_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	WorldMatrix.r[3].m128_f32[0] = _vPosition.x - CApplication::m_iWinSizeX * 0.5f;
	WorldMatrix.r[3].m128_f32[1] = -_vPosition.y + CApplication::m_iWinSizeY * 0.5f;
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CGameObjectUI::Free()
{
	CComposite::Free();
}
