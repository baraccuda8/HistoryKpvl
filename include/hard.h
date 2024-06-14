#pragma once

#include "Subscript.h"
#include "ClCodeSys.h"
#include "ValueTag.h"


#define WaitKant "�������� �������� �����"
#define WaitResv "�������� ������ �� ���"

extern std::string MaskKlapan1;
extern std::string MaskKlapan2;
extern std::string MaskKlapan;

extern HANDLE hThreadState2;

extern time_t PLC_KPVL_old_dt;

class ClassDataChangeKPVL: public ClassDataChange
{
public:
    void DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr);
};


class PLC_KPVL: public Client
{
public:

    PLC_KPVL(std::string uri, std::shared_ptr<spdlog::logger>& log): Client(uri, log){};
    //void Connect() override;

    void InitNodeId() override;;
    void InitTag() override;;
    void Run(int countconnect) override;
    void GetWD();
    bool WD();
};


//namespace {
    typedef struct T_PlateData{
        bool news = false;
        //Char ��� �����
        Value* AlloyCodeText;

        //Char ��� �������. ������� �� ����� �����.
        Value* ThiknessText;

        //DINT ����� ������
        Value* Melt;

        //DINT ����� �����
        Value* Slab;

        //DINT ����� ������
        Value* PartNo;

        //DINT �����
        Value* Pack;

        //DINT ����� �����
        Value* Sheet;

        //DINT �������� �����
        Value* SubSheet;
    };
    typedef struct T_TopBot{
        Value* Top;                  //REAL
        Value* Bot;                  //REAL
    };
    typedef struct T_Side{
        //REAL ������ ����� � �����1
        Value* h1;

        //REAL ������ ����� � �����2
        Value* h2;

        //REAL ������ ����� � �����3
        Value* h3;

        //REAL ������ ����� � �����4
        Value* h4;

        //REAL ������ ����� � �����5
        Value* h5;

        //REAL ������ ����� � �����6
        Value* h6;

        //REAL ������ ����� � �����7
        Value* h7;

        //REAL ������ ����� � �����8
        Value* h8;
    };
    typedef struct T_CassetteData{
        //DINT ��� ID �����
        Value* Year;

        //DINT ����� ID �����
        Value* Month;

        //DINT ���� ID �����
        Value* Day;

        //DINT ��� ID �����
        Value* Hour;

        //DINT ����� ������� �� ����
        Value* CassetteNo;

        //INT ����� ����� � ������
        Value* SheetInCassette;
    };

    typedef struct O_CassetteData{
        //DINT ID �������
        int32_t Id = 0;

        //DINT ��� ID �����
        int32_t Year = 0;

        //DINT ����� ID �����
        int32_t Month = 0;

        //DINT ���� ID �����
        int32_t Day = 0;

        //DINT ��� ID �����
        int32_t Hour = 0;


        //DINT ����� ������� �� ����
        int32_t CassetteNo = 0;

        //INT ����� ����� � ������
        int16_t SheetInCassette = 0;
    };


    typedef struct T_Par_Gen{
        //REAL �������� ��������
        Value* UnloadSpeed;

        //REAL ����� ������������ ��������� �������, ���
        Value* TimeForPlateHeat;

        //REAL ������� �������� ��� ������� �����������
        Value* PresToStartComp;
    };

    typedef struct T_AI_Hmi_210{
        //REAL �������� ���� ������� ��������� ����
        Value* LaminPressTop;

        //REAL �������� ���� ������� ��������� ���
        Value* LaminPressBot;

        //REAL ����������� ���� � �������
        Value* LAM_TE1;

        //REAL ����������� ���� � ����
        Value* Za_TE3;

        //REAL ����������� ���� � ����
        Value* Za_PT3;

        //REAL ��������
        Value* Za_TE4;
    };
    typedef struct T_HMISheetData{
        //DINT ������� ������ �����������
        Value* WDG;

        //BOOL ��� ����� ��� ����
        Value* WDG_toBase;

        //BOOL �������� ��� ����� ��� �����������
        Value* WDG_fromBase;

        //BOOL ����� ���� � ������
        Value* NewData;

        //BOOL ����� ����� ���� � ������
        Value* SaveDone;

        //BOOL ������� �����������
        Value* CasseteIsFill;

        ////BOOL ������ ����� �������
        //Value* StartNewCassette;          
        // 
        ////������� ������
        //Value* CassetteIsComplete;        

        //WORD ������� ����
        Value* Valve_1x;

        //WORD ������ ���
        Value* Valve_2x;

        //���������� ������
        T_TopBot SpeedSection;

        //���������� ������ 1
        T_TopBot LaminarSection1;

        //���������� ����� 2
        T_TopBot LaminarSection2;

        ////������ ����� �� ��������
        //T_PlateData SheetDataIN;              
        
        //������ ������ �� ��������  
        T_CassetteData Cassette;                

        //������ ���������� �� ��������
        T_Side Top_Side;

        //������ ���������� ����� ��������
        T_Side Bot_Side;
    };

    typedef struct T_GenSeqFromHmi{
        //REAL ������� ����������� � ����
        Value* TempSet1;
    };

    typedef struct T_GenSeqToHmi{
        //INT ����� ���� ������������������ �������� � ����
        Value* Seq_1_StateNo;

        //INT ����� ���� ������������������ �������� � ����
        Value* Seq_2_StateNo;

        //INT ����� ���� ������������������ ��������� � ���������
        Value* Seq_3_StateNo;

        //REAL ����� ������� � ���� 1
        Value* HeatTime_Z1;

        //REAL ����� ������� � ���� 2
        Value* HeatTime_Z2;

        //REAL ������� ������ ������� ����������
        Value* CoolTimeAct;
    };

    typedef struct T_Hmi210_1{
        float Temperature = 0;

        //REAL ����������� � ���� 1.1
        Value* Htr1_1;

        //REAL ����������� � ���� 1.2
        Value* Htr1_2;

        //REAL ����������� � ���� 1.3
        Value* Htr1_3;

        //REAL ����������� � ���� 1.4
        Value* Htr1_4;

        //REAL ����������� � ���� 2.1
        Value* Htr2_1;

        //REAL ����������� � ���� 2.2
        Value* Htr2_2;

        //REAL ����������� � ���� 2.3
        Value* Htr2_3;

        //REAL ����������� � ���� 2.4
        Value* Htr2_4;
    };



//};

extern T_Par_Gen Par_Gen;
extern T_AI_Hmi_210 AI_Hmi_210;
extern T_HMISheetData HMISheetData;
extern T_GenSeqFromHmi GenSeqFromHmi;
extern T_GenSeqToHmi GenSeqToHmi;
extern T_Hmi210_1 Hmi210_1;

extern T_PlateData PlateData[7];

