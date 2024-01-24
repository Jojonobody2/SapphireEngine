#pragma once

#include "Window.h"

namespace Sapphire
{
	struct ApplicationCreateInfo
	{
		const char* Name;
	};

	class Application
	{
	public:
		Application(const ApplicationCreateInfo& ApplicationCI);
		virtual ~Application();

		void Run();
		void Close();

		static Application& Get() { return *s_Application; }

	private:
		bool m_IsRunning{};
		
		std::shared_ptr<Window> m_Window{};

	private:
		static Application* s_Application;
	};
}