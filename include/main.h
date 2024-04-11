#pragma once


//Глобальный Run
extern bool isRun;

//Глобальный Instance программы
extern HINSTANCE hInstance;
extern bool MyServer;
extern std::string Server;

extern std::string szTitle;
extern std::string lpLogDir;
std::shared_ptr<spdlog::logger> InitLogger(std::string LoggerOut);


extern std::shared_ptr<spdlog::logger> AllLogger;

#ifdef _DEBUG
//#define TESTTEMPER
#define TESTGRAFF
//#define TESTSPIS
//#define TESTWIN
#endif

#define CATCH(_l, _s) catch(std::runtime_error& exc)LOG_ERROR(_l, _s + exc.what()) catch(...)LOG_ERROR(_l, _s + "Unknown error")


#define TIME_OUT 91000

extern uint32_t SizeLogger;
extern uint32_t CountLogger;
extern int CountWatchDogWait;

#define atoi_t(_t, _f, _s) static_cast<_t>(_f(_s.c_str()))


//namespace PlcType{
//    enum Type{
//        NOT = 0,
//        KPVL = 1,
//        S107 = 2,
//    };
//};


//typedef struct Plc{
//    std::string URI = "";
//    uint32_t SizeLogger = 1048576;
//    uint32_t CountLogger = 1000;
//    int CountWatchDogWait  = TIME_OUT / 1000;
//    int CountLastSheet = 500;
//    //spdlog::level::level_enum LogType = spdlog::level::level_enum::info;
//    std::string Comment = "";
//}Pls;


//extern std::map<PlcType::Type, Plc> PLC;
//
//#define KPVLTYPE PLC[PlcType::Type::KPVL]
//#define S107TYPE PLC[PlcType::Type::S107]

#define winmap(_s) mapWindow[_s].hWnd

//extern std::map<HWNDCLIENT, structWindow>mapWindow;


////Черная заливка
//extern HBRUSH TitleBrush0;
//
////Белая заливка
//extern HBRUSH TitleBrush1;
//
////темносиняя заливка
//extern HBRUSH TitleBrush2;
//
////Светлосиняя заливка
//extern HBRUSH TitleBrush3;
//
////Зеленая заливка
//extern HBRUSH TitleBrush4;
//extern std::map<int, HICON>Icon;
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

//Вывод строки ошибки выполнения программы
int WinErrorExit(HWND hWnd, const char* lpszFunction);

void TestTimeRun(ULONGLONG& time);

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
BOOL DataTimeOfString(std::string str, std::string format, int& d1, int& d2, int& d3, int& d4, int& d5, int& d6);
BOOL DataTimeOfString(std::string str, std::string format, std::tm& TM);

std::string GetStringData(std::string d);
