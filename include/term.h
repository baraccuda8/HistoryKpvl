#pragma once

#include "Subscript.h"
#include "ClCodeSys.h"
#include "ValueTag.h"


class ClassDataChangeS107: public ClassDataChange
{
public:
    void DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr);
};


class PLC_S107: public Client
{
public:

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
#define AppCassette cassetteArray.cassette
#define AppSelected1 cassetteArray.selected_cassetFurn1
#define AppSelected2 cassetteArray.selected_cassetFurn2
#define AppCassette1 cassetteArray.cassette[0]
#define AppCassette2 cassetteArray.cassette[1]
#define AppCassette3 cassetteArray.cassette[2]
#define AppCassette4 cassetteArray.cassette[3]
#define AppCassette5 cassetteArray.cassette[4]
#define AppCassette6 cassetteArray.cassette[5]
#define AppCassette7 cassetteArray.cassette[6]

#define PathPeth std::string("|var|SPK107 (M01).Application.")
#define StrFurn1 std::string("|var|SPK107 (M01).Application.ForBase_RelFurn_1.Data.")
#define StrFurn2 std::string("|var|SPK107 (M01).Application.ForBase_RelFurn_2.Data.")
//#define StrCassette PathPeth + "cassetteArray.data.cassette"
#define StrSelected1 std::string("|var|SPK107 (M01).Application.cassetteArray.data.selected_cassetFurn1")
#define StrSelected2 std::string("|var|SPK107 (M01).Application.cassetteArray.data.selected_casset")

#define StrCassette1 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[1].")
#define StrCassette2 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[2].")
#define StrCassette3 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[3].")
#define StrCassette4 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[4].")
#define StrCassette5 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[5].")
#define StrCassette6 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[6].")
#define StrCassette7 std::string("|var|SPK107 (M01).Application.cassetteArray.data.cassette[7].")

typedef struct T_cassette{
    Value* Year;
    Value* Month;
    Value* Day;
    Value* CassetteNo;
    std::string f_temper = "0";
};

typedef struct T_ForBase_RelFurn{
    time_t Furn_old_dt;
    Value* WDG_toBase;
    Value* WDG_fromBase;

    Value* PointTime_1;
    Value* PointRef_1;
    Value* PointDTime_2;

    Value* ProcRun;
    Value* ProcEnd;
    Value* ProcFault;
    Value* TimeProcSet;
    Value* ProcTimeMin;
    Value* TimeToProcEnd;
    Value* TempRef;
    Value* TempAct;
    Value* T1;
    Value* T2;

    T_cassette Cassette;
};

extern std::deque<TCassette> AllCassette;

namespace casCassette{
    namespace emCassette{
        enum {
            create_at=0,
            id,
            event,
            day,
            month,
            year,
            cassetteno,
            sheetincassette,
            close_at,
            peth,
            run_at,
            error_at,
            end_at,
            delete_at,
            tempref,            //Заданное значение температуры
            pointtime_1,        //Время разгона
            pointref_1,         //Уставка температуры
            timeprocset,        //Полное время процесса (уставка), мин
            pointdtime_2,       //Время выдержки
            f_temper,           //Факт температуры за 5 минут до конца отпуска
        };
    };

    enum cas {
        nn = 0,
        Id = 1,
        Create_at = 2,
        Year,
        Month,
        Day,
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
        f_temper,           //Факт температуры за 5 минут до конца отпуска
        PointTime_1,        //Время разгона
        TimeProcSet,        //Полное время процесса (уставка), мин
        PointDTime_2,       //Время выдержки
    };
};


//typedef struct T_ForBase_Data{
//    T_ForBase_RelFurn Data;
//};



extern T_ForBase_RelFurn ForBase_RelFurn_1;
extern T_ForBase_RelFurn ForBase_RelFurn_2;

namespace S107
{
    extern std::string ServerDataTime;
};