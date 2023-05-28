#include "CGameObject.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComposite(pDevice, pContext)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	:CComposite(rhs)
{
}
