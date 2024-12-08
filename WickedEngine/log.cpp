#include "log.h"

#ifndef WI_RETAIL
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#define LOG_FILENAME "Log.log"

std::shared_ptr<spdlog::logger> CLog::S_EngineLogger;
std::shared_ptr<spdlog::logger> CLog::S_ClientLogger;

void CLog::Init()
{
	if(S_EngineLogger)
		return;

    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    // Initialize the loggers
    S_EngineLogger = spdlog::stdout_color_mt("ENGINE");
    S_ClientLogger = spdlog::stdout_color_mt("CLIENT");

    // Add file sink with immediate flushing
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(LOG_FILENAME, true);
    file_sink->set_level(spdlog::level::trace); // Set the log level for the sink

    // Attach the sink to the loggers
    S_EngineLogger->sinks().push_back(file_sink);
    S_ClientLogger->sinks().push_back(file_sink);

    // Set log levels for the loggers
    S_EngineLogger->set_level(spdlog::level::trace);
    S_ClientLogger->set_level(spdlog::level::trace);

    // Set immediate flushing for the loggers
    S_EngineLogger->flush_on(spdlog::level::trace);
    S_ClientLogger->flush_on(spdlog::level::trace);
}

void CLog::Shutdown()
{
	LOG_TRACE( "Destroying Log" );
	// wait for the async logger to finish (if there is any)
	spdlog::drop_all();

	// shutdown all loggers
	spdlog::shutdown();
	S_EngineLogger.reset();
	S_ClientLogger.reset();
}
#endif
