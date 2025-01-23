#include "pch.h"

#include "service.h"
#include "main.h"
#include "file.h"

//void addLogMessage(const char* ss);
void addLogMessage(std::string ss);

#define CATCH3(_s) catch(std::exception& exc){\
addLogMessage(std::string(_s) + std::string(" ") + exc.what());\
}catch(...){\
addLogMessage(std::string(_s) + " Unknown error");\
};;

#define LogFileName "Log\\ProcessLog"

HANDLE PidStream = INVALID_HANDLE_VALUE;
bool OpenPid()
{
    PidStream = CreateFileA(LogFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL 
                            //| FILE_FLAG_OVERLAPPED /*| FILE_FLAG_WRITE_THROUGH*/
                            , NULL);
    if(PidStream != INVALID_HANDLE_VALUE)
    {
        return true;
    }
    return false;
}


void SavePid()
{
    if(PidStream != INVALID_HANDLE_VALUE)
    {
        DWORD CurrentProcessId = GetCurrentProcessId();
        char ss[256];
        sprintf_s(ss, 255, "%u", CurrentProcessId);
        DWORD len = (DWORD)strlen(ss);
        WriteFile(PidStream, ss, len, &len, NULL);
        FlushFileBuffers(PidStream);
    }
}

void ClosePid()
{
    if(PidStream != INVALID_HANDLE_VALUE)
    {
        CloseHandle(PidStream);;
    }
}

DWORD EnumProcess()
{
    try
    {
        if(OpenPid())
        {
            return 0;
        }
        else
        {
            DWORD aProcesses[1024];
            DWORD cbNeeded = 0;
            DWORD FindProcess = 0;
            std::ifstream s1(LogFileName, std::ios::binary | std::ios::in);
            if(s1.is_open())
            {
                char ss[256];
                s1 >> ss;
                FindProcess = atoi(ss);
                s1.close();
            }
            else
                throw std::exception((FUNCTION_LINE_NAME + (std::string("Не могу открыть файл для чтения: ") + LogFileName)).c_str());

            if(!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
                throw std::exception((FUNCTION_LINE_NAME + std::string("Ошибка запуска программы : !EnumProcesses")).c_str());

            DWORD cProcesses = cbNeeded / sizeof(DWORD);

            for(DWORD i = 0; i < cProcesses; i++)
            {
                if(aProcesses[i] != 0 && aProcesses[i] == FindProcess)
                {
                    return FindProcess;
                }
            }

        }
    }
    CATCH3("Error: ");
    return 0;
}

