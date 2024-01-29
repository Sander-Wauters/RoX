#pragma once

#include <string>
#include <fstream>
#include <chrono>
#include <time.h>
#include <stdio.h>

class Logger {
    public: 
        static void Debug(std::string message)    { Log( " | DEBUG   | ", message); }
        static void Info(std::string message)     { Log( " | INFO    | ", message); }
        static void Warning(std::string message)  { Log( " | WARNING | ", message); }
        static void Error(std::string message)    { Log( " | ERROR   | ", message); }

        static void Debug(std::wstring message)   { Log(L" | DEBUG   | ", message); }
        static void Info(std::wstring message)    { Log(L" | INFO    | ", message); }
        static void Warning(std::wstring message) { Log(L" | WARNING | ", message); }
        static void Error(std::wstring message)   { Log(L" | ERROR   | ", message); }

    private:
        static void Log(std::string severity, std::string message) {
            std::ofstream os("engine.log", std::ios::app);

            const std::time_t result = std::time(NULL);
            char str[26];
            ctime_s(str, sizeof(str), &result);

            std::string timeStamp = str; 
            timeStamp.pop_back(); // Remove newline.
            
            os << timeStamp << severity << message << std::endl;
        }

        static void Log(std::wstring severity, std::wstring message) {
            std::wofstream os("engine.log", std::ios::app);

            const std::time_t result = std::time(NULL);
            char str[26];
            ctime_s(str, sizeof(str), &result);

            std::wstring timeStamp(&str[0], &str[strlen(str)]);
            timeStamp.pop_back(); // Remove newline.
            
            os << timeStamp << severity << message << std::endl;
        }
};
