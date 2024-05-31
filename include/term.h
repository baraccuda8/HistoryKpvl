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
    //DINT ����
    Value* Day;

    //DINT �����
    Value* Month;

    //DINT ���
    Value* Year;

    //DINT ����� ������� �� ����
    Value* CassetteNo;

    std::string facttemper = "0";
};

typedef struct T_ForBase_RelFurn{
    time_t Furn_old_dt;

    //BOOL
    Value* WDG_toBase;

    //BOOL
    Value* WDG_fromBase;

    //REAL ������� ����� �������
    Value* PointTime_1;

    //REAL ������� �����������
    Value* PointRef_1;

    //REAL ������� ����� ��������
    Value* PointDTime_2;

    //BOOL ������
    Value* ProcRun;

    //BOOL ��������� ��������
    Value* ProcEnd;

    //BOOL ������ ��������
    Value* ProcFault;

    //REAL ������ ����� �������� (�������), ���
    Value* TimeProcSet;

    //REAL ����� ��������, ��� (0..XX)
    Value* ProcTimeMin;

    //REAL ����� �� ��������� ��������, ���
    Value* TimeToProcEnd;

    //REAL �������� �������� �����������
    Value* TempRef;

    //REAL ����������� �������� �����������
    Value* TempAct;

    //REAL ��������� 1
    Value* T1;

    //REAL ��������� 2
    Value* T2;

    //REAL ���� ����� �������
    Value* ActTimeHeatAcc;

    //REAL ���� ����� ��������
    Value* ActTimeHeatWait;

    //REAL ���� ����� ����� 
    Value* ActTimeTotal;

    T_cassette Cassette;
};

extern std::deque<TCassette> AllCassette;

namespace casCassette{
    //namespace emCassette{
    //    enum {
    //        create_at=0,
    //        id,
    //        event,
    //        day,
    //        month,
    //        year,
    //        cassetteno,
    //        sheetincassette,
    //        close_at,
    //        peth,
    //        run_at,
    //        error_at,
    //        end_at,
    //        delete_at,
    //        tempref,            //�������� �������� �����������
    //        pointtime_1,        //����� �������
    //        pointref_1,         //������� �����������
    //        timeprocset,        //������ ����� �������� (�������), ���
    //        pointdtime_2,       //����� ��������
    //        facttemper,           //���� ����������� �� 5 ����� �� ����� �������
    //        finish_at,
    //    };
    //};

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
        PointRef_1,         //������� �����������
        facttemper,           //���� ����������� �� 5 ����� �� ����� �������
        PointTime_1,        //����� �������
        TimeProcSet,        //������ ����� �������� (�������), ���
        PointDTime_2,       //����� ��������
    };
};


extern T_ForBase_RelFurn ForBase_RelFurn_1;
extern T_ForBase_RelFurn ForBase_RelFurn_2;

namespace S107
{
//#pragma region ������ ������� � �������
//    extern int Create_at;
//    extern int Id;
//    extern int Event;
//    extern int Day;
//    extern int Month;
//    extern int Year;
//    extern int CassetteNo;
//    extern int SheetInCassette;
//    extern int Close_at;
//    extern int Peth;
//    extern int Run_at;
//    extern int Error_at;
//    extern int End_at;
//    extern int Delete_at;
//    extern int TempRef;           //�������� �������� �����������
//    extern int PointTime_1;       //����� �������
//    extern int PointRef_1;        //������� �����������
//    extern int TimeProcSet;       //������ ����� �������� (�������), ���
//    extern int PointDTime_2;      //����� ��������
//    extern int facttemper;          //���� ����������� �� 5 ����� �� ����� �������
//    extern int Finish_at;
//#pragma endregion

    extern std::string ServerDataTime;
};

