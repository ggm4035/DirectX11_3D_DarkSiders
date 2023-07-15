#pragma once

#include "Engine_Defines.h"

namespace Engine
{
	/* Observer Animation */
	class ENGINE_DLL IObserver_Animation abstract
	{
	public:
		virtual ~IObserver_Animation() = default;
		virtual void Update_Observer() = 0;
	};

	class ENGINE_DLL IPublisher_Animation abstract
	{
	public:
		virtual ~IPublisher_Animation() = default;
		virtual void Attach(const float& fPoint, class IObserver_Animation * pObserver) = 0;
		virtual void Detach(const float& fPoint, class IObserver_Animation* pObserver) = 0;
		virtual void Notify(const float& fPoint) = 0;
	};
}