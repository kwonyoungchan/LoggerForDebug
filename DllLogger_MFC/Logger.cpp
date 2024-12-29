#include "pch.h"
#include "Logger.h"
#include <ctime>
#include <iostream>
#include <locale>
#if __cplusplus >= 201703L  // C++20 �̻�
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <direct.h>
#include <io.h>
#include <codecvt>
#endif


// Singleton �ν��Ͻ� ��ȯ
CLogger& CLogger::getInstance() {
    static CLogger instance;
    return instance;
}

CLogger::CLogger()
{
    // UTF-8 ���ڵ� ����
    // ofstream �ν��Ͻ��� �����Ǳ� ���� ���ڵ��� �����ؾ� �Ѵ�. 
    std::locale::global(std::locale("Korean"));
    saveToFile = false;
}
CLogger::~CLogger() {
}

/// <summary>
/// ����� �α� �ؽ�Ʈ ���� ���� �Լ�
/// </summary>
/// <param name="filename : ������ �α����� �̸�"></param>
/// <param name="enableFileLogging: �α׸� ���Ͽ� ���� ���� ����"></param>
void CLogger::configureLogging(const char* filename, bool enableFileLogging) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string logDir;

#if __cplusplus >= 201703L              // C++17 �̻��� ��    
    logDir = "Log";
    if (!fs::exists(logDir)) {
        fs::create_directory(logDir);
    }


#else                                   // C++14 �� ��
    // ���� �۾� ���丮 ��������
    char currentDir[FILENAME_MAX];
    if (_getcwd(currentDir, sizeof(currentDir)) == nullptr) {
        throw std::runtime_error("Failed to get current working directory.");
    }

    // "Log" ���丮 ��� ����
    logDir = std::string(currentDir) + "/Log";
    struct stat info;
    if (stat(logDir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) { // ���丮 ���� ���� Ȯ��
        if (_mkdir(logDir.c_str()) != 0) { // ���丮 ����
            throw std::runtime_error("Unable to create directory: " + logDir);
        }
    }


#endif
    // "Log" ���丮 ���� (������ ����)
    logFilename = logDir + "/" + filename;
    saveToFile = enableFileLogging;


    if (saveToFile)
    {
        std::ofstream logFile;
        logFile.open(logFilename, std::ios::trunc);
        if (!logFile.is_open()) {
            throw std::runtime_error("Unable to open log file: " + logFilename);
        }
        logFile.close();
    }

}

/// <summary>
/// �α� �޽��� ǥ�� �Լ�
/// </summary>
/// <param name="eLoglevl"></param>
/// <param name="message"></param>
/// <param name="functionName"></param>
/// <param name="fileName"></param>
/// <param name="lineNumber"></param>
void CLogger::logMessage(ELogLevel eLoglevel, const std::string& message, const char* functionName,
    const char* fileName, int lineNumber) {
    std::ostringstream logStream;

    logStream << "[" << getCurrentTime() << "]\t ";
    logStream << "[" << logLevelToString(eLoglevel) << "]\t";
    switch (eLoglevel)
    {
    case ELogLevel::LOG_DEBUG:
        logStream << "==> " << message;
        break;
    case ELogLevel::LOG_INFO:
        logStream << "\t--> " << message;
        break;
    case ELogLevel::LOG_WARNING:
        logStream << "** " << message;
        break;
    case ELogLevel::LOG_ERROR:
        logStream << "!! " << message;
        break;
    }

    logStream << " (Log from " << functionName
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
    std::ofstream logFile;
    logFile.open(logFilename, std::ios::app);
    if (saveToFile && logFile.is_open()) {
        logFile.write(logEntry.c_str(), logEntry.size());
        logFile.close();
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

CExcep::CExcep(const std::string& msg)
{
    message = msg;
}

const std::string& CExcep::what() const
{
    return message;
}

CExcep::CExcep()
{
}

CExcep::~CExcep()
{
}

CExcep::CExcep(const CExcep& other)
{
    // ���� ������, ��������� ����
}
