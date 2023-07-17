
#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CAttackAction final : public CBehavior
{
public:
	enum TYPE { TYPE_LIGHT, TYPE_HEAVY, TYPE_END };

private:
	explicit CAttackAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAttackAction(const CAttackAction& rhs);
	virtual ~CAttackAction() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

	void Push_Move_Message(_ubyte ubyKey) {
		m_Qmessage.push(ubyKey);
	}

private:
	TYPE m_eCurrentAttack = { TYPE_END };
	queue<_ubyte> m_Qmessage;

	_int m_iPreCombo = { 0 };
	_int m_iCombo = { 0 };

private:
	CModel* m_pModel = { nullptr };

private:
	void LightAttackCombo();
	void HeavyAttackCombo();

public:
	static CAttackAction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAttackAction* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END