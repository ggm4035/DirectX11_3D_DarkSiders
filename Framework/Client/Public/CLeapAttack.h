#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTransform;
END

BEGIN(Client)

class CLeapAttack final : public CBehavior
{
private:
	explicit CLeapAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLeapAttack(const CLeapAttack& rhs);
	virtual ~CLeapAttack() = default;

public:
	const _float* Get_CoolTimePtr() const {
		return &m_fCoolTime;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Reset();
	void Play();

private:
	_float m_fCoolTime = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_bool m_isPlay = { false };
	_bool m_isFirst = { true };
	_bool m_isRenderEffect = { false };

private:
	CModel* m_pModel = { nullptr };
	CTransform* m_pTransform = { nullptr };
	class CStone_Effect* m_pEffect = { nullptr };

public:
	static CLeapAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLeapAttack* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END