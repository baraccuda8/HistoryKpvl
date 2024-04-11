#include "pch.h"
#include "main.h"
#include "file.h"
//#include "ip.h"

HWND hWndDebug = NULL;

//Глобальный Instance программы
extern HINSTANCE hInstance;

#define AllDebugFile "Log\\All Debug.log"

std::string cp1251_to_utf8(std::string str)
{
    std::string res;
    std::wstring ures;

    int result_u, result_c;

    result_u = MultiByteToWideChar(1251, 0, &str[0], -1, 0, 0);
    if(!result_u) return "";

    ures.resize(result_u);
    if(!MultiByteToWideChar(1251, 0, &str[0], -1, &ures[0], result_u)) return "";

    result_c = WideCharToMultiByte(CP_UTF8, 0, &ures[0], -1, 0, 0, 0, 0);
    if(!result_c) return "";

    res.resize(result_c);
    if(!WideCharToMultiByte(CP_UTF8, 0, &ures[0], -1, &res[0], result_c, 0, 0)) return "";

    return res;
}

std::string utf8_to_cp1251(std::string str)
{
    if(!str.length()) return "";

    std::wstring res;

    int convertResult;
    convertResult = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
    if(convertResult <= 0)
        return "";

    res.resize(convertResult);
    if(MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &res[0], (int)res.size()) <= 0)
        return "";

    convertResult = WideCharToMultiByte(CP_ACP, 0, res.c_str(), (int)res.length(), NULL, 0, NULL, NULL);
    if(convertResult <= 0)
        return "";

    str.resize(convertResult);
    if(WideCharToMultiByte(CP_ACP, 0, res.c_str(), (int)res.length(), &str[0], (int)str.size(), NULL, NULL) <= 0)
        return "";

    return str;
}




void Log(std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl, std::string msg)
{
    if(logger)
        logger->log(spdlog::level::level_enum::info, msg);
}


std::string GetShortTimes()
{
    time_t st = time(0);
    std::tm TM;
    localtime_s(&TM, &st);
    char sFormat[1024];
    sprintf_s(sFormat, 50, "%02d.%02d %02d:%02d", TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min);
    return sFormat;
}


//void SendDebug(std::string o, std::string ss)
//{
//    time_t st = time(0);
//    std::tm TM;
//    localtime_s(&TM, &st);
//    char sFormat[1024];
//    sprintf_s(sFormat, 50, "%04d-%02d-%02d %02d:%02d:%02d ", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
//    SetWindowText(hWndDebug, (std::string(sFormat) + ss).c_str());
//
//    //sprintf_s(sFormat, 1024, "[%04d-%02d-%02d %02d:%02d:%02d.000] ", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
//    //std::ofstream s(AllDebugFile, std::ios::binary | std::ios::out | std::ios::app);
//    //if(s.is_open())
//    //{
//    //    s << sFormat << o << ss << std::endl;
//    //    s.close();
//    //}
//
//    LOGGER_INFO(AllLogger, o + ss);
//}


//Перечисление файлов в дирректории
std::Pathc getDirContents (const std::string& dirName)
{
    std::Pathc pathcs;
    std::copy (boost::filesystem::directory_iterator (dirName), boost::filesystem::directory_iterator (), std::inserter (pathcs, pathcs.end()));
    return pathcs;
}


void GetRessusce(int ID, const char* DATE, LPVOID* pLockedResource, DWORD* dwResourceSize)
{
    if(!pLockedResource || !dwResourceSize)
        throw std::exception(__FUN("GetRessusce Error Parametr"));

    HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(ID), DATE);
    if(!hResource)
        throw std::exception(__FUN("!FindResource"));

    HGLOBAL hLoadedResource = LoadResource(hInstance, hResource);
    if(!hLoadedResource)
        throw std::exception(__FUN("!hLoadedResource"));

    *pLockedResource = LockResource(hLoadedResource);
    if(!*pLockedResource)
        throw std::exception(__FUN("!*pLockedResource"));

    *dwResourceSize = SizeofResource(hInstance, hResource);
    if(!*dwResourceSize)
        throw std::exception(__FUN("!*dwResourceSize"));

    UnlockResource(hLoadedResource);
}


std::map<HWND, WINDOWPLACEMENT>wp_old;

void SaveWindowPos(HWND hWnd, const char* name) 
{
    if(hWnd == NULL) 
        throw std::exception(__FUN(std::string("Отсутстует Хендл окна \""))); 
    else
    {
        WINDOWPLACEMENT wp ={0};
        GetWindowPlacement(hWnd, &wp);
        if(memcmp(&wp_old[hWnd], &wp, sizeof(WINDOWPLACEMENT)))
        {
            //WPF_RESTORETOMAXIMIZED;
            if(/*wp.flags &&*/ wp.showCmd == 1 || wp.showCmd == 3)
            {
                //wp.showCmd = 1;
                wp_old[hWnd] = wp;
                //memcpy(&wp_old[hWnd], &wp, sizeof(WINDOWPLACEMENT));
                std::fstream s(name, std::fstream::binary | std::fstream::out);
                if(!s.is_open())
                    throw std::exception(__FUN(std::string("Ошибка открытия файла \"") + name + std::string("\"")));
                s.write((char*)&wp, sizeof(WINDOWPLACEMENT));
                s.close();

                //std::string ss = "cx ";
                //ss += std::to_string(wp.rcNormalPosition.right - wp.rcNormalPosition.left) + " cy: " + std::to_string(wp.rcNormalPosition.bottom - wp.rcNormalPosition.top);
                //SetWindowText(hWnd, ss.c_str());
            }
        }
    }
}

[[nodiscard]]
WINDOWPLACEMENT LoadWindowPos(HWND hWnd, const char* name)
{
    if(hWnd)
    {
        static WINDOWPLACEMENT wp ={0};
        std::fstream s(name, std::fstream::binary | std::fstream::in);
        if(s.is_open())
        {
            s.read((char*)&wp, sizeof(WINDOWPLACEMENT));
            s.close();
            SetWindowPlacement(hWnd, &wp);
        }
        return wp;
    }
    return LoadWindowPos(name);
}

[[nodiscard]]
WINDOWPLACEMENT LoadWindowPos(const char* name)
{
    static WINDOWPLACEMENT wp ={0};
    std::fstream s(name, std::fstream::binary | std::fstream::in);
    if(s.is_open())
    {
        s.read((char*)&wp, sizeof(WINDOWPLACEMENT));
        s.close();

    }
    return wp;
}

#define SQLFileName "PostgreSQL.dat"


extern std::string m_dbhost;
extern std::string m_dbport;
extern std::string m_dbname;
extern std::string m_dbuser;
extern std::string m_dbpass;

std::string pKey = "хабраbarracudabarracudaхабра";
char sCommand[1024];

void encode(char* pText, int len)
{
    for(int i = 0; i < len; i++)
        pText[i] = (byte)(pText[i] ^ pKey[i % pKey.length()]);
}

void SaveConnect()
{
    std::stringstream pass;
    pass << m_dbhost << std::endl
        << m_dbport << std::endl
        << m_dbname << std::endl
        << m_dbuser << std::endl
        << m_dbpass;

    std::string p = pass.str();
    memset(sCommand, 0, 1024);
    strcpy_s(sCommand, 255, p.c_str());;
    encode(sCommand, (int)p.length());

    std::ofstream s(SQLFileName, std::ios::binary | std::ios::out | std::ios::trunc);
    if(s.is_open())
    {
        s.write(sCommand, p.length());
        s.close();
    }
}

bool LoadConnect()
{
    memset(sCommand, 0, 1024);
    std::ifstream s(SQLFileName, std::ios::binary | std::ios::in);
    if(s.is_open())
    {
        s.read(sCommand, 1024);
        int len = (int)s.gcount();
        s.close();
        encode(sCommand, len);
        std::vector <std::string>split;
        boost::split(split, sCommand, boost::is_any_of("\n"));
        if(split.size() == 5)
        {
            m_dbhost = split[0];
            m_dbport = split[1];
            m_dbname = split[2];
            m_dbuser = split[3];
            m_dbpass = split[4];
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT rcChild, rcParent;
    int  cxChild, cyChild, cxParent, cyParent, cxScreen, cyScreen, xNew, yNew;
    HDC  hdc;

    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right - rcChild.left;
    cyChild = rcChild.bottom - rcChild.top;

    if(hwndParent)
    {
        GetWindowRect(hwndParent, &rcParent);
        cxParent = rcParent.right - rcParent.left;
        cyParent = rcParent.bottom - rcParent.top;
    }
    else
    {
        cxParent = GetSystemMetrics(SM_CXMAXIMIZED);
        cyParent = GetSystemMetrics(SM_CYMAXIMIZED);
        rcParent.left = 0;
        rcParent.top = 0;
    }

    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

    xNew = rcParent.left + (cxParent - cxChild) / 2;
    if(xNew < 0)xNew = 0; else if((xNew + cxChild) > cxScreen) xNew = cxScreen - cxChild;

    yNew = rcParent.top + (cyParent - cyChild) / 2;
    if(yNew < 0)yNew = 0; else if((yNew + cyChild) > cyScreen) yNew = cyScreen - cyChild;
    return SetWindowPos(hwndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

//std::string GetStringData(std::string d)
//{
//    if(!d.size())return "";
//    std::vector <std::string>split;
//    boost::split(split, d, boost::is_any_of("."), boost::token_compress_on);
//    if(split.size())
//        return split[0];
//    return d;
//}
