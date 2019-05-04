#pragma once

#include <fstream>
#include <stdio.h>
#include <string>
#include <chrono>
#include <GLEngineErrors.h>

class Logger {
public:
    static Logger* getInstance();
    void log(std::string message, bool important = false) {
        if(important) {
            message = "\n### " + message + " ###\n";
        }
        message = message + "\n";
        m_logFile << message;
        std::printf("%s", message.c_str());
        m_logFile.flush();
    }
private:
    Logger() {
        m_logFile.open("GameLog.txt", std::ios::app);
        if(m_logFile.fail()) {
            GLEngine::fatalError("Logger failed to open file: GameLog.txt");
        }
        auto a = std::chrono::system_clock::now();
        auto b = std::chrono::system_clock::to_time_t(a);

        std::string date = std::ctime(&b);
        date.pop_back();

        m_logFile << "\n\n\n### LOG: " << date << " ###\n\n";
        m_logFile.flush();
        m_init = true;
    }
    std::ofstream m_logFile;

    bool m_init = false;

    static Logger* m_instance;

};
