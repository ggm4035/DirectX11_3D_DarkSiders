#pragma once

#include "Engine_Defines.h"

/* IUnknown 레퍼런스 카운트를 관리한다. */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase();
	virtual ~CBase() = default;

public:
	_ulong AddRef(); /* 레퍼런스 카운트를 증가시키니다 .*/
	_ulong Release(); /* refcnt != 0 레퍼런슼 아누트를 감소시킨다. or RefCnt == 0 삭제하낟. */

protected: /* 레퍼런스 카운트 (참조갯수) */
	_ulong			m_dwRefCnt = { 0 };

public:
	virtual void Free() = 0;
};

END