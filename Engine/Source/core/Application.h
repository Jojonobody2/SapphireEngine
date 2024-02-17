#pragma once

#include "Window.h"
#include "Layer.h"

#include "imgui/ImGuiDebugger.h"

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
		std::shared_ptr<Debugger> GetDebugger() { return m_Debugger; }
		std::vector<Layer*> m_Layers{};

	private:
		bool m_IsRunning{};
		const char* m_Name{};

		std::shared_ptr<Window> m_Window{};
		std::shared_ptr<Debugger> m_Debugger{};

		static void EventCallback(Event& Event);

	private:
		static Application* s_Application;
	};
}