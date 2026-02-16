#include "Logger.hpp"
#include <sstream>

void Logger::log(LogLevel level, const std::string& message) {
    std::cout << levelToColor(level) 
              << "[" << getTimestamp() << "] "
              << "[" << levelToString(level) << "] "
              << message 
              << RESET << std::endl;
}

void Logger::info(const std::string& message) { log(INFO, message); }
void Logger::debug(const std::string& message) { log(DEBUG, message); }
void Logger::warning(const std::string& message) { log(WARNING, message); }
void Logger::error(const std::string& message) { log(ERROR, message); }

std::string Logger::getTimestamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", ltm);
    return std::string(buffer);
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case INFO: return "INFO";
        case DEBUG: return "DEBUG";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        default: return "LOG";
    }
}

std::string Logger::levelToColor(LogLevel level) {
    switch (level) {
        case INFO: return GREEN;
        case DEBUG: return CYAN;
        case WARNING: return YELLOW;
        case ERROR: return RED;
        default: return RESET;
    }
}