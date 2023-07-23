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

HRESULT CGameObjectUI::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(CComposite::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	UIDESC UIDesc;
	memmove(&UIDesc, pArg, sizeof UIDesc);

	Set_Scale(_float2(UIDesc.m_fSizeX, UIDesc.m_fSizeY));
	Set_Position(_float3(UIDesc.m_fX, UIDesc.m_fY, UIDesc.m_fDepth));

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

void CGameObjectUI::Set_Position(const _float3& _vPosition)
{
	_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	WorldMatrix.r[3].m128_f32[0] = _vPosition.x - CApplication::m_iWinSizeX * 0.5f;
	WorldMatrix.r[3].m128_f32[1] = -_vPosition.y + CApplication::m_iWinSizeY * 0.5f;
	WorldMatrix.r[3].m128_f32[2] = _vPosition.z;
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CGameObjectUI::Free()
{
	CGameObject::Free();
}
