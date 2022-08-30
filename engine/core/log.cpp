

#include <vector>

#include "log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace VGED {
    namespace Engine {
        inline namespace Core {
            std::shared_ptr<spdlog::logger> Log::m_EngineLogger;
            std::shared_ptr<spdlog::logger> Log::m_AppLogger;

            bool Log::init() {
                bool ok = false;
                std::vector<spdlog::sink_ptr> logSink;
                logSink.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

                spdlog::set_pattern("%^[%T] %n: %v%$");
                m_EngineLogger = std::make_shared<spdlog::logger>("Engine", begin(logSink), end(logSink));
                if (m_EngineLogger) {
                    ok = true;
                    spdlog::register_logger(m_EngineLogger);
                    m_EngineLogger->set_level(spdlog::level::trace);
                    m_EngineLogger->flush_on(spdlog::level::trace);
                }

                m_AppLogger = std::make_shared<spdlog::logger>("Application", begin(logSink), end(logSink));
                if (m_AppLogger) {
                    ok = true;
                    spdlog::register_logger(m_AppLogger);
                    m_EngineLogger->set_level(spdlog::level::trace);
                    m_EngineLogger->flush_on(spdlog::level::trace);
                }

                return ok;
            }
        }
    }
}
