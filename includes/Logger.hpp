#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include "Colors.hpp"

enum LogLevel {
	INFO,
	DEBUG,
	WARNING,
	ERROR
};

class Logger {
    public:
        static void init(const std::string& prefix);
        static void close();

        static void log(LogLevel level, const std::string& message);
        static void info(const std::string& message);
        static void debug(const std::string& message);
        static void warning(const std::string& message);
        static void error(const std::string& message);

    private:
        Logger();
        static std::string getTimestamp(std::string format);
        static std::string levelToString(LogLevel level);
        static std::string levelToColor(LogLevel level);
};

#define LOG_INFO(x) { std::stringstream ss; ss << x; Logger::log(INFO, ss.str()); }

#define LOG_DEBUG(x) { std::stringstream ss; ss << x; Logger::log(DEBUG, ss.str()); }

#define LOG_WARNING(x) { std::stringstream ss; ss << x; Logger::log(WARNING, ss.str()); }

#define LOG_ERROR(x) { std::stringstream ss; ss << x; Logger::log(ERROR, ss.str()); }