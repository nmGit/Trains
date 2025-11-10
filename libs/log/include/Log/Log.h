#pragma once
#include <chrono>
#include <stdio.h>
class LogContext {
  public:
    LogContext(const char *context);
    const char *GetContext() const;

  private:
    const char *m_context = nullptr;
};

class Log {
  public:
    static void Info(const LogContext &context, const char *fmt, ...);
    static void Warn(const LogContext &context, const char *fmt, ...);
    static void Error(const LogContext &context, const char *fmt, ...);
    static void Debug(const LogContext &context, const char *fmt, ...);

  private:
    static void PrintTime();
   
};
