#include "..\Public\CBase.h"

CBase::CBase()
	:m_dwRefCnt{ 1 }
{
}

_ulong CBase::AddRef()
{
	return ++m_dwRefCnt;	
}

_ulong CBase::Release()
{
	if (0 == --m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	
	return m_dwRefCnt;
}

