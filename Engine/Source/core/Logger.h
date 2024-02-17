#pragma once

#pragma warning(disable : 4996)

#include <spdlog/spdlog.h>
#include <imgui/ImGuiDebugger.h>

#include <memory>

namespace Sapphire
{
	class Logger
	{
	public:
		static void Init(Debugger* Debugger);

		static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

#define SAPPHIRE_INFO(...) Sapphire::Logger::GetLogger()->info(__VA_ARGS__)
#define SAPPHIRE_WARN(...) Sapphire::Logger::GetLogger()->warn(__VA_ARGS__)
#define SAPPHIRE_ERROR(...) Sapphire::Logger::GetLogger()->error(__VA_ARGS__)
#define SAPPHIRE_CRITICAL(...) Sapphire::Logger::GetLogger()->critical(__VA_ARGS__)