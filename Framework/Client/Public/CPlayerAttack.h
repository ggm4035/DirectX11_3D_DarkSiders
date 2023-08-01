
#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)

class CPlayerAttack final : public CBehavior
{
public:
	enum TYPE { TYPE_LIGHT, TYPE_HEAVY, TYPE_END };

private:
	explicit CPlayerAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerAttack(const CPlayerAttack& rhs);
	virtual ~CPlayerAttack() = default;

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
	_int m_iCombo = { -1 };

private:
	CModel* m_pModel = { nullptr };
	CTransform* m_pTransform = { nullptr };

public:
	static CPlayerAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPlayerAttack* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END