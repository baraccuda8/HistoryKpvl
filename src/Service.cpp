#include "pch.h"


#include "main.h"

#include "service.h"


#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")


char serviceName[MAX_STRING] = "PLC210 OPC-UA Service";

DWORD ProgProcessId = 0;

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;


#define user "Евгений"
#define domm "."
#define pass "987654321987654321"


extern std::string strPatchFileName;
extern std::string CurrentDirPatch;


#define DEBUG_FILE_NAME "Log\\Service.log"

HANDLE hToken;


extern DWORD ProcessId;
extern HANDLE hProcess;
DWORD StartInteractiveClientProcess (const char* lpszUsername, const char* lpszDomain, const char* lpszPassword, const char* lpCommandLine, const char* lpCurrentDirectory = NULL);


//void addLogMessage(const char* ss)
//{
//    std::ofstream s(DEBUG_FILE_NAME, std::ios::binary | std::ios::out | std::ios::app | std::ios::ate);
//    if(s.is_open())
//    {
//        std::string s1 = ss;
//        time_t st = time(0);
//        std::tm TM;
//        localtime_s(&TM, &st);
//        char sFormat[50] ={0};
//        sprintf_s(sFormat, 50, "%04d-%02d-%02d %02d:%02d:%02d;",
//                  TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday,
//                  TM.tm_hour, TM.tm_min, TM.tm_sec);
//        //std::string out = std::string(sFormat) + std::string(s1);
//        //SetWindowText(mapWindow[HWNDCLIENT::hEditDiagnose].hWnd, out.c_str());
//        s << sFormat << s1 << std::endl;
//        s.close();
//    }
//
//}
void addLogMessage(std::string ss)
{
    std::ofstream s(DEBUG_FILE_NAME, std::ios::binary | std::ios::out | std::ios::app | std::ios::ate);
    if(s.is_open())
    {
        time_t st = time(0);
        std::tm TM;
        localtime_s(&TM, &st);
        //char sFormat[50] ={0};
        //sprintf_s(sFormat, 50, "%04d-%02d-%02d %02d:%02d:%02d;",
        //          TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday,
        //          TM.tm_hour, TM.tm_min, TM.tm_sec);
        //std::string out = std::string(sFormat) + std::string(s1);
        //SetWindowText(mapWindow[HWNDCLIENT::hEditDiagnose].hWnd, out.c_str());
        s << boost::format("%|04|-") % (TM.tm_year + 1900);
        s << boost::format("%|02|-") % (TM.tm_mon + 1);
        s << boost::format("%|02| ") % TM.tm_mday;
        s << boost::format("%|02|:") % TM.tm_hour;
        s << boost::format("%|02|:") % TM.tm_min;
        s << boost::format("%|02| ") % TM.tm_sec;
        s << FUNCTION_LINE_NAME << "| " << ss << std::endl;
        s.close();
    }

}

//void addLogMessage2(const WCHAR* ss)
//{
//    std::wofstream s(DEBUG_FILE_NAME2, std::ios::binary | std::ios::out | std::ios::app | std::ios::ate);
//    if(s.is_open())
//    {
//        //std::wstring s1 = ss;
//        //time_t st = time(0);
//        //std::tm TM;
//        //localtime_s(&TM, &st);
//        //char sFormat[50] ={0};
//        //sprintf_s(sFormat, 50, "%04d-%02d-%02d %02d:%02d:%02d;",
//        //          TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday,
//        //          TM.tm_hour, TM.tm_min, TM.tm_sec);
//        //std::string out = std::string(sFormat) + std::string(s1);
//        //SetWindowText(mapWindow[HWNDCLIENT::hEditDiagnose].hWnd, out.c_str());
//        s << ss << std::endl;
//        s.close();
//    }
//
//}

//void addLogMessage3(const char* ss, DWORD dw)
//{
//    std::ofstream s(DEBUG_FILE_NAME, std::ios::binary | std::ios::out | std::ios::app | std::ios::ate);
//    if(s.is_open())
//    {
//        std::string s1 = ss;
//        time_t st = time(0);
//        std::tm TM;
//        localtime_s(&TM, &st);
//        char sFormat[50] ={0};
//        sprintf_s(sFormat, 50, "%04d-%02d-%02d %02d:%02d:%02d;",
//                  TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday,
//                  TM.tm_hour, TM.tm_min, TM.tm_sec);
//        //std::string out = std::string(sFormat) + std::string(s1);
//        //SetWindowText(mapWindow[HWNDCLIENT::hEditDiagnose].hWnd, out.c_str());
//        s << sFormat << s1 << " dw: " << dw << std::endl;
//        s.close();
//    }
//
//}

DWORD GetCurrentSessionId ()
{
    WTS_SESSION_INFO* pSessionInfo;
    DWORD n_sessions = 0;
    BOOL ok = WTSEnumerateSessions (WTS_CURRENT_SERVER, 0, 1, &pSessionInfo, &n_sessions);
    if(!ok)
        return 0;

    DWORD SessionId = 0;

    for(DWORD i = 0; i < n_sessions; ++i)
    {
        if(pSessionInfo[i].State == WTSActive)
        {
            SessionId = pSessionInfo[i].SessionId;
            break;
        }
    }

    WTSFreeMemory (pSessionInfo);
    return SessionId;
}


DWORD StartInteractiveClientProcess(DWORD SessionId, const char* process_path, const char* path)
{
    BOOL ok;
    HANDLE hToken = NULL;

    ok = WTSQueryUserToken (SessionId, &hToken);
    if(!ok)
    {
        addLogMessage("Not Ok. WTSQueryUserToken");
        return 0;
    }

    void* environment = NULL;
    ok = CreateEnvironmentBlock (&environment, hToken, TRUE);

    if(!ok)
    {
        addLogMessage("Not Ok. CreateEnvironmentBlock");
        CloseHandle (hToken);
        return 0;
    }

    STARTUPINFO si ={sizeof (si)};
    PROCESS_INFORMATION pi ={ };
    si.lpDesktop = (LPSTR)"winsta0\\default";

    ok = CreateProcessAsUser(hToken, NULL, (LPSTR)process_path, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, environment, path, &si, &pi);
    if(!ok)
        addLogMessage("Not ok CreateProcessAsUser");

    ProcessId = pi.dwProcessId;
    hProcess = pi.hProcess;

    addLogMessage("CreateProcessAsUser == " + std::to_string(ProcessId));

    if(ok && pi.hProcess != INVALID_HANDLE_VALUE)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
    }

    if(pi.hThread != INVALID_HANDLE_VALUE)
        CloseHandle(pi.hThread);

    if(hToken != INVALID_HANDLE_VALUE)
        CloseHandle(hToken);

    DestroyEnvironmentBlock (environment);

    return ok;
}

DWORD EnumProcess();
void ClosePid();

bool outProcess = false;
long oldSessionId = -1;
DWORD LaunchProcess(const char* process_path, const char* path)
{
    DWORD processID = EnumProcess();
    if(processID)
    {
        if(!outProcess)
            addLogMessage("EnumProcess = " + std::to_string(processID));
        outProcess = true;
        return 0;
    }
    outProcess = false;
    ClosePid();

    DWORD SessionId = GetCurrentSessionId();

    if(SessionId != oldSessionId)
        addLogMessage("GetCurrentSessionId == " + std::to_string(SessionId) + ", StartInteractiveClientProcess User");

    if(SessionId == 0)
        StartInteractiveClientProcess(user, domm, pass, process_path, path);
    else
        StartInteractiveClientProcess(SessionId, process_path, path);

    oldSessionId = SessionId;

    return 0;
}
//BOOL SetPrivilege(HANDLE hToken);
void ControlHandler(DWORD request) {
    switch(request)
    {
        case SERVICE_CONTROL_STOP:
            addLogMessage("Stopped.");

            //if(ProcessId && hProcess)
            //    TerminateProcess(hProcess, 0);
            ServiceStatus.dwWin32ExitCode = 0;
            ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus (ServiceStatusHandle, &ServiceStatus);
            return;

        case SERVICE_CONTROL_SHUTDOWN:
            addLogMessage("Shutdown.");

            ServiceStatus.dwWin32ExitCode = 0;
            ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus (ServiceStatusHandle, &ServiceStatus);
            return;

        default:
            break;
    }

    SetServiceStatus(ServiceStatusHandle, &ServiceStatus);

    return;
}


void WINAPI ServiceMain(DWORD dwNumServicesArgs, LPSTR* lpServiceArgVectors)
{
    int i = 0;

    addLogMessage("ServiceMain");

    ServiceStatus.dwServiceType    = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState    = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted  = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode   = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint     = 0;
    ServiceStatus.dwWaitHint      = 0;

    ServiceStatusHandle = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);
    if(ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
        return;

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(ServiceStatusHandle, &ServiceStatus);

    //OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
    //SetPrivilege(hToken);

    std::string RunPatch = strPatchFileName + " debug";
    while(ServiceStatus.dwCurrentState == SERVICE_RUNNING)
    {
        LaunchProcess (RunPatch.c_str(), CurrentDirPatch.c_str());
        Sleep(1000);
    }

    addLogMessage("ServiceMain: stop");
    ServiceStatus.dwCurrentState    = SERVICE_STOPPED;
    ServiceStatus.dwWin32ExitCode   = -1;
    SetServiceStatus(ServiceStatusHandle, &ServiceStatus);
    return;
}

int InstallService()
{
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if(!hSCManager)
    {
        DWORD d = GetLastError();
        if(d == ERROR_ACCESS_DENIED)
            addLogMessage("Error: Can't open Service Control Manager: ACCESS_DENIED");
        else if(d == ERROR_DATABASE_DOES_NOT_EXIST)
            addLogMessage("Error: Can't open Service Control Manager: DATABASE_DOES_NOT_EXIST");
        else
            addLogMessage("Error: Can't open Service Control Manager: " + std::to_string(d));
        return -1;
    }

    SC_HANDLE hService = CreateService(
        hSCManager,
        serviceName,
        serviceName,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
        SERVICE_AUTO_START, //SERVICE_DEMAND_START, //SERVICE_AUTO_START, //
        SERVICE_ERROR_NORMAL,
        (strPatchFileName + " service").c_str(),
        NULL, NULL, NULL, NULL, NULL //"NT AUTHORITY\\LocalService", NULL
    );

    CloseServiceHandle(hService);

    CloseServiceHandle(hSCManager);
    addLogMessage("Success install service!");
    return 0;
}

int RemoveService() {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(!hSCManager)
    {
        DWORD d = GetLastError();
        if(d == ERROR_ACCESS_DENIED)
            addLogMessage("Error: RemoveService: Can't open Service Control Manager: ACCESS_DENIED");
        else if(d == ERROR_DATABASE_DOES_NOT_EXIST)
            addLogMessage("Error: RemoveService: Can't open Service Control Manager: DATABASE_DOES_NOT_EXIST");
        else
            addLogMessage("Error: RemoveService: Can't open Service Control Manager: " + std::to_string(d));
        return -1;
    }
    SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP | DELETE);
    if(!hService)
    {
        DWORD d = GetLastError();
        addLogMessage("Error: RemoveService: Can't remove service: " + std::to_string(d));
        return -1;

        CloseServiceHandle(hSCManager);
        return -1;
    }

    DeleteService(hService);
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    addLogMessage("Success remove service!");
    return 0;
}

int StartServices() {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if(!hSCManager)
    {
        DWORD d = GetLastError();
        if(d == ERROR_ACCESS_DENIED)
            addLogMessage("Error: StartServices: Can't open Service Control Manager: ACCESS_DENIED");
        else if(d == ERROR_DATABASE_DOES_NOT_EXIST)
            addLogMessage("Error: StartServices: Can't open Service Control Manager: DATABASE_DOES_NOT_EXIST");
        else
            addLogMessage("Error: StartServices: Can't open Service Control Manager: " + std::to_string(d));
        return -1;
    }
    SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_START);
    if(!hService)
    {
        DWORD d = GetLastError();
        addLogMessage("Error: StartServices: Can't OpenService service: " + std::to_string(d));
        return -1;

        CloseServiceHandle(hSCManager);
        return -1;
    }
    if(!StartService(hService, 0, NULL))
    {
        CloseServiceHandle(hSCManager);

        DWORD d = GetLastError();
        addLogMessage("Error: StartServices: Can't start service" + std::to_string(d));
        return -1;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return 0;
}

int StoptServices()
{
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if(!hSCManager)
    {
        DWORD d = GetLastError();
        if(d == ERROR_ACCESS_DENIED)
            addLogMessage("Error: StoptServices: Can't open Service Control Manager: ACCESS_DENIED");
        else if(d == ERROR_DATABASE_DOES_NOT_EXIST)
            addLogMessage("Error: StoptServices: Can't open Service Control Manager: DATABASE_DOES_NOT_EXIST");
        else
            addLogMessage("Error: StoptServices: Can't open Service Control Manager: " + std::to_string(d));
        return -1;
    }
    SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP);
    if(!hService)
    {
        DWORD d = GetLastError();
        addLogMessage("Error: StoptServices: Can't OpenService service: " + std::to_string(d));
        return -1;

        CloseServiceHandle(hSCManager);
        return -1;
    }

    SERVICE_STATUS k_Status ={0};
    if(!ControlService(hService, SERVICE_CONTROL_STOP, &k_Status))
    {
        CloseServiceHandle(hSCManager);

        DWORD d = GetLastError();
        addLogMessage("Error: StoptServices: Can't start service" + std::to_string(d));
        return -1;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return 0;
}

int Services()
{
    SERVICE_TABLE_ENTRY ServiceTable[2];

    ServiceTable[0].lpServiceName = serviceName;
    ServiceTable[0].lpServiceProc = ServiceMain;
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;

    if(!StartServiceCtrlDispatcher(ServiceTable))
    {
        addLogMessage("Error: StartServiceCtrlDispatcher");
    }
    return 0;
}


void CurrentDir()
{
    char ss[256] = "";
    GetModuleFileNameA(NULL, ss, 255);
    char ss2[256];
    strcpy_s(ss2, 256, ss);
    char* s1 = ss2;
    char* s2 = NULL;
    while(s1 && *s1)
    {
        if(*s1 == '\\')s2 = s1;
        s1++;
    }
    if(s2)
    {
        *s2 = 0;
        SetCurrentDirectory(ss2); 
        CurrentDirPatch = ss2;
    }
    strPatchFileName = std::string(ss); 
}

