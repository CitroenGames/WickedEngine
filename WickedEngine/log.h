#pragma once

#ifndef WI_RETAIL
#include "spdlog/spdlog.h"
#include <memory>

class CLog
{
public:
    static void Init();
    static void Shutdown();

    static inline std::shared_ptr<spdlog::logger>& GetLogger()
    {
		if (S_EngineLogger == nullptr)
			Init();

		return S_EngineLogger;
    }

    static inline std::shared_ptr<spdlog::logger>& GetGameLogger()
    {
		if (S_ClientLogger == nullptr)
			Init();

        return S_ClientLogger;
    }

private:
    static std::shared_ptr<spdlog::logger> S_EngineLogger;
    static std::shared_ptr<spdlog::logger> S_ClientLogger;
};

#define INIT_LOG(...) CLog::Init();
#define SHUTDOWN_LOG(...) CLog::Shutdown();
#define LOG_FATAL( ... ) CLog::GetLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...) CLog::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) CLog::GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) CLog::GetLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) CLog::GetLogger()->trace(__VA_ARGS__)

#define LOG_GAME_ERROR(...) CLog::GetGameLogger()->error(__VA_ARGS__)
#define LOG_GAME_WARN(...) CLog::GetGameLogger()->warn(__VA_ARGS__)
#define LOG_GAME_INFO(...) CLog::GetGameLogger()->info(__VA_ARGS__)
#define LOG_GAME_TRACE(...) CLog::GetGameLogger()->trace(__VA_ARGS__)

#else

#define INIT_LOG(...)
#define SHUTDOWN_LOG(...)
#define LOG_FATAL(...)
#define LOG_ERROR(...)
#define LOG_WARN(...)
#define LOG_INFO(...)
#define LOG_TRACE(...) 

#define LOG_GAME_ERROR(...) 
#define LOG_GAME_WARN(...) 
#define LOG_GAME_INFO(...) 
#define LOG_GAME_TRACE(...) 

#endif
