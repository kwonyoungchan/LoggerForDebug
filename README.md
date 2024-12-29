# LoggerForDebug v1.02
## 변경사항
- MFC 동적 라이브러리 빌드 프로젝트 추가
- Logger 소스코드만 따로 분리
- Log 출력 방식 변경

## 개요
- C++ 프로젝트에서 멀티쓰레드 런타임 환경에서 각 함수들을 디버깅 하기 위해서 만들었음. 
- 작성된 로그들은 .txt 확장명의 로그 파일을 생성하여 작성되도록 만들어짐.


## 특징  
 - 멀티쓰레드 환경에서 사용 가능함.
 - C++ 14 표준 이상의 프로젝트에 적용 가능함.
 - 동적라이브러리 형태로 구현됨.
 - 로그를 카테고리 화 하여 어떤 종류의 로그인지 식별 가능함. (참조 1)


## DLL 빌드 방법 
`Dll4Logger.sln' 프로젝트에 들어가서 원하는 빌드 플랫폼을 설정하고 빌드하면 된다.  
자신이 개발하고자 하는 응용프로그램의 플랫폼에 맞게 빌드하도록 하자. 
> **주의**   
> 해당 프로젝트는 Visual Studio 2022 환경으로 생성되어서, 구 버전 프로젝트에서 해당 프로젝트를 열면 경고 메시지가 뜰 수 있음.  
> 가능한 visual studio 2022 환경에서 빌드하는 것을 권장함.


## DLL 적용 방법
1. `Logger.h`, `Dll4Logger.lib` 소스코드를 사용하고자 하는 응용프로그램 프로젝트에 추가
2. 속성 -> C/C++ -> 일반 -> 추가 포함 디렉토리 항목에 `Logger.h` 가 있는 주소를 기입
3. 속성 -> 링커 -> 입력 -> 추가 종속성 항목에 'Dll4Logger.lib' 이름을 추가 
4. 속성 -> 링커 -> 일반 -> 추가 라이브러리 디렉토리 항목에 `Dll4Logger.lib` 가 있는 주소를 기입


## 로그 사용하는 법 
### 로그 파일 이름 정의 
```cpp
#include "Logger.h"
int main()
{
// logger 인스턴스를 불러온다.
auto& logger = CLogger::getInstance();
// 로그 파일의 이름을 정의한다. (두번째 파라메터는 로그를 파일에 작성하는지 여부를 선택하는 설정이다.)
logger.configureLogging("debug_history.txt", true);
}
```
### 로그 작성 예시 
```cpp
LOG_DEBUG("This is DEBUG Log.");
LOG_INFO("This is INFO Log.");
LOG_WARNING("This is WARNING Log.");
LOG_ERROR("This is ERROR Log.");
```

### 예외 로그 작성 
> 해당 기능은 `try, catch' 문에서 사용하는 것을 권장함.  
```cpp
   try {
       if (num == nullptr)  // 예외 상황 발생 조건 
       {
           throw CException("input is nullptr!!");  // 예외 정보 throw 
       }
       else
       {
           // do anything
       }
       LOG_DEBUG("Next logic begin");
   }
   catch (const CException& e) {
       LOG_ERROR(e.what());  // throw 된 정보를 로그 형식으로 작성 
   }
```

## 참조 
### 참조 1. 로그 종류 
1. **LOG_DEBUG**: 디버그 결과, 일반적인 코멘트 작성할 때 사용
2. **LOG_INFO**: 연산 정보, 변수 정보 등을 작성할 때 사용
3. **LOG_WARNING**: 프로그램 운용에 치명적이지 않으나, 주의를 요해야 할 때 사용
4. **LOG_ERROR**: 예외 발생, 프로그램 운용에 치명적인 문제인 경우에 사용

### 참조 2. 로그 종류별 텍스트 형상
![image](https://github.com/user-attachments/assets/676f18e2-33f7-447b-8d16-ad9a5cc70811)  
|번호|설명|
|--|--|
|1|로그출력 시간|
|2|로그 종류|
|3|로그 메시지|
|4|로그가 출력된 소스코드 위치|

