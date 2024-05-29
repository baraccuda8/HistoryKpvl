#include "pch.h"
#include "main.h"
#include "file.h"
#include "exel.h"
#include "win.h"
#include "Graff.h"

#include "SQL.h"
#include "Event.h"

#ifdef _MYDEBUG
#define FULLRUN 0
#else
#define FULLRUN 1
#endif

const std::string FORMATTIME = "(\\d{1,4}).(\\d{1,2}).(\\d{1,2}) (\\d{1,2}):(\\d{1,2}):(\\d{1,2}).*";


std::string szTitle = "История данных по Комплексу Подготовки Высокопрочного Листа. Версия ";

//Текущий рабочий каталог
std::string strPatchFileName = "";
std::string CurrentDirPatch = "";
//std::string lpSheetDir = "Sheet";
//std::string lpCassetteDir  = "Cassette";
//std::string lpLCassetteDir = "Cassette";
std::string lpLogDir = "Log";
std::string lpLogPdf = "Pdf";

//Глобальный Run
bool isRun = true;

//void OpenAllParam();
void Open_KPVL();
void Close_KPVL();

void Open_FURN();
void Close_FURN();



uint32_t SizeLogger = 104857600;
uint32_t CountLogger = 1000;
int CountWatchDogWait  = TIME_OUT / 1000;

//Глобальный Instance программы
HINSTANCE hInstance = NULL;
bool MyServer = false;
std::string Server = "";

//std::map<PlcType::Type, Plc> PLC;

std::shared_ptr<spdlog::logger> InitLogger(std::string LoggerOut)
{
    try
    {
        std::shared_ptr<spdlog::logger> Logger = spdlog::details::registry::instance().get(LoggerOut);
        if(!Logger.get())
        {
            std::string FileName = lpLogDir + "\\" + LoggerOut + ".log";

            std::ofstream s(FileName, std::ios::binary | std::ios::out | std::ios::app);
            if(s.is_open())
            {
                s << std::endl << std::endl;
                s.close();
            }

            Logger = spdlog::rotating_logger_mt(LoggerOut, FileName, SizeLogger, CountLogger);
        }
        Logger->set_level(spdlog::level::level_enum::info);
        LOG_INFO(Logger, "");
        LOG_INFO(Logger, "");
        LOG_INFO(Logger, "{:90}| Старт программы", FUNCTION_LINE_NAME);
        return Logger;
    }
    catch(...) {}
    return 0;
}

std::shared_ptr<spdlog::logger> AllLogger = NULL;



void DisplayContextMenu(HWND hwnd, int ID)
{
    DWORD dwpos = GetMessagePos();
    POINT pt;
    pt.x = LOWORD(dwpos);
    pt.y = HIWORD(dwpos);

    HMENU hmenu = LoadMenu(hInstance, MAKEINTRESOURCE(ID));
    if(hmenu == NULL) return;

    HMENU hmenuTrackPopup = GetSubMenu(hmenu, 0);
    TrackPopupMenu(hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hmenu);
}

void SetWacthDog()
{
}

std::string GetLastErrorString()
{
    char* lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
        GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL
    );
    std::string out = lpMsgBuf;
    LocalFree(lpMsgBuf);
    return lpMsgBuf;
}

std::string ToString(const OpcUa::DateTime& t)
{
    std::time_t st = OpcUa::DateTime::ToTimeT(t);
    tm curr_tm;
    localtime_s(&curr_tm, &st);

    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (curr_tm.tm_year + 1900);
    sdt << boost::format("%|02|-") % (curr_tm.tm_mon + 1);
    sdt << boost::format("%|02| ") % curr_tm.tm_mday;
    sdt << boost::format("%|02|:") % curr_tm.tm_hour;
    sdt << boost::format("%|02|:") % curr_tm.tm_min;
    sdt << boost::format("%|02|") % curr_tm.tm_sec;
    return sdt.str();
}

std::string GetDataTimeString(std::time_t& st)
{
    st = time(NULL);
    tm curr_tm;
    localtime_s(&curr_tm, &st);

    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (curr_tm.tm_year + 1900);
    sdt << boost::format("%|02|-") % (curr_tm.tm_mon + 1);
    sdt << boost::format("%|02| ") % curr_tm.tm_mday;
    sdt << boost::format("%|02|:") % curr_tm.tm_hour;
    sdt << boost::format("%|02|:") % curr_tm.tm_min;
    sdt << boost::format("%|02|") % curr_tm.tm_sec;
    return sdt.str();
}

std::string GetDataTimeString(std::tm& st)
{
    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (st.tm_year + 1900);
    sdt << boost::format("%|02|-") % (st.tm_mon + 1);
    sdt << boost::format("%|02| ") % st.tm_mday;
    sdt << boost::format("%|02|:") % st.tm_hour;
    sdt << boost::format("%|02|:") % st.tm_min;
    sdt << boost::format("%|02|") % st.tm_sec;
    return sdt.str();
}

std::string GetDataTimeString(std::time_t* st)
{
    //st = time(NULL);
    tm curr_tm;
    localtime_s(&curr_tm, st);

    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (curr_tm.tm_year + 1900);
    sdt << boost::format("%|02|-") % (curr_tm.tm_mon + 1);
    sdt << boost::format("%|02| ") % curr_tm.tm_mday;
    sdt << boost::format("%|02|:") % curr_tm.tm_hour;
    sdt << boost::format("%|02|:") % curr_tm.tm_min;
    sdt << boost::format("%|02|") % curr_tm.tm_sec;
    return sdt.str();
}

std::wstring GetData(std::wstring str)
{
    std::wstring::const_iterator start = str.begin();
    std::wstring::const_iterator end = str.end();
    boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}):\\d{1,2}.*");
    boost::match_results<std::wstring::const_iterator> what;

    boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size();
    return what[1].str();
}


std::string GetDataTimeString()
{
    std::time_t st = time(NULL);
    tm curr_tm;
    localtime_s(&curr_tm, &st);

    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (curr_tm.tm_year + 1900);
    sdt << boost::format("%|02|-") % (curr_tm.tm_mon + 1);
    sdt << boost::format("%|02| ") % curr_tm.tm_mday;
    sdt << boost::format("%|02|:") % curr_tm.tm_hour;
    sdt << boost::format("%|02|:") % curr_tm.tm_min;
    sdt << boost::format("%|02|") % curr_tm.tm_sec;
    return sdt.str();
}

time_t DataTimeOfString(std::string str, std::string format, int& d1, int& d2, int& d3, int& d4, int& d5, int& d6)
{
    d1 = 0; d2 = 0; d3 = 0; d4 = 0; d5 = 0; d6 = 0;
    std::string::const_iterator start = str.begin();
    std::string::const_iterator end = str.end();
    boost::regex xRegEx(format);
    boost::match_results<std::string::const_iterator> what;

    if(boost::regex_search(start, end, what, xRegEx, boost::match_default))
    {
        size_t size = what.size();
        if(size >= 1)d1 = atoi(what[1].str().c_str());
        if(size >= 2)d2 = atoi(what[2].str().c_str());
        if(size >= 3)d3 = atoi(what[3].str().c_str());
        if(size >= 4)d4 = atoi(what[4].str().c_str());
        if(size >= 5)d5 = atoi(what[5].str().c_str());
        if(size >= 6)d6 = atoi(what[6].str().c_str());

        std::tm tm;
        tm.tm_year = d1 - 1900;
        tm.tm_mon = d2 - 1;
        tm.tm_mday = d3;
        tm.tm_hour = d4;
        tm.tm_min = d5;
        tm.tm_sec = d6;
        return mktime(&tm);
    }
    return 0;
}


time_t DataTimeOfString(std::string str, std::string format, std::tm& TM)
{
    TM.tm_year = 0; TM.tm_mon = 0; TM.tm_mday = 0; TM.tm_hour = 0; TM.tm_min = 0; TM.tm_sec = 0;
    std::string::const_iterator start = str.begin();
    std::string::const_iterator end = str.end();
    boost::regex xRegEx(format);
    boost::match_results<std::string::const_iterator> what;

    if(boost::regex_search(start, end, what, xRegEx, boost::match_default))
    {
        size_t size = what.size();
        if(size >= 1)TM.tm_year = Stoi(what[1].str());
        if(size >= 2)TM.tm_mon = Stoi(what[2].str());
        if(size >= 3)TM.tm_mday = Stoi(what[3].str());
        if(size >= 4)TM.tm_hour = Stoi(what[4].str());
        if(size >= 5)TM.tm_min = Stoi(what[5].str());
        if(size >= 6)TM.tm_sec = Stoi(what[6].str());

        std::tm tm = TM;
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        return mktime(&tm); 
    }
    return 0;
}

time_t DataTimeDiff(std::string str1, std::string str2, std::string format)
{
    std::tm TM;
    time_t tm1 = DataTimeOfString(str1, format, TM);
    time_t tm2 = DataTimeOfString(str2, format, TM);
    return (time_t)difftime(tm1, tm2);
}

std::string GetStringData(std::string d)
{
    if(!d.size())return "";
    std::string ss = d;
    std::vector <std::string>split1;
    boost::split(split1, d, boost::is_any_of("."), boost::token_compress_on);
    if(split1.size())
        d = split1[0];

    std::vector <std::string>split2;
    boost::split(split2, d, boost::is_any_of("+"), boost::token_compress_on);
    if(split2.size())
        d = split2[0];

    return d;
}


//Вывод строки ошибки выполнения программы
int WinErrorExit(HWND hWnd, const char* lpszFunction)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s\r\nfailed with error %d:\r\n%s"), lpszFunction, dw, lpMsgBuf);

    //if(MainLogger) MainLogger->error(std::string((char*)lpDisplayBuf));
    MessageBox(hWnd, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    PostQuitMessage(0);
    return 1;
}


//Структура инициализации добавочных контролов
INITCOMMONCONTROLSEX initcontrol ={sizeof(INITCOMMONCONTROLSEX),
      ICC_LISTVIEW_CLASSES    //listview, header
    | ICC_TREEVIEW_CLASSES    // treeview, tooltips
    | ICC_BAR_CLASSES         // toolbar, statusbar, trackbar, tooltips
    | ICC_TAB_CLASSES         // tab, tooltips
    | ICC_UPDOWN_CLASS        // updown
    | ICC_PROGRESS_CLASS      // progress
    | ICC_ANIMATE_CLASS       // animate
    | ICC_WIN95_CLASSES
    | ICC_DATE_CLASSES        // month picker, date picker, time picker, updown
    | ICC_USEREX_CLASSES      // comboex
    | ICC_COOL_CLASSES        // rebar (coolbar) control
    | ICC_INTERNET_CLASSES
    | ICC_PAGESCROLLER_CLASS  // page scroller
    | ICC_NATIVEFNTCTL_CLASS  // native font control
#if (NTDDI_VERSION >= NTDDI_WINXP)
    | ICC_STANDARD_CLASSES
    | ICC_LINK_CLASS
#endif

};



//Масив фонтов от 8 до 16 размера жирный шрифт Arial
std::map<emFont, HFONT> Font;


std::map<int, MYHICON>Icon;
//std::map<HICON, int>IconSize;

#define LOAD_ICON(_i, _z) {HICON _b = LoadIcon(hInstance, MAKEINTRESOURCE(_i));\
        if (!_b)throw std::exception(__FUN(std::string("Не могу загрузить ресурс: Icon \"") + std::to_string(IDI_EXIT) + "\""));\
        Icon[_i] = {_b, _z};/*IconSize[_b] = _z;*/}
#define cooltab 23


void InitIcon()
{
    try
    {
        //HICON _b = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
        //Icon[IDI_ICON1] = {_b, 32};
        LOAD_ICON(IDI_ICON1, 32);
        LOAD_ICON(IDI_COLOR3, 32);
        LOAD_ICON(IDI_EXIT, 16);
        LOAD_ICON(IDI_ICON1, 32);
        LOAD_ICON(IDI_MAXI, 16);
        LOAD_ICON(IDI_MINI, 16);
        LOAD_ICON(IDI_NEWDATA1, 16);
        LOAD_ICON(IDI_NEWDATA2, 16);
        LOAD_ICON(IDI_NEWDATA3, 16);
        LOAD_ICON(IDI_PLC, 32);
    }
    catch(std::exception& exc)
    {
        WinErrorExit(NULL, exc.what());
    }
    catch(...)
    {
        WinErrorExit(NULL, "Unknown error.");
    }
}
//Инициализация масова фонтов от 8 до 16 размера жирный шрифт Arial
void InitFont()
{
    HDC hDc = GetDC(NULL);
    int DeviceCapsCX = GetDeviceCaps(hDc, LOGPIXELSY);
    int nHeight06 = -MulDiv(6, DeviceCapsCX, 72);
    int nHeight07 = -MulDiv(7, DeviceCapsCX, 72);
    int nHeight08 = -MulDiv(8, DeviceCapsCX, 72);
    int nHeight09 = -MulDiv(9, DeviceCapsCX, 72);
    int nHeight10 = -MulDiv(10, DeviceCapsCX, 72);
    int nHeight11 = -MulDiv(11, DeviceCapsCX, 72);
    int nHeight12 = -MulDiv(12, DeviceCapsCX, 72);
    int nHeight13 = -MulDiv(13, DeviceCapsCX, 72);
    int nHeight14 = -MulDiv(14, DeviceCapsCX, 72);
    int nHeight15 = -MulDiv(15, DeviceCapsCX, 72);
    int nHeight16 = -MulDiv(16, DeviceCapsCX, 72);

    Font[emFont::Font06] = CreateFont(nHeight06, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font07] = CreateFont(nHeight07, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font08] = CreateFont(nHeight08, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font09] = CreateFont(nHeight09, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font10] = CreateFont(nHeight10, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font11] = CreateFont(nHeight11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font12] = CreateFont(nHeight12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font13] = CreateFont(nHeight13, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font14] = CreateFont(nHeight14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font15] = CreateFont(nHeight15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
    Font[emFont::Font16] = CreateFont(nHeight16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 0, "Arial");
}



void CheckDir(std::string dir)
{
    struct stat buff;
    if(stat(dir.c_str(), &buff))
        _mkdir(dir.c_str());
}


void CheckAllDir()
{
    CheckDir(lpLogDir);
    CheckDir(lpLogPdf);
    std::string SERVER = "SERVER11";
    char buffer[256];
    DWORD size = 256;
    GetComputerName(buffer, &size);
    Server = buffer;
    if(SERVER == Server)
    {
        MyServer = true;
        //std::Pathc patch = getDirContents (lpLogDir);
        //for(auto p : patch)
        //{
        //    if(p.string().length())
        //        remove(p.string().c_str());
        //}
    }
}

void GetVersionProg()
{
    char* pLockedResource = NULL;
    DWORD dwResourceSize = 0;
    GetRessusce(IDR_TXT1, "TXT", (LPVOID*)&pLockedResource, &dwResourceSize);
    std::string rc = pLockedResource;
    boost::replace_all(rc, "\r\n", " ");
    boost::replace_all(rc, "  ", " ");
    boost::replace_last(rc, " ", "");
    szTitle += rc;
}

std::string Formats(float f, int n)
{
    std::stringstream s;
    //s << std::setw(4) << std::setfill('0') << f;
    s << 
        std::setprecision(n) << std::fixed << f;
    //s << boost::format("%.1f") % f;
    std::string d = s.str();
    boost::replace_all(d, ".", ",");
    return d;
}



void WaitCloseTheread(HANDLE h, std::string hamd)
{
    DWORD dwEvent = WaitForSingleObject(h, INFINITE);
    if(dwEvent == WAIT_OBJECT_0)
    {
        LOG_WARN(AllLogger, std::string("WaitForSingleObject( " + hamd + " ) = WAIT_OBJECT_0 "));
    }
    else
    {
        if(dwEvent == WAIT_ABANDONED)
        {
            LOG_WARN(AllLogger, std::string("WaitForSingleObject( " + hamd + " ) = WAIT_ABANDONED "));
        }
        else if(dwEvent == WAIT_TIMEOUT)
        {
            LOG_WARN(AllLogger, std::string("WaitForSingleObject( " + hamd + " ) = WAIT_TIMEOUT "));
        }
        else if(dwEvent == WAIT_FAILED)
        {
            LOG_WARN(AllLogger, std::string("WaitForSingleObject( " + hamd + " ) = WAIT_FAILED "));
        }
        else
        {
            LOG_WARN(AllLogger, std::string("WaitForSingleObject( " + hamd + " ) = ") + std::to_string(dwEvent) + " : ");
        }

        //TerminateProcess(h, 0);
    }
}

void TestTimeRun(ULONGLONG& time)
{
    double t = 1000.0 - ((double)(GetTickCount64() - time) / 1000.0);
    if(t <= 0) t = 1000.0;
    Sleep((DWORD)t);
    time = GetTickCount64();
}

void f(std::string _s)
{
    std::stringstream streams;
    streams << std::setw(30) << FUNCTION_LINE_NAME << _s;
    //streams.str().c_str();
}


void Start()
{
    Open_Event();
    Open_KPVL();
    Open_FURN();
    //OpenClientUDP();
}

void Stop()
{
    StopGraff();
    Close_FURN();
    Close_KPVL();
    Close_Event();
}

DWORD EnumProcess();
void SavePid();
void ClosePid();

//Запуск главного окна
int Run()
{
    std::string dd;
    DWORD pid = EnumProcess();
    if(pid)
    {
        WinErrorExit(NULL, (std::string("Программа уже запущена. Pid = ") + std::to_string(pid)).c_str());
        return 0;
    }
    SavePid();

    LOG_INFO(AllLogger, "{:90}|", FUNCTION_LINE_NAME);

    //CheckDir(lpSheetDir);
    //CheckDir(lpCassetteDir);
    //CheckDir(lpLCassetteDir);
    //CheckDir(lpLogDir);

    CheckAllDir();

    try
    {

#if FULLRUN
        AllLogger = InitLogger("All Debug");
        if(!AllLogger.get())
        {
            WinErrorExit(NULL, "Программа уже запущена");
            return 0;
        }
#endif

        LOG_DEBUG(AllLogger, "{:90}| Старт программы", FUNCTION_LINE_NAME);


        isRun = true;
        GetVersionProg();

        if(!hInstance) throw std::exception(__FUN("Ошибка запуска программы : hInst = NULL"));
        if(!InitCommonControlsEx(&initcontrol)) throw std::exception(__FUN("Ошибка загрузки общего модуля"));

        BOOL bBool = FALSE;

        if(!SetUserObjectInformation(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &bBool, sizeof(BOOL)))
            throw std::exception(__FUN("Ошибка загрузки пользовательсих модулей"));
        

        InitFont();
        InitIcon();

        if(InitSQL())
        {

            InitWindow(hInstance);
            //OpenAllParam();

                // Цикл основного сообщения:
            MSG msg;
            //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DATACOLLACTION));

#if FULLRUN
            Start();
#endif

            while(GetMessage(&msg, nullptr, 0, 0) && isRun)
            {
                //if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            isRun = false;
            DestroyWindow(Global0);
        }
    }
    catch(std::exception& exc)
    {
        WinErrorExit(NULL, exc.what());
    }
    catch(...)
    {
        WinErrorExit(NULL, "Unknown error.");
    }
    isRun = false;

#if FULLRUN
    Stop();
#endif

    ClosePid();

    LOG_DEBUG(AllLogger, "{:90}| Стоп программы", FUNCTION_LINE_NAME);
    return 0;
}

void CurrentDir();

int Services();
int InstallService();
int RemoveService();
int StartServices();
int StoptServices();



//Глобальная функция для консоли
int main()
{
    hInstance = GetModuleHandle(0); //main //GetModuleHandle()
    HWND hwndC = GetConsoleWindow();

    CurrentDir();

    return Run();
}

#pragma region std::string InitInfoText
std::string InitInfoText = std::string("Для запуска программы используйте ключи:\r\n\r\n")
+ "     1. Инсталяция сервиса \"PLC210 OPC-UA Service\"\r\n"
+ "             HistoryKpvl64v143.exe install\r\n\r\n"
+ "     2. Запуск сервиса \"PLC210 OPC-UA Service\"\r\n"
+ "             HistoryKpvl64v143.exe start\r\n\r\n"
+ "     3. Остановка сервиса \"PLC210 OPC-UA Service\"\r\n"
+ "             HistoryKpvl64v143.exe stop\r\n\r\n"
+ "     4. Деинсталяция сервиса \"PLC210 OPC-UA Service\"\r\n"
+ "             HistoryKpvl64v143.exe remove\r\n\r\n"
+ "     5. Деинсталяция сервиса \"PLC210 OPC-UA Service\"\r\n"
+ "             HistoryKpvl64v143.exe remove\r\n\r\n"
+ "     6. Запуск программы без сервиса\r\n"
+ "             HistoryKpvl64v143.exe debug";
#pragma endregion


DLLRESULT CALLBACK InfoStartProgramm(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_INITDIALOG)
    {
        SetWindowText(GetDlgItem(hWnd, IDC_STATIC1), InitInfoText.c_str());
    }
    if(message == WM_COMMAND && (wParam == IDCANCEL || wParam == IDOK))
    {
        EndDialog(hWnd, FALSE);
    }
    return (0);
}

//Глобальная функция для оконного
int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    hInstance = hInst; // Сохранить маркер экземпляра в глобальной переменной

    int argc;
    char** argv;

    LPWSTR* pCmdLine = CommandLineToArgvW(GetCommandLineW(), &argc);
    argv = (char**)malloc(argc * sizeof(char*));

    CurrentDir();

    if(argc < 2)
    {
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, InfoStartProgramm); //IDC_STATIC1
        return -1;
    }



    WCHAR* cmdl = pCmdLine[argc - 1];
    if(_wcsicmp(cmdl, L"debug") == 0)
        return Run();
    else if(_wcsicmp(cmdl, L"service") == 0)
        return Services();
    else if(_wcsicmp(cmdl, L"install") == 0)
        return InstallService();
    else if(_wcsicmp(cmdl, L"remove") == 0)
        return RemoveService();
    else if(_wcsicmp(cmdl, L"start") == 0)
        return StartServices();
    else if(_wcsicmp(cmdl, L"stop") == 0)
        return StoptServices();
    else
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, InfoStartProgramm); 

    //return Run();
    return -1;
}