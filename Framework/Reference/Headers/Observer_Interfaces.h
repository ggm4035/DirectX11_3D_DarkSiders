#pragma once

#include "Engine_Defines.h"

namespace Engine
{
	/* Observer Animation */
	class IObserver_Animation
	{
	public:
		virtual ~IObserver_Animation() = default;
		virtual void Update_Observer(NOTIFYDESC* pNotifyDesc) = 0;
	};

	class IPublisher_Animation
	{
	public:
		virtual ~IPublisher_Animation() = default;
		virtual void Init_Observers(const vector<TIMERANGE>& vecRanges) = 0;
		virtual void Clear_Observers() = 0;
	};
}