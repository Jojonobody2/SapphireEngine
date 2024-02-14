#pragma once

#include "Window.h"
#include "Layer.h"

#include <vector>

namespace Sapphire
{
	struct ApplicationCreateInfo
	{
		const char* Name;
		Layer* BaseLayer;
	};

	class Application
	{
	public:
		Application(const ApplicationCreateInfo& ApplicationCI);
		virtual ~Application();

		void Run();
		void Close();

		void PushLayer(Layer* Layer);

		const char* GetName() { return m_Name; }

		static Application& Get() { return *s_Application; }

	public:
		std::shared_ptr<Window> GetWindow() { return m_Window; }

	private:
		bool m_IsRunning{};
		const char* m_Name{};

		std::vector<Layer*> m_Layers{};

		std::shared_ptr<Window> m_Window{};

	private:
		static Application* s_Application;
	};
}