#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTransform;
class CSequence;
END

BEGIN(Client)

class CWheelWind final : public CBehavior
{
private:
	explicit CWheelWind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWheelWind(const CWheelWind& rhs);
	virtual ~CWheelWind() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	HRESULT AssembleBehaviors();
	void Reset();
	void Play();

private:
	_float m_fCoolTime = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_bool m_isPlay = { false };

	class CAction* pAction_Start = { nullptr };
	class CAction* pAction_Ing = { nullptr };
	class CAction* pAction_End = { nullptr };

public:
	static CWheelWind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWheelWind* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END