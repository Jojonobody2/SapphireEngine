#include "Logger.h"

#include "Sapphire/Core/Application.h"

#include <spdlog/sinks/basic_file_sink.h>

namespace Sapphire
{
    std::shared_ptr<spdlog::logger> Logger::s_EngineLogger;
    std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

    void Logger::Init()
    {
        std::vector<spdlog::sink_ptr> LogSinks =
                {
                        CreateSharedPtr<spdlog::sinks::basic_file_sink_mt>  ("Sapphire.log", true)
                };

        LogSinks[0]->set_pattern("[%T] [%l] %n: %v");

        s_EngineLogger = CreateSharedPtr<spdlog::logger>("ENGINE", begin(LogSinks), end(LogSinks));
        spdlog::register_logger(s_EngineLogger);
        s_EngineLogger->set_level(spdlog::level::info);
        s_EngineLogger->flush_on(spdlog::level::info);

        s_ClientLogger = CreateSharedPtr<spdlog::logger>(Application::Get().GetName(), begin(LogSinks), end(LogSinks));
        spdlog::register_logger(s_ClientLogger);
        s_ClientLogger->set_level(spdlog::level::info);
        s_ClientLogger->flush_on(spdlog::level::info);
    }
}