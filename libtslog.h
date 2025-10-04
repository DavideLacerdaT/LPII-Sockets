#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

using namespace std;

class ThreadSafeLogger {
private:
    mutex log_mutex;

    string pegar_tempo_atual() {
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        auto ms = chrono::duration_cast<chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        stringstream ss;
        ss << put_time(localtime(&time_t), "%H:%M:%S");
        ss << "." << setfill('0') << setw(3) << ms.count();
        return ss.str();
    }

public:
    static ThreadSafeLogger& getInstance() {
        static ThreadSafeLogger instance;
        return instance;
    }
    
    void log_info(const string& message) {
        lock_guard<mutex> lock(log_mutex);
        
        cout << "[" << pegar_tempo_atual() << "] " 
                 << "[ INFO ] "
                 << "[Thread " << this_thread::get_id() << "] "
                 << message << endl; 
    }

    void log_warn(const string& message) {
        lock_guard<mutex> lock(log_mutex);
        
        cout << "[" << pegar_tempo_atual() << "] " 
                 << "[ WARN ] "
                 << "[Thread " << this_thread::get_id() << "] "
                 << message << endl; 
    }

    void log_error(const string& message) {
        lock_guard<mutex> lock(log_mutex);
        
        cout << "[" << pegar_tempo_atual() << "] " 
                 << "[ ERROR ] "
                 << "[Thread " << this_thread::get_id() << "] "
                 << message << endl; 
    }
};

// Macros
#define LOG_INFO(msg) ThreadSafeLogger::getInstance().log_info(msg)
#define LOG_WARN(msg) ThreadSafeLogger::getInstance().log_warn(msg)
#define LOG_ERROR(msg) ThreadSafeLogger::getInstance().log_error(msg)