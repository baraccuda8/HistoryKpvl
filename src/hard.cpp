#include "pch.h"

#include "win.h"
#include "main.h"
#include "StringData.h"
#include "file.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "hard.h"
#include "KPVL.h"
#include "pdf.h"

extern PGConnection conn_kpvl;

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

//#define PLCsec0 sec00500
//#define PLCsec1 sec01000
//#define PLCsec2 sec02000
//#define PLCsec5 sec05000

extern std::string lpLogDir;

std::shared_ptr<spdlog::logger> HardLogger = NULL;


bool isInitPLC_KPVL = false;

time_t PLC_KPVL_old_dt = 0;

TextBool CantTextOut = {
    {false, WaitCant},
    {true, WaitResv},
};
TextBool CassetTextOut = {
    {false, WaitCassette},
    {true, FillCassette},
};

TextInt GenSeq1 ={
    {0, "���������"},
    {1, "����������"},
    {2, "�������"},
    {3, "������� ������� �����"},
    {4, "�������� � ����"},
    {5, "������� ������� �����"},
    {6, "������ �����"},
    {7, "�������� �� 2 ��������"},
    {8, "�������� �� 2-� �������� ����"},
};
TextInt GenSeq2 ={
    {0, "���������"},
    {1, "����������"},
    {2, "�������"},
    {3, "����� ��������� � 1-�� ��������� ����"},
    {4, "���������. ������ �����"},
    {5, "������� �������� �����"},
    {6, "������ � ����� �������"},
    {7, "������� �������� �����"},
};
TextInt GenSeq3 ={
    {0, "���������"},
    {1, "�������� �����"},
    {2, "���������. ���.�����."},
    {3, "������ ���������"},
    {4, "��������� ����� ���������"},
};


//HANDLE hFindSheet = NULL;

HANDLE hKPVLURI = NULL;
HANDLE hKPVLSQL = NULL;
HANDLE hRunAllPdf = NULL;
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

const std::string AppGenSeqFromHmi1 = AppGenSeqFromHmi + "PlateData.";
const std::string AppGenSeqToHmi1 =  AppGenSeqToHmi + "PlateData_";
const std::string AppHMISheetData1 = AppHMISheetData + "SheetDataIN.";
const std::string AppHMISheetData2 = AppHMISheetData + "Cassette.";


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


extern MAP_VALUE AllTagPeth;
MAP_VALUE AllTagKpvl = {
        #pragma region ������ WDG
            {HMISheetData.WDG = new Value(AppHMISheetData + "WDG",            HWNDCLIENT::hEditWDG, 0, &conn_kpvl)},    //������� ������ �����������
            {HMISheetData.WDG_toBase = new Value(AppHMISheetData + "WDG_toBase",     HWNDCLIENT::hEditState_WDG, KPVL::SheetData_WDG_toBase, &conn_kpvl)},  //�������� ��� ����� ��� �����������
            {HMISheetData.WDG_fromBase = new Value(AppHMISheetData + "WDG_fromBase",   HWNDCLIENT::hNull, 0, &conn_kpvl)}, //������������� ���� ����� ��� �����������
        #pragma endregion

        #pragma region ������ � �����
            {HMISheetData.NewData = new Value(AppHMISheetData + "NewData",            HWNDCLIENT::hGroup05, KPVL::Sheet::Z6::NewSheetData,       &conn_kpvl)},   //����� ���� � ������
            {HMISheetData.SaveDone = new Value(AppHMISheetData + "SaveDone",           HWNDCLIENT::hNull,    0, &conn_kpvl)},                                     //����� ����� ���� � ������
            {HMISheetData.CasseteIsFill = new Value(AppHMISheetData + "CasseteIsFill",      HWNDCLIENT::hStatSheet_CassetteIsFull,    KPVL::Cassette::CasseteIsFill,       &conn_kpvl)},   //������� �����������
            //{HMISheetData.StartNewCassette   = new Value(AppHMISheetData + "StartNewCassette",   HWNDCLIENT::hNull,    KPVL::Cassette::StartNewCassette,    &conn_kpvl)},   //������ ����� �������
            //{HMISheetData.CassetteIsComplete = new Value(AppHMISheetData + "CassetteIsComplete", HWNDCLIENT::hNull,    KPVL::Cassette::CassetteIsComplete,    &conn_kpvl)},   //������� ������
        #pragma endregion

        #pragma region �������� ��������, ������� �����������, ��������, ����������� ����
            {Par_Gen.UnloadSpeed = new Value(AppPar_Gen + "UnloadSpeed",         HWNDCLIENT::hEditUnloadSpeed,     KPVL::An::UnloadSpeed, &conn_kpvl)},            //�������� ��������
            {Par_Gen.TimeForPlateHeat = new Value(AppPar_Gen + "TimeForPlateHeat",    HWNDCLIENT::hTimeForPlateHeat,    KPVL::An::fTimeForPlateHeat, &conn_kpvl)},   //����� ������������ ��������� �������, ���
            {Par_Gen.PresToStartComp = new Value(AppPar_Gen + "PresToStartComp",     HWNDCLIENT::hPresToStartComp,     KPVL::An::fPresToStartComp, &conn_kpvl)},    //������� �������� ��� ������� �����������


            {GenSeqFromHmi.TempSet1 = new Value(AppGenSeqFromHmi + "TempSet1",          HWNDCLIENT::hEditTempSet,         KPVL::An::TempSet1, &conn_kpvl)}, //������� ����������� 
            {AI_Hmi_210.LaminPressTop = new Value(AppAI_Hmi_210 + "LaminPressTop.AI_eu",  HWNDCLIENT::hEditZakPressTopSet,  0, &conn_kpvl)},                  //�������� ���� ������� ��������� ����
            {AI_Hmi_210.LaminPressBot = new Value(AppAI_Hmi_210 + "LaminPressBot.AI_eu",  HWNDCLIENT::hEditZakPressBotSet,  0, &conn_kpvl)},                  //�������� ���� ������� ��������� ���
            {AI_Hmi_210.LAM_TE1 = new Value(AppAI_Hmi_210 + "LAM_TE1.AI_eu",        HWNDCLIENT::hEditLAM_TE1Set,      0, &conn_kpvl)},                  //����������� ���� � �������
            {AI_Hmi_210.Za_TE3 = new Value(AppAI_Hmi_210 + "Za_TE3.AI_eu",         HWNDCLIENT::hEditLAM_TE3Set,      0, &conn_kpvl)},                  //����������� ���� � ����
            {AI_Hmi_210.Za_PT3 = new Value(AppAI_Hmi_210 + "Za_PT3.AI_eu",         HWNDCLIENT::hEditLAM_PT3Set,      0, &conn_kpvl)},                  //�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)
            {AI_Hmi_210.Za_TE4 = new Value(AppAI_Hmi_210 + "Za_TE4.AI_eu",         HWNDCLIENT::hEditTempPerometr,    0, &conn_kpvl)},                  //��������
        #pragma endregion

        #pragma region ����� ������ �������
            {HMISheetData.Valve_1x = new Value(AppHMISheetData + "Valve_1x",               HWNDCLIENT::hEditClapTop0,    KPVL::Mask::DataMaskKlapan1, &conn_kpvl)},    //����� ������ ������� 1x
            {HMISheetData.Valve_2x = new Value(AppHMISheetData + "Valve_2x",               HWNDCLIENT::hEditClapBot0,    KPVL::Mask::DataMaskKlapan2, &conn_kpvl)},    //����� ������ ������� 2x
            {HMISheetData.SpeedSection.Top = new Value(AppHMISheetData + "SpeedSection.Top",       HWNDCLIENT::hEditSpeedTopSet, KPVL::Speed::SpeedSectionTop, &conn_kpvl)},    //������. ���������� ������. ����
            {HMISheetData.SpeedSection.Bot = new Value(AppHMISheetData + "SpeedSection.Bottom",    HWNDCLIENT::hEditSpeedBotSet, KPVL::Speed::SpeedSectionBot, &conn_kpvl)},    //������. ���������� ������. ���
            {HMISheetData.LaminarSection1.Top = new Value(AppHMISheetData + "LaminarSection1.Top",    HWNDCLIENT::hEditLAM1_TopSet, KPVL::Lam1::LaminarSection1Top, &conn_kpvl)}, //������. ���������� ������ 1. ����
            {HMISheetData.LaminarSection1.Bot = new Value(AppHMISheetData + "LaminarSection1.Bottom", HWNDCLIENT::hEditLAM1_BotSet, KPVL::Lam1::LaminarSection1Bot, &conn_kpvl)}, //������. ���������� ������ 1. ���
            {HMISheetData.LaminarSection2.Top = new Value(AppHMISheetData + "LaminarSection2.Top",    HWNDCLIENT::hEditLAM2_TopSet, KPVL::Lam2::LaminarSection2Top, &conn_kpvl)}, //������. ���������� ������ 2. ����
            {HMISheetData.LaminarSection2.Bot = new Value(AppHMISheetData + "LaminarSection2.Bottom", HWNDCLIENT::hEditLAM2_BotSet, KPVL::Lam2::LaminarSection2Bot, &conn_kpvl)}, //������. ���������� ������ 2. ���
        #pragma endregion

        #pragma region ���� 0 ���� ����� �����
            {PlateData[0].AlloyCodeText = new Value(AppGenSeqFromHmi1 + "AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ0_AlloyCode,   KPVL::Sheet::Z0::DataAlloyThikn, &conn_kpvl)}, //����� �����
            {PlateData[0].ThiknessText = new Value(AppGenSeqFromHmi1 + "ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ0_Thikness,    KPVL::Sheet::Z0::DataAlloyThikn, &conn_kpvl)}, //������� �����
            {PlateData[0].Melt = new Value(AppGenSeqFromHmi1 + "Melt",                HWNDCLIENT::hEditPlate_DataZ0_Melt,        KPVL::Sheet::Z0::Data, &conn_kpvl)},    //����� ������
            {PlateData[0].Slab = new Value(AppGenSeqFromHmi1 + "Slab",                HWNDCLIENT::hEditPlate_DataZ0_Slab,        KPVL::Sheet::Z0::Data, &conn_kpvl)},    //����� �����
            {PlateData[0].PartNo = new Value(AppGenSeqFromHmi1 + "PartNo",              HWNDCLIENT::hEditPlate_DataZ0_PartNo,      KPVL::Sheet::Z0::Data, &conn_kpvl)},    //����� ������
            {PlateData[0].Pack = new Value(AppGenSeqFromHmi1 + "Pack",                HWNDCLIENT::hEditPlate_DataZ0_Pack,        KPVL::Sheet::Z0::Data, &conn_kpvl)},    //����� �����
            {PlateData[0].Sheet = new Value(AppGenSeqFromHmi1 + "Sheet",               HWNDCLIENT::hEditPlate_DataZ0_Sheet,       KPVL::Sheet::Z0::Data, &conn_kpvl)},    //����� �����
            {PlateData[0].SubSheet = new Value(AppGenSeqFromHmi1 + "SubSheet",            HWNDCLIENT::hEditPlate_DataZ0_SubSheet,    KPVL::Sheet::Z0::Data, &conn_kpvl)},    //����� ��������
        #pragma endregion

        #pragma region ���� 1 ������ ������ (����, ����)
            {PlateData[1].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z1.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ1_AlloyCode,  KPVL::Sheet::Z1::DataAlloyThikn, &conn_kpvl)}, //����� �����
            {PlateData[1].ThiknessText = new Value(AppGenSeqToHmi1 + "Z1.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ1_Thikness,   KPVL::Sheet::Z1::DataAlloyThikn, &conn_kpvl)}, //������� �����
            {PlateData[1].Melt = new Value(AppGenSeqToHmi1 + "Z1.Melt",                HWNDCLIENT::hEditPlate_DataZ1_Melt,       KPVL::Sheet::Z1::Data, &conn_kpvl)},    //����� ������
            {PlateData[1].Slab = new Value(AppGenSeqToHmi1 + "Z1.Slab",                HWNDCLIENT::hEditPlate_DataZ1_Slab,       KPVL::Sheet::Z1::Data, &conn_kpvl)},    //����� �����
            {PlateData[1].PartNo = new Value(AppGenSeqToHmi1 + "Z1.PartNo",              HWNDCLIENT::hEditPlate_DataZ1_PartNo,     KPVL::Sheet::Z1::Data, &conn_kpvl)},    //����� ������
            {PlateData[1].Pack = new Value(AppGenSeqToHmi1 + "Z1.Pack",                HWNDCLIENT::hEditPlate_DataZ1_Pack,       KPVL::Sheet::Z1::Data, &conn_kpvl)},    //����� �����
            {PlateData[1].Sheet = new Value(AppGenSeqToHmi1 + "Z1.Sheet",               HWNDCLIENT::hEditPlate_DataZ1_Sheet,      KPVL::Sheet::Z1::Data, &conn_kpvl)},    //����� �����
            {PlateData[1].SubSheet = new Value(AppGenSeqToHmi1 + "Z1.SubSheet",            HWNDCLIENT::hEditPlate_DataZ1_SubSheet,   0, &conn_kpvl)},    //����� ��������
        #pragma endregion

        #pragma region ���� 2 ������ ������ (�������)
            {PlateData[2].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z2.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ2_AlloyCode,  KPVL::Sheet::Z2::DataAlloyThikn, &conn_kpvl)}, //����� �����
            {PlateData[2].ThiknessText = new Value(AppGenSeqToHmi1 + "Z2.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ2_Thikness,   KPVL::Sheet::Z2::DataAlloyThikn, &conn_kpvl)}, //������� �����
            {PlateData[2].Melt = new Value(AppGenSeqToHmi1 + "Z2.Melt",                HWNDCLIENT::hEditPlate_DataZ2_Melt,       KPVL::Sheet::Z2::Data, &conn_kpvl)},    //����� ������
            {PlateData[2].Slab = new Value(AppGenSeqToHmi1 + "Z2.Slab",                HWNDCLIENT::hEditPlate_DataZ2_Slab,       KPVL::Sheet::Z2::Data, &conn_kpvl)},    //����� �����
            {PlateData[2].PartNo = new Value(AppGenSeqToHmi1 + "Z2.PartNo",              HWNDCLIENT::hEditPlate_DataZ2_PartNo,     KPVL::Sheet::Z2::Data, &conn_kpvl)},    //����� ������
            {PlateData[2].Pack = new Value(AppGenSeqToHmi1 + "Z2.Pack",                HWNDCLIENT::hEditPlate_DataZ2_Pack,       KPVL::Sheet::Z2::Data, &conn_kpvl)},    //����� �����
            {PlateData[2].Sheet = new Value(AppGenSeqToHmi1 + "Z2.Sheet",               HWNDCLIENT::hEditPlate_DataZ2_Sheet,      KPVL::Sheet::Z2::Data, &conn_kpvl)},    //����� �����
            {PlateData[2].SubSheet = new Value(AppGenSeqToHmi1 + "Z2.SubSheet",            HWNDCLIENT::hEditPlate_DataZ2_SubSheet,   0, &conn_kpvl)}, //����� ��������
        #pragma endregion

        #pragma region ���� 3 ������ ������ (���������)
            {PlateData[3].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z3.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ3_AlloyCode,  KPVL::Sheet::Z3::DataAlloyThikn, &conn_kpvl)}, //��� �����
            {PlateData[3].ThiknessText = new Value(AppGenSeqToHmi1 + "Z3.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ3_Thikness,   KPVL::Sheet::Z3::DataAlloyThikn, &conn_kpvl)}, //������� �����
            {PlateData[3].Melt = new Value(AppGenSeqToHmi1 + "Z3.Melt",                HWNDCLIENT::hEditPlate_DataZ3_Melt,       KPVL::Sheet::Z3::Data, &conn_kpvl)},    //����� ������
            {PlateData[3].Slab = new Value(AppGenSeqToHmi1 + "Z3.Slab",                HWNDCLIENT::hEditPlate_DataZ3_Slab,       KPVL::Sheet::Z3::Data, &conn_kpvl)},    //����� �����
            {PlateData[3].PartNo = new Value(AppGenSeqToHmi1 + "Z3.PartNo",              HWNDCLIENT::hEditPlate_DataZ3_PartNo,     KPVL::Sheet::Z3::Data, &conn_kpvl)},    //����� ������
            {PlateData[3].Pack = new Value(AppGenSeqToHmi1 + "Z3.Pack",                HWNDCLIENT::hEditPlate_DataZ3_Pack,       KPVL::Sheet::Z3::Data, &conn_kpvl)},    //����� �����
            {PlateData[3].Sheet = new Value(AppGenSeqToHmi1 + "Z3.Sheet",               HWNDCLIENT::hEditPlate_DataZ3_Sheet,      KPVL::Sheet::Z3::Data, &conn_kpvl)},    //����� �����
            {PlateData[3].SubSheet = new Value(AppGenSeqToHmi1 + "Z3.SubSheet",            HWNDCLIENT::hEditPlate_DataZ3_SubSheet,   0, &conn_kpvl)},    //����� ��������
        #pragma endregion

        #pragma region ���� 4 ������ ������ (����������)
            {PlateData[4].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z4.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ4_AlloyCode,  KPVL::Sheet::Z4::DataAlloyThikn, &conn_kpvl)}, //����� �����
            {PlateData[4].ThiknessText = new Value(AppGenSeqToHmi1 + "Z4.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ4_Thikness,   KPVL::Sheet::Z4::DataAlloyThikn, &conn_kpvl)}, //������� �����
            {PlateData[4].Melt = new Value(AppGenSeqToHmi1 + "Z4.Melt",                HWNDCLIENT::hEditPlate_DataZ4_Melt,       KPVL::Sheet::Z4::Data, &conn_kpvl)},    //����� ������
            {PlateData[4].Slab = new Value(AppGenSeqToHmi1 + "Z4.Slab",                HWNDCLIENT::hEditPlate_DataZ4_Slab,       KPVL::Sheet::Z4::Data, &conn_kpvl)},    //����� �����
            {PlateData[4].PartNo = new Value(AppGenSeqToHmi1 + "Z4.PartNo",              HWNDCLIENT::hEditPlate_DataZ4_PartNo,     KPVL::Sheet::Z4::Data, &conn_kpvl)},    //����� ������
            {PlateData[4].Pack = new Value(AppGenSeqToHmi1 + "Z4.Pack",                HWNDCLIENT::hEditPlate_DataZ4_Pack,       KPVL::Sheet::Z4::Data, &conn_kpvl)},    //����� �����
            {PlateData[4].Sheet = new Value(AppGenSeqToHmi1 + "Z4.Sheet",               HWNDCLIENT::hEditPlate_DataZ4_Sheet,      KPVL::Sheet::Z4::Data, &conn_kpvl)},    //����� �����
            {PlateData[4].SubSheet = new Value(AppGenSeqToHmi1 + "Z4.SubSheet",            HWNDCLIENT::hEditPlate_DataZ4_SubSheet,   0, &conn_kpvl)},    //����� ��������
        #pragma endregion

        #pragma region ���� 5 ������
            {PlateData[5].AlloyCodeText = new Value(AppGenSeqToHmi1 + "Z5.AlloyCodeText.sText", HWNDCLIENT::hEditPlate_DataZ5_AlloyCode,  KPVL::Sheet::Z5::DataAlloyThikn, &conn_kpvl)}, //����� �����
            {PlateData[5].ThiknessText = new Value(AppGenSeqToHmi1 + "Z5.ThiknessText.sText",  HWNDCLIENT::hEditPlate_DataZ5_Thikness,   KPVL::Sheet::Z5::DataAlloyThikn, &conn_kpvl)}, //������� �����
            {PlateData[5].Melt = new Value(AppGenSeqToHmi1 + "Z5.Melt",                HWNDCLIENT::hEditPlate_DataZ5_Melt,       KPVL::Sheet::Z5::Data, &conn_kpvl)},    //����� ������
            {PlateData[5].Slab = new Value(AppGenSeqToHmi1 + "Z5.Slab",                HWNDCLIENT::hEditPlate_DataZ5_Slab,       KPVL::Sheet::Z5::Data, &conn_kpvl)},    //����� �����
            {PlateData[5].PartNo = new Value(AppGenSeqToHmi1 + "Z5.PartNo",              HWNDCLIENT::hEditPlate_DataZ5_PartNo,     KPVL::Sheet::Z5::Data, &conn_kpvl)},    //����� ������
            {PlateData[5].Pack = new Value(AppGenSeqToHmi1 + "Z5.Pack",                HWNDCLIENT::hEditPlate_DataZ5_Pack,       KPVL::Sheet::Z5::Data, &conn_kpvl)},    //����� �����
            {PlateData[5].Sheet = new Value(AppGenSeqToHmi1 + "Z5.Sheet",               HWNDCLIENT::hEditPlate_DataZ5_Sheet,      KPVL::Sheet::Z5::Data, &conn_kpvl)},    //����� �����
            {PlateData[5].SubSheet = new Value(AppGenSeqToHmi1 + "Z5.SubSheet",            HWNDCLIENT::hEditPlate_DataZ5_SubSheet,   0, &conn_kpvl)},    //����� ��������
        #pragma endregion

        #pragma region ���� 6 ���� �� ��������
            {PlateData[6].AlloyCodeText = new Value(AppHMISheetData1 + "AlloyCodeText.sText", HWNDCLIENT::hEdit_Sheet_AlloyCode,    KPVL::Sheet::Z6::DataAlloyThikn, &conn_kpvl)},   //����� �����
            {PlateData[6].ThiknessText = new Value(AppHMISheetData1 + "ThiknessText.sText",  HWNDCLIENT::hEdit_Sheet_Thikness,     KPVL::Sheet::Z6::DataAlloyThikn, &conn_kpvl)},   //������� �����
            {PlateData[6].Melt = new Value(AppHMISheetData1 + "Melt",                HWNDCLIENT::hEdit_Sheet_Melt,         KPVL::Sheet::Z6::Data, &conn_kpvl)},      //����� ������
            {PlateData[6].Slab = new Value(AppHMISheetData1 + "Slab",                HWNDCLIENT::hEdit_Sheet_Slab,         KPVL::Sheet::Z6::Data, &conn_kpvl)},    //����� �����
            {PlateData[6].PartNo = new Value(AppHMISheetData1 + "PartNo",              HWNDCLIENT::hEdit_Sheet_PartNo,       KPVL::Sheet::Z6::Data, &conn_kpvl)},      //����� ������
            {PlateData[6].Pack = new Value(AppHMISheetData1 + "Pack",                HWNDCLIENT::hEdit_Sheet_Pack,         KPVL::Sheet::Z6::Data, &conn_kpvl)},      //����� �����
            {PlateData[6].Sheet = new Value(AppHMISheetData1 + "Sheet",               HWNDCLIENT::hEdit_Sheet_Sheet,        KPVL::Sheet::Z6::Data, &conn_kpvl)},      //����� �����
            {PlateData[6].SubSheet = new Value(AppHMISheetData1 + "SubSheet",            HWNDCLIENT::hEdit_Sheet_SubSheet,     0, &conn_kpvl)},                            //����� ��������
        #pragma endregion

        #pragma region ������� �� ��������
            {HMISheetData.Cassette.CassetteNo = new Value(AppHMISheetData2 + "CassetteNo",          HWNDCLIENT::hEdit_Sheet_CassetteNo,      KPVL::Cassette::CassetteNo, &conn_kpvl)},        //����� ������� �� ����
            {HMISheetData.Cassette.Hour = new Value(AppHMISheetData2 + "Hour",                HWNDCLIENT::hEdit_Sheet_Cassette_Hour,    KPVL::Cassette::CassetteHour, &conn_kpvl)},     //��� ID �����
            {HMISheetData.Cassette.Day = new Value(AppHMISheetData2 + "Day",                 HWNDCLIENT::hEdit_Sheet_Cassette_Day,     KPVL::Cassette::CassetteDay, &conn_kpvl)},      //���� ID �����
            {HMISheetData.Cassette.Month = new Value(AppHMISheetData2 + "Month",               HWNDCLIENT::hEdit_Sheet_Cassette_Month,   KPVL::Cassette::CassetteMonth, &conn_kpvl)},    //����� ID �����
            {HMISheetData.Cassette.Year = new Value(AppHMISheetData2 + "Year",                HWNDCLIENT::hEdit_Sheet_Cassette_Year,    KPVL::Cassette::CassetteYear, &conn_kpvl)},     //��� ID �����
            {HMISheetData.Cassette.SheetInCassette = new Value(AppHMISheetData2 + "SheetInCassette",     HWNDCLIENT::hEdit_Sheet_InCassette,       KPVL::Cassette::Sheet_InCassette, &conn_kpvl)}, //����� ����� � ������
        #pragma endregion

        #pragma region ���������� ����� �� ��������
            {HMISheetData.Top_Side.h1 = new Value(AppHMISheetData + "Top_Side.h1",     HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� �� ��������
            {HMISheetData.Top_Side.h2 = new Value(AppHMISheetData + "Top_Side.h2",     HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� �� ��������
            {HMISheetData.Top_Side.h3 = new Value(AppHMISheetData + "Top_Side.h3",     HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� �� ��������
            {HMISheetData.Top_Side.h4 = new Value(AppHMISheetData + "Top_Side.h4",     HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� �� ��������
            {HMISheetData.Top_Side.h5 = new Value(AppHMISheetData + "Top_Side.h5",     HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� �� ��������
            {HMISheetData.Top_Side.h6 = new Value(AppHMISheetData + "Top_Side.h6",     HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� �� ��������
            {HMISheetData.Top_Side.h7 = new Value(AppHMISheetData + "Top_Side.h7",     HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� �� ��������
            {HMISheetData.Top_Side.h8 = new Value(AppHMISheetData + "Top_Side.h8",     HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� �� ��������

            {HMISheetData.Bot_Side.h1 = new Value(AppHMISheetData + "Bottom_Side.h1",  HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� ����� ��������
            {HMISheetData.Bot_Side.h2 = new Value(AppHMISheetData + "Bottom_Side.h2",  HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� ����� ��������
            {HMISheetData.Bot_Side.h3 = new Value(AppHMISheetData + "Bottom_Side.h3",  HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� ����� ��������
            {HMISheetData.Bot_Side.h4 = new Value(AppHMISheetData + "Bottom_Side.h4",  HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� ����� ��������
            {HMISheetData.Bot_Side.h5 = new Value(AppHMISheetData + "Bottom_Side.h5",  HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� ����� ��������
            {HMISheetData.Bot_Side.h6 = new Value(AppHMISheetData + "Bottom_Side.h6",  HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� ����� ��������
            {HMISheetData.Bot_Side.h7 = new Value(AppHMISheetData + "Bottom_Side.h7",  HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� ����� ��������
            {HMISheetData.Bot_Side.h8 = new Value(AppHMISheetData + "Bottom_Side.h8",  HWNDCLIENT::hNull, KPVL::Side, &conn_kpvl)}, //���������� ����� ��������
        #pragma endregion

        #pragma region ��� ����������� �����
            {GenSeqToHmi.Seq_1_StateNo = new Value(AppGenSeqToHmi + "Seq_1_StateNo",  HWNDCLIENT::hEditState_11, KPVL::ZState::DataPosState_1, &conn_kpvl)},    //����� ���� ������������������ �������� � ����
            {GenSeqToHmi.Seq_2_StateNo = new Value(AppGenSeqToHmi + "Seq_2_StateNo",  HWNDCLIENT::hEditState_21, KPVL::ZState::DataPosState_2, &conn_kpvl)},    //����� ���� ������������������ �������� � ����
            {GenSeqToHmi.Seq_3_StateNo = new Value(AppGenSeqToHmi + "Seq_3_StateNo",  HWNDCLIENT::hEditState_31, KPVL::ZState::DataPosState_3, &conn_kpvl)},    //����� ���� ������������������ ��������� � ���������

            {GenSeqToHmi.HeatTime_Z1 = new Value(AppGenSeqToHmi + "HeatTime_Z1",  HWNDCLIENT::hEditState_13, 0, &conn_kpvl)},      //����� ������� � ���� 1
            {GenSeqToHmi.HeatTime_Z2 = new Value(AppGenSeqToHmi + "HeatTime_Z2",  HWNDCLIENT::hEditState_23, 0, &conn_kpvl)},      //����� ������� � ���� 2
            {GenSeqToHmi.CoolTimeAct = new Value(AppGenSeqToHmi + "CoolTimeAct",  HWNDCLIENT::hEditState_33, 0, &conn_kpvl)},      //������� ������ ������� ����������
        #pragma endregion

        #pragma region ����������� ����
            {Hmi210_1.Htr1_1 = new Value(PathKpvl + "Hmi210_1.Htr_1.ToHmi.TAct",   HWNDCLIENT::hEditTemp11TAct, 0, &conn_kpvl)},       //����������� � ���� 1.1
            {Hmi210_1.Htr1_2 = new Value(PathKpvl + "Hmi210_1.Htr_2.ToHmi.TAct",   HWNDCLIENT::hEditTemp12TAct, 0, &conn_kpvl)},       //����������� � ���� 1.2
            {Hmi210_1.Htr1_3 = new Value(PathKpvl + "Hmi210_1.Htr_3.ToHmi.TAct",   HWNDCLIENT::hEditTemp13TAct, 0, &conn_kpvl)},       //����������� � ���� 1.3
            {Hmi210_1.Htr1_4 = new Value(PathKpvl + "Hmi210_1.Htr_4.ToHmi.TAct",   HWNDCLIENT::hEditTemp14TAct, 0, &conn_kpvl)},       //����������� � ���� 1.4
            {Hmi210_1.Htr2_1 = new Value(PathKpvl + "Hmi210_1.Htr2_1.ToHmi.TAct",  HWNDCLIENT::hEditTemp21TAct, KPVL::Temperature, &conn_kpvl)},       //����������� � ���� 2.1
            {Hmi210_1.Htr2_2 = new Value(PathKpvl + "Hmi210_1.Htr2_2.ToHmi.TAct",  HWNDCLIENT::hEditTemp22TAct, KPVL::Temperature, &conn_kpvl)},       //����������� � ���� 2.2
            {Hmi210_1.Htr2_3 = new Value(PathKpvl + "Hmi210_1.Htr2_3.ToHmi.TAct",  HWNDCLIENT::hEditTemp23TAct, KPVL::Temperature, &conn_kpvl)},       //����������� � ���� 2.3
            {Hmi210_1.Htr2_4 = new Value(PathKpvl + "Hmi210_1.Htr2_4.ToHmi.TAct",  HWNDCLIENT::hEditTemp24TAct, KPVL::Temperature, &conn_kpvl)},       //����������� � ���� 2.4
        #pragma endregion

};


bool cmpAllTagKpvl(Value* first, Value* second)
{
    return first->Patch > second->Patch;
}



void PLC_KPVL::DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr)
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
                            return;
                        }
                    }
                    SetWindowText(winmap(hEditMode1), "���� ������...");
                }

            }
            CATCH(HardLogger, "");
            //catch(std::runtime_error& exc)
            //{
            //    SetWindowText(winmap(hEditMode1), "DataChange runtime_error");
            //    LOG_ERROR(HardLogger, "{:90| DataChange Error {}, {}", FUNCTION_LINE_NAME, exc.what(), node.ToString());
            //}
            //catch(...)
            //{
            //    SetWindowText(winmap(hEditMode1), "Unknown error");
            //    LOG_ERROR(HardLogger, "{:90| DataChange Error 'Unknown error' {}", FUNCTION_LINE_NAME, node.ToString());
            //};
        }
    }
}


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
    //        //bool WDG_toBase = AllTagKpvl[std::string("HMISheetData.Sheet.WDG_toBase")].GetBool();
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
    if(WatchDog == TRUE)
    {
        WatchDog = FALSE;
        if(WatchDogWait)
        {
            if(WatchDogWait >= CountWatchDogWait - 1)
                LOG_INFO(Logger, "{:90}| ���������� ����� {}", FUNCTION_LINE_NAME, WatchDogWait);
            SetWindowText(winmap(hEditDiagnose8), KPVL::ServerDataTime.c_str());
        }
        WatchDogWait = 0;
    }
    else
    {
        WatchDogWait++; //�������������� ������� ������ �����
        if(WatchDogWait >= CountWatchDogWait)
        {
            LOG_INFO(Logger, "{:90}| ����������: ���� ����� ��� ������ {} ������", FUNCTION_LINE_NAME, CountWatchDogWait);
            SetWindowText(winmap(hEditDiagnose8), KPVL::ServerDataTime.c_str());
            SetWindowText(winmap(hEditDiagnose6), std::to_string(WatchDogWait).c_str());
            return true;
        }
        else
        {
            if(WatchDogWait >= CountWatchDogWait - 1)
                LOG_INFO(Logger, "{:90}| ���� ����� ��� ������ {} ������", FUNCTION_LINE_NAME, WatchDogWait);
            SetWindowText(winmap(hEditDiagnose8), KPVL::ServerDataTime.c_str());
        }
    }
    SetWindowText(winmap(hEditDiagnose6), std::to_string(WatchDogWait).c_str());
    return false;
}


void PLC_KPVL::InitNodeId()
{
    try{
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
    }
    CATCH(PethLogger, "");

#ifndef TESTTEMPER
    Par_Gen.UnloadSpeed->coeff = 1000;
#endif
}

void PLC_KPVL::InitTag()
{
    LOG_INFO(Logger, "{:90}| ������������� ����������... countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    try
    {
        //������� ����� �����������
        avid[MSSEC::sec01000].push_back({nodeCurrentTime.GetId(), OpcUa::AttributeId::Value});

        for(auto& a : AllTagKpvl)
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
                LOG_INFO(Logger, "{:90}| SubscribeDataChange msec: {}, count: {}", FUNCTION_LINE_NAME, ar.first, ar.second.size());
                css[ar.first].Create(*this, ar.first, Logger);;
                std::vector<uint32_t> monitoredItemsIds = css[ar.first].sub->SubscribeDataChange(ar.second);
            }
        }   
        CATCHINIT();
    }
}

void SetText(Value* v, TextBool& t)
{
    SetWindowText(winmap(v->winId), t[v->Val.As<bool>()].c_str());
    InvalidateRect(winmap(v->winId), NULL, 0);
}

void InitText()
{
        KPVL::Mask::DataMaskKlapan1(HMISheetData.Valve_1x);
        KPVL::Mask::DataMaskKlapan2(HMISheetData.Valve_2x);

        SetText(HMISheetData.NewData, CantTextOut);
        SetText(HMISheetData.CasseteIsFill, CassetTextOut);

        SetWindowText(winmap(hEditState_12), GenSeq1[GenSeqToHmi.Seq_1_StateNo->GetInt()].c_str());
        SetWindowText(winmap(hEditState_22), GenSeq2[GenSeqToHmi.Seq_2_StateNo->GetInt()].c_str());
        SetWindowText(winmap(hEditState_32), GenSeq3[GenSeqToHmi.Seq_3_StateNo->GetInt()].c_str());
}

void PLC_KPVL::Run(int count)
{
    countconnect1 = count;
    countconnect2 = 0;

    LOG_INFO(Logger, "{:90}| Run... : countconnect = {}.{} to: {}", FUNCTION_LINE_NAME, countconnect1, countconnect2, Uri);

    try
    {
        InitGoot = FALSE;
        countget = 1;

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

        LOG_INFO(Logger, "{:90}| ����������� ������� countconnect = {}.{}\r\n", FUNCTION_LINE_NAME, countconnect1, countconnect2);

        for(auto val : AllTagKpvl)
            MySetWindowText(val);

        WatchDogWait = 0;
        SetWindowText(winmap(hEditDiagnose6), std::to_string(WatchDogWait).c_str());

        InitGoot = TRUE;
        ULONGLONG time1 = GetTickCount64();


        InitText();

        isInitPLC_KPVL = true;
        SekRun = time(NULL);
        SetWindowText(winmap(hEditMode1), "������ ������");
        //KPVL::Sheet::Z6::Old_SheetInCassette = HMISheetData.Cassette.SheetInCassette->GetValue().As<int16_t>();
        while(isRun /*&& KeepAlive.Running*/)
        {
			if(!KeepAlive.Running)
				LOG_WARN(Logger, "{:90}| KeepAlive.Running = 0", FUNCTION_LINE_NAME, countconnect1, countconnect2);

            HMISheetData.WDG_fromBase->Set_Value(true);

            //��������� �� ����� ���� �� ��������
            if(HMISheetData.NewData->Val.As<bool>())                   //���� ���� �����
            {
                if(++NewDataVal > 10)    //10 ������ �� 1 �������
                {
                    LOG_INFO(Logger, "{:90}| SaveDone.Set_Value (true)\r\n", FUNCTION_LINE_NAME);
                    //PGConnection conn_temp; 
                    //CONNECTION1(conn_temp, Logger);
                    CreateThread(0, 0, KPVL::Sheet::Z6::SetSaveDone, (LPVOID)0, 0, 0);
                    //KPVL::Sheet::Z6::SetSaveDone(conn_temp);
                }
            }
            else
            {
                NewDataVal = 0;
            }


            //��������� WatchDog
            if(WD())
                throw std::runtime_error(std::string(std::string("����������: ���� ����� ��� ������ ") + std::to_string(CountWatchDogWait) + " ������").c_str());

            //�������� 1 �������, ����� ����� ���������� �� ������
            TestTimeRun(time1);

            //������� ����� ������
            GetWD();

        }

        for(auto&a : css)
            a.second.Delete();

        return;
    }
    CATCH_RUN(Logger);

    LOG_INFO(Logger, "{:90}| ����� �� ������ 1 countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
    if(isRun)
    {
        LOG_INFO(Logger, "{:90}| ���� 5 ������... ��� {}", FUNCTION_LINE_NAME, Uri);
        int f = 5;
        while(--f && isRun) std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    try
    {
        //LOG_INFO(Logger, "{:90}| delete Sub countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
        //SetWindowText(winmap(hEditMode1), "delete Sub");
        //for(auto s : cssKPVL)s.second.Delete();

#if NEWS
        LOG_INFO(Logger, "{:90}| reset countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
        SetWindowText(winmap(hEditMode1), "reset");
        client.reset();
#endif
    }
    CATCH_RUN(Logger);

    LOG_INFO(Logger, "{:90}| ... ����� {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
};


DWORD WINAPI Open_KPVL_RUN(LPVOID)
{
    std::shared_ptr<spdlog::logger> Logger = HardLogger;

    LOG_INFO(Logger, "{:90}| ����� to: {}", FUNCTION_LINE_NAME, KPVL::URI);

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
            //auto PLC = std::unique_ptr<PLC_KPVL>(new PLC_KPVL(KPVL::URI, Logger));
            //LOG_INFO(Logger, "{:90}| ����������� {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);
            //PLC->Run(countconnect);

            LOG_INFO(Logger, "{:90}| �������� ������ PLC_KPVL {}", FUNCTION_LINE_NAME, countconnect);
            SetWindowText(winmap(hEditMode1), "�������� �������");

            PLC_KPVL PLC(KPVL::URI, Logger);
            LOG_INFO(Logger, "{:90}| ����������� {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);
            PLC.Run(countconnect);
            LOG_INFO(Logger, "{:90}| ����� �� Run countconnect = {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);
        }
        CATCH_OPEN(Logger, KPVL::URI);

        LOG_INFO(Logger, "{:90}| ����� �� ������ countconnect = {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);

        if(isRun)
        {
            int f = 10;
            while(--f && isRun) std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            countconnect++;
            LOG_INFO(Logger, "{:90}| ��������� ������� {} to: {}", FUNCTION_LINE_NAME, countconnect, KPVL::URI);
        }
    }

    LOG_INFO(Logger, "{:90}| ExitThread. isRun = {}", FUNCTION_LINE_NAME, isRun);

    return 0;
}


void GetEndData(PGConnection& conn, TSheet& sheet)
{
    std::string command = "SELECT FROM todos WHERE pos > 2 AND id = " + sheet.id;
    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
    PGresult* res = conn.PGexec(command);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        LOG_ERR_SQL(HardLogger, res, command);
    PQclear(res);
}


void TestSheet(PGConnection& conn)
{
    std::vector <std::string> IDS;
    //�������� "�����" ������
    //DELETE FROM sheet WHERE id = 15825;
    std::string command = "SELECT id FROM sheet WHERE delete_at IS NULL AND datatime_end IS NULL AND (pos > 5 AND secondpos_at IS NULL AND datatime_end IS NULL AND correct IS NULL AND pdf = '') AND create_at > '2024-09-11 00:00:00' ORDER BY id;";
    PGresult* res = conn.PGexec(command);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
        {
            std::string ids = conn.PGgetvalue(res, l, 0);
            if(ids.length())
                IDS.push_back(ids);
        }
    }
    else
        LOG_ERR_SQL(HardLogger, res, command);
    PQclear(res);

    for(auto& ids : IDS)
    {
        std::stringstream command;
        //command << "DELETE FROM sheet WHERE id = " << ids;
        command << "UPDATE sheet SET delete_at = now(), correct = now() WHERE id = " << ids;
        LOG_INFO(HardLogger, "{:90}| {}", command.str());
        SETUPDATESQL(HardLogger, conn, command);
    }
}

DWORD WINAPI Open_KPVL_SQL(LPVOID)
{
    size_t old_count = 0;
    
    LOG_INFO(HardLogger, "{:90}| Start Open_KPVL_SQL", FUNCTION_LINE_NAME);

    PGConnection conn;
    CONNECTION1(conn, HardLogger);
    while(isRun)
    {
        try
        {
            TestSheet(conn);
            KPVL::SQL::KPVL_SQL(conn, AllSheet);
            if(isRun)
            for(auto& TS : AllSheet)
            {
                int Pos = Stoi(TS.Pos);
                if(Pos >= 20)
                {
                    int pos1 = Pos % 10;
                    int pos2 = Pos / 10;
                    TS.Pos = std::to_string(pos1 + pos2);
                    std::stringstream sss;
                    sss << "UPDATE sheet SET pos = " << TS.Pos << " WHERE delete_at IS NULL AND id = " << TS.id;
                    LOG_INFO(HardLogger, "{:90}| {}", sss.str());
                    SETUPDATESQL(HardLogger, conn, sss);
                }
    //#ifndef _DEBUG
                KPVL::SQL::GetDataTime_All(conn, TS);
    //#endif
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
#endif
            }

            int f = 30;
            while(isRun && (--f))
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        CATCH(HardLogger, "");
    }
    LOG_INFO(HardLogger, "{:90} Stop Open_KPVL_SQL", FUNCTION_LINE_NAME);

    return 0;
}



void Open_KPVL()
{
    InitLogger(HardLogger);

    if(AllTagKpvl.size() > 2)
        std::sort(AllTagKpvl.begin() + 2, AllTagKpvl.end(), cmpAllTagKpvl);

#ifndef TESTSPIS
#ifndef TESTWIN
#ifndef TESTGRAFF
    hKPVLURI = CreateThread(0, 0, Open_KPVL_RUN, (LPVOID)0, 0, 0);
    hKPVLSQL = CreateThread(0, 0, Open_KPVL_SQL, (LPVOID)0, 0, 0);
    hRunAllPdf = CreateThread(0, 0, PDF::RunCassettelPdf, (LPVOID)0, 0, 0);
    //CreateThread(0, 0, PDF::CorrectSheet, (LPVOID)0, 0, 0);
#endif

#ifdef _DEBUG
    //hKPVLSQL = CreateThread(0, 0, Open_KPVL_SQL, (LPVOID)0, 0, 0);
    //CreateThread(0, 0, PDF::CorrectSheet2, (LPVOID)0, 0, 0);
    hRunAllPdf = CreateThread(0, 0, PDF::RunCassettelPdf, (LPVOID)0, 0, 0);
    //CreateThread(0, 0, PDF::CorrectSheet, (LPVOID)0, 0, 0);
#endif // _DEBUG

#endif
#endif


    

}

void Close_KPVL()
{
    WaitCloseTheread(hThreadState2, "ThreadState2");
    WaitCloseTheread(hKPVLURI, "hKPVLURI");
    WaitCloseTheread(hKPVLSQL, "hKPVLSQL");
    WaitCloseTheread(hRunAllPdf, "hRunAllPdf");
       
}
