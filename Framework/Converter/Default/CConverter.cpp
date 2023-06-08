#include "CConverter.h"

CConverter::CConverter()
{
}

HRESULT CConverter::Initialize()
{
	return S_OK;
}

void CConverter::Tick()
{
	int i = 0;
}

CConverter* CConverter::Create()
{
	CConverter* pInstance = new CConverter;

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Create Converter");
	}
	return pInstance;
}
