#include "Logger.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>

static std::ofstream g_logFile;

void Logger::init(const std::string& prefix) {
    mkdir("logs", 0777);
    std::string filename = "logs/" + prefix + "_" + getTimestamp("%Y-%m-%d_%H-%M-%S") + ".log";
    g_logFile.open(filename.c_str(), std::ios::out | std::ios::app);
    
    if (g_logFile.is_open()) {
        Logger::log(INFO, "=== Log Session Started: " + filename + " ===");
    } else {
        std::cerr << RED << "[ERROR] Could not open log file: " << filename << RESET << std::endl;
    }
}

void Logger::close() {
    if (g_logFile.is_open()) {
        Logger::log(INFO, "=== Log Session Ended ===");
        g_logFile.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::string timestemp = getTimestamp("%Y-%m-%d %H:%M:%S");
    std::string levelStr = levelToString(level);
    std::cout << levelToColor(level) 
              << "[" << timestemp << "] "
              << "[" << levelStr << "] "
              << message 
              << RESET << std::endl;
    if (g_logFile.is_open()) {
        g_logFile << "[" << timestemp << "] "
                  << "[" << levelStr << "] "
                  << message << std::endl;
    }
}

void Logger::info(const std::string& message) { log(INFO, message); }
void Logger::debug(const std::string& message) { log(DEBUG, message); }
void Logger::warning(const std::string& message) { log(WARNING, message); }
void Logger::error(const std::string& message) { log(ERROR, message); }

std::string Logger::getTimestamp(std::string format) {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, 80, format.c_str(), ltm);
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