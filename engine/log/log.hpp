

#pragma once

#include <memory>

#include "spdlog/spdlog.h"
#include <spdlog/fmt/ostr.h>

namespace VGED
{
    class Log {
    public:
        static bool init();

        inline static std::shared_ptr<spdlog::logger>& getLogger() {
            return m_EngineLogger;
        }

        inline static std::shared_ptr<spdlog::logger>& getAppLogger() {
            return m_AppLogger;
        }

    private: 

        static std::shared_ptr<spdlog::logger> m_EngineLogger;
        static std::shared_ptr<spdlog::logger> m_AppLogger;

    };
}
