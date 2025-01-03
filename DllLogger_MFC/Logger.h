// CLogger.h
#ifndef CLogger_H
#define CLogger_H

#include <string>
#include <fstream>
#include <mutex>
#include <exception>
#include <chrono>
#include <sstream>



// 로그 종류 열거자 
enum class ELogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};
class AFX_EXT_CLASS CLogger {
public:
    static CLogger& getInstance();

    void configureLogging(const char* filename, bool enableFileLogging = true);
    void logMessage(ELogLevel eLoglevel, const std::string& message, const char* functionName, const char* fileName, int lineNumber);

private:
    CLogger();
    ~CLogger();
    // 해당 클래스는 싱글톤 패턴이므로 복사와 대입을 차단
    CLogger(const CLogger&) = delete;
    CLogger& operator=(const CLogger&) = delete;

    std::string extractFileName(const std::string& filePath) const;
    void writeLog(const std::string& logEntry);
    std::string getCurrentTime() const;
    std::string logLevelToString(ELogLevel eLogLevel);

    std::string logFilename = "";
    bool saveToFile = false;
    std::mutex logMutex;
};

// 예외 메시지 클래스
class AFX_EXT_CLASS CExcep {
public:
    explicit CExcep(const std::string& msg);
    const std::string& what() const;

    CExcep();
    ~CExcep();
    CExcep(const CExcep& other);    // 

private:
    std::string message;
};

// 로그 매크로 : 

#define LOG_INFO(message) CLogger::getInstance().logMessage(ELogLevel::LOG_INFO,message, __FUNCTION__, __FILE__, __LINE__)

#define LOG_DEBUG(message) CLogger::getInstance().logMessage(ELogLevel::LOG_DEBUG,message, __FUNCTION__, __FILE__, __LINE__)

#define LOG_WARNING(message) CLogger::getInstance().logMessage(ELogLevel::LOG_WARNING,message, __FUNCTION__, __FILE__, __LINE__)

#define LOG_ERROR(message) CLogger::getInstance().logMessage(ELogLevel::LOG_ERROR,message, __FUNCTION__, __FILE__, __LINE__)

#endif // CLogger_H
