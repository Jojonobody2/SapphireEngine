#include "Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <imgui/ImGuiDebuggerLogSink.h>

namespace Sapphire
{
	std::shared_ptr<spdlog::logger> Logger::s_Logger;

	void Logger::Init(Debugger* Debugger)
	{
		std::vector<spdlog::sink_ptr> LogSinks{};
		LogSinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Sapphire.log", true));
		LogSinks.push_back(std::make_shared<spdlog::sinks::debugger_sink_mt>(Debugger));

		LogSinks[0]->set_pattern("[%T] [%l] %n: %v");
		LogSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_Logger = std::make_shared<spdlog::logger>("SAPPHIRE ENGINE", begin(LogSinks), end(LogSinks));
		spdlog::register_logger(s_Logger);
		s_Logger->set_level(spdlog::level::info);
		s_Logger->flush_on(spdlog::level::info);
	}
}