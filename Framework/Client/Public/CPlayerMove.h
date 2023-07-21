#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CPlayerMove final : public CBehavior
{
private:
	explicit CPlayerMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerMove(const CPlayerMove& rhs);
	virtual ~CPlayerMove() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

	void Push_Move_Message(_ubyte ubyKey) {
		m_Qmessage.push(ubyKey);
	}

private:
	queue<_ubyte> m_Qmessage;

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

public:
	static CPlayerMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayerMove* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END