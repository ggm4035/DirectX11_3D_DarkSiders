#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CJumpAction final : public CBehavior
{
private:
	explicit CJumpAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CJumpAction(const CJumpAction& rhs);
	virtual ~CJumpAction() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Push_Jump_Message(_ubyte ubyKey) {
		m_Qmessage.push(ubyKey);
	}

private:
	queue<_ubyte> m_Qmessage;

	_bool m_isJump = { false };
	_bool m_isDoubleJump = { false };

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

public:
	static CJumpAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CJumpAction* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END