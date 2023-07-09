#include "CPublisher_Animation.h"

#include "CAnimation.h"
#include "CCollider_Manager.h"

CPublisher_Animation::CPublisher_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CPublisher_Animation::CPublisher_Animation(const CPublisher_Animation& rhs)
	:CComponent(rhs)
{
}

HRESULT CPublisher_Animation::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	return S_OK;
}

void CPublisher_Animation::Tick(const _float& fTimeAcc)
{
	/* 구간은 많아야 5개 옵저버 개수는 5개가 안되고  */
	for (auto& Range : m_Observers)
	{
		if (fTimeAcc > Range.TimeRange.fStartPoint && fTimeAcc <= Range.TimeRange.fEndPoint)
		{
			for (_uint i = 0; i < OBSERVERTYPE::TYPE_END; ++i)
			{
				switch (i)
				{
				case OBSERVERTYPE::TYPE_ANIMATION:
					if (nullptr != Range.arrObserver[i])
						Range.arrObserver[i]->Update_Observer(nullptr);
					break;

				case OBSERVERTYPE::TYPE_COLLIDER:
					/* Collider Manager *//* Collider 관련 구조체 뭐 위치나 기타등등 */
					if (nullptr != Range.arrObserver[i])
					{
						CCollider_Manager::NOTIFYCOLLIDERDESC Desc;
						Desc.wstrTag = L"Attack";
						Range.arrObserver[i]->Update_Observer(&Desc);
					}
					break;

				case OBSERVERTYPE::TYPE_SOUND:
					/* Sound Manager *//* Sound 관련 구조체 소리의 종류? */
					break;
				}
			}
		}
	}
}

void CPublisher_Animation::Init_Observers(const vector<TIMERANGE>& vecRanges)
{
	for (auto& Range : vecRanges)
	{
		OBSERVER Observer;
		Observer.TimeRange = Range;

		for (_uint i = 0; i < OBSERVERTYPE::TYPE_END; ++i)
		{
			Observer.arrObserver[i] = { nullptr };

			switch (Range.arrTypes[i])
			{
			case OBSERVERTYPE::TYPE_ANIMATION:
				Observer.arrObserver[i] = m_pCurrentAnimation;
				break;

			case OBSERVERTYPE::TYPE_COLLIDER:
				CCollider_Manager* pCollider_Manager = CCollider_Manager::GetInstance();
				Observer.arrObserver[i] = (IObserver_Animation*)pCollider_Manager;
				break;

			//case OBSERVERTYPE::TYPE_SOUND:
			//	/* Sound Manager */
			//	break;
			}
		}
		m_Observers.push_back(Observer);
	}
}

void CPublisher_Animation::Clear_Observers()
{
	m_Observers.clear();
	m_Observers.shrink_to_fit();
}

CPublisher_Animation* CPublisher_Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPublisher_Animation* pInstance = new CPublisher_Animation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPublisher_Animation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPublisher_Animation* CPublisher_Animation::Clone(const _uint & iLevelIndex, CComponent * pOwner, void* pArg)
{
	CPublisher_Animation* pInstance = new CPublisher_Animation(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CPublisher_Animation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPublisher_Animation::Free()
{
	m_Observers.clear();
	m_Observers.shrink_to_fit();

	CComponent::Free();
}
