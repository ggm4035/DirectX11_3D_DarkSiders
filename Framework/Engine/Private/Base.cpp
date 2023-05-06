#include "..\Public\Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{

	/* 증가시키고난 이후의 값을 리턴한다. */
	return ++m_dwRefCnt;	
}

_ulong CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		/* 삭제하낟. */
		delete this;

		return 0;
	}
	
	else /* 감소시키기 전의 값을 리턴한다. */	
		return m_dwRefCnt--;	
}

