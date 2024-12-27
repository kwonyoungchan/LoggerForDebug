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


// Singleton �ν��Ͻ� ��ȯ
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
/// ����� �α� �ؽ�Ʈ ���� ���� �Լ�
/// </summary>
/// <param name="filename : ������ �α����� �̸�"></param>
/// <param name="enableFileLogging: �α׸� ���Ͽ� ���� ���� ����"></param>
void CLogger::configureLogging(const char* filename, bool enableFileLogging) {
    std::lock_guard<std::mutex> lock(logMutex);
    
#if __cplusplus >= 201703L              // C++17 �̻��� ��
    filesystem::CurrentDir
    const std::string logDir = "Log";
    if (!filesystem::exists(logDir)) {
        std::filesystem::create_directory(logDir);
    }


#else                                   // C++14 �� ��
    // ���� �۾� ���丮 ��������
    char currentDir[FILENAME_MAX];
    if (_getcwd(currentDir, sizeof(currentDir)) == nullptr) {
        throw std::runtime_error("Failed to get current working directory.");
    }

    // "Log" ���丮 ��� ����
    std::string logDir = std::string(currentDir) + "/Log";
    if (_access(logDir.c_str(), 0) != 0) { // ���丮 ���� ���� Ȯ��
        if (_mkdir(logDir.c_str()) != 0) { // ���丮 ����
            throw std::runtime_error("Unable to create directory: " + logDir);
        }
    }


#endif
    // "Log" ���丮 ���� (������ ����)
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
/// ���ܰ� �߻����� ��, �޽��� ��ȯ �Լ�
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
/// �α� �޽��� ǥ�� �Լ�
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
/// ��ü ���� ���丮 �߿� ������ ���� �̸��� �߶� ��ȯ
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
/// �α� ������ �α�����, ����â�� ����
/// </summary>
/// <param name="logEntry"></param>
void CLogger::writeLog(const std::string& logEntry) {
    // ��Ƽ������ ȯ�濡�� ���� �����尡 ���ÿ� ���� �ڿ��� �����ϴ� ���� ���� ���� ����� 
    std::lock_guard<std::mutex> lock(logMutex);
    if (saveToFile && logFile.is_open()) {
        logFile << logEntry;
        logFile.flush();
    }
    std::cout << logEntry;

    // mutex ��� ������ 
}

/// <summary>
/// ���� �ð� ���� ��ȯ
/// �� ��, milliseconds ������ �����ǵ��� ���� ����
/// </summary>
/// <returns></returns>
std::string CLogger::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    // ���� �ð� ���� ����ü�� ���ÿ� ����
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
    // ���� ������, ��������� ����
}
