/*
 * HighPrecisionLogger.cpp
 *
 *  Created on: 20.10.2025
 *      Author: ergenc
 */

#include "HighPrecisionLogger.h"

#include <chrono>

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

    // steady_clock is monotonic and portable (QueryPerformanceCounter on Windows, clock_gettime(CLOCK_MONOTONIC) on Linux)
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    double t_ns = double(std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());

    file << std::fixed << std::setprecision(0) << modulePath  << "#" << description;
    if(!packet.empty())
        file << " " << packet;
    file << " at: " << t_ns << std::endl;
}

