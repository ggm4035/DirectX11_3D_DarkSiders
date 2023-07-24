#pragma once

#include "Engine_Defines.h"

namespace Engine
{
	/* Observer Animation */
	class ENGINE_DLL IObserver_Animation abstract
	{
	public:
		typedef struct tagParam
		{
		}BASEPARAM;
		enum OBSERVERTYPE { ANIMATION, COLLIDER, TYPE_END };

	public:
		virtual ~IObserver_Animation() = default;
		virtual void Update_Observer(BASEPARAM* pParamDesc) = 0;
	};

	class ENGINE_DLL IPublisher_Animation abstract
	{
	public:
		virtual ~IPublisher_Animation() = default;
		virtual void Notify(const float& fPoint) = 0;
	};
}