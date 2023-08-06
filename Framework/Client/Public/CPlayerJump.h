#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPlayerJump final : public CBehavior
{
private:
	explicit CPlayerJump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerJump(const CPlayerJump& rhs);
	virtual ~CPlayerJump() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Push_Jump_Message(_ubyte ubyKey) {
		m_Qmessage.push(ubyKey);
	}
	void Reset();

private:
	queue<_ubyte> m_Qmessage;

	_bool m_isJump = { false };
	_bool m_isDoubleJump = { false };
	_bool m_isFirst = { true };

public:
	static CPlayerJump* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayerJump* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END