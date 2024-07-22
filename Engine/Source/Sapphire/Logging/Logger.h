#pragma once

#include "Sapphire/Core/Base.h"

#include <spdlog/spdlog.h>

namespace Sapphire
{
    class Logger
    {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_EngineLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

#define SAPPHIRE_ENGINE_INFO(...) Sapphire::Logger::GetEngineLogger()->info(__VA_ARGS__)
#define SAPPHIRE_ENGINE_WARN(...) Sapphire::Logger::GetEngineLogger()->warn(__VA_ARGS__)
#define SAPPHIRE_ENGINE_ERROR(...) Sapphire::Logger::GetEngineLogger()->error(__VA_ARGS__)
#define SAPPHIRE_ENGINE_CRITICAL(...) Sapphire::Logger::GetEngineLogger()->critical(__VA_ARGS__)

#define SAPPHIRE_CLIENT_INFO(...) Sapphire::Logger::GetClientLogger()->info(__VA_ARGS__)
#define SAPPHIRE_CLIENT_WARN(...) Sapphire::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define SAPPHIRE_CLIENT_ERROR(...) Sapphire::Logger::GetClientLogger()->error(__VA_ARGS__)
#define SAPPHIRE_CLIENT_CRITICAL(...) Sapphire::Logger::GetClientLogger()->critical(__VA_ARGS__)