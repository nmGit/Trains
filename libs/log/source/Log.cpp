#include "Log/Log.h"

#include <cstdarg>
#include <format>
#include <varargs.h>

static const std::chrono::high_resolution_clock::time_point cs_start_time =
    std::chrono::high_resolution_clock::now();

LogContext::LogContext(const char *context) : m_context(context) {}
const char *LogContext::GetContext() const {
    return m_context;
}

void Log::Info(const LogContext &context, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("\x1b[2m");
    PrintTime();
    printf("[%s] [Info] \x1b[0m", context.GetContext());
    vprintf(fmt, args);
    printf("\r\n");
    va_end(args);
}

void Log::Warn(const LogContext &context, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("\x1b[2m");
    PrintTime();
    printf("\x1b[33m[Warning] [%s]\x1b[0m ", context.GetContext());
    vprintf(fmt, args);
    printf("\r\n");
    va_end(args);
}

void Log::Error(const LogContext &context, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("\x1b[2m");
    PrintTime();
    printf("\x1b[31m[Error] [%s]\x1b[0m ", context.GetContext());
    vprintf(fmt, args);
    printf("\r\n");
    va_end(args);
}

void Log::Debug(const LogContext &context, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("\x1b[2m");
    PrintTime();
    printf("\x1b[34m[Debug] [%s]\x1b[0m ", context.GetContext());
    vprintf(fmt, args);
    printf("\r\n");
    va_end(args);
}

void Log::PrintTime() {
    auto now       = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - cs_start_time);

    auto hours = std::chrono::duration_cast<std::chrono::hours>(time_span);
    time_span -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time_span);
    time_span -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_span);
    time_span -= seconds;
    printf("[%03d:%02d:%02d:%03d] ", hours.count(), minutes.count(),
           seconds.count(), time_span.count());
}
