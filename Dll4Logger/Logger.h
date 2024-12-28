// CLogger.h
#ifndef CLogger_H
#define CLogger_H

#include <string>
#include <fstream>
#include <mutex>
#include <exception>
#include <chrono>
#include <sstream>

// 내보내기 매크로 정의
#ifdef _WIN32
#ifdef BUILD_DLL
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#else
#define DLLEXPORT
#endif

// 로그 종류 열거자 
enum class ELogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};
class DLLEXPORT CLogger {
public:
    static CLogger& getInstance();

    void configureLogging(const char* filename, bool enableFileLogging = true);
    void logException(const std::string& message, const char* functionName, const char* fileName, int lineNumber);
    void logMessage(ELogLevel eLoglevl, const std::string& message, const char* functionName, const char* fileName, int lineNumber);

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

    std::string logFilename ="";
    bool saveToFile = false;
    std::ofstream* logFile = nullptr;
    std::mutex logMutex;
};

// 예외 메시지 클래스
class DLLEXPORT CException {
public:
    explicit CException(const std::string& msg); 
    const std::string& what() const; 

    CException();
    ~CException();
    CException(const CException& other);    // 

private:
    std::string message;
};
// 매크로 정의
#define LOG_EXCEPTION(message) CLogger::getInstance().logException(message, __FUNCTION__, __FILE__, __LINE__)
#define LOG_MESSAGE(logLevel, message) CLogger::getInstance().logMessage(logLevel,message, __FUNCTION__, __FILE__, __LINE__)

#endif // CLogger_H
