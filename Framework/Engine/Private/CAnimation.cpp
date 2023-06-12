#include "CAnimation.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize()
{
	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix()
{
}

CAnimation* CAnimation::Create()
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
}
