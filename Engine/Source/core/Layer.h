#pragma once

#include "event/SapphireEvent.h"

#include <string>

namespace Sapphire
{
	class Layer
	{
	public:
		virtual void OnAttach() {};
		virtual void OnUpdate() {};
		virtual void OnEvent(Event& Event) {};
		virtual void OnDetach() {};
	};
}