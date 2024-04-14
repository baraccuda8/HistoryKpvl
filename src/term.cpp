#include "pch.h"

#include "win.h"
#include "main.h"
#include "file.h"
#include "SQL.h"
#include "ValueTag.h"
#include "term.h"
#include "hard.h"
#include "Graff.h"
#include "Furn.h"


#define SPKsec1 sec01000
#define SPKsec5 sec05000

extern std::string lpLogDir;

std::shared_ptr<spdlog::logger> PethLogger = NULL;

extern std::string  FORMATTIME;

bool isInitPLC_S107 = false;

time_t PLC_S107_old_dt = 0;


std::thread hS107URI;
std::thread hS107SQL;

std::deque<TCassette> AllCassette;

T_ForBase_RelFurn ForBase_RelFurn_1;
T_ForBase_RelFurn ForBase_RelFurn_2;

struct T_cassetteArray{
    //struct _data{
        T_cassette cassette[7];
        Value* selected_cassetFurn1;
        Value* selected_cassetFurn2;
    //}data;
}cassetteArray;

//SPK107 (M01).Application.AppFurn1.PointTime_1

#define SETALLTAG2(_t, _f, _e, _s,  _d) SETALLTAG(PathPeth, _t, _f, _e, _s,  _d)

//template<>
std::deque<Value*> AllTagPeth = {

    //Первая печь
    {AppFurn1.WDG_toBase            = new Value(StrFurn1 + "WDG_toBase",            HWNDCLIENT::hEditTimeServer_1,          S107::Furn1::Data_WDG_toBase, &conn_temp, MSSEC::sec00500)}, //вачдог
    {AppFurn1.WDG_fromBase          = new Value(StrFurn1 + "WDG_fromBase",          HWNDCLIENT::hNull,                      0, &conn_temp, MSSEC::sec00500)}, //вачдог для обратной связи 

    {AppFurn1.PointTime_1           = new Value(StrFurn1 + "PointTime_1",           HWNDCLIENT::RelF1_Edit_PointTime_1,     0, &conn_temp)}, //: REAL; //Время разгона
    {AppFurn1.PointRef_1            = new Value(StrFurn1 + "PointRef_1",            HWNDCLIENT::RelF1_Edit_PointRef_1,      0, &conn_temp)}, //: REAL;  //Уставка температуры
    {AppFurn1.PointDTime_2          = new Value(StrFurn1 + "PointDTime_2",          HWNDCLIENT::RelF1_Edit_PointDTime_2,    0, &conn_temp)}, //: REAL;//Время выдержки

    {AppFurn1.ProcRun               = new Value(StrFurn1 + "ProcRun",               HWNDCLIENT::RelF1_Edit_ProcRun,         S107::Furn1::ProcRun, &conn_temp)}, //: BOOL;//Работа
    {AppFurn1.ProcEnd               = new Value(StrFurn1 + "ProcEnd",               HWNDCLIENT::RelF1_Edit_ProcEnd,         S107::Furn1::ProcEnd, &conn_temp)}, //: BOOL;//Окончание процесса
    {AppFurn1.ProcFault             = new Value(StrFurn1 + "ProcFault",             HWNDCLIENT::RelF1_Edit_ProcFault,       S107::Furn1::ProcError, &conn_temp)}, //: BOOL;//Авария процесса

    {AppFurn1.TimeProcSet           = new Value(StrFurn1 + "TimeProcSet",           HWNDCLIENT::RelF1_Edit_TimeProcSet,     0, &conn_temp)}, //: REAL;//Полное время процесса (уставка), мин
    {AppFurn1.ProcTimeMin           = new Value(StrFurn1 + "ProcTimeMin",           HWNDCLIENT::RelF1_Edit_ProcTimeMin,     0, &conn_temp)}, //: REAL;//Время процесса, час (0..XX)
    {AppFurn1.TimeToProcEnd         = new Value(StrFurn1 + "TimeToProcEnd",         HWNDCLIENT::RelF1_Edit_TimeToProcEnd,   0, &conn_temp)}, //: REAL;//Время до окончания процесса, мин
    {AppFurn1.TempRef               = new Value(StrFurn1 + "TempRef",               HWNDCLIENT::RelF1_Edit_TempRef,         0, &conn_temp)}, //: REAL;//Заданное значение температуры
    {AppFurn1.TempAct               = new Value(StrFurn1 + "TempAct",               HWNDCLIENT::RelF1_Edit_TempAct,         0, &conn_temp)}, //: REAL;//Фактическое значение температуры
    {AppFurn1.T1                    = new Value(StrFurn1 + "T1",                    HWNDCLIENT::RelF1_Edit_T1,              0, &conn_temp)}, //: REAL; // термопара 1
    {AppFurn1.T2                    = new Value(StrFurn1 + "T2",                    HWNDCLIENT::RelF1_Edit_T2,              0, &conn_temp)}, //: REAL; // термопара 2

    {AppFurn1.Cassette.Day          = new Value(StrFurn1 + "Cassette.Day",          HWNDCLIENT::RelF1_Edit_Cassette_Day,    S107::Furn1::SetNull_Temper, &conn_temp)}, //ID касеты день
    {AppFurn1.Cassette.Month        = new Value(StrFurn1 + "Cassette.Month",        HWNDCLIENT::RelF1_Edit_Cassette_Month,  S107::Furn1::SetNull_Temper, &conn_temp)}, //ID касеты месяц
    {AppFurn1.Cassette.Year         = new Value(StrFurn1 + "Cassette.Year",         HWNDCLIENT::RelF1_Edit_Cassette_Year,   S107::Furn1::SetNull_Temper, &conn_temp)}, //ID касеты год
    {AppFurn1.Cassette.CassetteNo   = new Value(StrFurn1 + "Cassette.CaasetteNo",   HWNDCLIENT::RelF1_Edit_CassetteNo,      S107::Furn1::SetNull_Temper, &conn_temp)}, //ID касеты номер



    //Вторая печь
    {AppFurn2.WDG_toBase            = new Value(StrFurn2 + "WDG_toBase",            HWNDCLIENT::hEditTimeServer_2,           S107::Furn2::Data_WDG_toBase, &conn_temp, MSSEC::sec00500)}, //вачдог
    {AppFurn2.WDG_fromBase          = new Value(StrFurn2 + "WDG_fromBase",          HWNDCLIENT::hNull,                      0, &conn_temp, MSSEC::sec00500)}, //вачдог для обратной связи 

    {AppFurn2.PointTime_1           = new Value(StrFurn2 + "PointTime_1",           HWNDCLIENT::RelF2_Edit_PointTime_1,     0, &conn_temp)}, //: REAL;//Время разгона
    {AppFurn2.PointRef_1            = new Value(StrFurn2 + "PointRef_1",            HWNDCLIENT::RelF2_Edit_PointRef_1,      0, &conn_temp)}, //: REAL;//Уставка температуры
    {AppFurn2.PointDTime_2          = new Value(StrFurn2 + "PointDTime_2",          HWNDCLIENT::RelF2_Edit_PointDTime_2,    0, &conn_temp)}, //: REAL;//Время выдержки

    {AppFurn2.ProcRun               = new Value(StrFurn2 + "ProcRun",               HWNDCLIENT::RelF2_Edit_ProcRun,         S107::Furn2::ProcRun, &conn_temp)}, //: BOOL;//Работа
    {AppFurn2.ProcEnd               = new Value(StrFurn2 + "ProcEnd",               HWNDCLIENT::RelF2_Edit_ProcEnd,         S107::Furn2::ProcEnd, &conn_temp)}, //: BOOL;//Окончание процесса
    {AppFurn2.ProcFault             = new Value(StrFurn2 + "ProcFault",             HWNDCLIENT::RelF2_Edit_ProcFault,       S107::Furn2::ProcError, &conn_temp)}, //: BOOL;//Авария процесса

    {AppFurn2.TimeProcSet           = new Value(StrFurn2 + "TimeProcSet",           HWNDCLIENT::RelF2_Edit_TimeProcSet,     0, &conn_temp)}, //: REAL;//Полное время процесса (уставка), мин
    {AppFurn2.ProcTimeMin           = new Value(StrFurn2 + "ProcTimeMin",           HWNDCLIENT::RelF2_Edit_ProcTimeMin,     0, &conn_temp)}, //: REAL;//Время процесса, час (0..XX)
    {AppFurn2.TimeToProcEnd         = new Value(StrFurn2 + "TimeToProcEnd",         HWNDCLIENT::RelF2_Edit_TimeToProcEnd,   0, &conn_temp)}, //: REAL;//Время до окончания процесса, мин
    {AppFurn2.TempRef               = new Value(StrFurn2 + "TempRef",               HWNDCLIENT::RelF2_Edit_TempRef,         0, &conn_temp)}, //: REAL;//Заданное значение температуры
    {AppFurn2.TempAct               = new Value(StrFurn2 + "TempAct",               HWNDCLIENT::RelF2_Edit_TempAct,         0, &conn_temp)}, //: REAL;//Фактическое значение температуры
    {AppFurn2.T1                    = new Value(StrFurn2 + "T1",                    HWNDCLIENT::RelF2_Edit_T1,              0, &conn_temp)}, //: REAL; // термопара 1
    {AppFurn2.T2                    = new Value(StrFurn2 + "T2",                    HWNDCLIENT::RelF2_Edit_T2,              0, &conn_temp)}, //: REAL; // термопара 2

    {AppFurn2.Cassette.Day          = new Value(StrFurn2 + "Cassette.Day",          HWNDCLIENT::RelF2_Edit_Cassette_Day,    S107::Furn2::SetNull_Temper, &conn_temp)}, ///ID касеты день
    {AppFurn2.Cassette.Month        = new Value(StrFurn2 + "Cassette.Month",        HWNDCLIENT::RelF2_Edit_Cassette_Month,  S107::Furn2::SetNull_Temper, &conn_temp)}, //ID касеты месяц
    {AppFurn2.Cassette.Year         = new Value(StrFurn2 + "Cassette.Year",         HWNDCLIENT::RelF2_Edit_Cassette_Year,   S107::Furn2::SetNull_Temper, &conn_temp)},  //ID касеты год
    {AppFurn2.Cassette.CassetteNo   = new Value(StrFurn2 + "Cassette.CaasetteNo",   HWNDCLIENT::RelF2_Edit_CassetteNo,      S107::Furn2::SetNull_Temper, &conn_temp)}, //ID касеты номер

    //Список касет
    {AppCassette1.Year         = new Value(StrCassette1 + "Year",       HWNDCLIENT::hEditState1_Year,     0, &conn_temp)}, //ID касеты год
    {AppCassette1.Month        = new Value(StrCassette1 + "Month",      HWNDCLIENT::hEditState1_Month,    0, &conn_temp)}, //ID касеты месяц
    {AppCassette1.Day          = new Value(StrCassette1 + "Day",        HWNDCLIENT::hEditState1_Day,      0, &conn_temp)}, //ID касеты день
    {AppCassette1.CassetteNo   = new Value(StrCassette1 + "CaasetteNo", HWNDCLIENT::hEditState1_Cassette, 0, &conn_temp)}, //ID касеты номер

    {AppCassette2.Year         = new Value(StrCassette2 + "Year",       HWNDCLIENT::hEditState2_Year,     0, &conn_temp)}, //ID касеты год
    {AppCassette2.Month        = new Value(StrCassette2 + "Month",      HWNDCLIENT::hEditState2_Month,    0, &conn_temp)}, //ID касеты месяц
    {AppCassette2.Day          = new Value(StrCassette2 + "Day",        HWNDCLIENT::hEditState2_Day,      0, &conn_temp)}, //ID касеты день
    {AppCassette2.CassetteNo   = new Value(StrCassette2 + "CaasetteNo", HWNDCLIENT::hEditState2_Cassette, 0, &conn_temp)}, //ID касеты номер

    {AppCassette3.Year         = new Value(StrCassette3 + "Year",       HWNDCLIENT::hEditState3_Year,     0, &conn_temp)}, //ID касеты год
    {AppCassette3.Month        = new Value(StrCassette3 + "Month",      HWNDCLIENT::hEditState3_Month,    0, &conn_temp)}, //ID касеты месяц
    {AppCassette3.Day          = new Value(StrCassette3 + "Day",        HWNDCLIENT::hEditState3_Day,      0, &conn_temp)}, //ID касеты день
    {AppCassette3.CassetteNo   = new Value(StrCassette3 + "CaasetteNo", HWNDCLIENT::hEditState3_Cassette, 0, &conn_temp)}, //ID касеты номер

    {AppCassette4.Year         = new Value(StrCassette4 + "Year",       HWNDCLIENT::hEditState4_Year,     0, &conn_temp)}, //ID касеты год
    {AppCassette4.Month        = new Value(StrCassette4 + "Month",      HWNDCLIENT::hEditState4_Month,    0, &conn_temp)}, //ID касеты месяц
    {AppCassette4.Day          = new Value(StrCassette4 + "Day",        HWNDCLIENT::hEditState4_Day,      0, &conn_temp)}, //ID касеты день
    {AppCassette4.CassetteNo   = new Value(StrCassette4 + "CaasetteNo", HWNDCLIENT::hEditState4_Cassette, 0, &conn_temp)}, //ID касеты номер

    {AppCassette5.Year         = new Value(StrCassette5 + "Year",       HWNDCLIENT::hEditState5_Year,     0, &conn_temp)}, //ID касеты год
    {AppCassette5.Month        = new Value(StrCassette5 + "Month",      HWNDCLIENT::hEditState5_Month,    0, &conn_temp)}, //ID касеты месяц
    {AppCassette5.Day          = new Value(StrCassette5 + "Day",        HWNDCLIENT::hEditState5_Day,      0, &conn_temp)}, //ID касеты день
    {AppCassette5.CassetteNo   = new Value(StrCassette5 + "CaasetteNo", HWNDCLIENT::hEditState5_Cassette, 0, &conn_temp)}, //ID касеты номер

    {AppCassette6.Year         = new Value(StrCassette6 + "Year",       HWNDCLIENT::hEditState6_Year,     0, &conn_temp)}, //ID касеты год
    {AppCassette6.Month        = new Value(StrCassette6 + "Month",      HWNDCLIENT::hEditState6_Month,    0, &conn_temp)}, //ID касеты месяц
    {AppCassette6.Day          = new Value(StrCassette6 + "Day",        HWNDCLIENT::hEditState6_Day,      0, &conn_temp)}, //ID касеты день
    {AppCassette6.CassetteNo   = new Value(StrCassette6 + "CaasetteNo", HWNDCLIENT::hEditState6_Cassette, 0, &conn_temp)}, //ID касеты номер

    {AppCassette7.Year         = new Value(StrCassette7 + "Year",       HWNDCLIENT::hEditState7_Year,     0, &conn_temp)}, //ID касеты год
    {AppCassette7.Month        = new Value(StrCassette7 + "Month",      HWNDCLIENT::hEditState7_Month,    0, &conn_temp)}, //ID касеты месяц
    {AppCassette7.Day          = new Value(StrCassette7 + "Day",        HWNDCLIENT::hEditState7_Day,      0, &conn_temp)}, //ID касеты день
    {AppCassette7.CassetteNo   = new Value(StrCassette7 + "CaasetteNo", HWNDCLIENT::hEditState7_Cassette, 0, &conn_temp)}, //ID касеты номер

    {AppSelected1 = new Value(StrSelected1,  hEditState_selected_casset1, 0, &conn_temp)}, //Выбор кассеты в 1-й печи
    {AppSelected2 = new Value(StrSelected2,  hEditState_selected_casset2, 0, &conn_temp)}, //Выбор кассеты в 2-й печи
};


bool cmpAllTagPeth(Value* first, Value* second)
{
    return first->Patch < second->Patch;
}


std::map<MSSEC, ChannelSubscription>cssS107;

ClassDataChangeS107 DataChangeS107;

void ClassDataChangeS107::DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr)
{
    std::string patch = "";
    if(attr == OpcUa::AttributeId::Value)
    {
        try
        {
            if(WatchDogWait)
            {
                SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
            }

            OpcUa::NodeId id = node.GetId();
            if(id.IsInteger())
            {
                if((uint32_t)OpcUa::ObjectId::Server_ServerStatus_CurrentTime == id.GetIntegerIdentifier())
                {
                    WatchDog = TRUE; //Бит жизни
                    S107::ServerDataTime = val.ToString();
                    SetWindowText(winmap(hEditTimeServer_2), S107::ServerDataTime.c_str());
                }
            }
            else if(id.IsString())
            {
                SetWindowText(winmap(hEditMode2), "Пришли данные...");
                patch = id.GetStringIdentifier();
                OpcUa::Variant vals = val;
                for(auto& a : AllTagPeth)
                {
                    if(patch == a->Patch)
                    {
                        if(!a->Node.IsValid())
                        {
                            a->Node = node;
                            a->handle = handle;
                            a->attr = attr;
                        }
                        a->Find(handle, vals);
                        SetWindowText(winmap(hEditMode2), "Жду данные...");
                        return;
                    }
                }
                SetWindowText(winmap(hEditMode2), "Жду данные...");
            }
        }
        catch(std::runtime_error& exc)
        {
            SetWindowText(winmap(hEditMode1), "DataChange runtime_error");
            LOG_ERROR(PethLogger, "{:90| DataChange Error {}, {}", FUNCTION_LINE_NAME, exc.what(), node.ToString());
        }
        catch(...)
        {
            SetWindowText(winmap(hEditMode1), "Unknown error");
            LOG_ERROR(PethLogger, "{:90| DataChange Error 'Unknown error' {}", FUNCTION_LINE_NAME, node.ToString());
        };

    }
}



void PLC_S107::InitNodeId()
{
    LOG_INFO(Logger, "{:90}| Инициализация узлов... countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    LOG_INFO(Logger, "{:90}| Проверка Patch = Server_ServerStatus_CurrentTime", FUNCTION_LINE_NAME);

    nodeCurrentTime = GetNode(OpcUa::ObjectId::Server_ServerStatus_CurrentTime); //Node текущее время

    if(nodeCurrentTime.IsValid())
    {
        OpcUa::Variant val = nodeCurrentTime.GetValue();
        S107::ServerDataTime = val.ToString();
        SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
        //SetWindowText(winmap(hEditTime_2), (S107::ServerDataTime + " (" + std::to_string(dataCangeS107.WatchDogWait) + ")").c_str());
    }

    LOG_INFO(Logger, "{:90}| Инициализация NodeId", FUNCTION_LINE_NAME);
    for(auto& a : AllTagPeth)
    {
        a->InitNodeId(this);
    }
}

void PLC_S107::InitTag()
{
    LOG_INFO(Logger, "{:90}| Инициализация переменных... countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    try
    {
        //Создание Subscribe
        LOG_INFO(Logger, "{:90}| Создание Subscribe countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);

        CREATESUBSCRIPT(cssS107, sec00500, &DataChangeS107, Logger);
        CREATESUBSCRIPT(cssS107, SPKsec1, &DataChangeS107, Logger);
        CREATESUBSCRIPT(cssS107, SPKsec5, &DataChangeS107, Logger);

        cssS107[sec00500].Subscribe(nodeCurrentTime);
    }
    catch(std::runtime_error& exc)
    {
        LOG_ERROR(Logger, "{:90}| ", FUNCTION_LINE_NAME, exc.what());
        throw std::runtime_error(exc);
    }
    catch(...)
    {
        LOG_ERROR(Logger, "{:90}| Unknown error", FUNCTION_LINE_NAME);
        throw;
    }


    std::map< MSSEC, std::vector<OpcUa::ReadValueId>> avid;
    for(auto& a : AllTagPeth)
    {
        //if(a->TestNode(this))
        if(a->Sec)
            avid[a->Sec].push_back({a->NodeId, OpcUa::AttributeId::Value});
        else 
        {
            int t = 0;
        }
        //else
        //    LOG_WARN(Logger, "{:90}| Error tag {}", FUNCTION_LINE_NAME, a->Patch);
    }


    for(auto& ar : avid)
    {
        try
        {
            LOG_INFO(Logger, "{:90}| SubscribeDataChange msec: {}, count: {}", FUNCTION_LINE_NAME, cssS107[ar.first].msec, ar.second.size());
            std::vector<uint32_t> monitoredItemsIds = cssS107[ar.first].sub->SubscribeDataChange(ar.second);
        }
        catch(std::runtime_error& exc)
        {
            LOG_ERROR(Logger, exc.what());
            throw std::runtime_error(exc);
        }
        catch(...)
        {
            LOG_ERROR(Logger, "{:90}| Unknown error", FUNCTION_LINE_NAME);
            throw;
        }
    }
}

void PLC_S107::Run(int count)
{
    countconnect1 = count;
    countconnect2 = 0;
    DataChangeS107.WatchDogWait = 0;

    LOG_INFO(Logger, "{:90}| Run... : countconnect = {}.{} to: {}", FUNCTION_LINE_NAME, countconnect1, countconnect2, Uri);

    SetWindowText(winmap(hEditTime_1), std::to_string(DataChangeS107.WatchDogWait).c_str());

    try
    {
        DataChangeS107.InitGoot = FALSE;
        countget = 1;

        client = std::shared_ptr<OpcUa::UaClient>(new OpcUa::UaClient(Logger));

        SetWindowText(winmap(hEditMode2), "Connect");
        w1 = hEditTime_3;
        Connect();
            

        SetWindowText(winmap(hEditMode2), "GetRoot");
        GetRoot();

        SetWindowText(winmap(hEditMode2), "GetNameSpace");
        GetNameSpace();


        SetWindowText(winmap(hEditMode2), "InitNodeId");
        InitNodeId();

        SetWindowText(winmap(hEditMode2), "InitTag");
        InitTag();

        LOG_INFO(Logger, "{:90}| Подключение успешно countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);

        DataChangeS107.InitGoot = TRUE;
        ULONGLONG time1 = GetTickCount64();

        isInitPLC_S107 = true;
        SekRun = time(NULL);
        SetWindowText(winmap(hEditMode2), "Чтение данных");
        while(isRun)
        {
            //Проверяем WatchDog
            if(WD())
                throw std::runtime_error(std::string(std::string("Перезапуск: Бита жизни нет больше ") + std::to_string(CountWatchDogWait) + " секунд").c_str());

            TestTimeRun(time1);

            GetWD();
        }

        SetWindowText(winmap(hEditMode2), "delete Sub");
        for(auto s : cssS107)s.second.Delete();

#if NEWS
        SetWindowText(winmap(hEditMode2), "reset");
        client.reset();
#else
        delete client;
#endif
        return;
    }
    CATCH_RUN(Logger);

    LOG_INFO(Logger, "{:90}| Выход из опроса 1 countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    if(isRun)
    {
        LOG_INFO(Logger, "{:90}| Ждем 5 секунд... для {}", FUNCTION_LINE_NAME, Uri);
        Sleep(5000);
    }

    try
    {
        LOG_INFO(Logger, "{:90}| delete Sub countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
        SetWindowText(winmap(hEditMode2), "delete Sub");
        for(auto s : cssS107)s.second.Delete();

        LOG_INFO(Logger, "{:90}| reset countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
        SetWindowText(winmap(hEditMode2), "reset");
        client.reset();
    }
    CATCH_RUN(Logger);

    LOG_INFO(PethLogger, "{:90}| ... Вышли countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
};

void PLC_S107::GetWD()
{
    time_t old = time(NULL);
    time_t dt = (time_t)difftime(old, SekRun);
    struct tm TM;
    errno_t err = gmtime_s(&TM, &dt);

    if(!err)
    {
        char sFormat[50];
        sprintf_s(sFormat, 50, "%04d-%02d-%02d %02d:%02d:%02d", TM.tm_year-70, TM.tm_mon, TM.tm_mday - 1, TM.tm_hour, TM.tm_min, TM.tm_sec);
        SetWindowText(winmap(hEditTime_4), sFormat);
    }


    //if(PLC_KPVL_old_dt != 0 && PLC_KPVL_old_dt - time(NULL) > 5)
    //{
    //    try
    //    {
    //        //bool WDG_toBase = AllTagKpvl[std::string("HMISheetData.Sheet.WDG_toBase")].Val.As<bool>();
    //        for(auto& a : AllTagKpvl)
    //        {
    //            if("|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.WDG_toBase" == a.second.Patch)
    //            {
    //            //HMISheetData.Sheet.WDG_toBase.GetValue();
    //            //if(WDG_toBase)
    //                PLC_KPVL_old_dt = time(NULL);
    //                //if(!MyServer)
    //                {
    //                    HARD_LOGGER("HMISheetData.Sheet.WDG_fromBase.Set_Value(true)");
    //                    a.second.
    //                    //OpcUa::Variant var = true;
    //                    //AllTagKpvl["HMISheetData.Sheet.WDG_toBase"].Set_Value(true);
    //                    //HMISheetData.Sheet.WDG_fromBase.Set_Value(true);
    //                }
    //
    //                WDGSheetData_To++;
    //                struct tm TM;
    //                localtime_s(&TM, &PLC_KPVL_old_dt);
    //
    //                SetWindowText(winmap(hEditState_WDG), string_time(&TM).c_str());
    //            }
    //        }
    //    }
    //    LOG_CATCH("Ошибка данных HMISheetData.Sheet.WDG_fromBase.Set_Value");
    //
    //}
}


bool PLC_S107::WD()
{
    //Бит жизни
    if(DataChangeS107.WatchDog == TRUE)
    {
        DataChangeS107.WatchDog = FALSE;
        if(DataChangeS107.WatchDogWait)
        {
            if(DataChangeS107.WatchDogWait >= CountWatchDogWait - 1)
                LOG_INFO(Logger, "{:90}| Продолжаем опрос {}", FUNCTION_LINE_NAME, DataChangeS107.WatchDogWait);
            SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());

        }
        DataChangeS107.WatchDogWait = 0;
    }
    else
    {
        DataChangeS107.WatchDogWait++; //Инкрементируем счетчик ошибок связи
        if(DataChangeS107.WatchDogWait >= CountWatchDogWait) //Если бита жизни нет больше CountWatchDogWait секунд
        {
            LOG_INFO(Logger, "{:90}| Перезапуск: Бита жизни нет больше {} секунд", FUNCTION_LINE_NAME, CountWatchDogWait);
            SetWindowText(winmap(hEditTime_1), std::to_string(DataChangeS107.WatchDogWait).c_str());
            SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
            //SekRun = time(NULL);
            return true;
        }
        else
        {
            if(DataChangeS107.WatchDogWait >= CountWatchDogWait - 1)
                LOG_INFO(Logger, "{:90}| Бита жизни нет больше {} секунд", FUNCTION_LINE_NAME, DataChangeS107.WatchDogWait);
            SetWindowText(winmap(hEditTime_1), std::to_string(DataChangeS107.WatchDogWait).c_str());
            SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
            //SekRun = time(NULL);
        }
    }
    return false;
}

void Open_FURN_RUN()
{
    //std::string Uri = (char*)pv;
    std::shared_ptr<spdlog::logger> Logger = PethLogger;

    LOG_INFO(Logger, "{:90}| Старт to: {}", FUNCTION_LINE_NAME, S107::URI);

    int countconnect = 1;
    LOG_INFO(Logger, "{:90}| Создание класса PLC_S107 {}", FUNCTION_LINE_NAME, countconnect);
    //Динамичесокая работа памяти с умным unique_ptr
    SetWindowText(winmap(hEditMode2), "Создание объекта");
    auto PLC = std::unique_ptr<PLC_S107>(new PLC_S107(S107::URI, PethLogger));

    while(isRun)
    {
        try
        {
            ////Автоматическая работа пямяти
            ////Возможен сбой программы
            //PLC_S107 CS_S107(Uri, PethLogger);
            //CS_S107.Run(countconnect);
            //
            ////Динамичесокая работа памяти с умным shared_ptr
            //auto CS_S107 = std::shared_ptr<PLC_S107>(new PLC_S107(Uri, PethLogger));
            //CS_S107->Run(countconnect);
            //CS_S107.reset();
            // 
            ////Динамичесокая работа памяти с класическим new/delete
            ////Не безопасная работа с памятью.
            //auto CS_S107 = new CS_S107(Uri, PethLogger);
            //CS_S107->Run(countconnect);
            //delete CS_S107;

            LOG_INFO(Logger, "{:90}| Подключение {} to: {}", FUNCTION_LINE_NAME, countconnect, S107::URI);
            PLC->Run(countconnect);
        }
        CATCH_OPEN(Logger, S107::URI);

        LOG_INFO(Logger, "{:90}| Выход из опроса countconnect = {} to: {}", FUNCTION_LINE_NAME, countconnect, S107::URI);

        if(isRun)
        {
            countconnect++;
            LOG_INFO(Logger, "{:90}| Повторяем попытку {} to: {}", FUNCTION_LINE_NAME, countconnect, S107::URI);
            Sleep(1000);
        }
    }
    SetWindowText(winmap(hEditMode2), "Удаление PLC");
    PLC.reset();

    LOG_INFO(Logger, "{:90}| ExitThread. isRun = {}", FUNCTION_LINE_NAME, isRun);

#ifdef API
    return 0;
#endif
}

void GetCasseteData(T_ForBase_RelFurn& app, TCassette& TC)
{

    if(/*TC.CassetteNo == "3" &&*/ TC.Day == "11" && TC.Month == "4")
    {
        int tt = 0;
    }

    int ev = atoi(TC.Event.c_str());
    if(ev == evCassete::Nul || ev == evCassete::Rel || ev == evCassete::Fill) return;
    if(!TC.Run_at.length())
        return;

    if(TC.Finish_at.length())
        return;

    
    PGresult* res = NULL;
    std::string comand = "";
    std::string end_at = "";
    std::string err_at = "";
    std::string tmp_at = "";

    std::stringstream sddEnd;
    sddEnd << "SELECT min(create_at) FROM todos WHERE create_at > '" << TC.Run_at << "' AND id_name = " << app.ProcEnd->ID << " AND content = 'true';";
    comand = sddEnd.str();
    res = conn_spic.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        if(PQntuples(res))
            end_at = conn_spic.PGgetvalue(res, 0, 0);
    }
    else
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    std::stringstream sddErr;
    sddErr << "SELECT min(create_at) FROM todos WHERE create_at > '" << TC.Run_at << "' AND id_name = " << app.ProcFault->ID << " AND content = 'true';";

    comand = sddErr.str();
    res = conn_spic.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        if(PQntuples(res))
            err_at = conn_spic.PGgetvalue(res, 0, 0);
    }
    else
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    if(!TC.End_at.length() && end_at.length())
    {
        TC.End_at = end_at;
        std::stringstream sde;
        sde << "UPDATE cassette SET end_at = '" << end_at << "' WHERE ";
        sde << "end_at IS NULL AND ";
        sde << "day = " << TC.Day << " AND ";
        sde << "month = " << TC.Month << " AND ";
        sde << "year = " << TC.Year << " AND ";
        sde << "cassetteno = " << TC.CassetteNo << ";";

        comand = sde.str();
        res = conn_spic.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        }
        PQclear(res);
    }

    if(!TC.Error_at.length() && err_at.length())
    {
        TC.Error_at = err_at;
        std::stringstream sde;
        sde << "UPDATE cassette SET err_at = '" << err_at << "' WHERE ";
        sde << "err_at IS NULL AND ";
        sde << "day = " << TC.Day << " AND ";
        sde << "month = " << TC.Month << " AND ";
        sde << "year = " << TC.Year << " AND ";
        sde << "cassetteno = " << TC.CassetteNo << ";";

        comand = sde.str();
        res = conn_spic.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        }
        PQclear(res);
    }

    tmp_at = end_at;
    if(!tmp_at.length())tmp_at = err_at;


    if(tmp_at.length())
    {
        std::tm TM_Temp ={0};
        DataTimeOfString(tmp_at, FORMATTIME, TM_Temp);
        TM_Temp.tm_year -= 1900;
        TM_Temp.tm_mon -= 1;

        time_t tStop1 = mktime(&TM_Temp) + (60 * 15); //15 сминут
        localtime_s(&TM_Temp, &tStop1);

        std::stringstream sdw;
        sdw << boost::format("%|04|-") % (TM_Temp.tm_year + 1900);
        sdw << boost::format("%|02|-") % (TM_Temp.tm_mon + 1);
        sdw << boost::format("%|02| ") % TM_Temp.tm_mday;
        sdw << boost::format("%|02|:") % TM_Temp.tm_hour;
        sdw << boost::format("%|02|:") % TM_Temp.tm_min;
        sdw << boost::format("%|02|") % TM_Temp.tm_sec;
        std::string finish_at = sdw.str();

        if(finish_at <= GetDataTimeString())
        {
            std::stringstream sdf;
            sdf << "UPDATE cassette SET finish_at = '" << finish_at << "', event = 5 WHERE finish_at IS NULL AND ";
            sdf << "day = " << TC.Day << " AND ";
            sdf << "month = " << TC.Month << " AND ";
            sdf << "year = " << TC.Year << " AND ";
            sdf << "cassetteno = " << TC.CassetteNo << ";";
            comand = sdf.str();
            res = conn_spic.PGexec(comand);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
            PQclear(res);
        }
    }

}

void Open_FURN_SQL()
{
    size_t old_count = 0;
    LOG_INFO(SQLLogger, "{:90}| Start Open_FURN_SQL", FUNCTION_LINE_NAME);

    while(isRun)
    {
#pragma region Рисуем текущий график

        S107::SQL::FURN_SQL();

        size_t count = AllCassette.size();

        if(old_count != count)
        {
            old_count = count;

            ListView_DeleteAllItems(hwndCassette);
            for(size_t i = 0; i < count; i++)
                AddHistoriCassette(false);
        }
        else
        {
            int TopIndex = ListView_GetTopIndex(hwndCassette);
            int Index = ListView_GetNextItem(hwndCassette, -1, LVNI_SELECTED);

            InvalidateRect(hwndCassette, NULL, false);

            ListView_EnsureVisible(hwndCassette, TopIndex, FALSE);
            ListView_SetItemState(hwndCassette, Index, LVIS_SELECTED, LVIS_OVERLAYMASK);
        }
#pragma endregion


#pragma region Вычисляем дату финала
        for(auto TC : AllCassette)
        {
            if(atoi(TC.Peth.c_str()) == 1)
                GetCasseteData(AppFurn1, TC);
            if(atoi(TC.Peth.c_str()) == 2)
                GetCasseteData(AppFurn2, TC);
        }
#pragma endregion

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    };
}

void Open_FURN()
{
    //Сортировка переменных по имени
    std::sort(AllTagPeth.begin(), AllTagPeth.end(), cmpAllTagPeth);
    PethLogger = InitLogger("PLC_S107");

#ifndef TESTSPIS
#ifndef TESTWIN
#ifndef TESTGRAFF
    hS107URI = std::thread(Open_FURN_RUN);
#endif
    hS107SQL = std::thread(Open_FURN_SQL);
    
#endif
#endif
}

void WaitClose_FURN(std::thread& h, std::string hamd)
{
    LOG_INFO(PethLogger, "{:90}| {}", FUNCTION_LINE_NAME, hamd);
    if(h.joinable())
        h.join();
}

void Close_FURN()
{
    HANDLE* h = NULL;
    int size = 0;
    //if(hS107URI)
    WaitClose_FURN(hS107URI, "hS107URI");
    WaitClose_FURN(hS107SQL, "hS107SQL");
    
}
