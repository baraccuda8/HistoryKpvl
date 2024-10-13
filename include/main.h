#pragma once
#include <filesystem>


//Глобальный Run
extern bool isRun;


//Глобальный Instance программы
extern HINSTANCE hInstance;
extern bool MyServer;
extern std::string Server;

extern std::string szTitle;
extern std::string lpLogDir;
std::shared_ptr<spdlog::logger> InitLogger(std::string LoggerOut);

extern const std::string FORMATTIME1;
extern const std::string FORMATTIME2;


extern std::shared_ptr<spdlog::logger> AllLogger;

#ifdef _DEBUG
//#define TESTTEMPER
#define TESTGRAFF
//#define TESTSPIS
//#define TESTWIN
#endif

LRESULT Quit();

#define CATCH(_l, _s) \
    catch(std::filesystem::filesystem_error& exc) { LOG_ERROR(_l, "{:89}| {} Error {}", FUNCTION_LINE_NAME, std::string(_s),  exc.what())} \
    catch(std::runtime_error& exc){LOG_ERROR(_l, "{:89}| {} Error {}", FUNCTION_LINE_NAME, std::string(_s), exc.what())} \
    catch(std::exception& exc){LOG_ERROR(_l, "{:89}| {} Error {}", FUNCTION_LINE_NAME, std::string(_s), exc.what())} \
    catch(...){LOG_ERROR(_l, "{:89}| Error {} {}", FUNCTION_LINE_NAME, std::string(_s), "Unknown error")}

#define CATCH_RUN(_l) \
    catch(std::runtime_error& exc){LOG_ERROR(_l, "{:89}| Error {} countconnect = {}.{}", FUNCTION_LINE_NAME, exc.what(), countconnect1, countconnect2);\
    }catch(...){LOG_ERROR(_l, "{:89}| Unknown error countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);};

#define CATCH_OPEN(_l, _u) \
catch(std::runtime_error& exc)\
{\
    LOG_ERROR(_l, "{:89}| Error {} countconnect = {} to: {}", FUNCTION_LINE_NAME, exc.what(), countconnect, _u);\
}\
catch(...)\
{\
    LOG_ERROR(_l, "{:89}| Unknown error countconnect = {} to: {}", FUNCTION_LINE_NAME, countconnect, _u);\
};


#define LOG_ERR_SQL(_l, _r, _c){\
LOG_ERROR(_l, "{:89}| {}", FUNCTION_LINE_NAME, _c);\
LOG_ERROR(_l, "{:89}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(_r)));\
}\




#define TIME_OUT 91000

extern uint32_t SizeLogger;
extern uint32_t CountLogger;
extern int CountWatchDogWait;

#define atoi_t(_t, _f, _s) static_cast<_t>(_f(_s.c_str()))

#define winmap(_s) mapWindow[_s].hWnd


inline int Stoi(std::string input)
{
    std::optional<int> out = 0;
    try
    {
        return std::stoi(input);
    }
    catch(...) { }
    return 0;
}

 inline float Stof(std::string input)
{
    try
    {
        return std::stof(input);
    }
    catch(...) { }
    return 0.0f;
}

 inline double Stod(std::string input)
 {
     try
     {
         return std::stod(input);
     }
     catch(...) { }
     return 0.0;
 }

 typedef struct MYHICON{
    HICON icon;
    int size;
};
extern std::map<int, MYHICON>Icon;
//extern std::map<HICON, int>IconSize;


//Перечисление фонтов от 6 до 16
enum emFont {
    FontNull = -1,
    Font06 = 6,
    Font07 = 7,
    Font08 = 8,
    Font09 = 9,
    Font10 = 10,
    Font11 = 11,
    Font12 = 12,
    Font13 = 13,
    Font14 = 14,
    Font15 = 15,
    Font16 = 16,
};

//Масив фонтов от 6 до 16 размера жирный шрифт Arial
extern std::map<emFont, HFONT> Font;

void DisplayContextMenu(HWND hwnd, int ID);
//Вывод строки ошибки выполнения программы
int WinErrorExit(HWND hWnd, const char* lpszFunction);

void TestTimeRun(ULONGLONG& time);


void CheckDir(std::string dir);

inline std::string string_time(struct tm const* TM)
{
    std::string str(50, '\0');
    sprintf_s(&str[0], 50, "%04d-%02d-%02d %02d:%02d:%02d ", TM->tm_year + 1900, TM->tm_mon + 1, TM->tm_mday, TM->tm_hour, TM->tm_min, TM->tm_sec);
    boost::replace_all(str, "\n", " ");
    return str;
}


inline time_t ToTimeT(OpcUa::DateTime dateTime)
{
    const int64_t daysBetween1601And1970 = 134774;
    const int64_t secsFrom1601To1970 = daysBetween1601And1970 * 24 * 3600LL;

    if(dateTime.Value < secsFrom1601To1970)
    {
        std::stringstream stream;
        stream << "Дата и время не могут быть меньше " << secsFrom1601To1970;
        throw std::invalid_argument(stream.str());
    }

    const int64_t secsFrom1970 = dateTime.Value / 10000000LL - secsFrom1601To1970;
    return secsFrom1970;
}

std::string GetLastErrorString();

std::string ToString(const OpcUa::DateTime& t);
std::string GetDataTimeString();
std::string GetDataTimeString(std::time_t& st);
std::string GetDataTimeString(std::time_t* st);
std::string GetDataTimeString(std::tm& st);
time_t DataTimeDiff(std::string str1, std::string str2);


time_t DataTimeOfString(std::string str, int& d1, int& d2, int& d3, int& d4, int& d5, int& d6);
time_t DataTimeOfString(std::string str, std::tm& TM);
time_t DataTimeOfString(std::string str);

std::string GetStringData(std::string d);
std::string Formats(float f, int n = 1);
std::wstring GetData(std::wstring str);

std::string GetDataTimeStr(std::string str);
std::string GetDataTimeStr(std::string str, std::string& outDate, std::string& outTime);

void WaitCloseTheread(HANDLE h, std::string hamd);
