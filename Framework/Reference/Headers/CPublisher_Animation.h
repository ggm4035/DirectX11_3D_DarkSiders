#pragma once

#include "Observer_Interfaces.h"
#include "CComponent.h"

BEGIN(Engine)

/* Animation Publisher는 해당 구간을 정해주면 그 구간에 진입했을 경우에 가지고있는 
   옵저버들에게 알림을 제공한다. */
class CPublisher_Animation final : public CComponent, IPublisher_Animation
{
	typedef struct tagObserverDesc
	{
		TIMERANGE TimeRange;
		IObserver_Animation* arrObserver[OBSERVERTYPE::TYPE_END];
	}OBSERVER;

private:
	explicit CPublisher_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPublisher_Animation(const CPublisher_Animation& rhs);
	virtual ~CPublisher_Animation() = default;

public:
	void Bind_Current_Animation(class IObserver_Animation* pCurrentAnimation) {
		m_pCurrentAnimation = pCurrentAnimation;
	}

public:
	virtual HRESULT Initialize_Prototype() { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	void Tick(const _float& fTimeAcc);

public: /* For. Observer */
	virtual void Init_Observers(const vector<TIMERANGE>& vecRanges) override;
	virtual void Clear_Observers() override;

private:
	class IObserver_Animation* m_pCurrentAnimation = { nullptr };
	vector<OBSERVER> m_Observers;

public:
	static CPublisher_Animation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPublisher_Animation* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END