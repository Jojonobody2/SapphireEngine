#pragma once

#include <string>

namespace Sapphire
{
	class Layer
	{
	public:
		virtual void OnAttach() {};
		virtual void OnUpdate() {};
		virtual void OnDetach() {};
	};
}