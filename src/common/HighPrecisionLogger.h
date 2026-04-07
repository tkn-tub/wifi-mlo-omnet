/*
 * HighPrecisionLogger.h
 *
 *  Created on: 20.10.2025
 *      Author: ergenc
 */

#ifndef COMMON_HIGHPRECISIONLOGGER_H_
#define COMMON_HIGHPRECISIONLOGGER_H_

// HighPrecisionLogger.h
#include <windows.h>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <string>

class HighPrecisionLogger {
  private:
    static std::ofstream file;
    static std::mutex file_mutex;
    static bool initialized;
    static void initialize();
  public:
    static void logEvent(const std::string& modulePath,
                         const std::string& description,
                         const std::string& packet = "");
};


#endif /* COMMON_HIGHPRECISIONLOGGER_H_ */
