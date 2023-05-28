#include "CPipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

_matrix CPipeLine::Get_Transform_Matrix(TRANSFORMSTATE eState)
{
	if (0 > eState || STATE_END <= eState)
		return _matrix();

	return XMLoadFloat4x4(&m_Transform[eState]);
}

_matrix CPipeLine::Get_Transform_Inverse_Matrix(TRANSFORMSTATE eState)
{
	if (0 > eState || STATE_END <= eState)
		return _matrix();

	return XMLoadFloat4x4(&m_Transform_Inverse[eState]);
}

_float4x4 CPipeLine::Get_Transform_Float4x4(TRANSFORMSTATE eState)
{
	if (0 > eState || STATE_END <= eState)
		return _float4x4();

	return m_Transform[eState];
}

_float4x4 CPipeLine::Get_Trasnform_Inverse_Float4x4(TRANSFORMSTATE eState)
{
	if (0 > eState || STATE_END <= eState)
		return _float4x4();

	return m_Transform_Inverse[eState];
}

_matrix CPipeLine::Get_UI_View_Matrix()
{
	return XMMatrixIdentity();
}

_float4x4 CPipeLine::Get_UI_View_Float4x4()
{
	_float4x4 RetVal;
	XMStoreFloat4x4(&RetVal, XMMatrixIdentity());
	return RetVal;
}

_matrix CPipeLine::Get_UI_Proj_Matrix(const _uint iWinSizeX, const _uint iWinSizeY)
{
	return XMMatrixOrthographicLH((_float)iWinSizeX, (_float)iWinSizeY, 0.f, 1.f);
}

_float4x4 CPipeLine::Get_UI_Proj_Float4x4(const _uint iWinSizeX, const _uint iWinSizeY)
{
	_float4x4 RetVal;
	XMStoreFloat4x4(&RetVal, XMMatrixOrthographicLH((_float)iWinSizeX, (_float)iWinSizeY, 0.f, 1.f));
	return RetVal;
}

HRESULT CPipeLine::Set_Transform(TRANSFORMSTATE eState, _fmatrix _Matrix)
{
	if (0 > eState || STATE_END <= eState)
		return E_FAIL;

	XMStoreFloat4x4(&m_Transform[eState], _Matrix);

	return S_OK;
}

CPipeLine::CPipeLine()
{
	for (_uint i = 0; i < STATE_END; ++i)
	{
		XMStoreFloat4x4(&m_Transform[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixIdentity());
	}
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < STATE_END; ++i)
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_Transform[i])));
}

void CPipeLine::Free()
{
}
