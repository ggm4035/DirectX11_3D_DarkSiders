#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CDecoration final : public CBase
{
private:
	explicit CDecoration() = default;
	virtual ~CDecoration() = default;

public:
	HRESULT Initialize(function<_bool(class CBlackBoard*)> Func) {
		m_Function = Func;
		return S_OK;
	}
	HRESULT Bind_Decoration(function<_bool(class CBlackBoard*)> Func);
	_bool Is_Execute(class CBlackBoard* pBlackBoard);

private:
	function<_bool(class CBlackBoard*)> m_Function = { nullptr };

public:
	static CDecoration* Create(function<_bool(class CBlackBoard*)> Func);
	virtual void Free() override;
};

END