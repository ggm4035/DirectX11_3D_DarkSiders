#pragma once

#include "Client_Defines.h"
#include "CBehavior.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CMoveAction final : public CBehavior
{
private:
	explicit CMoveAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMoveAction(const CMoveAction& rhs);
	virtual ~CMoveAction() = default;

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
	static CMoveAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CMoveAction* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END