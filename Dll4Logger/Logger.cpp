#include "pch.h"
#include "Logger.h"
#include <ctime>
#include <iostream>
#if __cplusplus >= 201703L
#include <filesystem>
#else
#include <direct.h>
#include <io.h>
#endif


// Singleton 인스턴스 반환
CLogger& CLogger::getInstance() {
    static CLogger instance;
    return instance;
}

CLogger::CLogger() : saveToFile(false) {}
CLogger::~CLogger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

/// <summary>
/// 디버그 로그 텍스트 파일 생성 함수
/// </summary>
/// <param name="filename : 생성할 로그파일 이름"></param>
/// <param name="enableFileLogging: 로그를 파일에 저장 유무 선택"></param>
void CLogger::configureLogging(const char* filename, bool enableFileLogging) {
    std::lock_guard<std::mutex> lock(logMutex);
    
#if __cplusplus >= 201703L              // C++17 이상일 때
    filesystem::CurrentDir
    const std::string logDir = "Log";
    if (!filesystem::exists(logDir)) {
        std::filesystem::create_directory(logDir);
    }


#else                                   // C++14 일 때
    // 현재 작업 디렉토리 가져오기
    char currentDir[FILENAME_MAX];
    if (_getcwd(currentDir, sizeof(currentDir)) == nullptr) {
        throw std::runtime_error("Failed to get current working directory.");
    }

    // "Log" 디렉토리 경로 설정
    std::string logDir = std::string(currentDir) + "/Log";
    if (_access(logDir.c_str(), 0) != 0) { // 디렉토리 존재 여부 확인
        if (_mkdir(logDir.c_str()) != 0) { // 디렉토리 생성
            throw std::runtime_error("Unable to create directory: " + logDir);
        }
    }


#endif
    // "Log" 디렉토리 생성 (없으면 생성)
    logFilename = logDir + "/" + filename;
    saveToFile = enableFileLogging;

    if (saveToFile) {
        logFile.open(logFilename, std::ios::trunc);
        if (!logFile.is_open()) {
            throw std::runtime_error("Unable to open log file: " + logFilename);
        }
    }
}

/// <summary>
/// 예외가 발생했을 때, 메시지 반환 함수
/// </summary>
/// <param name="message"></param>
/// <param name="functionName"></param>
/// <param name="fileName"></param>
/// <param name="lineNumber"></param>
void CLogger::logException(const std::string& message, const char* functionName,
    const char* fileName, int lineNumber) {
    std::ostringstream logStream;
    logStream << "[" << logLevelToString(ELogLevel::LOG_ERROR) << "]";
    logStream << "[" << getCurrentTime() << "] ";
    logStream << "Message: " << message ;
    logStream <<"(Exception in " << functionName;
    logStream << " at " << extractFileName(fileName) << ":" << lineNumber << ")" << "\n";
    writeLog(logStream.str());
}

/// <summary>
/// 로그 메시지 표출 함수
/// </summary>
/// <param name="eLoglevl"></param>
/// <param name="message"></param>
/// <param name="functionName"></param>
/// <param name="fileName"></param>
/// <param name="lineNumber"></param>
void CLogger::logMessage(ELogLevel eLoglevl, const std::string& message, const char* functionName,
    const char* fileName, int lineNumber) {
    std::ostringstream logStream;
    logStream << "[" << logLevelToString(eLoglevl) << "]";
    logStream << "[" << getCurrentTime() << "] ";
    logStream<< message << "(Logged from " << functionName
    << " at " << extractFileName(fileName) << ":" << lineNumber << ")\n";
    writeLog(logStream.str());
}

/// <summary>
/// 전체 파일 디렉토리 중에 마지막 파일 이름만 잘라서 반환
/// </summary>
/// <param name="filePath"></param>
/// <returns></returns>
std::string CLogger::extractFileName(const std::string& filePath) const {
#if __cplusplus >= 201703L
    return std::filesystem::path(filePath).filename().string();
#else
    size_t lastSlashPos = filePath.find_last_of("/\\");
    if (lastSlashPos == std::string::npos) {
        return filePath;
    }
    return filePath.substr(lastSlashPos + 1);
#endif
}

/// <summary>
/// 로그 내용을 로그파일, 실행창에 전시
/// </summary>
/// <param name="logEntry"></param>
void CLogger::writeLog(const std::string& logEntry) {
    // 멀티스레드 환경에서 여러 스레드가 동시에 공유 자원에 접근하는 것을 막기 위해 사용함 
    std::lock_guard<std::mutex> lock(logMutex);
    if (saveToFile && logFile.is_open()) {
        logFile << logEntry;
        logFile.flush();
    }
    std::cout << logEntry;

    // mutex 잠금 해제됨 
}

/// <summary>
/// 현재 시간 정보 반환
/// 추 후, milliseconds 단위로 측정되도록 개선 예정
/// </summary>
/// <returns></returns>
std::string CLogger::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    // 현재 시간 정보 구조체를 로컬에 복사
    localtime_s(&localTime, &nowTime);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return std::string(buffer);
}

std::string CLogger::logLevelToString(ELogLevel eLogLevel)
{
    switch (eLogLevel) {
    case ELogLevel::LOG_DEBUG: return "DEBUG";
    case ELogLevel::LOG_INFO: return "INFO";
    case ELogLevel::LOG_WARNING: return "WARNING";
    case ELogLevel::LOG_ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}

CException::CException(const std::string& msg)
{
    message = msg;
}

const std::string& CException::what() const
{
    return message;
}

CException::CException()
{
}

CException::~CException()
{
}

CException::CException(const CException& other)
{
    // 복사 생성자, 명시적으로 정의
}
