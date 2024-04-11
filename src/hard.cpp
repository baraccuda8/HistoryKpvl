#include "pch.h"

#include "win.h"
#include "main.h"
#include "file.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "hard.h"
#include "KPVL.h"

//FFFF F000 0000 0000 ������
//0000 0FFF 0000 0000 ������
//0000 0000 F000 0000 �����
//0000 0000 0FFF 0000 ����
//0000 0000 0000 F000 �������
//0000 0000 0000 0FFF ����


//#define API


namespace KPVL {
    const std::string URI = "opc.tcp://192.168.9.1:4840";
    //const std::string  URI = "opc.tcp://85.116.124.32:4840";
};

#define PLCsec1 sec01000
#define PLCsec2 sec02000
#define PLCsec5 sec05000

extern std::string lpLogDir;

std::shared_ptr<spdlog::logger> HardLogger = NULL;


bool isInitPLC_KPVL = false;

time_t PLC_KPVL_old_dt = 0;


std::thread hFindSheet;

std::thread hKPVLURI;
std::thread hKPVLSQL;
HANDLE hThreadState2 = NULL;


std::string MaskKlapan1 = "000000000";
std::string MaskKlapan2 = "000000000";
std::string MaskKlapan;


const std::string PathKpvl = "|var|PLC210 OPC-UA.Application.";
const std::string AppPar_Gen = PathKpvl + "Par_Gen.Par.";
const std::string AppGenSeqFromHmi = PathKpvl + "GenSeqFromHmi.Data.";
const std::string AppGenSeqToHmi = PathKpvl + "GenSeqToHmi.Data.";
const std::string AppAI_Hmi_210 = PathKpvl + "AI_Hmi_210.Hmi.";
const std::string AppHMISheetData = PathKpvl + "HMISheetData.Sheet.";

std::string AppGenSeqFromHmi1 = AppGenSeqFromHmi + "PlateData.";
std::string AppGenSeqToHmi1 =  AppGenSeqToHmi + "PlateData_";
std::string AppHMISheetData1 = AppHMISheetData + "SheetDataIN.";
std::string AppHMISheetData2 = AppHMISheetData + "Cassette.";


//std::string AppGenSeqToHmi = PathKpvl + "GenSeqToHmi.Data.";



    extern std::map<int, const char*> NamePos2;


#define SETALLTAG1(_t, _f, _e, _s,  _d) SETALLTAG(PathKpvl, _t, _f, _e, _s,  _d)



T_Par_Gen Par_Gen;
T_AI_Hmi_210 AI_Hmi_210;
T_HMISheetData HMISheetData;
T_GenSeqFromHmi GenSeqFromHmi;
T_GenSeqToHmi GenSeqToHmi;
T_Hmi210_1 Hmi210_1;
T_PlateData PlateData[7];

    //struct T_TextData{
    //        Value* PlateTypeArray[7];
    //        Value* PlateThicknessArray[7];
    //}TextData;

//}


    //���� 1 ���� ����
    ////��������� �����������
    ////����� �������
    ////����������� �������
    ////�������� �������
    ////����� �����
    //
    ////���� �������
    //namespace TEMP {
    //
    //    void CloseAllCassette(T_Cassette& CD)
    //    {
    //        auto t = CD.Day->GetType();
    //        if(GetVal<int>(CD.Day) && CD.Month.Val && CD.Year.Val && CD.CassetteNo.Val)
    //        {
    //            std::string comand = "UPDATE cassette SET";
    //            comand += " close_at = now()";
    //            comand += " WHERE close_at IS NULL";
    //            comand += " AND day = " + std::to_string(CD.Day.Val);
    //            comand += " AND month = " + std::to_string(CD.Month.Val);
    //            comand += " AND year = " + std::to_string(CD.Year.Val);
    //            comand += " AND cassetteno = " + std::to_string(CD.CassetteNo.Val);
    //            comand += ";";
    //
    //            PGresult* res = conn_temp.PGexec(comand);
    //            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //            {
    //                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  utf8_to_cp1251(PQresultErrorMessage(res)));
    //                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            }
    //            PQclear(res);
    //        }
    //    }
    //
    //    void CloseAllCassette2(TAG_SPK1::T_cassette& CD, int Peth)
    //    {
    //        if(
    //            (CD.Day.Val && CD.Month.Val && CD.Year.Val && CD.CassetteNo.Val) ||
    //            (!CD.Day.Val && !CD.Month.Val && !CD.Year.Val && !CD.CassetteNo.Val)
    //            )
    //        {
    //            std::string comand = "UPDATE cassette SET ";
    //            comand += " event = 5,";
    //            comand += " end_at = now()";
    //            comand += " WHERE end_at IS NULL";
    //            comand += " AND event = 3";
    //            comand += " AND peth = " + std::to_string(Peth);
    //            if(CD.Day.Val && CD.Month.Val && CD.Year.Val && CD.CassetteNo.Val)
    //            {
    //                comand += " AND day <> " + std::to_string(CD.Day.Val);
    //                comand += " AND month <> " + std::to_string(CD.Month.Val);
    //                comand += " AND year <> " + std::to_string(CD.Year.Val);
    //                comand += " AND cassetteno <> " + std::to_string(CD.CassetteNo.Val);
    //            }
    //            comand += ";";
    //
    //            PGresult* res = conn_temp.PGexec(comand);
    //            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //            {
    //                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
    //                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //            }
    //            PQclear(res);
    //        }
    //    }
    //
    //    void UpdateCassetteProcRun(TAG_SPK1::T_cassette& CD, TAG_SPK1::T_ForBase_RelFurn& Furn, int Peth)
    //    {
    //        if(CD.Day.Val && CD.Month.Val && CD.Year.Val && CD.CassetteNo.Val)
    //        {
    //            //CloseAllCassette(CD);
    //
    //            std::string comand = "UPDATE cassette SET ";
    //            comand += "peth = " + std::to_string(Peth);
    //            comand += ", run_at = now()";
    //
    //            comand += ", event = 3";
    //            comand += ", tempref = " + Furn.Data.TempRef.GetString();
    //            comand += ", pointtime_1 = " + Furn.Data.PointTime_1.GetString();
    //            comand += ", pointref_1 = " + Furn.Data.PointRef_1.GetString();
    //            comand += ", timeprocset = " + Furn.Data.TimeProcSet.GetString();
    //            comand += ", pointdtime_2 = " + Furn.Data.PointDTime_2.GetString();
    //
    //            comand += " WHERE run_at IS NULL";
    //            comand += " AND day = " + std::to_string(CD.Day.Val);
    //            comand += " AND month = " + std::to_string(CD.Month.Val);
    //            comand += " AND year = " + std::to_string(CD.Year.Val);
    //            comand += " AND cassetteno = " + std::to_string(CD.CassetteNo.Val);
    //            comand += ";";
    //
    //            PGresult* res = conn_temp.PGexec(comand);
    //            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //            {
    //                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  utf8_to_cp1251(PQresultErrorMessage(res)));
    //                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            }
    //            PQclear(res);
    //        }
    //    }
    //    void UpdateCassetteProcEnd(TAG_SPK1::T_cassette& CD, int Peth)
    //    {
    //        if(CD.Day.Val && CD.Month.Val && CD.Year.Val && CD.CassetteNo.Val)
    //        {
    //            std::string comand = "UPDATE cassette SET ";
    //            comand += "end_at = now()";
    //            comand += ", event = 5";
    //            comand += " WHERE end_at IS NULL";
    //            comand += " AND peth = " + std::to_string(Peth);
    //            comand += " AND day = " + std::to_string(CD.Day.Val);
    //            comand += " AND month = " + std::to_string(CD.Month.Val);
    //            comand += " AND year = " + std::to_string(CD.Year.Val);
    //            comand += " AND cassetteno = " + std::to_string(CD.CassetteNo.Val);
    //            comand += ";";
    //
    //            PGresult* res = conn_temp.PGexec(comand);
    //            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //            {
    //                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  utf8_to_cp1251(PQresultErrorMessage(res)));
    //                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            }
    //            PQclear(res);
    //        }
    //    }
    //    void UpdateCassetteProcError(TAG_SPK1::T_cassette& CD, int Peth)
    //    {
    //        if(CD.Day.Val && CD.Month.Val && CD.Year.Val && CD.CassetteNo.Val)
    //        {
    //            std::string comand = "UPDATE cassette SET ";
    //            comand += "error_at = now()";
    //            comand += ", event = 4";
    //            comand += " WHERE error_at IS NULL";
    //            comand += " AND peth = " + std::to_string(Peth);
    //            comand += " AND day = " + std::to_string(CD.Day.Val);
    //            comand += " AND month = " + std::to_string(CD.Month.Val);
    //            comand += " AND year = " + std::to_string(CD.Year.Val);
    //            comand += " AND cassetteno = " + std::to_string(CD.CassetteNo.Val);
    //            comand += ";";
    //
    //            PGresult* res = conn_temp.PGexec(comand);
    //            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //            {
    //                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  utf8_to_cp1251(PQresultErrorMessage(res)));
    //                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            }
    //            PQclear(res);
    //        }
    //    }
    //    void SetTemperCassette(TAG_SPK1::T_cassette& CD, std::string teper)
    //    {
    //        if(teper != CD.f_temper)
    //        if(CD.Day.Val && CD.Month.Val && CD.Year.Val && CD.CassetteNo.Val)
    //        {
    //            CD.f_temper = teper;
    //            std::string comand = "UPDATE cassette SET";
    //            comand += " facttemper = " + teper;
    //            comand += " WHERE ";
    //            //comand += " facttemper = 0.0 AND";
    //            comand += " day = " + std::to_string(CD.Day.Val);
    //            comand += " AND month = " + std::to_string(CD.Month.Val);
    //            comand += " AND year = " + std::to_string(CD.Year.Val);
    //            comand += " AND cassetteno = " + std::to_string(CD.CassetteNo.Val);
    //            comand += ";";
    //
    //            PGresult* res = conn_temp.PGexec(comand);
    //            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //            {
    //                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  utf8_to_cp1251(PQresultErrorMessage(res)));
    //                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
    //            }
    //            PQclear(res);
    //        }
    //    }
    //
    //
    ////��� ������� #1
    //namespace Furn1{
    //    DWORD Data_WDG_toBase(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����� �������
    //    DWORD PointTime_1(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ������� �����������
    //    DWORD PointRef_1(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����� ��������
    //    DWORD PointDTime_2(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //BOOL ������
    //    DWORD ProcRun(Value* value)
    //    {
    //        std::string out = "�������";
    //        if(value->Val)
    //        {
    //            out = GetShortTimes();
    //            UpdateCassetteProcRun(AppFurn1.Cassette, TAG_PLC_SPK1.Application.ForBase_RelFurn_1, 1);
    //        }
    //        MySetWindowText(winmap(value->winId), out.c_str());
    //        return 0;
    //    }
    //    //BOOL ��������� ��������
    //    DWORD ProcEnd(Value* value)
    //    {
    //        std::string out = "";
    //        if(value->Val)
    //        {
    //            out = GetShortTimes();
    //            UpdateCassetteProcEnd(AppFurn1.Cassette, 1);
    //        }
    //        MySetWindowText(winmap(value->winId), out.c_str());
    //        return 0;
    //    }
    //    //BOOL ������ ��������
    //    DWORD ProcError(Value* value)
    //    {
    //        std::string out = "";
    //        if(value->Val)
    //        {
    //            out = GetShortTimes();
    //            UpdateCassetteProcError(AppFurn1.Cassette, 1);
    //        }
    //        MySetWindowText(winmap(value->winId), out.c_str());
    //        return 0;
    //    }
    //    //REAL ������ ����� �������� (�������), ���
    //    DWORD TimeProcSet(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����� ��������, ��� (0..XX)
    //    DWORD ProcTimeMin(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����� �� ��������� ��������, ���
    //    DWORD TimeToProcEnd(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        if(value->Val <= 5.0 && value->Val >= 4.9)
    //        {
    //            MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.TempAct.GetString().c_str());
    //            SetTemperCassette(AppFurn1.Cassette, AppFurn1.TempAct.GetString());
    //        }
    //        else
    //            if(value->Val >= AppFurn1.TimeProcSet.Val)
    //            {
    //                AppFurn1.Cassette.f_temper = "0";
    //                MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.Cassette.f_temper.c_str());
    //            }
    //        return 0;
    //    }
    //    //REAL �������� �������� �����������
    //    DWORD TempRef(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����������� �������� �����������
    //    DWORD TempAct(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ��������� 1
    //    DWORD T1(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ��������� 2
    //    DWORD T2(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //ID ������ ���
    //    DWORD Year(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        CloseAllCassette2(AppFurn1.Cassette, 1);
    //        return 0;
    //    }
    //    //ID ������ �����
    //    DWORD Month(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        CloseAllCassette2(AppFurn1.Cassette, 1);
    //        return 0;
    //    }
    //    //ID ������ ����
    //    DWORD Day(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        CloseAllCassette2(AppFurn1.Cassette, 1);
    //        return 0;
    //    }
    //    //ID ������ �����
    //    DWORD CassetteNo(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        CloseAllCassette2(AppFurn1.Cassette, 1);
    //        return 0;
    //    }
    //    //�����
    //    DWORD SelectedCassete(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //}
    ////��� ������� #2
    //namespace Furn2{
    //    DWORD Data_WDG_toBase(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����� �������
    //    DWORD PointTime_1(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ������� �����������
    //    DWORD PointRef_1(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����� ��������
    //    DWORD PointDTime_2(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //BOOL ������
    //    DWORD ProcRun(Value* value)
    //    {
    //        std::string out = "�������";
    //        if(value->Val)
    //        {
    //            out = GetShortTimes();
    //            UpdateCassetteProcRun(AppFurn2.Cassette, TAG_PLC_SPK1.Application.ForBase_RelFurn_2, 2);
    //        }
    //        MySetWindowText(winmap(value->winId), out.c_str());
    //        return 0;
    //    }
    //    //BOOL ��������� ��������
    //    DWORD ProcEnd(Value* value)
    //    {
    //        std::string out = "";
    //        if(value->Val)
    //        {
    //            out = GetShortTimes();
    //            UpdateCassetteProcEnd(AppFurn2.Cassette, 2);
    //        }
    //        MySetWindowText(winmap(value->winId), out.c_str());
    //        return 0;
    //    }
    //    //BOOL ������ ��������
    //    DWORD ProcError(Value* value)
    //    {
    //        std::string out = "";
    //        if(value->Val)
    //        {
    //            out = GetShortTimes();
    //            UpdateCassetteProcError(AppFurn2.Cassette, 2);
    //        }
    //        MySetWindowText(winmap(value->winId), out.c_str());
    //        return 0;
    //    }
    //    //REAL ������ ����� �������� (�������), ���
    //    DWORD TimeProcSet(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����� ��������, ��� (0..XX)
    //    DWORD ProcTimeMin(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����� �� ��������� ��������, ���
    //    DWORD TimeToProcEnd(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        if(value->Val <= 5.0 && value->Val >= 4.9)
    //        {
    //            MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.TempAct.GetString().c_str());
    //            SetTemperCassette(AppFurn2.Cassette, AppFurn2.TempAct.GetString());
    //        }
    //        else
    //            if(value->Val >= AppFurn1.TimeProcSet.Val)
    //            {
    //                AppFurn2.Cassette.f_temper = "0";
    //                MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.Cassette.f_temper.c_str());
    //            }
    //        return 0;
    //    }
    //    //REAL �������� �������� �����������
    //    DWORD TempRef(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ����������� �������� �����������
    //    DWORD TempAct(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ��������� 1
    //    DWORD T1(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //REAL ��������� 2
    //    DWORD T2(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //ID ������ ���
    //    DWORD Year(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        CloseAllCassette2(AppFurn2.Cassette, 2);
    //        return 0;
    //    }
    //    //ID ������ �����
    //    DWORD Month(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        CloseAllCassette2(AppFurn2.Cassette, 2);
    //        return 0;
    //    }
    //    //ID ������ ����
    //    DWORD Day(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        CloseAllCassette2(AppFurn2.Cassette, 2);
    //        return 0;
    //    }
    //    //ID ������ �����
    //    DWORD CassetteNo(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        CloseAllCassette2(AppFurn2.Cassette, 2);
    //        return 0;
    //    }
    //    //�����
    //    DWORD SelectedCassete(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //};
    //


//namespace KPVL{
//    DWORD Hmi210_1_Mas(Value* value)
//    {
//
//        return 0;
//    }
//    
//}

extern std::deque<Value*> AllTagPeth;
std::deque<Value*> AllTagKpvl = {
#ifndef TESTTEMPER
//������ WDG
#pragma region ������ WDG
    {HMISheetData.WDG           = new Value(AppHMISheetData + "WDG",            HWNDCLIENT::hEditWDG, 0, &conn_kpvl, MSSEC::sec00500)},    //������� ������ �����������
    {HMISheetData.WDG_toBase    = new Value(AppHMISheetData + "WDG_toBase",     HWNDCLIENT::hEditState_WDG, KPVL::WDG::SheetData_WDG_toBase, &conn_kpvl, MSSEC::sec00500)},  //�������� ��� ����� ��� �����������
    {HMISheetData.WDG_fromBase  = new Value(AppHMISheetData + "WDG_fromBase",   HWNDCLIENT::hNull, 0, &conn_kpvl, MSSEC::sec00500)}, //������������� ���� ����� ��� �����������
#pragma endregion

//������ � �����
#pragma region ������ � �����
    {HMISheetData.NewData           = new Value(AppHMISheetData + "NewData",            HWNDCLIENT::hGroup05, KPVL::Z6::NewSheetData, &conn_kpvl)},       //����� ���� � ������
    {HMISheetData.SaveDone           = new Value(AppHMISheetData + "SaveDone",          HWNDCLIENT::hNull, 0, &conn_kpvl)},                         //����� ����� ���� � ������
    {HMISheetData.CasseteIsFill     = new Value(AppHMISheetData + "CasseteIsFill",      HWNDCLIENT::hNull,    KPVL::Z6::CassetteIsFill, &conn_kpvl)},     //������� �����������
    {HMISheetData.StartNewCassette  = new Value(AppHMISheetData + "StartNewCassette",   HWNDCLIENT::hNull,    KPVL::Z6::StartNewCassette, &conn_kpvl)},  //������ ����� �������
#pragma endregion

//�������� ��������, ������� �����������, ��������, ����������� ����
#pragma region �������, �������
    //|var|PLC210 OPC-UA.Application.Par_Gen.Par.UnloadSpeed
    {Par_Gen.UnloadSpeed        = new Value(AppPar_Gen + "UnloadSpeed",         HWNDCLIENT::hEditUnloadSpeed,     0, &conn_kpvl)},            //�������� ��������
    {Par_Gen.TimeForPlateHeat   = new Value(AppPar_Gen + "TimeForPlateHeat",    HWNDCLIENT::hTimeForPlateHeat,    KPVL::An::fTimeForPlateHeat, &conn_kpvl)},   //����� ������������ ��������� �������, ���
    {Par_Gen.PresToStartComp    = new Value(AppPar_Gen + "PresToStartComp",     HWNDCLIENT::hPresToStartComp,     KPVL::An::fPresToStartComp, &conn_kpvl)},    //������� �������� ��� ������� �����������


    {GenSeqFromHmi.TempSet1     = new Value(AppGenSeqFromHmi + "TempSet1",          HWNDCLIENT::hEditTempSet,         KPVL::An::TempSet1, &conn_kpvl)},       //������� ����������� 
    {AI_Hmi_210.LaminPressTop   = new Value(AppAI_Hmi_210 + "LaminPressTop.AI_eu",  HWNDCLIENT::hEditZakPressTopSet,  0, &conn_kpvl)},  //�������� ���� ������� ��������� ����
    {AI_Hmi_210.LaminPressBot   = new Value(AppAI_Hmi_210 + "LaminPressBot.AI_eu",  HWNDCLIENT::hEditZakPressBotSet,  0, &conn_kpvl)},  //�������� ���� ������� ��������� ���
    {AI_Hmi_210.LAM_TE1         = new Value(AppAI_Hmi_210 + "LAM_TE1.AI_eu",        HWNDCLIENT::hEditLAM_TE1Set,      0, &conn_kpvl)},        //����������� ���� � �������
    {AI_Hmi_210.Za_TE3          = new Value(AppAI_Hmi_210 + "Za_TE3.AI_eu",         HWNDCLIENT::hEditLAM_TE3Set,      0, &conn_kpvl)},         //����������� ���� � ����
    {AI_Hmi_210.Za_PT3          = new Value(AppAI_Hmi_210 + "Za_PT3.AI_eu",         HWNDCLIENT::hEditLAM_PT3Set,      0, &conn_kpvl)},         //�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)
    {AI_Hmi_210.Za_TE4          = new Value(AppAI_Hmi_210 + "Za_TE4.AI_eu",         HWNDCLIENT::hEditTempPerometr,    0, &conn_kpvl)},                    //��������
#pragma endregion

//����� ������ �������
#pragma region ����� ������ �������
    {HMISheetData.Valve_1x              = new Value(AppHMISheetData + "Valve_1x",               HWNDCLIENT::hEditClapTop0,    KPVL::Mask::DataMaskKlapan1, &conn_kpvl)},    //����� ������ ������� 1x
    {HMISheetData.Valve_2x              = new Value(AppHMISheetData + "Valve_2x",               HWNDCLIENT::hEditClapBot0,    KPVL::Mask::DataMaskKlapan2, &conn_kpvl)},    //����� ������ ������� 2x
    {HMISheetData.SpeedSection.Top      = new Value(AppHMISheetData + "SpeedSection.Top",       HWNDCLIENT::hEditSpeedTopSet, KPVL::Speed::SpeedSectionTop, &conn_kpvl)},    //������. ���������� ������. ����
    {HMISheetData.SpeedSection.Bot      = new Value(AppHMISheetData + "SpeedSection.Bottom",    HWNDCLIENT::hEditSpeedBotSet, KPVL::Speed::SpeedSectionBot, &conn_kpvl)},    //������. ���������� ������. ���
    {HMISheetData.LaminarSection1.Top   = new Value(AppHMISheetData + "LaminarSection1.Top",    HWNDCLIENT::hEditLAM1_TopSet, KPVL::Lam1::LaminarSection1Top, &conn_kpvl)}, //������. ���������� ������ 1. ����
    {HMISheetData.LaminarSection1.Bot   = new Value(AppHMISheetData + "LaminarSection1.Bottom", HWNDCLIENT::hEditLAM1_BotSet, KPVL::Lam1::LaminarSection1Bot, &conn_kpvl)}, //������. ���������� ������ 1. ���
    {HMISheetData.LaminarSection2.Top   = new Value(AppHMISheetData + "LaminarSection2.Top",    HWNDCLIENT::hEditLAM2_TopSet, KPVL::Lam2::LaminarSection2Top, &conn_kpvl)}, //������. ���������� ������ 2. ����
    {HMISheetData.LaminarSection2.Bot   = new Value(AppHMISheetData + "LaminarSection2.Bottom", HWNDCLIENT::hEditLAM2_BotSet, KPVL::Lam2::LaminarSection2Bot, &conn_kpvl)}, //������. ���������� ������ 2. ���
#pragma endregion

//���� 0 ���� ����� �����
#pragma region ���� 0 ���� ����� �����
    {PlateData[0].AlloyCodeText  = new Value(AppGenSeqFromHmi1 + "AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ0_AlloyCode,   KPVL::Z0::DataAlloyThikn, &conn_kpvl)}, //����� �����
    {PlateData[0].ThiknessText   = new Value(AppGenSeqFromHmi1 + "ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ0_Thikness,    KPVL::Z0::DataAlloyThikn, &conn_kpvl)}, //������� �����
    {PlateData[0].Melt           = new Value(AppGenSeqFromHmi1 + "Melt",                HWNDCLIENT::hEditPlate_DataZ0_Melt,        KPVL::Z0::Data, &conn_kpvl)},    //����� ������
    {PlateData[0].Slab           = new Value(AppGenSeqFromHmi1 + "Slab",                HWNDCLIENT::hEditPlate_DataZ0_Slab,        KPVL::Z0::Data, &conn_kpvl)},    //����� �����
    {PlateData[0].PartNo         = new Value(AppGenSeqFromHmi1 + "PartNo",              HWNDCLIENT::hEditPlate_DataZ0_PartNo,      KPVL::Z0::Data, &conn_kpvl)},    //����� ������
    {PlateData[0].Pack           = new Value(AppGenSeqFromHmi1 + "Pack",                HWNDCLIENT::hEditPlate_DataZ0_Pack,        KPVL::Z0::Data, &conn_kpvl)},    //����� �����
    {PlateData[0].Sheet          = new Value(AppGenSeqFromHmi1 + "Sheet",               HWNDCLIENT::hEditPlate_DataZ0_Sheet,       KPVL::Z0::Data, &conn_kpvl)},    //����� �����
    {PlateData[0].SubSheet       = new Value(AppGenSeqFromHmi1 + "SubSheet",            HWNDCLIENT::hEditPlate_DataZ0_SubSheet,    KPVL::Z0::Data, &conn_kpvl)},    //����� ��������
#pragma endregion

//���� 1 ������ ������ (����, ����)
#pragma region ���� 1 ������ ������ (����, ����)
    {PlateData[1].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z1.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ1_AlloyCode,  KPVL::Z1::DataAlloyThikn, &conn_kpvl)}, //����� �����
    {PlateData[1].ThiknessText  = new Value(AppGenSeqToHmi1 + "Z1.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ1_Thikness,   KPVL::Z1::DataAlloyThikn, &conn_kpvl)}, //������� �����
    {PlateData[1].Melt          = new Value(AppGenSeqToHmi1 + "Z1.Melt",                HWNDCLIENT::hEditPlate_DataZ1_Melt,       KPVL::Z1::Data, &conn_kpvl)},    //����� ������
    {PlateData[1].Slab          = new Value(AppGenSeqToHmi1 + "Z1.Slab",                HWNDCLIENT::hEditPlate_DataZ1_Slab,       KPVL::Z1::Data, &conn_kpvl)},    //����� �����
    {PlateData[1].PartNo        = new Value(AppGenSeqToHmi1 + "Z1.PartNo",              HWNDCLIENT::hEditPlate_DataZ1_PartNo,     KPVL::Z1::Data, &conn_kpvl)},    //����� ������
    {PlateData[1].Pack          = new Value(AppGenSeqToHmi1 + "Z1.Pack",                HWNDCLIENT::hEditPlate_DataZ1_Pack,       KPVL::Z1::Data, &conn_kpvl)},    //����� �����
    {PlateData[1].Sheet         = new Value(AppGenSeqToHmi1 + "Z1.Sheet",               HWNDCLIENT::hEditPlate_DataZ1_Sheet,      KPVL::Z1::Data, &conn_kpvl)},    //����� �����
    {PlateData[1].SubSheet      = new Value(AppGenSeqToHmi1 + "Z1.SubSheet",            HWNDCLIENT::hEditPlate_DataZ1_SubSheet,   KPVL::Z1::Data, &conn_kpvl)},    //����� ��������
#pragma endregion

//���� 2 ������ ������ (�������)
#pragma region ���� 2 ������ ������ (�������)
    {PlateData[2].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z2.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ2_AlloyCode,  KPVL::Z2::DataAlloyThikn, &conn_kpvl)}, //����� �����
    {PlateData[2].ThiknessText  = new Value(AppGenSeqToHmi1 + "Z2.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ2_Thikness,   KPVL::Z2::DataAlloyThikn, &conn_kpvl)}, //������� �����
    {PlateData[2].Melt          = new Value(AppGenSeqToHmi1 + "Z2.Melt",                HWNDCLIENT::hEditPlate_DataZ2_Melt,       KPVL::Z2::Data, &conn_kpvl)},    //����� ������
    {PlateData[2].Slab          = new Value(AppGenSeqToHmi1 + "Z2.Slab",                HWNDCLIENT::hEditPlate_DataZ2_Slab,       KPVL::Z2::Data, &conn_kpvl)},    //����� �����
    {PlateData[2].PartNo        = new Value(AppGenSeqToHmi1 + "Z2.PartNo",              HWNDCLIENT::hEditPlate_DataZ2_PartNo,     KPVL::Z2::Data, &conn_kpvl)},    //����� ������
    {PlateData[2].Pack          = new Value(AppGenSeqToHmi1 + "Z2.Pack",                HWNDCLIENT::hEditPlate_DataZ2_Pack,       KPVL::Z2::Data, &conn_kpvl)},    //����� �����
    {PlateData[2].Sheet         = new Value(AppGenSeqToHmi1 + "Z2.Sheet",               HWNDCLIENT::hEditPlate_DataZ2_Sheet,      KPVL::Z2::Data, &conn_kpvl)},    //����� �����
    {PlateData[2].SubSheet      = new Value(AppGenSeqToHmi1 + "Z2.SubSheet",            HWNDCLIENT::hEditPlate_DataZ2_SubSheet,   KPVL::Z2::Data, &conn_kpvl)}, //����� ��������
#pragma endregion

//���� 3 ������ ������ (���������)
#pragma region ���� 3 ������ ������ (���������)
    {PlateData[3].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z3.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ3_AlloyCode,  KPVL::Z3::DataAlloyThikn, &conn_kpvl)}, //��� �����
    {PlateData[3].ThiknessText  = new Value(AppGenSeqToHmi1 + "Z3.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ3_Thikness,   KPVL::Z3::DataAlloyThikn, &conn_kpvl)}, //������� �����
    {PlateData[3].Melt          = new Value(AppGenSeqToHmi1 + "Z3.Melt",                HWNDCLIENT::hEditPlate_DataZ3_Melt,       KPVL::Z3::Data, &conn_kpvl)},    //����� ������
    {PlateData[3].Slab          = new Value(AppGenSeqToHmi1 + "Z3.Slab",                HWNDCLIENT::hEditPlate_DataZ3_Slab,       KPVL::Z3::Data, &conn_kpvl)},    //����� �����
    {PlateData[3].PartNo        = new Value(AppGenSeqToHmi1 + "Z3.PartNo",              HWNDCLIENT::hEditPlate_DataZ3_PartNo,     KPVL::Z3::Data, &conn_kpvl)},    //����� ������
    {PlateData[3].Pack          = new Value(AppGenSeqToHmi1 + "Z3.Pack",                HWNDCLIENT::hEditPlate_DataZ3_Pack,       KPVL::Z3::Data, &conn_kpvl)},    //����� �����
    {PlateData[3].Sheet         = new Value(AppGenSeqToHmi1 + "Z3.Sheet",               HWNDCLIENT::hEditPlate_DataZ3_Sheet,      KPVL::Z3::Data, &conn_kpvl)},    //����� �����
    {PlateData[3].SubSheet      = new Value(AppGenSeqToHmi1 + "Z3.SubSheet",            HWNDCLIENT::hEditPlate_DataZ3_SubSheet,   KPVL::Z3::Data, &conn_kpvl)},    //����� ��������
#pragma endregion

//���� 4 ������ ������ (����������)
#pragma region ���� 4 ������ ������ (����������)
    {PlateData[4].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z4.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ4_AlloyCode,  KPVL::Z4::DataAlloyThikn, &conn_kpvl)}, //����� �����
    {PlateData[4].ThiknessText  = new Value(AppGenSeqToHmi1 + "Z4.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ4_Thikness,   KPVL::Z4::DataAlloyThikn, &conn_kpvl)}, //������� �����
    {PlateData[4].Melt          = new Value(AppGenSeqToHmi1 + "Z4.Melt",                HWNDCLIENT::hEditPlate_DataZ4_Melt,       KPVL::Z4::Data, &conn_kpvl)},    //����� ������
    {PlateData[4].Slab          = new Value(AppGenSeqToHmi1 + "Z4.Slab",                HWNDCLIENT::hEditPlate_DataZ4_Slab,       KPVL::Z4::Data, &conn_kpvl)},    //����� �����
    {PlateData[4].PartNo        = new Value(AppGenSeqToHmi1 + "Z4.PartNo",              HWNDCLIENT::hEditPlate_DataZ4_PartNo,     KPVL::Z4::Data, &conn_kpvl)},    //����� ������
    {PlateData[4].Pack          = new Value(AppGenSeqToHmi1 + "Z4.Pack",                HWNDCLIENT::hEditPlate_DataZ4_Pack,       KPVL::Z4::Data, &conn_kpvl)},    //����� �����
    {PlateData[4].Sheet         = new Value(AppGenSeqToHmi1 + "Z4.Sheet",               HWNDCLIENT::hEditPlate_DataZ4_Sheet,      KPVL::Z4::Data, &conn_kpvl)},    //����� �����
    {PlateData[4].SubSheet      = new Value(AppGenSeqToHmi1 + "Z4.SubSheet",            HWNDCLIENT::hEditPlate_DataZ4_SubSheet,   KPVL::Z4::Data, &conn_kpvl)},    //����� ��������
#pragma endregion


//���� 5 ������
#pragma region ���� 5 ������
    {PlateData[5].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z5.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ5_AlloyCode,  KPVL::Z5::DataAlloyThikn, &conn_kpvl)}, //����� �����
    {PlateData[5].ThiknessText  = new Value(AppGenSeqToHmi1 + "Z5.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ5_Thikness,   KPVL::Z5::DataAlloyThikn, &conn_kpvl)}, //������� �����
    {PlateData[5].Melt          = new Value(AppGenSeqToHmi1 + "Z5.Melt",                HWNDCLIENT::hEditPlate_DataZ5_Melt,       KPVL::Z5::Data, &conn_kpvl)},    //����� ������
    {PlateData[5].Slab          = new Value(AppGenSeqToHmi1 + "Z5.Slab",                HWNDCLIENT::hEditPlate_DataZ5_Slab,       KPVL::Z5::Data, &conn_kpvl)},    //����� �����
    {PlateData[5].PartNo        = new Value(AppGenSeqToHmi1 + "Z5.PartNo",              HWNDCLIENT::hEditPlate_DataZ5_PartNo,     KPVL::Z5::Data, &conn_kpvl)},    //����� ������
    {PlateData[5].Pack          = new Value(AppGenSeqToHmi1 + "Z5.Pack",                HWNDCLIENT::hEditPlate_DataZ5_Pack,       KPVL::Z5::Data, &conn_kpvl)},    //����� �����
    {PlateData[5].Sheet         = new Value(AppGenSeqToHmi1 + "Z5.Sheet",               HWNDCLIENT::hEditPlate_DataZ5_Sheet,      KPVL::Z5::Data, &conn_kpvl)},    //����� �����
    {PlateData[5].SubSheet      = new Value(AppGenSeqToHmi1 + "Z5.SubSheet",            HWNDCLIENT::hEditPlate_DataZ5_SubSheet,   KPVL::Z5::Data, &conn_kpvl)},    //����� ��������
#pragma endregion


//���� �� ��������
#pragma region ���� �� ��������
    {PlateData[6].AlloyCodeText = new Value(AppHMISheetData1 + "AlloyCodeText.sText", HWNDCLIENT::hEdit_Sheet_AlloyCode,    KPVL::Z6::DataAlloyThikn, &conn_kpvl)},   //����� �����
    {PlateData[6].ThiknessText  = new Value(AppHMISheetData1 + "ThiknessText.sText",  HWNDCLIENT::hEdit_Sheet_Thikness,     KPVL::Z6::DataAlloyThikn, &conn_kpvl)},   //������� �����
    {PlateData[6].Melt          = new Value(AppHMISheetData1 + "Melt",                HWNDCLIENT::hEdit_Sheet_Melt,         KPVL::Z6::Data, &conn_kpvl)},      //����� ������
    {PlateData[6].Slab          = new Value(AppHMISheetData1 + "Slab",                HWNDCLIENT::hEdit_Sheet_Slab,         KPVL::Z6::Data, &conn_kpvl)},      //����� �����
    {PlateData[6].PartNo        = new Value(AppHMISheetData1 + "PartNo",              HWNDCLIENT::hEdit_Sheet_PartNo,       KPVL::Z6::Data, &conn_kpvl)},      //����� ������
    {PlateData[6].Pack          = new Value(AppHMISheetData1 + "Pack",                HWNDCLIENT::hEdit_Sheet_Pack,         KPVL::Z6::Data, &conn_kpvl)},      //����� �����
    {PlateData[6].Sheet         = new Value(AppHMISheetData1 + "Sheet",               HWNDCLIENT::hEdit_Sheet_Sheet,        KPVL::Z6::Data, &conn_kpvl)},      //����� �����
    {PlateData[6].SubSheet      = new Value(AppHMISheetData1 + "SubSheet",            HWNDCLIENT::hEdit_Sheet_SubSheet,     KPVL::Z6::Data, &conn_kpvl)},      //����� ��������

    {HMISheetData.Cassette.CassetteNo       = new Value(AppHMISheetData2 + "CassetteNo",          HWNDCLIENT::hEdit_Sheet_CassetteNew,      KPVL::Z6::CassetteNo, &conn_kpvl)},       //����� ������� �� ����
    {HMISheetData.Cassette.Day              = new Value(AppHMISheetData2 + "Day",                 HWNDCLIENT::hEdit_Sheet_Cassette_Day,     KPVL::Z6::InitCassette, &conn_kpvl)},     //���� ID �����
    {HMISheetData.Cassette.Month            = new Value(AppHMISheetData2 + "Month",               HWNDCLIENT::hEdit_Sheet_Cassette_Month,   KPVL::Z6::InitCassette, &conn_kpvl)},     //����� ID �����
    {HMISheetData.Cassette.Year             = new Value(AppHMISheetData2 + "Year",                HWNDCLIENT::hEdit_Sheet_Cassette_Year,    KPVL::Z6::InitCassette, &conn_kpvl)},     //��� ID �����
    {HMISheetData.Cassette.SheetInCassette  = new Value(AppHMISheetData2 + "SheetInCassette",     HWNDCLIENT::hEdit_Sheet_InCassette,       KPVL::Z6::Sheet_InCassette, &conn_kpvl)}, //����� ����� � ������
#pragma endregion


//���������� ����� �� ��������
#pragma region ���������� ����� �� ��������
    {HMISheetData.Top_Side.h1 = new Value(AppHMISheetData + "Top_Side.h1",     HWNDCLIENT::hNull, KPVL::Side::SheetTop1, &conn_kpvl)}, //���������� �� ��������
    {HMISheetData.Top_Side.h2 = new Value(AppHMISheetData + "Top_Side.h2",     HWNDCLIENT::hNull, KPVL::Side::SheetTop2, &conn_kpvl)}, //���������� �� ��������
    {HMISheetData.Top_Side.h3 = new Value(AppHMISheetData + "Top_Side.h3",     HWNDCLIENT::hNull, KPVL::Side::SheetTop3, &conn_kpvl)}, //���������� �� ��������
    {HMISheetData.Top_Side.h4 = new Value(AppHMISheetData + "Top_Side.h4",     HWNDCLIENT::hNull, KPVL::Side::SheetTop4, &conn_kpvl)}, //���������� �� ��������
    {HMISheetData.Top_Side.h5 = new Value(AppHMISheetData + "Top_Side.h5",     HWNDCLIENT::hNull, KPVL::Side::SheetTop5, &conn_kpvl)}, //���������� �� ��������
    {HMISheetData.Top_Side.h6 = new Value(AppHMISheetData + "Top_Side.h6",     HWNDCLIENT::hNull, KPVL::Side::SheetTop6, &conn_kpvl)}, //���������� �� ��������
    {HMISheetData.Top_Side.h7 = new Value(AppHMISheetData + "Top_Side.h7",     HWNDCLIENT::hNull, KPVL::Side::SheetTop7, &conn_kpvl)}, //���������� �� ��������
    {HMISheetData.Top_Side.h8 = new Value(AppHMISheetData + "Top_Side.h8",     HWNDCLIENT::hNull, KPVL::Side::SheetTop8, &conn_kpvl)}, //���������� �� ��������

    {HMISheetData.Bot_Side.h1 = new Value(AppHMISheetData + "Bottom_Side.h1",  HWNDCLIENT::hNull, KPVL::Side::SheetBot1, &conn_kpvl)}, //���������� ����� ��������
    {HMISheetData.Bot_Side.h2 = new Value(AppHMISheetData + "Bottom_Side.h2",  HWNDCLIENT::hNull, KPVL::Side::SheetBot1, &conn_kpvl)}, //���������� ����� ��������
    {HMISheetData.Bot_Side.h3 = new Value(AppHMISheetData + "Bottom_Side.h3",  HWNDCLIENT::hNull, KPVL::Side::SheetBot1, &conn_kpvl)}, //���������� ����� ��������
    {HMISheetData.Bot_Side.h4 = new Value(AppHMISheetData + "Bottom_Side.h4",  HWNDCLIENT::hNull, KPVL::Side::SheetBot1, &conn_kpvl)}, //���������� ����� ��������
    {HMISheetData.Bot_Side.h5 = new Value(AppHMISheetData + "Bottom_Side.h5",  HWNDCLIENT::hNull, KPVL::Side::SheetBot1, &conn_kpvl)}, //���������� ����� ��������
    {HMISheetData.Bot_Side.h6 = new Value(AppHMISheetData + "Bottom_Side.h6",  HWNDCLIENT::hNull, KPVL::Side::SheetBot1, &conn_kpvl)}, //���������� ����� ��������
    {HMISheetData.Bot_Side.h7 = new Value(AppHMISheetData + "Bottom_Side.h7",  HWNDCLIENT::hNull, KPVL::Side::SheetBot1, &conn_kpvl)}, //���������� ����� ��������
    {HMISheetData.Bot_Side.h8 = new Value(AppHMISheetData + "Bottom_Side.h8",  HWNDCLIENT::hNull, KPVL::Side::SheetBot1, &conn_kpvl)}, //���������� ����� ��������
#pragma endregion

//��� ����������� �����
#pragma region ��� ����������� �����
    {GenSeqToHmi.Seq_1_StateNo = new Value(AppGenSeqToHmi + "Seq_1_StateNo",  HWNDCLIENT::hEditState_11, KPVL::ZState::DataPosState_1, &conn_kpvl)},    //����� ���� ������������������ �������� � ����
    {GenSeqToHmi.Seq_2_StateNo = new Value(AppGenSeqToHmi + "Seq_2_StateNo",  HWNDCLIENT::hEditState_21, KPVL::ZState::DataPosState_2, &conn_kpvl)},    //����� ���� ������������������ �������� � ����
    {GenSeqToHmi.Seq_3_StateNo = new Value(AppGenSeqToHmi + "Seq_3_StateNo",  HWNDCLIENT::hEditState_31, KPVL::ZState::DataPosState_3, &conn_kpvl)},    //����� ���� ������������������ ��������� � ���������

    {GenSeqToHmi.HeatTime_Z1 = new Value(AppGenSeqToHmi + "HeatTime_Z1",  HWNDCLIENT::hEditState_13, 0, &conn_kpvl)},      //����� ������� � ���� 1
    {GenSeqToHmi.HeatTime_Z2 = new Value(AppGenSeqToHmi + "HeatTime_Z2",  HWNDCLIENT::hEditState_23, 0, &conn_kpvl)},      //����� ������� � ���� 2
    {GenSeqToHmi.CoolTimeAct = new Value(AppGenSeqToHmi + "CoolTimeAct",  HWNDCLIENT::hEditState_33, 0, &conn_kpvl)},      //������� ������ ������� ����������
#pragma endregion
#endif

//����������� ����
#pragma region ����������� ����
    {Hmi210_1.Htr1_1  = new Value(PathKpvl + "Hmi210_1.Htr_1.ToHmi.TAct",   HWNDCLIENT::hEditTemp11TAct, 0, &conn_kpvl)},       //����������� � ���� 1.1
    {Hmi210_1.Htr1_2  = new Value(PathKpvl + "Hmi210_1.Htr_2.ToHmi.TAct",   HWNDCLIENT::hEditTemp12TAct, 0, &conn_kpvl)},       //����������� � ���� 1.2
    {Hmi210_1.Htr1_3  = new Value(PathKpvl + "Hmi210_1.Htr_3.ToHmi.TAct",   HWNDCLIENT::hEditTemp13TAct, 0, &conn_kpvl)},       //����������� � ���� 1.3
    {Hmi210_1.Htr1_4  = new Value(PathKpvl + "Hmi210_1.Htr_4.ToHmi.TAct",   HWNDCLIENT::hEditTemp14TAct, 0, &conn_kpvl)},       //����������� � ���� 1.4
    {Hmi210_1.Htr2_1 = new Value(PathKpvl + "Hmi210_1.Htr2_1.ToHmi.TAct",  HWNDCLIENT::hEditTemp21TAct, 0, &conn_kpvl)},       //����������� � ���� 2.1
    {Hmi210_1.Htr2_2 = new Value(PathKpvl + "Hmi210_1.Htr2_2.ToHmi.TAct",  HWNDCLIENT::hEditTemp22TAct, 0, &conn_kpvl)},       //����������� � ���� 2.2
    {Hmi210_1.Htr2_3 = new Value(PathKpvl + "Hmi210_1.Htr2_3.ToHmi.TAct",  HWNDCLIENT::hEditTemp23TAct, 0, &conn_kpvl)},       //����������� � ���� 2.3
    {Hmi210_1.Htr2_4 = new Value(PathKpvl + "Hmi210_1.Htr2_4.ToHmi.TAct",  HWNDCLIENT::hEditTemp24TAct, 0, &conn_kpvl)},       //����������� � ���� 2.4
#pragma endregion


    ////������ ����� � �������� �����
    //{TextData.PlateText.PlateTypeArray[0].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[0].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy0, 0)},
    //{TextData.PlateText.PlateTypeArray[1].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[1].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy1, 0)},
    //{TextData.PlateText.PlateTypeArray[2].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[2].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy2, 0)},
    //{TextData.PlateText.PlateTypeArray[3].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[3].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy3, 0)},
    //{TextData.PlateText.PlateTypeArray[4].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[4].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy4, 0)},
    //{TextData.PlateText.PlateTypeArray[5].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[5].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy5, 0)},
    //{TextData.PlateText.PlateTypeArray[6].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[6].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy6, 0)},
    //{TextData.PlateText.PlateTypeArray[7].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[7].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy7, 0)},
    //{TextData.PlateText.PlateTypeArray[8].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[8].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy8, 0)},
    //{TextData.PlateText.PlateTypeArray[9].sText = new Value(PathKpvl + "TextData.PlateText.PlateTypeArray[9].sText",  PLCsec1, "", HWNDCLIENT::hEditAlloy9, 0)},
    //
    //{TextData.PlateText.PlateThicknessArray[0].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[0].sText",  PLCsec1, "", HWNDCLIENT::hEditThick0, 0)},
    //{TextData.PlateText.PlateThicknessArray[1].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[1].sText",  PLCsec1, "", HWNDCLIENT::hEditThick1, 0)},
    //{TextData.PlateText.PlateThicknessArray[2].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[2].sText",  PLCsec1, "", HWNDCLIENT::hEditThick2, 0)},
    //{TextData.PlateText.PlateThicknessArray[3].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[3].sText",  PLCsec1, "", HWNDCLIENT::hEditThick3, 0)},
    //{TextData.PlateText.PlateThicknessArray[4].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[4].sText",  PLCsec1, "", HWNDCLIENT::hEditThick4, 0)},
    //{TextData.PlateText.PlateThicknessArray[5].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[5].sText",  PLCsec1, "", HWNDCLIENT::hEditThick5, 0)},
    //{TextData.PlateText.PlateThicknessArray[6].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[6].sText",  PLCsec1, "", HWNDCLIENT::hEditThick6, 0)},
    //{TextData.PlateText.PlateThicknessArray[7].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[7].sText",  PLCsec1, "", HWNDCLIENT::hEditThick7, 0)},
    //{TextData.PlateText.PlateThicknessArray[8].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[8].sText",  PLCsec1, "", HWNDCLIENT::hEditThick8, 0)},
    //{TextData.PlateText.PlateThicknessArray[9].sText = new Value(PathKpvl + "TextData.PlateText.PlateThicknessArray[9].sText",  PLCsec1, "", HWNDCLIENT::hEditThick9, 0)},
};


bool cmpAllTagKpvl(Value* first, Value* second)
{
    return first->Patch > second->Patch;
}

std::map<MSSEC, ChannelSubscription>cssKPVL;


ClassDataChangeKPVL DataChangeKPVL;

void ClassDataChangeKPVL::DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr)
{
    std::string patch = "";
    if(isRun)
    {
        if(attr == OpcUa::AttributeId::Value)
        {
            try
            {
                if(WatchDogWait)
                {
                    SetWindowText(winmap(hEditDiagnose8), (KPVL::ServerDataTime + " (" + std::to_string(WatchDogWait) + ")").c_str());
                }
                OpcUa::NodeId id = node.GetId();
                if(id.IsInteger())
                {
                    if((uint32_t)OpcUa::ObjectId::Server_ServerStatus_CurrentTime == id.GetIntegerIdentifier())
                    {
                        WatchDog = TRUE; //��� �����

                        KPVL::ServerDataTime = val.ToString();
                        SetWindowText(winmap(hEditTimeServer), KPVL::ServerDataTime.c_str());
                    }
                }
                else if(id.IsString())
                {
                    SetWindowText(winmap(hEditMode1), "������ ������...");
                    patch = id.GetStringIdentifier();
                    OpcUa::Variant vals = val;
                    for(auto& a : AllTagKpvl)
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
                            SetWindowText(winmap(hEditMode1), "��� ������...");
                            return;
                        }
                    }
                    SetWindowText(winmap(hEditMode1), "��� ������...");
                }
            }
            catch(std::runtime_error& exc)
            {
                SetWindowText(winmap(hEditMode1), "DataChange runtime_error");
                LOG_ERROR(HardLogger, "{:90| DataChange Error {}, {}", FUNCTION_LINE_NAME, exc.what(), node.ToString());
            }
            catch(...)
            {
                SetWindowText(winmap(hEditMode1), "Unknown error");
                LOG_ERROR(HardLogger, "{:90| DataChange Error 'Unknown error' {}", FUNCTION_LINE_NAME, node.ToString());
            };
        }
    }
}

void PLC_KPVL::InitNodeId()
{
    LOG_INFO(Logger, "{:90}| ������������� �����... countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    LOG_INFO(Logger, "{:90}| �������� Patch = Server_ServerStatus_CurrentTime", FUNCTION_LINE_NAME);
    
    nodeCurrentTime = GetNode(OpcUa::ObjectId::Server_ServerStatus_CurrentTime); //Node ������� �����

    if(nodeCurrentTime.IsValid())
    {
        OpcUa::Variant val = nodeCurrentTime.GetValue();
        KPVL::ServerDataTime = val.ToString();
        SetWindowText(winmap(hEditTimeServer), KPVL::ServerDataTime.c_str());
        SetWindowText(winmap(hEditDiagnose8), KPVL::ServerDataTime.c_str());
    }

    LOG_INFO(Logger, "{:90}| ������������� NodeId", FUNCTION_LINE_NAME);
    for(auto& a : AllTagKpvl)
    {
        a->InitNodeId(this);
    }

#ifndef TESTTEMPER
    Par_Gen.UnloadSpeed->coeff = 1000;
#endif
}

void PLC_KPVL::InitTag()
{
    LOG_INFO(Logger, "{:90}| ������������� ����������... countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    try
    {
        //�������� Subscribe
        LOG_INFO(Logger, "{:90}| �������� Subscribe countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);

        CREATESUBSCRIPT(cssKPVL, sec00500, &DataChangeKPVL, Logger);
        CREATESUBSCRIPT(cssKPVL, PLCsec1, &DataChangeKPVL, Logger);
        CREATESUBSCRIPT(cssKPVL, PLCsec2, &DataChangeKPVL, Logger);
        CREATESUBSCRIPT(cssKPVL, PLCsec5, &DataChangeKPVL, Logger);

        cssKPVL[sec00500].Subscribe(nodeCurrentTime);
    }
    catch(std::runtime_error& exc)
    {
        LOG_ERROR(Logger, "{:90}| Error {}", FUNCTION_LINE_NAME, exc.what());
        throw std::runtime_error(exc);
    }
    catch(...)
    {
        LOG_ERROR(Logger, "{:90}| Unknown error", FUNCTION_LINE_NAME);
        throw;
    }


    std::map< MSSEC, std::vector<OpcUa::ReadValueId>> avid;
    for(auto& a : AllTagKpvl)
            avid[a->Sec].push_back({a->NodeId, OpcUa::AttributeId::Value});

    LOG_INFO(Logger, "{:90}| cssKPVL {} ms, count {}", FUNCTION_LINE_NAME, sec00500, avid[sec00500].size());
    LOG_INFO(Logger, "{:90}| cssKPVL {} ms, count {}", FUNCTION_LINE_NAME, PLCsec1, avid[PLCsec1].size());
    LOG_INFO(Logger, "{:90}| cssKPVL {} ms, count {}", FUNCTION_LINE_NAME, PLCsec2, avid[PLCsec2].size());
    LOG_INFO(Logger, "{:90}| cssKPVL {} ms, count {}", FUNCTION_LINE_NAME, PLCsec5, avid[PLCsec5].size());


    for(auto& ar : avid)
    {
        try
        {
            if(ar.second.size())
            {
                LOG_INFO(Logger, "{:90}| SubscribeDataChange msec: {}, count: {}", FUNCTION_LINE_NAME, cssKPVL[ar.first].msec, ar.second.size());
                std::vector<uint32_t> monitoredItemsIds = cssKPVL[ar.first].sub->SubscribeDataChange(ar.second);
            }
        }
        catch(std::runtime_error& exc)
        {
            LOG_INFO(Logger, "{:90}| Error {}", FUNCTION_LINE_NAME, exc.what());
            for(auto& a : AllTagKpvl)
            {
                if(!a->TestNode(this))
                    LOG_ERROR(Logger, "{:90}| Error Patch: {}", FUNCTION_LINE_NAME, a->Patch);
            }

            throw std::runtime_error(exc);
        }
        catch(...)
        {
            LOG_INFO(Logger, "{:90}| Unknown error", FUNCTION_LINE_NAME);
            for(auto& a : AllTagKpvl)
            {
                if(!a->TestNode(this))
                    LOG_ERROR(Logger, "{:90}| Error Patch: {}", FUNCTION_LINE_NAME, a->Patch);
            }
            throw;
        }
    }
}

//void PLC_KPVL::Connect()
//{
//    while(isRun)
//    {
//        try
//        {
//            countconnect2++;
//            HARD_LOGGER(std::string("����� PLC_KPVL::Run") + " countconnect = " + std::to_string(countconnect1) + "." + std::to_string(countconnect2));
//
//            SetWindowText(winmap(hEditDiagnose7), (std::to_string(countconnect1) + "." + std::to_string(countconnect2)).c_str());
//
//            //client->DefaultTimeout = 59000;
//            //client->KeepAlive
//            client->Connect(Uri);
//            return;
//        }
//        catch(std::runtime_error& exc)
//        {
//            HARD_LOGGER2(std::string("Run: Error Connect: ") + exc.what() + " countconnect = " + std::to_string(countconnect1) + "." + std::to_string(countconnect2));
//        }
//        catch(...)
//        {
//            HARD_LOGGER2(std::string("Run: Error Connect: ") + "Unknown error countconnect = " + std::to_string(countconnect1) + "." + std::to_string(countconnect2));
//        };
//        Sleep(2000);
//    }
//}

void PLC_KPVL::Run(int count)
{
    countconnect1 = count;
    countconnect2 = 0;
    DataChangeKPVL.WatchDogWait = 0;

    LOG_INFO(Logger, "{:90}| Run... : countconnect = {}.{} to: {}", FUNCTION_LINE_NAME, countconnect1, countconnect2, Uri);

    SetWindowText(winmap(hEditDiagnose6), std::to_string(DataChangeKPVL.WatchDogWait).c_str());

    try
    {
        DataChangeKPVL.InitGoot = FALSE;
        countget = 1;

        client = std::shared_ptr<OpcUa::UaClient>(new OpcUa::UaClient(Logger));

        SetWindowText(winmap(hEditMode1), "Connect");
        w1 = hEditDiagnose7;
        Connect();


        SetWindowText(winmap(hEditMode1), "GetRoot");
        GetRoot();

        SetWindowText(winmap(hEditMode1), "GetNameSpace");
        GetNameSpace();


        SetWindowText(winmap(hEditMode1), "InitNodeId");
        InitNodeId();

        SetWindowText(winmap(hEditMode1), "InitTag");
        InitTag();

        LOG_INFO(Logger, "{:90}| ����������� ������� countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);

        DataChangeKPVL.InitGoot = TRUE;
        ULONGLONG time1 = GetTickCount64();

        isInitPLC_KPVL = true;
        SekRun = time(NULL);
        SetWindowText(winmap(hEditMode1), "������ ������");
        int  NewDataVal = 0;
        while(isRun)
        {
            //��������� �� ����� ���� �� ��������
            if(HMISheetData.NewData->Val.As<bool>())                   //���� ���� �����
            {
                if(++NewDataVal > 5)
                {
                    LOG_INFO(Logger, "{:90}| SaveDone.Set_Value (true)", FUNCTION_LINE_NAME);
                    if(!MyServer)
                    {
                        LOG_INFO(SQLLogger, "{:90}| SaveDone->Set_Value(true)", FUNCTION_LINE_NAME);
                        KPVL::Z6::SetSaveDone();
                        //HMISheetData.SaveDone->Set_Value(true);
                    }
                }
            }
            else
            {
                NewDataVal = 0;
            }


            //��������� WatchDog
            if(WD())
                throw std::runtime_error(std::string(std::string("����������: ���� ����� ��� ������ ") + std::to_string(CountWatchDogWait) + " ������").c_str());
            /*
            if(bSendNewSheet)
            {
                SetNewSheet();
            }
            if(bSendNewCassette)
            {
                SetNewCassette();
            }
            */

            TestTimeRun(time1);

            //������� ����� ������
            GetWD();

        }

        SetWindowText(winmap(hEditMode1), "delete Sub");
        for(auto s : cssKPVL)s.second.Delete();

#if NEWS
        SetWindowText(winmap(hEditMode1), "reset");
        client.reset();
#else
        delete client;
#endif
        return;
    }
    catch(std::runtime_error& exc)
    {
        LOG_ERROR(Logger, "{:90}| Error {} countconnect = {}.{}", FUNCTION_LINE_NAME, exc.what(), countconnect1, countconnect2);
    }
    catch(...)
    {
        LOG_ERROR(Logger, "{:90}| Unknown error countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    };

    LOG_INFO(Logger, "{:90}| ����� �� ������ 1 countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    if(isRun)
    {
        LOG_INFO(Logger, "{:90}| ���� 5 ������... ��� {}", FUNCTION_LINE_NAME, Uri);
        Sleep(5000);
    }

    try
    {
        LOG_INFO(Logger, "{:90}| delete Sub countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
        SetWindowText(winmap(hEditMode1), "delete Sub");
        for(auto s : cssKPVL)s.second.Delete();

#if NEWS
        LOG_INFO(Logger, "{:90}| reset countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
        SetWindowText(winmap(hEditMode1), "reset");
        client.reset();
#else
        delete client;
#endif
    }
    catch(std::runtime_error& exc)
    {
        LOG_ERROR(Logger, "{:90}| Error {} countconnect = {}.{}", FUNCTION_LINE_NAME, exc.what(), countconnect1, countconnect2);
    }
    catch(...)
    {
        LOG_ERROR(Logger, "{:90}| Unknown error countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    };

    LOG_INFO(Logger, "{:90}| ... ����� {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
};

void PLC_KPVL::GetWD()
{
    time_t old = time(NULL);
    time_t dt = (time_t)difftime(old, SekRun);
    struct tm TM;
    errno_t err = gmtime_s(&TM, &dt);

    if(!err)
    {
        char sFormat[50];
        sprintf_s(sFormat, 50, "%04d-%02d-%02d %02d:%02d:%02d", TM.tm_year - 70, TM.tm_mon, TM.tm_mday - 1, TM.tm_hour, TM.tm_min, TM.tm_sec);
        SetWindowText(winmap(hEditDiagnose9), sFormat);

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
    //    LOG_CATCH("������ ������ HMISheetData.Sheet.WDG_fromBase.Set_Value");
    //
    //}
}


bool PLC_KPVL::WD()
{
    //��� �����
    if(DataChangeKPVL.WatchDog == TRUE)
    {
        DataChangeKPVL.WatchDog = FALSE;
        if(DataChangeKPVL.WatchDogWait)
        {
            if(DataChangeKPVL.WatchDogWait >= CountWatchDogWait - 1)
                LOG_INFO(Logger, "{:90}| ���������� ����� {}", FUNCTION_LINE_NAME, DataChangeKPVL.WatchDogWait);
            SetWindowText(winmap(hEditDiagnose6), std::to_string(DataChangeKPVL.WatchDogWait).c_str());
            SetWindowText(winmap(hEditDiagnose8), KPVL::ServerDataTime.c_str());
        }
        DataChangeKPVL.WatchDogWait = 0;
    }
    else
    {
        DataChangeKPVL.WatchDogWait++; //�������������� ������� ������ �����
        if(DataChangeKPVL.WatchDogWait >= CountWatchDogWait)
        {
            LOG_INFO(Logger, "{:90}| ����������: ���� ����� ��� ������ {} ������", FUNCTION_LINE_NAME, CountWatchDogWait);
            SetWindowText(winmap(hEditDiagnose6), std::to_string(DataChangeKPVL.WatchDogWait).c_str());
            SetWindowText(winmap(hEditDiagnose8), KPVL::ServerDataTime.c_str());
            //SekRun = time(NULL);
            return true;
        }
        else
        {
            if(DataChangeKPVL.WatchDogWait >= CountWatchDogWait - 1)
                LOG_INFO(Logger, "{:90}| ���� ����� ��� ������ {} ������", FUNCTION_LINE_NAME, DataChangeKPVL.WatchDogWait);
            SetWindowText(winmap(hEditDiagnose6), std::to_string(DataChangeKPVL.WatchDogWait).c_str());
            SetWindowText(winmap(hEditDiagnose8), KPVL::ServerDataTime.c_str());
            //SekRun = time(NULL);
        }
    }
    return false;
}

void Open_KPVL_RUN()
{
    std::shared_ptr<spdlog::logger> Logger = HardLogger;

    LOG_INFO(Logger, "{:90}| ����� to: {}", FUNCTION_LINE_NAME, KPVL::URI);

    int countconnect = 1;
    LOG_INFO(Logger, "{:90}| �������� ������ PLC_KPVL {}", FUNCTION_LINE_NAME, countconnect);
    //������������� ������ ������ � ����� unique_ptr
    SetWindowText(winmap(hEditMode1), "�������� �������");
    auto PLC = std::unique_ptr<PLC_KPVL>(new PLC_KPVL(KPVL::URI, Logger));

    while(isRun)
    {
        try
        {
            ////�������������� ������ ������
            ////�������� ���� ���������
            //PLC_S107 CS_S107(Uri, PethLogger);
            //CS_S107.Run(countconnect);
            //
            ////������������� ������ ������ � ����� shared_ptr
            //auto CS_S107 = std::shared_ptr<PLC_S107>(new PLC_S107(Uri, PethLogger));
            //CS_S107->Run(countconnect);
            //CS_S107.reset();
            // 
            ////������������� ������ ������ � ����������� new/delete
            ////�� ���������� ������ � �������.
            //auto CS_S107 = new CS_S107(Uri, PethLogger);
            //CS_S107->Run(countconnect);
            //delete CS_S107;

            LOG_INFO(Logger, "{:90}| ����������� {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);
            PLC->Run(countconnect);
        }
        catch(std::runtime_error& exc)
        {
            SetWindowText(winmap(hEditMode1), "runtime_error");
            LOG_ERROR(Logger, "{:90}| Error {} countconnect = {} to: {}", FUNCTION_LINE_NAME, exc.what(), countconnect, KPVL::URI);
        }
        catch(...)
        {
            SetWindowText(winmap(hEditMode1), "Unknown error");
            LOG_ERROR(Logger, "{:90}| Unknown error countconnect = {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);
        };

        LOG_INFO(Logger, "{:90}| ����� �� ������ countconnect = {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);

        if(isRun)
        {
            countconnect++;
            LOG_INFO(Logger, "{:90}| ��������� ������� {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);
            Sleep(1000);
        }
    }
    SetWindowText(winmap(hEditMode1), "�������� PLC");
    PLC.reset();

    LOG_INFO(Logger, "{:90}| ExitThread. isRun = {}", FUNCTION_LINE_NAME, isRun);
    //ExitThread(0);

}

void GetEndData(TSheet& sheet)
{
    std::string comand = "SELECT FROM todos WHERE pos > 2 AND id = " + sheet.id;
    PGresult* res = conn_spis.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);
}

extern std::string  FORMATTIME;

void UpdateSheetPos()
{
    std::string comand = "DELETE FROM sheet WHERE pos = 10 OR pos = 20 OR pos = 30";
    PGresult* res = conn_spis.PGexec(comand);
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    comand = "UPDATE sheet SET pos = 7 WHERE news = 1;";
    res = conn_spis.PGexec(comand);
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);
}

void GetDataTime_All(TSheet& TS)
{

}

void Open_KPVL_SQL()
{
    size_t old_count = 0;
    int f = 5;
    //int64_t Next = 1;
    
    LOG_INFO(SQLLogger, "{:90}| Start Open_KPVL_SQL", FUNCTION_LINE_NAME);
    while(isRun)
    {
        //SepState_2();
        UpdateSheetPos();

        KPVL::SQL::KPVL_SQL();
        for(auto& TS : AllSheet)
        {
            if(atof(TS.DataTime_All.c_str()) == 0)
            {
                GetDataTime_All(TS);
            }
        }
        size_t count = AllSheet.size();
        if(old_count != count)
        {
            old_count = count;

            ListView_DeleteAllItems(hwndSheet);
            for(size_t i = 0; i < count; i++)
                AddHistoriSheet(false);
        }
        else
        {
            int TopIndex = ListView_GetTopIndex(hwndSheet);
            int Index = ListView_GetNextItem(hwndSheet, -1, LVNI_SELECTED);

            InvalidateRect(hwndSheet, NULL, false);

            ListView_EnsureVisible(hwndSheet, TopIndex, FALSE); 
            ListView_SetItemState(hwndSheet, Index, LVIS_SELECTED, LVIS_OVERLAYMASK);

#ifndef TESTGRAFF
#else
            InitCurentTag();
            for(auto& val : AllTagKpvl)
                MySetWindowText(winmap(val->winId), val->GetString().c_str());

            for(auto& val : AllTagPeth)
                MySetWindowText(winmap(val->winId), val->GetString().c_str());
            f = 2;
#endif
        }

        //while(isRun &&  f--)
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    LOG_INFO(SQLLogger, "{:90} Stop Open_KPVL_SQL", FUNCTION_LINE_NAME);

}



void Open_KPVL()
{
    //auto f = AllTagKpvl[6];
    //���������� ���������� �� �����
    if(AllTagKpvl.size() > 2)
        std::sort(AllTagKpvl.begin() + 2, AllTagKpvl.end(), cmpAllTagKpvl);

    //T_PlateData *pp0 = PlateData[0];
    //PlateData[0] = &GenSeqFromHmi.PlateData;
    //PlateData[1] = &GenSeqToHmi.PlateData_Z1;
    //PlateData[2] = &GenSeqToHmi.PlateData_Z1;
    //PlateData[3] = &GenSeqToHmi.PlateData_Z1;
    //PlateData[4] = &GenSeqToHmi.PlateData_Z1;
    //PlateData[5] = &GenSeqToHmi.PlateData_Z5;
    //PlateData[6] = &PlateData[6];

    HardLogger = InitLogger("PLC_KPVL");

    
#ifndef TESTSPIS
#ifndef TESTWIN
#ifndef TESTGRAFF
    hKPVLURI = std::thread(Open_KPVL_RUN);
#endif
    hKPVLSQL = std::thread(Open_KPVL_SQL);
#endif
#endif


    

}

void WaitCloseKPVL(std::thread& h, std::string hamd)
{
    LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, hamd);
    if(h.joinable())
        h.join();
}

void WaitCloseKPVL(HANDLE h, std::string hamd)
{
    DWORD dwEvent = WaitForSingleObject(h, INFINITE);
    //if(dwEvent == WAIT_OBJECT_0)
    //    HARD_LOGGER2(std::string("Close_KPVL_FURN: WaitForSingleObject( " + hamd + " ) = WAIT_OBJECT_0 "))
    //else if(dwEvent == WAIT_ABANDONED)
    //    HARD_LOGGER2(std::string("Close_KPVL_FURN: WaitForSingleObject( " + hamd + " ) = WAIT_ABANDONED "))
    //else if(dwEvent == WAIT_TIMEOUT)
    //    HARD_LOGGER2(std::string("Close_KPVL_FURN: WaitForSingleObject( " + hamd + " ) = WAIT_TIMEOUT "))
    //else if(dwEvent == WAIT_FAILED)
    //    HARD_LOGGER2(std::string("Close_KPVL_FURN: WaitForSingleObject( " + hamd + " ) = WAIT_FAILED "))
    //else
    //    HARD_LOGGER2(std::string("Close_KPVL_FURN: WaitForSingleObject( " + hamd + " ) = ") + std::to_string(dwEvent) + " : ")
    //{
    //    Log(HardLogger, spdlog::level::level_enum::info, std::string("Close_KPVL_FURN: WaitForSingleObject( " + hamd + " ) = ") + std::to_string(dwEvent) + " : "); SendDebug(std::string("Close_KPVL_FURN: WaitForSingleObject( " + hamd + " ) = ") + std::to_string(dwEvent) + " : ");
    //}
}

void Close_KPVL()
{
    HANDLE* h = NULL;
    int size = 0;
        WaitCloseKPVL(hFindSheet, "hFindSheet");
        WaitCloseKPVL(hThreadState2, "ThreadState2");
        WaitCloseKPVL(hKPVLURI, "hKPVLURI");
        WaitCloseKPVL(hKPVLSQL, "hKPVLSQL");
}
