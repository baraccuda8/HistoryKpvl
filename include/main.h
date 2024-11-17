#pragma once
#include <filesystem>



//Глобальный Run
extern bool isRun;


//Глобальный Instance программы
extern HINSTANCE hInstance;
//extern bool MyServer;
//extern std::string Server;

extern std::string szTitle;
extern std::string lpLogDir;
//std::shared_ptr<spdlog::logger> InitLogger(std::string LoggerOut);

extern const std::string FORMATTIME1;
extern const std::string FORMATTIME2;


extern std::shared_ptr<spdlog::logger> AllLogger;

extern uint32_t SizeLogger;    //Размер лог файда
extern uint32_t CountLogger;        //Количество лог файлов
extern int CountWatchDogWait;

inline void outF(std::string name)
{
    std::ofstream f(name, std::ios::binary | std::ios::out | std::ios::app);
    if(f.is_open()) { f << std::endl << std::endl; f.close(); }
}

#define InitLogger(_l) {\
try{if(!_l){\
	_l = spdlog::details::registry::instance().get(#_l);\
	if(!_l.get())\
	{\
        std::string _fn = lpLogDir + "\\" + #_l + ".log";\
        outF(_fn);\
		_l = spdlog::rotating_logger_mt(#_l, _fn, SizeLogger, CountLogger);\
	}\
	_l->set_level(spdlog::level::level_enum::info);\
	LOG_INFO(_l, "{:90}| Старт программы", FUNCTION_LINE_NAME);\
}}catch(...) {}}

//inline std::shared_ptr<spdlog::logger> InitLogger(std::string LoggerOut)\
//{\
//    try\
//    {\
//        std::shared_ptr<spdlog::logger> Logger = spdlog::details::registry::instance().get(LoggerOut);\
//        if(!Logger.get())\
//        {\
//            std::string FileName = lpLogDir + "\\" + LoggerOut + ".log";\
//            std::ofstream l(FileName, std::ios::binary | std::ios::out | std::ios::app);\
//            if(l.is_open())\
//            {\
//                l << std::endl << std::endl;\
//                l.close();\
//            }\
//            Logger = spdlog::rotating_logger_mt(LoggerOut, FileName, SizeLogger, CountLogger);\
//        }\
//        Logger->set_level(spdlog::level::level_enum::info);\
//        LOG_INFO(Logger, "{:90}| Старт программы", FUNCTION_LINE_NAME);\
//        return Logger;\
//    }\
//    catch(...) {}\
//    return 0;\
//}

#ifdef _DEBUG
//#define TESTTEMPER
#define TESTGRAFF
//#define TESTSPIS
//#define TESTWIN
#endif

LRESULT Quit();

#define CATCH(_l, _s) \
    catch(std::invalid_argument& exc) { LOG_ERROR(_l, "{:89}| {} invalid_argument Error {}", FUNCTION_LINE_NAME, std::string(_s),  exc.what())} \
    catch(std::filesystem::filesystem_error& exc) { LOG_ERROR(_l, "{:89}| {} filesystem_error Error {}", FUNCTION_LINE_NAME, std::string(_s),  exc.what())} \
    catch(std::runtime_error& exc){LOG_ERROR(_l, "{:89}| {} runtime_error Error {}", FUNCTION_LINE_NAME, std::string(_s), exc.what())} \
    catch(std::exception& exc){LOG_ERROR(_l, "{:89}| {} exception Error {}", FUNCTION_LINE_NAME, std::string(_s), exc.what())} \
    catch(...){LOG_ERROR(_l, "{:89}| {} ... Error {}", FUNCTION_LINE_NAME, std::string(_s), "Unknown error")}

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

void TestTimeRun(uint64_t& time);


void CheckDir(std::string dir);




std::string GetLastErrorString();



void WaitCloseTheread(HANDLE h, std::string hamd);
