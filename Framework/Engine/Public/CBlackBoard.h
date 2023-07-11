#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CBlackBoard final : public CBase
{
private:
	explicit CBlackBoard() = default;
	explicit CBlackBoard(const CBlackBoard& rhs) 
		: CBase(rhs)
		, m_umapType(rhs.m_umapType) { }
	virtual ~CBlackBoard() = default;

public:
	HRESULT Set_Type(IN const wstring & Typename, IN any Type)
	{
		auto iter = m_umapType.find(Typename);

		if (iter == m_umapType.end())
			m_umapType.emplace(Typename, Type);

		else
			iter->second = Type;

		return S_OK;
	}

	template<typename T>
	HRESULT Get_Type(IN const wstring & Typename, OUT T& Out)
	{
		auto iter = m_umapType.find(Typename);
		if (iter == m_umapType.end())
			return E_FAIL;

		Out = any_cast<T>(iter->second);

		return S_OK;
	}

private:
	unordered_map<wstring, any>		m_umapType;

public:
	static CBlackBoard* Create() {
		return new CBlackBoard();
	}
	virtual void Free() override {}
};

END