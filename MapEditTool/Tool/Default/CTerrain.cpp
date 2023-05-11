#include "pch.h"
#include "CTerrain.h"

CTerrain::CTerrain()
{
}

HRESULT CTerrain::Initialize()
{
	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
}

HRESULT CTerrain::Render()
{
	return S_OK;
}

void CTerrain::Set_Ratio(_float4x4* pOut, const _float fRatioX, const _float fRatioY)
{
	pOut->_11 *= fRatioX;
	pOut->_21 *= fRatioX;
	pOut->_31 *= fRatioX;
	pOut->_41 *= fRatioX;

	pOut->_12 *= fRatioY;
	pOut->_22 *= fRatioY;
	pOut->_32 *= fRatioY;
	pOut->_42 *= fRatioY;
}