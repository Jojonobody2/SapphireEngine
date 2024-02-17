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
		std::shared_ptr<Layer> BaseLayer;
	};

	class Application
	{
	public:
		Application(const ApplicationCreateInfo& ApplicationCI);
		virtual ~Application();

		void Run();
		void Close();

		void PushLayer(std::shared_ptr<Layer> Layer);

		const char* GetName() { return m_Name; }

		static Application& Get() { return *s_Application; }

	public:
		std::shared_ptr<Window> GetWindow() { return m_Window; }

	private:
		bool m_IsRunning{};
		const char* m_Name{};

		std::shared_ptr<Window> m_Window{};
		std::shared_ptr<Debugger> m_Debugger{};

		std::vector<std::shared_ptr<Layer>> m_Layers;

		static void EventCallback(Event& Event);

	private:
		static Application* s_Application;
	};
}