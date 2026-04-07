/*
 * HighPrecisionLogger.cpp
 *
 *  Created on: 20.10.2025
 *      Author: ergenc
 */

#include "HighPrecisionLogger.h"

std::ofstream HighPrecisionLogger::file;
std::mutex HighPrecisionLogger::file_mutex;
bool HighPrecisionLogger::initialized = false;

void HighPrecisionLogger::initialize() {
    if(!initialized) {
        file.open("results/time_log.txt", std::ios::app);
        initialized = true;
    }
}

void HighPrecisionLogger::logEvent(const std::string& modulePath,
                                   const std::string& description,
                                   const std::string& packet) {
    std::lock_guard<std::mutex> lock(file_mutex);
    initialize();

    LARGE_INTEGER counter, freq;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);

    double t_ns = double(counter.QuadPart) * 1e9 / double(freq.QuadPart);

    file << std::fixed << std::setprecision(0) << modulePath  << "#" << description;
    if(!packet.empty())
        file << " " << packet;
    file << " at: " << t_ns << std::endl;
}

