

#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace VGED {
class Log {
public:
    static bool init();

    inline static std::shared_ptr<spdlog::logger> &getLogger() {
        return m_EngineLogger;
    }

    inline static std::shared_ptr<spdlog::logger> &getAppLogger() {
        return m_AppLogger;
    }

private:
    static std::shared_ptr<spdlog::logger> m_EngineLogger;
    static std::shared_ptr<spdlog::logger> m_AppLogger;
};
}

#ifndef DISTRIBUTION // disables engine logging (currently its never disabled)
#define VGED_ENGINE_TRACE(...) ::VGED::Log::getLogger()->trace(__VA_ARGS__)
#define VGED_ENGINE_INFO(...) ::VGED::Log::getLogger()->info(__VA_ARGS__)
#define VGED_ENGINE_WARN(...) ::VGED::Log::getLogger()->warn(__VA_ARGS__)
#define VGED_ENGINE_ERROR(...) ::VGED::Log::getLogger()->error(__VA_ARGS__)
#define VGED_ENGINE_FATAL(...) ::VGED::Log::getLogger()->fatal(__VA_ARGS__)
#elif
#define VGED_ENGINE_TRACE(...)
#define VGED_ENGINE_INFO(...)
#define VGED_ENGINE_WARN(...)
#define VGED_ENGINE_ERROR(...)
#define VGED_ENGINE_FATAL(...)
#endif

#define VGED_TRACE(...) ::VGED::Engine::Core::Log::getAppLogger()->trace(__VA_ARGS__)
#define VGED_INFO(...) ::VGED::Engine::Core::Log::getAppLogger()->info(__VA_ARGS__)
#define VGED_WARN(...) ::VGED::Engine::Core::Log::getAppLogger()->warn(__VA_ARGS__)
#define VGED_ERROR(...) ::VGED::Engine::Core::Log::getAppLogger()->error(__VA_ARGS__)
#define VGED_FATAL(...) ::VGED::Engine::Core::Log::getAppLogger()->fatal(__VA_ARGS__)
