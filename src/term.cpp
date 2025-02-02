#include "pch.h"

#include "win.h"
#include "main.h"
#include "StringData.h"
#include "file.h"
#include "SQL.h"
#include "ValueTag.h"
#include "term.h"
#include "hard.h"
#include "Graff.h"
#include "Furn.h"
#include "pdf.h"

extern PGConnection conn_temp;

//#define SPKsec0 sec00500
//#define SPKsec1 sec01000
//#define SPKsec2 sec02000
//#define SPKsec5 sec05000

extern std::string lpLogDir;

std::shared_ptr<spdlog::logger> PethLogger = NULL;
std::shared_ptr<spdlog::logger> FurnLogger = NULL;
std::shared_ptr<spdlog::logger> TestLogger = NULL;

bool isInitPLC_S107 = false;

time_t PLC_S107_old_dt = 0;


HANDLE hS107URI = NULL;
HANDLE hS107SQL = NULL;
HANDLE hSQL = NULL;

std::deque<TCassette> AllCassette;

T_ForBase_RelFurn ForBase_RelFurn_1;
T_ForBase_RelFurn ForBase_RelFurn_2;

struct T_FcassetteArray{
	T_Fcassette cassette[7];
	Value* selected_cassetFurn1 = NULL;
	Value* selected_cassetFurn2 = NULL;
}FcassetteArray;


#define SETALLTAG2(_t, _f, _e, _s,  _d) SETALLTAG(PathPeth, _t, _f, _e, _s,  _d)


MAP_VALUE AllTagPeth = {
    
    #pragma region ������ ����
        {AppFurn1.WDG_toBase = new Value(StrFurn1 + "WDG_toBase",            HWNDCLIENT::hEditTimeServer_1,          S107::Furn1::Data_WDG_toBase, &conn_temp, true, 1, 1, MSSEC::sec01000, "", (bool)false, "������")}, //������
        {AppFurn1.WDG_fromBase = new Value(StrFurn1 + "WDG_fromBase",          HWNDCLIENT::hNull,                      0, &conn_temp, true, 1, 1, MSSEC::sec01000, "", (bool)false, "������ ��� �������� �����")}, //������ ��� �������� ����� 

        {AppFurn1.PointRef_1 = new Value(StrFurn1 + "PointRef_1",            HWNDCLIENT::RelF1_Edit_PointRef_1,      0, &conn_temp)}, //: REAL;  //������� �����������
        {AppFurn1.PointTime_1 = new Value(StrFurn1 + "PointTime_1",           HWNDCLIENT::RelF1_Edit_PointTime_1,     0, &conn_temp)}, //: REAL; //����� �������
        {AppFurn1.PointTime_2 = new Value(StrFurn1 + "PointDTime_2",          HWNDCLIENT::RelF1_Edit_PointDTime_2,    0, &conn_temp)}, //: REAL;//����� ��������

        {AppFurn1.ProcRun = new Value(StrFurn1 + "ProcRun",               HWNDCLIENT::RelF1_Edit_ProcRun,         S107::Furn1::ProcRun, &conn_temp)}, //: BOOL;//������
        {AppFurn1.ProcEnd = new Value(StrFurn1 + "ProcEnd",               HWNDCLIENT::RelF1_Edit_ProcEnd,         S107::Furn1::ProcEnd, &conn_temp)}, //: BOOL;//��������� ��������
        {AppFurn1.ProcFault = new Value(StrFurn1 + "ProcFault",             HWNDCLIENT::RelF1_Edit_ProcFault,       S107::Furn1::ProcError, &conn_temp)}, //: BOOL;//������ ��������

        {AppFurn1.ReturnCassetteCmd = new Value(StrFurn1 + "ReturnCassetteCmd",     HWNDCLIENT::hNull,                      S107::Furn1::ReturnCassetteCmd, &conn_temp)}, //: BOOL;//������� ������ � ������


        {AppFurn1.TimeProcSet = new Value(StrFurn1 + "TimeProcSet",           HWNDCLIENT::RelF1_Edit_TimeProcSet,     0, &conn_temp)}, //: REAL;//������ ����� �������� (�������), ���
        {AppFurn1.ProcTimeMin = new Value(StrFurn1 + "ProcTimeMin",           HWNDCLIENT::RelF1_Edit_ProcTimeMin,     0, &conn_temp)}, //: REAL;//����� ��������, ��� (0..XX)
        {AppFurn1.TimeToProcEnd = new Value(StrFurn1 + "TimeToProcEnd",         HWNDCLIENT::RelF1_Edit_TimeToProcEnd,   S107::Furn1::TimeToProcEnd, &conn_temp)}, //: REAL;//����� �� ��������� ��������, ���
        {AppFurn1.TempRef = new Value(StrFurn1 + "TempRef",               HWNDCLIENT::RelF1_Edit_TempRef,         0, &conn_temp)}, //: REAL;//�������� �������� �����������
        {AppFurn1.TempAct = new Value(StrFurn1 + "TempAct",               HWNDCLIENT::RelF1_Edit_TempAct,         0, &conn_temp)}, //: REAL;//����������� �������� �����������
        {AppFurn1.T1 = new Value(StrFurn1 + "T1",                    HWNDCLIENT::RelF1_Edit_T1,              0, &conn_temp)}, //: REAL; // ��������� 1
        {AppFurn1.T2 = new Value(StrFurn1 + "T2",                    HWNDCLIENT::RelF1_Edit_T2,              0, &conn_temp)}, //: REAL; // ��������� 2
        {AppFurn1.ActTimeHeatAcc = new Value(StrFurn1 + "ActTimeHeatAcc",        HWNDCLIENT::RelF1_Edit_TimeHeatAcc,     S107::Furn1::TimeHeatAcc, &conn_temp)}, //: REAL; // ���� ����� �������
        {AppFurn1.ActTimeHeatWait = new Value(StrFurn1 + "ActTimeHeatWait",       HWNDCLIENT::RelF1_Edit_TimeHeatWait,    S107::Furn1::TimeHeatWait, &conn_temp)}, //: REAL; // ���� ����� ��������
        {AppFurn1.ActTimeTotal = new Value(StrFurn1 + "ActTimeTotal",          HWNDCLIENT::RelF1_Edit_TimeTotal,       S107::Furn1::TimeTotal, &conn_temp)}, //: REAL; // ���� ����� �����

        {AppFurn1.Cassette.Hour = new Value(StrFurn1 + "Cassette.Hour",         HWNDCLIENT::RelF1_Edit_Cassette_Hour,   S107::Furn1::Hour, &conn_temp)}, ///ID ������ ���
        {AppFurn1.Cassette.Day = new Value(StrFurn1 + "Cassette.Day",          HWNDCLIENT::RelF1_Edit_Cassette_Day,    S107::Furn1::Day, &conn_temp)}, //ID ������ ����
        {AppFurn1.Cassette.Month = new Value(StrFurn1 + "Cassette.Month",        HWNDCLIENT::RelF1_Edit_Cassette_Month,  S107::Furn1::Month, &conn_temp)}, //ID ������ �����
        {AppFurn1.Cassette.Year = new Value(StrFurn1 + "Cassette.Year",         HWNDCLIENT::RelF1_Edit_Cassette_Year,   S107::Furn1::Year, &conn_temp)}, //ID ������ ���
        {AppFurn1.Cassette.CassetteNo = new Value(StrFurn1 + "Cassette.CaasetteNo",   HWNDCLIENT::RelF1_Edit_CassetteNo,      S107::Furn1::No, &conn_temp)}, //ID ������ �����
    #pragma endregion

    #pragma region ������ ����
        {AppFurn2.WDG_toBase = new Value(StrFurn2 + "WDG_toBase",            HWNDCLIENT::hEditTimeServer_2,          S107::Furn2::Data_WDG_toBase, &conn_temp, true, 1, 1, MSSEC::sec01000, "", (bool)false,  "������")}, //������
        {AppFurn2.WDG_fromBase = new Value(StrFurn2 + "WDG_fromBase",          HWNDCLIENT::hNull,                      0, &conn_temp, true, 1, 1, MSSEC::sec01000, "", (bool)false, "������ ��� �������� �����")}, //������ ��� �������� ����� 

        {AppFurn2.PointRef_1 = new Value(StrFurn2 + "PointRef_1",            HWNDCLIENT::RelF2_Edit_PointRef_1,      0, &conn_temp)}, //: REAL;//������� �����������
        {AppFurn2.PointTime_1 = new Value(StrFurn2 + "PointTime_1",           HWNDCLIENT::RelF2_Edit_PointTime_1,     0, &conn_temp)}, //: REAL;//����� �������
        {AppFurn2.PointTime_2 = new Value(StrFurn2 + "PointDTime_2",          HWNDCLIENT::RelF2_Edit_PointDTime_2,    0, &conn_temp)}, //: REAL;//����� ��������

        {AppFurn2.ProcRun = new Value(StrFurn2 + "ProcRun",               HWNDCLIENT::RelF2_Edit_ProcRun,         S107::Furn2::ProcRun, &conn_temp)}, //: BOOL;//������
        {AppFurn2.ProcEnd = new Value(StrFurn2 + "ProcEnd",               HWNDCLIENT::RelF2_Edit_ProcEnd,         S107::Furn2::ProcEnd, &conn_temp)}, //: BOOL;//��������� ��������
        {AppFurn2.ProcFault = new Value(StrFurn2 + "ProcFault",             HWNDCLIENT::RelF2_Edit_ProcFault,       S107::Furn2::ProcError, &conn_temp)}, //: BOOL;//������ ��������

        {AppFurn2.ReturnCassetteCmd = new Value(StrFurn2 + "ReturnCassetteCmd",     HWNDCLIENT::hNull,                      S107::Furn2::ReturnCassetteCmd, &conn_temp)}, //: BOOL;//������� ������ � ������

        {AppFurn2.TimeProcSet = new Value(StrFurn2 + "TimeProcSet",           HWNDCLIENT::RelF2_Edit_TimeProcSet,     0, &conn_temp)}, //: REAL;//������ ����� �������� (�������), ���
        {AppFurn2.ProcTimeMin = new Value(StrFurn2 + "ProcTimeMin",           HWNDCLIENT::RelF2_Edit_ProcTimeMin,     0, &conn_temp)}, //: REAL;//����� ��������, ��� (0..XX)
        {AppFurn2.TimeToProcEnd = new Value(StrFurn2 + "TimeToProcEnd",         HWNDCLIENT::RelF2_Edit_TimeToProcEnd,   S107::Furn2::TimeToProcEnd, &conn_temp)}, //: REAL;//����� �� ��������� ��������, ���


        {AppFurn2.TempRef = new Value(StrFurn2 + "TempRef",               HWNDCLIENT::RelF2_Edit_TempRef,         0, &conn_temp)}, //: REAL;//�������� �������� �����������
        {AppFurn2.TempAct = new Value(StrFurn2 + "TempAct",               HWNDCLIENT::RelF2_Edit_TempAct,         0, &conn_temp)}, //: REAL;//����������� �������� �����������
        {AppFurn2.T1 = new Value(StrFurn2 + "T1",                    HWNDCLIENT::RelF2_Edit_T1,              0, &conn_temp)}, //: REAL; // ��������� 1
        {AppFurn2.T2 = new Value(StrFurn2 + "T2",                    HWNDCLIENT::RelF2_Edit_T2,              0, &conn_temp)}, //: REAL; // ��������� 2

        {AppFurn2.ActTimeHeatAcc = new Value(StrFurn2 + "ActTimeHeatAcc",        HWNDCLIENT::RelF2_Edit_TimeHeatAcc,     S107::Furn2::TimeHeatAcc, &conn_temp)}, //: REAL; // ���� ����� �������
        {AppFurn2.ActTimeHeatWait = new Value(StrFurn2 + "ActTimeHeatWait",       HWNDCLIENT::RelF2_Edit_TimeHeatWait,    S107::Furn2::TimeHeatWait, &conn_temp)}, //: REAL; // ���� ����� ��������
        {AppFurn2.ActTimeTotal = new Value(StrFurn2 + "ActTimeTotal",          HWNDCLIENT::RelF2_Edit_TimeTotal,       S107::Furn2::TimeTotal, &conn_temp)}, //: REAL; // ���� ����� �����

        {AppFurn2.Cassette.Hour = new Value(StrFurn2 + "Cassette.Hour",         HWNDCLIENT::RelF2_Edit_Cassette_Hour,   S107::Furn2::Hour, &conn_temp)}, ///ID ������ ���
        {AppFurn2.Cassette.Day = new Value(StrFurn2 + "Cassette.Day",          HWNDCLIENT::RelF2_Edit_Cassette_Day,    S107::Furn2::Day, &conn_temp)}, ///ID ������ ����
        {AppFurn2.Cassette.Month = new Value(StrFurn2 + "Cassette.Month",        HWNDCLIENT::RelF2_Edit_Cassette_Month,  S107::Furn2::Month, &conn_temp)}, //ID ������ �����
        {AppFurn2.Cassette.Year = new Value(StrFurn2 + "Cassette.Year",         HWNDCLIENT::RelF2_Edit_Cassette_Year,   S107::Furn2::Year, &conn_temp)},  //ID ������ ���
        {AppFurn2.Cassette.CassetteNo = new Value(StrFurn2 + "Cassette.CaasetteNo",   HWNDCLIENT::RelF2_Edit_CassetteNo,      S107::Furn2::No, &conn_temp)}, //ID ������ �����
    #pragma endregion

    #pragma region ������ �����
        {AppCassette1.Year = new Value(StrCassette1 + "Year",       HWNDCLIENT::hEditState1_Year,     0, &conn_temp)}, //ID ������ ���
        {AppCassette1.Month = new Value(StrCassette1 + "Month",      HWNDCLIENT::hEditState1_Month,    0, &conn_temp)}, //ID ������ �����
        {AppCassette1.Day = new Value(StrCassette1 + "Day",        HWNDCLIENT::hEditState1_Day,      0, &conn_temp)}, //ID ������ ����
        {AppCassette1.Hour = new Value(StrCassette1 + "Hour",       HWNDCLIENT::hEditState1_Hour,     0, &conn_temp)}, //ID ������ ���
        {AppCassette1.CassetteNo = new Value(StrCassette1 + "CaasetteNo", HWNDCLIENT::hEditState1_Cassette, 0, &conn_temp)}, //ID ������ �����

        {AppCassette2.Year = new Value(StrCassette2 + "Year",       HWNDCLIENT::hEditState2_Year,     0, &conn_temp)}, //ID ������ ���
        {AppCassette2.Month = new Value(StrCassette2 + "Month",      HWNDCLIENT::hEditState2_Month,    0, &conn_temp)}, //ID ������ �����
        {AppCassette2.Day = new Value(StrCassette2 + "Day",        HWNDCLIENT::hEditState2_Day,      0, &conn_temp)}, //ID ������ ����
        {AppCassette2.Hour = new Value(StrCassette2 + "Hour",       HWNDCLIENT::hEditState2_Hour,     0, &conn_temp)}, //ID ������ ���
        {AppCassette2.CassetteNo = new Value(StrCassette2 + "CaasetteNo", HWNDCLIENT::hEditState2_Cassette, 0, &conn_temp)}, //ID ������ �����

        {AppCassette3.Year = new Value(StrCassette3 + "Year",       HWNDCLIENT::hEditState3_Year,     0, &conn_temp)}, //ID ������ ���
        {AppCassette3.Month = new Value(StrCassette3 + "Month",      HWNDCLIENT::hEditState3_Month,    0, &conn_temp)}, //ID ������ �����
        {AppCassette3.Day = new Value(StrCassette3 + "Day",        HWNDCLIENT::hEditState3_Day,      0, &conn_temp)}, //ID ������ ����
        {AppCassette3.Hour = new Value(StrCassette3 + "Hour",       HWNDCLIENT::hEditState3_Hour,     0, &conn_temp)}, //ID ������ ���
        {AppCassette3.CassetteNo = new Value(StrCassette3 + "CaasetteNo", HWNDCLIENT::hEditState3_Cassette, 0, &conn_temp)}, //ID ������ �����

        {AppCassette4.Year = new Value(StrCassette4 + "Year",       HWNDCLIENT::hEditState4_Year,     0, &conn_temp)}, //ID ������ ���
        {AppCassette4.Month = new Value(StrCassette4 + "Month",      HWNDCLIENT::hEditState4_Month,    0, &conn_temp)}, //ID ������ �����
        {AppCassette4.Day = new Value(StrCassette4 + "Day",        HWNDCLIENT::hEditState4_Day,      0, &conn_temp)}, //ID ������ ����
        {AppCassette4.Hour = new Value(StrCassette4 + "Hour",       HWNDCLIENT::hEditState4_Hour,     0, &conn_temp)}, //ID ������ ���
        {AppCassette4.CassetteNo = new Value(StrCassette4 + "CaasetteNo", HWNDCLIENT::hEditState4_Cassette, 0, &conn_temp)}, //ID ������ �����

        {AppCassette5.Year = new Value(StrCassette5 + "Year",       HWNDCLIENT::hEditState5_Year,     0, &conn_temp)}, //ID ������ ���
        {AppCassette5.Month = new Value(StrCassette5 + "Month",      HWNDCLIENT::hEditState5_Month,    0, &conn_temp)}, //ID ������ �����
        {AppCassette5.Day = new Value(StrCassette5 + "Day",        HWNDCLIENT::hEditState5_Day,      0, &conn_temp)}, //ID ������ ����
        {AppCassette5.Hour = new Value(StrCassette5 + "Hour",       HWNDCLIENT::hEditState5_Hour,     0, &conn_temp)}, //ID ������ ���
        {AppCassette5.CassetteNo = new Value(StrCassette5 + "CaasetteNo", HWNDCLIENT::hEditState5_Cassette, 0, &conn_temp)}, //ID ������ �����

        {AppCassette6.Year = new Value(StrCassette6 + "Year",       HWNDCLIENT::hEditState6_Year,     0, &conn_temp)}, //ID ������ ���
        {AppCassette6.Month = new Value(StrCassette6 + "Month",      HWNDCLIENT::hEditState6_Month,    0, &conn_temp)}, //ID ������ �����
        {AppCassette6.Day = new Value(StrCassette6 + "Day",        HWNDCLIENT::hEditState6_Day,      0, &conn_temp)}, //ID ������ ����
        {AppCassette6.Hour = new Value(StrCassette6 + "Hour",       HWNDCLIENT::hEditState6_Hour,     0, &conn_temp)}, //ID ������ ���
        {AppCassette6.CassetteNo = new Value(StrCassette6 + "CaasetteNo", HWNDCLIENT::hEditState6_Cassette, 0, &conn_temp)}, //ID ������ �����

        {AppCassette7.Year = new Value(StrCassette7 + "Year",       HWNDCLIENT::hEditState7_Year,     0, &conn_temp)}, //ID ������ ���
        {AppCassette7.Month = new Value(StrCassette7 + "Month",      HWNDCLIENT::hEditState7_Month,    0, &conn_temp)}, //ID ������ �����
        {AppCassette7.Day = new Value(StrCassette7 + "Day",        HWNDCLIENT::hEditState7_Day,      0, &conn_temp)}, //ID ������ ����
        {AppCassette7.Hour = new Value(StrCassette7 + "Hour",       HWNDCLIENT::hEditState7_Hour,     0, &conn_temp)}, //ID ������ ���
        {AppCassette7.CassetteNo = new Value(StrCassette7 + "CaasetteNo", HWNDCLIENT::hEditState7_Cassette, 0, &conn_temp)}, //ID ������ �����

        {AppSelected1 = new Value(StrSelected1,  hEditState_selected_casset1, 0, &conn_temp)}, //����� ������� � 1-� ����
        {AppSelected2 = new Value(StrSelected2,  hEditState_selected_casset2, 0, &conn_temp)}, //����� ������� � 2-� ����
    #pragma endregion

};


bool cmpAllTagPeth(Value* first, Value* second)
{
    try
    {
        return first->Patch < second->Patch;
    }
    CATCH(PethLogger, "");
    return false;
}



void PLC_S107::DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr)
{
    std::string patch = "";
    if(isRun)
    {
        if(attr == OpcUa::AttributeId::Value)
        {
            try
            {
                WatchDog = TRUE; //��� �����

                OpcUa::NodeId id = node.GetId();
                if(id.IsInteger())
                {
                    if((uint32_t)OpcUa::ObjectId::Server_ServerStatus_CurrentTime == id.GetIntegerIdentifier())
                    {
                        S107::ServerDataTime = val.ToString();
                        SetWindowText(winmap(hEditTimeServer_2), S107::ServerDataTime.c_str());
                    }
                }
                else if(id.IsString())
                {
                    SetWindowText(winmap(hEditMode2), "������ ������...");
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
                                //a->attr = attr;
                            }
                            a->Find(handle, vals);
                            return;
                        }
                    }
                    SetWindowText(winmap(hEditMode2), "���� ������...");
                }

            }
            CATCH(PethLogger, "");
            //catch(std::runtime_error& exc)
            //{
            //    SetWindowText(winmap(hEditMode1), "DataChange runtime_error");
            //    LOG_ERROR(PethLogger, "{:90| DataChange Error {}, {}", FUNCTION_LINE_NAME, exc.what(), node.ToString());
            //}
            //catch(...)
            //{
            //    SetWindowText(winmap(hEditMode1), "Unknown error");
            //    LOG_ERROR(PethLogger, "{:90| DataChange Error 'Unknown error' {}", FUNCTION_LINE_NAME, node.ToString());
            //};
        }
    }
}


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
}

bool PLC_S107::WD()
{
    //��� �����
    if(WatchDog == TRUE)
    {
        WatchDog = FALSE;
        if(WatchDogWait)
        {
			if(WatchDogWait >= CountWatchDogWait - 1)
			{
				LOG_INFO(Logger, "{:90}| ���������� ����� {}", FUNCTION_LINE_NAME, WatchDogWait);
				SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
			}
        }
        WatchDogWait = 0;
    }
    else
    {
        WatchDogWait++; //�������������� ������� ������ �����
        if(WatchDogWait >= CountWatchDogWait) //���� ���� ����� ��� ������ CountWatchDogWait ������
        {
            LOG_INFO(Logger, "{:90}| ����������: ���� ����� ��� ������ {} ������", FUNCTION_LINE_NAME, CountWatchDogWait);
            SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
            SetWindowText(winmap(hEditTime_1), std::to_string(WatchDogWait).c_str());
			throw std::runtime_error(std::string(std::string("����������: ���� ����� ��� ������ ") + std::to_string(CountWatchDogWait) + " ������").c_str());
            return true;
        }
        else
        {
			if(WatchDogWait >= CountWatchDogWait - 1)
			{
				LOG_INFO(Logger, "{:90}| ���� ����� ��� ������ {} ������", FUNCTION_LINE_NAME, WatchDogWait);
				SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
			}
        }
    }
    //SetWindowText(winmap(hEditTime_1), std::to_string(WatchDogWait).c_str());
    return false;
}


void PLC_S107::InitNodeId()
{
    try
    {
        LOG_INFO(Logger, "{:90}| ������������� �����... countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
        LOG_INFO(Logger, "{:90}| �������� Patch = Server_ServerStatus_CurrentTime", FUNCTION_LINE_NAME);

        nodeCurrentTime = GetNode(OpcUa::ObjectId::Server_ServerStatus_CurrentTime); //Node ������� �����

        if(nodeCurrentTime.IsValid())
        {
            OpcUa::Variant val = nodeCurrentTime.GetValue();
            S107::ServerDataTime = val.ToString();
            SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
        }

        LOG_INFO(Logger, "{:90}| ������������� NodeId", FUNCTION_LINE_NAME);
        for(auto& a : AllTagPeth)
        {
            a->InitNodeId(this);
        }
    }
    CATCH(PethLogger, "");
}

void InitFurnIdCassette(int Peth)
{
	T_ForBase_RelFurn & F = S107::GetBaseRelFurn(Peth);
	Tcass& P = S107::GetIgCassetteFurn(Peth);

	P.Run = F.ProcRun->GetBool();
	if(P.Run)
	{
		P.Run_at = GetStringDataTime();
		P.Year = F.Cassette.Year->GetString();
		P.Month = F.Cassette.Month->GetString();
		P.Day = F.Cassette.Day->GetString();
		P.Hour = F.Cassette.Hour->GetString();
		P.CassetteNo = F.Cassette.CassetteNo->GetString();
	}
	else
	{
		P = Tcass(Peth);
		//P.Run_at = "";
		//P.Year = "0";
		//P.Month = "0";
		//P.Day = "0";
		//P.Hour = "0";
		//P.CassetteNo = "0";
	}

	MySetWindowText(winmap(F.Cassette.Year->winId), P.Year);
	MySetWindowText(winmap(F.Cassette.Month->winId), P.Month);
	MySetWindowText(winmap(F.Cassette.Day->winId), P.Day);
	MySetWindowText(winmap(F.Cassette.Hour->winId), P.Hour);
	MySetWindowText(winmap(F.Cassette.CassetteNo->winId), P.CassetteNo);
}

void PLC_S107::InitTag()
{
    LOG_INFO(Logger, "{:90}| ������������� ����������... countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);

	InitFurnIdCassette(1);
	InitFurnIdCassette(2);

    try
    {
        //������� ����� �����������
        avid[MSSEC::sec01000].push_back({nodeCurrentTime.GetId(), OpcUa::AttributeId::Value});

        for(auto& a : AllTagPeth)
        {
            if(a->Sec)
                avid[a->Sec].push_back({a->GetId(), OpcUa::AttributeId::Value});
        }
    } 
    CATCHINIT();

    for(auto& ar : avid)
    {
        try
        {
            if(ar.second.size())
            {
                css[ar.first].Create(*this, ar.first, Logger);
                LOG_INFO(Logger, "{:90}| SubscribeDataChange msec: {}, count: {}", FUNCTION_LINE_NAME, css[ar.first].msec, ar.second.size());
                std::vector<uint32_t> monitoredItemsIds = css[ar.first].sub->SubscribeDataChange(ar.second);
            }
        } 
        CATCHINIT();
    }

}

void PLC_S107::Run(int count)
{
    countconnect1 = count;
    countconnect2 = 0;

    LOG_INFO(Logger, "{:90}| Run... : countconnect = {}.{} to: {}", FUNCTION_LINE_NAME, countconnect1, countconnect2, Uri);

    try
    {
        InitGoot = FALSE;
        countget = 1;

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

        LOG_INFO(Logger, "{:90}| ����������� ������� countconnect = {}.{}\r\n", FUNCTION_LINE_NAME, countconnect1, countconnect2);

        for(auto val : AllTagPeth)
            MySetWindowText(val);

        WatchDogWait = 0;
        SetWindowText(winmap(hEditTime_1), std::to_string(WatchDogWait).c_str());

        InitGoot = TRUE;
        ULONGLONG time1 = GetTickCount64();

        isInitPLC_S107 = true;
        SekRun = time(NULL);
        SetWindowText(winmap(hEditMode2), "������ ������");
		bool Running = KeepAlive.Running;
        while(isRun)
        {
			if(KeepAlive.Running != Running)
			{
				Running = KeepAlive.Running;
				LOG_WARN(Logger, "{:90}| KeepAlive.Running = {}", FUNCTION_LINE_NAME, Running);
			}


            AppFurn1.WDG_fromBase->Set_Value(true);
            AppFurn2.WDG_fromBase->Set_Value(true);

            //��������� WatchDog
			WD();
                //throw std::runtime_error(std::string(std::string("����������: ���� ����� ��� ������ ") + std::to_string(CountWatchDogWait) + " ������").c_str());

            TestTimeRun(time1);

            GetWD();
        }

        for(auto&a : css)
            a.second.Delete();

        return;
    }
    CATCH_RUN(Logger);

    LOG_INFO(Logger, "{:90}| ����� �� ������ 1 countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);

    //if(isRun)
    {
        LOG_INFO(Logger, "{:90}| ���� 5 ������... ��� {}", FUNCTION_LINE_NAME, Uri);
        int f = 5;
        while(--f && isRun) std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    LOG_INFO(Logger, "{:90}| ... ����� {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
};


DWORD WINAPI Open_FURN_RUN(LPVOID)
{
    std::shared_ptr<spdlog::logger> Logger = PethLogger;

    LOG_INFO(Logger, "{:90}| ����� to: {}", FUNCTION_LINE_NAME, S107::URI);

    int countconnect = 1;

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
            //������������� ������ ������ � ����� unique_ptr
            //auto PLC = std::unique_ptr<PLC_S107>(new PLC_S107(S107::URI, PethLogger));
            //LOG_INFO(Logger, "{:90}| ����������� {} to: {}", FUNCTION_LINE_NAME, countconnect, S107::URI);
            //PLC->Run(countconnect);

            LOG_INFO(Logger, "{:90}| �������� ������ PLC_S107 {}", FUNCTION_LINE_NAME, countconnect);
            SetWindowText(winmap(hEditMode2), "�������� �������");

            PLC_S107 PLC(S107::URI, Logger);
            LOG_INFO(Logger, "{:90}| ����������� {} to: {}", FUNCTION_LINE_NAME, countconnect, S107::URI);
            PLC.Run(countconnect);
            LOG_INFO(Logger, "{:90}| ����� �� Run countconnect = {} to: {}", FUNCTION_LINE_NAME, countconnect, S107::URI);
        }
        CATCH_OPEN(Logger, S107::URI);

        LOG_INFO(Logger, "{:90}| ����� �� ������ countconnect = {} to: {}", FUNCTION_LINE_NAME, countconnect, S107::URI);

        if(isRun)
        {
            int f = 10;
            while(--f && isRun) std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            countconnect++;
            LOG_INFO(Logger, "{:90}| ��������� ������� {} to: {}", FUNCTION_LINE_NAME, countconnect, S107::URI);
        }
    }

    LOG_INFO(Logger, "{:90}| ExitThread. isRun = {}", FUNCTION_LINE_NAME, isRun);

    return 0;
}


typedef struct isrun_s{
    std::string create_at = "";
    int id_name = 0;
    std::string content = "";
}isrun_s;

typedef struct isrun{
    std::string create_at = "";
    int day = 0;
    int month = 0;
    int year = 0;
    int cassetteno = 0;
    bool run_pr = false;
    bool error_pr = false;
    bool end_pr = false;
    bool finish_pr = false;
    int peth = 0;
}isrun;

std::vector<isrun_s>ISRUN;

void InsertIsRun(PGConnection& conn, isrun& IR)
{
    try
    {
        if(IR.day && IR.month && IR.year && IR.cassetteno)
        {
            std::stringstream sd;
            sd << "INSERT INTO isrun (create_at, day, month, year, cassetteno, run_pr, error_pr, end_pr, peth) VALUES (";
            sd << "'" << IR.create_at << "', ";
            sd << IR.day << ", ";
            sd << IR.month << ", ";
            sd << IR.year << ", ";
            sd << IR.cassetteno << ", ";
            sd << IR.run_pr << ", ";
            sd << IR.error_pr << ", ";
            sd << IR.end_pr << ", ";
            sd << IR.peth << ");";

            SETUPDATESQL(PethLogger, conn, sd);
        }
    }
    CATCH(PethLogger, "");
}


#define CountCaseteInRel 7
const TCassette NullCasete;
TCassette CassetteInRel[CountCaseteInRel] ={
    NullCasete,
    NullCasete,
    NullCasete,
    NullCasete,
    NullCasete,
    NullCasete,
    NullCasete,
};

void SetCassetteToBase(int i)
{
    try
    {
        //LOG_INFO(FurnLogger, "{:90}| ", FUNCTION_LINE_NAME);
        int32_t Year = Stoi(CassetteInRel[i].Year);
        int32_t Month = Stoi(CassetteInRel[i].Month);
        int32_t Day = Stoi(CassetteInRel[i].Day);
        uint16_t Hour = Stoi(CassetteInRel[i].Hour);
        int32_t CassetteNo = Stoi(CassetteInRel[i].CassetteNo);

        int32_t aYear = AppCassette[i].Year->Val.As<int32_t>();
        int32_t aMonth = AppCassette[i].Month->Val.As<int32_t>();
        int32_t aDay = AppCassette[i].Day->Val.As<int32_t>();
        uint16_t aHour = AppCassette[i].Hour->Val.As<uint16_t>();
        int32_t aCassetteNo = AppCassette[i].CassetteNo->Val.As<int32_t>();
        
        //LOG_INFO(FurnLogger, "{:90}| << {:04}-{:02}-{:02}-{:02}-{:02}", FUNCTION_LINE_NAME, aYear, aMonth, aDay, aHour, aCassetteNo);

        if(aYear != Year)            AppCassette[i].Year->Set_Value(Year);
        if(aMonth != Month)          AppCassette[i].Month->Set_Value(Month);
        if(aDay != Day)              AppCassette[i].Day->Set_Value(Day);
        if(aHour != Hour)            AppCassette[i].Hour->Set_Value((uint16_t)Hour);
        if(aCassetteNo != CassetteNo)AppCassette[i].CassetteNo->Set_Value(CassetteNo);

        aYear = AppCassette[i].Year->Val.As<int32_t>();
        aMonth = AppCassette[i].Month->Val.As<int32_t>();
        aDay = AppCassette[i].Day->Val.As<int32_t>();
        aHour = AppCassette[i].Hour->Val.As<uint16_t>();
        aCassetteNo = AppCassette[i].CassetteNo->Val.As<int32_t>();

        //LOG_INFO(FurnLogger, "{:90}| >> {:04}-{:02}-{:02}-{:02}-{:02}", FUNCTION_LINE_NAME, aYear, aMonth, aDay, aHour, aCassetteNo);
    }
    CATCH(FurnLogger, "");
}

bool cmpCasete(TCassette& first, TCassette& second)
{
    return first.Create_at < second.Create_at;
}


int SCassett[CountCaseteInRel];
int OldSCassett[CountCaseteInRel];

typedef struct _cassette{
    int Year = 0;
    int Month = 0;
    int Day = 0;
    int Hour = 0;
    int CassetteNo = 0;

    _cassette()
    {
        Year = 0;
        Month = 0;
        Day = 0;
        Hour = 0;
        CassetteNo = 0;
    }
    _cassette(T_Fcassette& c)
    {
        try
        {
            Year = c.Year->Val.As<int32_t>();
            Month = c.Month->Val.As<int32_t>();
            Day = c.Day->Val.As<int32_t>();
            Hour = c.Hour->Val.As<uint16_t>();
            CassetteNo = c.CassetteNo->Val.As<int32_t>();
        }
        CATCH(FurnLogger, "");
    }
    _cassette(T_CassetteData& c)
    {
        try
        {
            Year = c.Year->Val.As<int32_t>();
            Month = c.Month->Val.As<int32_t>();
            Day = c.Day->Val.As<int32_t>();
            Hour = c.Hour->Val.As<uint16_t>();
            CassetteNo = c.CassetteNo->Val.As<int32_t>();
        }
        CATCH(FurnLogger, "");
    }
    _cassette(TCassette& c)
    {
        try
        {
            Year = Stoi(c.Year);
            Month = Stoi(c.Month);
            Day = Stoi(c.Day);
            Hour = Stoi(c.Hour);
            CassetteNo = Stoi(c.CassetteNo);
        }
        CATCH(FurnLogger, "");
    }

    bool operator == (_cassette& T)
    {
        return Year == T.Year && Month == T.Month && Day == T.Day && Hour == T.Hour && CassetteNo == T.CassetteNo;
    }
    bool operator != (_cassette& T)
    {
        return Year != T.Year || Month != T.Month || Day != T.Day || Hour != T.Hour || CassetteNo != T.CassetteNo;
    }
}_cassette;

_cassette Sheet;
_cassette Furn1;
_cassette Furn2;
_cassette SheetIT;



//��������� ������� ������ ��� ����� �������
void CaseteInRel(std::deque<TCassette>& CIl)
{
    try
    {
        std::sort(CIl.begin(), CIl.end(), cmpCasete);
        for(int i = 0; i < CountCaseteInRel; i++)
        {
            if((int)CIl.size() > i)
            {
                CassetteInRel[i] = CIl[i];
                SetCassetteToBase(i); //���������� � ����
                SCassett[i] = Stoi(CassetteInRel[i].Id);
            }
            else
            {
                CassetteInRel[i] = TCassette();
                if(AppCassette[i].TestNull(PethLogger))
                {
                    AppCassette[i].SetNull(PethLogger); //���������� � ����
                    SCassett[i] = 0;
                }
            }
        }

#pragma region ��������� ������� ������ ��� ����� ������� � ���� ��� �������
        if(memcmp(OldSCassett, SCassett, sizeof(SCassett)))
        {
            memcpy(OldSCassett, SCassett, sizeof(SCassett));
            std::fstream fSpCassette = std::fstream("SpCassette.csv", std::fstream::binary | std::fstream::out | std::ios::app);
            fSpCassette << " " << GetStringDataTime() << ";";

            for(int i = 0; i < CountCaseteInRel; i++)
                fSpCassette << SCassett[i] << ";";
            fSpCassette << std::endl;
            fSpCassette.close();
        }
#pragma endregion

    }
    CATCH(PethLogger, "");
}

//������� ������ ������
void OutListCassette(size_t& old_count)
{
    try
    {
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
    }
    CATCH(PethLogger, "");
}

//�� �������� Event = 1
int SetCassetteInCant(PGConnection& conn, std::shared_ptr<spdlog::logger> L, TCassette& it)
{
    try
    {
        it.Event = "1";
        it.Peth = "0";
        it.Run_at = "";
        it.Error_at = "";
        it.End_at = "";
        it.Finish_at = "";
        it.Correct = "";
        it.Pdf = "";

        std::stringstream sd;
        sd << "UPDATE cassette SET event = " << it.Event <<", peth = " << it.Peth << ", run_at = DEFAULT, end_at = DEFAULT, finish_at = DEFAULT, correct = DEFAULT, pdf = DEFAULT, error_at = DEFAULT WHERE id = " << it.Id;
        std::string command = sd.str();
        LOG_INFO(L, "{:90}| {}", FUNCTION_LINE_NAME, command);
        SETUPDATESQL(L, conn, sd);
    }
    CATCH(L, "");
    return Stoi(it.Event);
}

//� �������� Event = 2
int SetCassetteInWait(PGConnection& conn, std::shared_ptr<spdlog::logger> L, TCassette& it)
{
    try
    {
        std::string run_at = it.Run_at;
        it.Event = "2";
        it.Run_at = "";
        std::stringstream sd;
		sd << "UPDATE cassette SET run_at = DEFAULT, end_at = DEFAULT, finish_at = DEFAULT, event = " << it.Event;
        //if(run_at.length())
        //    sd << ", return_at = '" << run_at << "'";
        sd << " WHERE id = " << it.Id;
        std::string command = sd.str();
        LOG_INFO(L, "{:90}| {}", FUNCTION_LINE_NAME, command);
        SETUPDATESQL(L, conn, sd);
    }
    CATCH(L, "");
    return Stoi(it.Event);
}

//� ���� Event = 3
int SetCassetteInFurn(PGConnection& conn, std::shared_ptr<spdlog::logger> L, TCassette& it, int Peth)
{
    try
    {
        it.Event = "3";
        it.Peth = std::to_string(Peth);
        it.End_at = "";
        it.Finish_at = "";
        it.Correct = "";
        it.Pdf = "";
        std::stringstream sd;
        sd << "UPDATE cassette SET event = " << it.Event << ", peth = " << it.Peth << ", run_at = now(), end_at = DEFAULT, finish_at = DEFAULT, correct = DEFAULT, pdf = DEFAULT, error_at = DEFAULT WHERE id = " << it.Id;
        std::string command = sd.str();
        LOG_INFO(L, "{:90}| {}", FUNCTION_LINE_NAME, command);
        SETUPDATESQL(L, conn, sd);
    }
    CATCH(L, "");
    return Stoi(it.Event);
}

//� ��������� Event = 7
int SetCassetteInDelete(PGConnection& conn, std::shared_ptr<spdlog::logger> L, TCassette& it)
{
    try
    {
        it.Event = "7";
        it.Delete_at = GetStringDataTime();
        std::stringstream sd;
        sd << "UPDATE cassette SET event = " << it.Event;
        sd << ", delete_at = '" << it.Delete_at << "'";
		//sd << ", correct = now()";
		//sd << ", pdf = now()";
        sd << "WHERE id = " + it.Id;
        std::string command = sd.str();
        LOG_INFO(L, "{:90}| {}", FUNCTION_LINE_NAME, command);
        SETUPDATESQL(L, conn, sd);
    }
    CATCH(L, "");
    return Stoi(it.Event);
}


std::string GetDataEnd(PGConnection& conn, std::shared_ptr<spdlog::logger> L, std::string sd)
{
    std::string at = "";
    try
    {
        PGresult* res = conn.PGexec(sd);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            if(PQntuples(res))
                at = conn.PGgetvalue(res, 0, 0);
        }
        else
            LOG_ERR_SQL(L, res, sd);
        PQclear(res);
    }
    CATCH(L, "");
    return at;
}

//���� �����
void FindEnd(PGConnection& conn, std::shared_ptr<spdlog::logger> L, TCassette& it)
{
    try
    {
        T_ForBase_RelFurn* Furn = NULL;
        if(it.Peth == "1")Furn = &AppFurn1;
        if(it.Peth == "2")Furn = &AppFurn2;
        if(!Furn) return;

        int ProcEndID = Furn->ProcEnd->ID;
        int ProcRunID = Furn->ProcRun->ID;
        if(!ProcEndID) return; //���� ���������� ���� �������

        std::string End_at1 = "";
        std::string End_at2 = "";
        {
            std::stringstream sd;
            sd << "SELECT create_at FROM todos WHERE create_at >= '" << it.Run_at << "' AND content = 'true' AND ";
            sd << "id_name = " << ProcEndID;
            sd << " ORDER BY create_at DESC LIMIT 1";
            End_at1 = GetDataEnd(conn, L, sd.str());
        }
        {
            std::stringstream sd;
            sd << "SELECT create_at FROM todos WHERE create_at >= '" << it.Run_at << "' AND content = 'false' AND ";
            sd << "id_name = " << ProcRunID;
            sd << " ORDER BY create_at DESC LIMIT 1";
            End_at2 = GetDataEnd(conn, L, sd.str());
        }

        if(End_at1 > End_at2)
            it.End_at = End_at1;
        else
            it.End_at = End_at2;

        if(it.End_at.length())
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET end_at = '" << it.End_at << "' WHERE end_at IS NULL AND id = " << it.Id;
            LOG_INFO(L, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
            SETUPDATESQL(L, conn, sd);
        }
    }
    CATCH(L, "");
}


//� ����� Event = 5
int SetCassetteInFinal(PGConnection& conn, std::shared_ptr<spdlog::logger> L, TCassette& it)
{
    try
    {
        it.Event = "5";
        std::stringstream sd;
        sd << "UPDATE cassette SET event = 5, finish_at = '" << it.Finish_at << "' WHERE id = " << it.Id;
        LOG_INFO(L, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
        SETUPDATESQL(L, conn, sd);
		CreateThread(0, 0, PDF::CorrectCassetteFinal, (LPVOID)&it, 0, 0);
    }
    CATCH(L, "");

    return Stoi(it.Event);
}

void OutLoggin(std::shared_ptr<spdlog::logger> L, std::string F, std::string st, TCassette& it)
{
#define id  it.Id
#define Ev  it.Event

#define Y   it.Year
#define M   it.Month
#define D   it.Day
#define H   it.Hour
#define N   it.CassetteNo

#define Rn  it.Run_at
#define Ed  it.End_at
#define Fs  it.Finish_at
#define Cr  it.Correct

    LOG_INFO(L, 
             "{:90}| id:{} {} EV:{} ID(Y:{} M:{} D:{} H:{} N:{}) Rn:{} Ed:{} Fs:{} Cr:{}", 
              F,     id,   st,Ev,      Y,   M,   D,   H,   N,    Rn,   Ed,   Fs,   Cr)

#undef id
#undef Ev

#undef Y 
#undef M
#undef D
#undef H
#undef n

#undef Rn
#undef Ed
#undef Fs
#undef Cr
}


void TestCassete(PGConnection& conn, std::shared_ptr<spdlog::logger>L, std::deque<TCassette>& CIl)
{
    try
    {
        Sheet = _cassette(HMISheetData.Cassette);
        Furn1 = _cassette(AppFurn1.Cassette);
        Furn2 = _cassette(AppFurn2.Cassette);

        for(auto& it : AllCassette)
        {
            if(!isRun) return;

            int Event = Stoi(it.Event);
            bool DeleteIT = (bool)it.Delete_at.length();

            if((Event != 7 && DeleteIT) || (Event == 7 && !DeleteIT))
            {
                //� ��������� Event = 7
                OutLoggin(L, FUNCTION_LINE_NAME, ">> � ���������", it);
                Event = SetCassetteInDelete(conn, L, it);
                OutLoggin(L, FUNCTION_LINE_NAME, "<< � ���������", it);
            }
            int SheetInCassette = Stoi(it.SheetInCassette);
			int CassetteNo = Stoi(it.CassetteNo); 

            //���� ������� �� �������
            if(Event != 7 && CassetteNo != 255)
            {
                SheetIT = _cassette(it);
                int Peth = Stoi(it.Peth);

                bool YesSh = (SheetIT == Sheet);
                bool YesF1 = (SheetIT == Furn1);
                bool YesF2 = (SheetIT == Furn2);

                //if(it.End_at.length())
                //    FindFinish(L, conn, it);
                 
                
                //���� ������� �� ��������
                if(YesSh)
                {
                    if(Event != 1 || Peth != 0)
                    {
                        Peth = 0;
                        //�� �������� Event = 1
                        OutLoggin(L, FUNCTION_LINE_NAME, ">> �� ��������", it);
                        Event = SetCassetteInCant(conn, L, it);
                        OutLoggin(L, FUNCTION_LINE_NAME, "<< �� ��������", it);
                    }
                }

                //���� ������� � 1-� ���� �������
                else if(YesF1)
                {
                    if(Event != 3 || Peth != 1)
                    {
                        Peth = 1;
                        //� ���� Event = 3
                        OutLoggin(L, FUNCTION_LINE_NAME, ">> � ���� 1", it);
                        Event = SetCassetteInFurn(conn, L, it, Peth);
                        OutLoggin(L, FUNCTION_LINE_NAME, "<< � ���� 1", it);
                    }
                }

                //���� ������� �� 2-� ���� �������
                else if(YesF2)
                {
                    if(Event != 3 || Peth != 2)
                    {
                        Peth = 2;
                        //� ���� Event = 3
                        OutLoggin(L, FUNCTION_LINE_NAME, ">> � ���� 2", it);
                        Event = SetCassetteInFurn(conn, L, it, Peth);
                        OutLoggin(L, FUNCTION_LINE_NAME, "<< � ���� 2", it);
                    }
                }

                //���� ������� �� �� �������� � �� � ����� �������
                else
                {
                    if(Event != 2 && Event != 5 && !YesSh && !YesF1 && !YesF2)
                    {
                        //���� ���������� ������ � ������ �� ����
                        if(SheetInCassette > 0)
                        {
                            if(it.Run_at.length())
                            {
                                //���� �����
                                if(!it.End_at.length())
                                {
                                    OutLoggin(L, FUNCTION_LINE_NAME, ">> ���� �����", it);
                                    FindEnd(conn, L, it);
                                    OutLoggin(L, FUNCTION_LINE_NAME, "<< ���� �����", it);
                                }

                                if(it.End_at.length())
                                {
                                    //���� 15 ����� �����
                                    if(!it.Finish_at.length())
                                    {
                                        //������������
                                        //OutLoggin(L, FUNCTION_LINE_NAME, ">> ������������", it);
										//FindFinish(conn, L, it);
                                        S107::GetFinishCassete(conn, L, it);
										//CreateThread(0, 0, PDF::CorrectCassetteFinal, (LPVOID)0, 0, 0);
                                        //FindFinish(L, conn, it);
                                        //OutLoggin(L, FUNCTION_LINE_NAME, "<< ������������", it);
                                    }
                                }
                                else
                                {
									Tcass& P = S107::GetIgCassetteFurn(Peth);
									if(S107::IsCassette(P))
									{ 
										if(Sheet.Year != Stoi(P.Year) ||
											  Sheet.Month != Stoi(P.Month) ||
											  Sheet.Day != Stoi(P.Day) ||
											  Sheet.Hour != Stoi(P.Hour) ||
											  Sheet.CassetteNo != Stoi(P.CassetteNo))
										{
											//� ��������
											OutLoggin(L, FUNCTION_LINE_NAME, ">> � ��������", it);
											Event = SetCassetteInWait(conn, L, it);
											OutLoggin(L, FUNCTION_LINE_NAME, "<< � ��������", it);
										}
									}
                                }

                            }

                            //���� ���� �����
                            if(it.Finish_at.length())
                            {
                                //� ����� Event = 5
                                OutLoggin(L, FUNCTION_LINE_NAME, ">> � �����", it);
                                Event = SetCassetteInFinal(conn, L, it);
                                OutLoggin(L, FUNCTION_LINE_NAME, "<< � �����", it);
                            }
                        }
                    }
                }

                //���� �� �� �������� � ���������� ������ � ������ ���� ���������� � ����������
                if(Event != 5 && !YesSh && !YesF1 && !YesF2)
                {
                    //���� ��� ���������� ������ ����������� � ���������
                    if(SheetInCassette <= 0 && CassetteNo != 255 && !it.End_at.length())
                    {
                        //� ��������� Event = 7
                        OutLoggin(L, FUNCTION_LINE_NAME, ">> � ���������", it);
                        Event = SetCassetteInDelete(conn, L, it);
                        OutLoggin(L, FUNCTION_LINE_NAME, "<< � ���������", it);
                    }
                    else
                    {
                        //���� ��� ������ � ��� ������ � �� ������
                        if(!it.Run_at.length() && !it.End_at.length() && !DeleteIT)
                        {
                            //���� �� � ��������
                            if(Event != 2)
                            {
                                //� �������� Event = 2
                                OutLoggin(L, FUNCTION_LINE_NAME, ">> � ��������", it);
                                Event = SetCassetteInWait(conn, L, it);
                                OutLoggin(L, FUNCTION_LINE_NAME, "<< � ��������", it);
                            }
                        }
                    }
                }

                //�� ������ � �� ������������� �� ���� Finish_at
                if(Event != 5 && Event != 7 && it.Finish_at.length())
                {
                    //� ����� Event = 5
                    OutLoggin(L, FUNCTION_LINE_NAME, ">> � �����", it);
                    Event = SetCassetteInFinal(conn, L, it);
					//CreateThread(0, 0, PDF::CorrectCassetteFinal, (LPVOID)&it, 0, 0);
                    OutLoggin(L, FUNCTION_LINE_NAME, "<< � �����", it);
                }

                //��������� ��������� ������ ��� �����
                if(Event == 2)
                {
                    if(SheetInCassette > 0)
                        CIl.push_back(it);
                }
            }
        }
    }CATCH(L, "");
}

DWORD WINAPI Open_FURN_SQL(LPVOID)
{
#pragma region ������ while(isRun)

    size_t old_count = 0;
    InitLogger(FurnLogger);
    InitLogger(TestLogger);
    LOG_INFO(FurnLogger, "{:90}| Start Open_FURN_SQL", FUNCTION_LINE_NAME);
    LOG_INFO(TestLogger, "{:90}| Start Open_FURN_SQL", FUNCTION_LINE_NAME);

    PGConnection conn;
    CONNECTION1(conn, FurnLogger);
    

    while(isRun)
    {
        try
        {
#pragma endregion
            
#pragma region ����������� ������ ������
            try
            {
                S107::SQL::FURN_SQL(conn, AllCassette, PethLogger);
            }
            catch(...) {}
#pragma endregion

            std::deque<TCassette> CIl;

#ifndef _DEBUG
            TestCassete(conn, TestLogger, CIl);
#endif

            //��������� ������� ������ ��� ����� �������
            CaseteInRel(CIl);

            //������� �� ����� ������ ������
            OutListCassette(old_count);


#pragma region ����� while(isRun)

        }CATCH(PethLogger, "");

        if(!isRun) return 0;

        int f = 30; //30 ������
        while(isRun && (--f))
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
#pragma endregion
    LOG_INFO(TestLogger, "{:90} Stop Open_FURN_SQL", FUNCTION_LINE_NAME);
    LOG_INFO(FurnLogger, "{:90} Stop Open_FURN_SQL", FUNCTION_LINE_NAME);

    return 0;
}

void Open_FURN()
{
    InitLogger(PethLogger);
	InitLogger(PethLogger);
	
	S107::Furn1::Logger = InitLogger2("Furn1");
	S107::Furn2::Logger = InitLogger2("Furn2");

#ifndef _ReleaseD

    //���������� ���������� �� �����
    std::sort(AllTagPeth.begin(), AllTagPeth.end(), cmpAllTagPeth);

    LOG_INFO(PethLogger, "{:90}| Start Open_FURN", FUNCTION_LINE_NAME);


#ifndef TESTSPIS
#ifndef TESTWIN
#ifndef TESTGRAFF
    hS107URI = CreateThread(0, 0, Open_FURN_RUN, (LPVOID)0, 0, 0);
    hS107SQL = CreateThread(0, 0, Open_FURN_SQL, (LPVOID)0, 0, 0);
#else
    //hS107SQL = CreateThread(0, 0, Open_FURN_SQL, (LPVOID)0, 0, 0);
#endif


#endif
#endif

#endif
}

void Close_FURN()
{
#ifndef _ReleaseD
    WaitCloseTheread(hS107URI, "hS107URI");
    WaitCloseTheread(hS107SQL, "hS107SQL");
    WaitCloseTheread(hSQL, "hSQL");
#endif
}
