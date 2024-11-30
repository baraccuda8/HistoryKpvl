#pragma once

#include "Subscript.h"
#include "ClCodeSys.h"
#include "ValueTag.h"


extern std::shared_ptr<spdlog::logger> PethLogger;
extern std::shared_ptr<spdlog::logger> FurnLogger;
extern std::shared_ptr<spdlog::logger> TestLogger;


class PLC_S107: public Client
{
    HDC hDC = NULL;
public:
    void DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr);

    //ClassDataChangeS107 DataChangeS107;

    PLC_S107(std::string uri, std::shared_ptr<spdlog::logger>& log): Client(uri, log){};
    //void Connect() override;

    void InitNodeId() override;
    void InitTag() override;
    void Run(int countconnect) override;
    void GetWD();
    bool WD();
};


#define AppFurn1 ForBase_RelFurn_1
#define AppFurn2 ForBase_RelFurn_2
#define AppCassette FcassetteArray.cassette
#define AppSelected1 FcassetteArray.selected_cassetFurn1
#define AppSelected2 FcassetteArray.selected_cassetFurn2
#define AppCassette1 FcassetteArray.cassette[0]
#define AppCassette2 FcassetteArray.cassette[1]
#define AppCassette3 FcassetteArray.cassette[2]
#define AppCassette4 FcassetteArray.cassette[3]
#define AppCassette5 FcassetteArray.cassette[4]
#define AppCassette6 FcassetteArray.cassette[5]
#define AppCassette7 FcassetteArray.cassette[6]

#define PathPeth std::string("|var|SPK107 (M01).Application.")
#define StrFurn1 std::string("|var|SPK107 (M01).Application.ForBase_RelFurn_1.Data.")
#define StrFurn2 std::string("|var|SPK107 (M01).Application.ForBase_RelFurn_2.Data.")
//#define StrCassette PathPeth + "FcassetteArray.data.cassette"
#define StrSelected1 std::string("|var|SPK107 (M01).Application.cassetteArray.data.selected_cassetFurn1")
#define StrSelected2 std::string("|var|SPK107 (M01).Application.cassetteArray.data.selected_casset")

#define StrCassette1 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[1].")
#define StrCassette2 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[2].")
#define StrCassette3 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[3].")
#define StrCassette4 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[4].")
#define StrCassette5 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[5].")
#define StrCassette6 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[6].")
#define StrCassette7 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[7].")

typedef struct T_Fcassette{

    //uint16_t Час ID листа
    Value* Hour;

    //int32_t День
    Value* Day;

    //int32_t Месяц
    Value* Month;

    //int32_t Год
    Value* Year;

    //int32_t Номер кассеты за день
    Value* CassetteNo;

    std::string facttemper = "0";

    bool TestNull(){

        bool b = false;
        try
        {
            b = CassetteNo->GetInt() != 0 ||
                //AppCassette[i].Hour->GetInt() != 0 ||
                Day->GetInt() != 0 ||
                Month->GetInt() != 0 ||
                Year->GetInt() != 0;
        }CATCH(PethLogger, "");
        return b;
    }

    void SetNull()
    {
        //Отбравляем в печь
        try
        {
            Year->Set_Value(int32_t(0));
            Month->Set_Value(int32_t(0));
            Day->Set_Value(int32_t(0));
            Hour->Set_Value(uint16_t(0));
            CassetteNo->Set_Value(int32_t(0));
        }CATCH(PethLogger, "");
    }
}T_Fcassette;

typedef struct T_ForBase_RelFurn{
    time_t Furn_old_dt;

    //bool
    Value* WDG_toBase;

    //bool
    Value* WDG_fromBase;

    //float Уставка температуры
    Value* PointRef_1;

    //float Задание Время разгона
    Value* PointTime_1;

    //float Задание Время выдержки
    Value* PointTime_2;

    //bool Работа
    Value* ProcRun;

    //bool Окончание процесса
    Value* ProcEnd;

    //bool Авария процесса
    Value* ProcFault;

    //float Полное время процесса (уставка), мин
    Value* TimeProcSet;

    //float Время процесса, час (0..XX)
    Value* ProcTimeMin;

    //float Время до окончания процесса, мин
    Value* TimeToProcEnd;

    //float Заданное значение температуры
    Value* TempRef;

    //float Фактическое значение температуры
    Value* TempAct;

    //float Термопара 1
    Value* T1;

    //float Термопара 2
    Value* T2;

    //float Факт время нагрева
    Value* ActTimeHeatAcc;

    //float Факт время выдержки
    Value* ActTimeHeatWait;

    //float Факт общее время 
    Value* ActTimeTotal;

    //bool Возврат касеты в список
    Value* ReturnCassetteCmd;

    T_Fcassette Cassette;
}T_ForBase_RelFurn;

extern std::deque<TCassette> AllCassette;

namespace casCassette{
    enum cas {
        nn = 0,
        Id = 1,
        Create_at = 2,
        Year,
        Month,
        Day,
        Hour,
        CassetteNo,
        SheetInCassette,
        Close_at,
        Event,
        Run_at,
        End_at,
        Error_at,
        Delete_at,
        Peth,
        PointRef_1,         //Уставка температуры
        facttemper,           //Факт температуры за 5 минут до конца отпуска
        PointTime_1,        //Время разгона
        TimeProcSet,        //Полное время процесса (уставка), мин
        PointTime_2,       //Время выдержки
    };
};


extern T_ForBase_RelFurn ForBase_RelFurn_1;
extern T_ForBase_RelFurn ForBase_RelFurn_2;

namespace S107
{
    extern std::string ServerDataTime;
};




