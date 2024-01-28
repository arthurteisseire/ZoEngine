#ifndef ENGINE_LOGGER_H
#define ENGINE_LOGGER_H

#include <cstdio>

class Logger
{
public:
    template<typename... Args>
    static void error(Args ... args)
    {
        std::printf(args ...);
        std::fflush(stdout);
    }

    template<typename... Args>
    static void info(Args ... args)
    {
        std::printf(args ...);
        std::fflush(stdout);
    }

    static void setLogLevel(unsigned int logLevel)
    {
        m_logLevel = logLevel;
    }

private:
    static unsigned int m_logLevel;
};


#endif //ENGINE_LOGGER_H
