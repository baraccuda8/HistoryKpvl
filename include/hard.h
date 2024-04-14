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
        Value* AlloyCodeText;       //Char ��� �����
        Value* ThiknessText;        //Char ��� �������. ������� �� ����� �����.
        Value* Melt;                //DINT ����� ������
        Value* Slab;                //DINT ����� �����
        Value* PartNo;              //DINT ����� ������
        Value* Pack;                //DINT �����
        Value* Sheet;               //DINT ����� �����
        Value* SubSheet;            //DINT �������� �����
    };
    typedef struct T_TopBot{
        Value* Top;                  //REAL
        Value* Bot;                  //REAL
    };
    typedef struct T_Side{
        Value* h1; //REAL ������ ����� � �����1
        Value* h2; //REAL ������ ����� � �����2
        Value* h3; //REAL ������ ����� � �����3
        Value* h4; //REAL ������ ����� � �����4
        Value* h5; //REAL ������ ����� � �����5
        Value* h6; //REAL ������ ����� � �����6
        Value* h7; //REAL ������ ����� � �����7
        Value* h8; //REAL ������ ����� � �����8
    };
    typedef struct T_CassetteData{
        Value* Year; //DINT ��� ID �����
        Value* Month; //DINT ����� ID �����
        Value* Day; //DINT ���� ID �����
        Value* CassetteNo; //DINT ����� ������� �� ����
        Value* SheetInCassette; //INT ����� ����� � ������
    };

    typedef struct O_CassetteData{
        int32_t Id = 0; //DINT ����� ������� �� ����
        int32_t Year = 0; //DINT ��� ID �����
        int32_t Month = 0; //DINT ����� ID �����
        int32_t Day = 0; //DINT ���� ID �����
        int32_t CassetteNo = 0; //DINT ����� ������� �� ����
        int16_t SheetInCassette = 0; //INT ����� ����� � ������
    };


    typedef struct T_Par_Gen{
        Value* UnloadSpeed;          //REAL �������� ��������
        Value* TimeForPlateHeat;     //REAL ����� ������������ ��������� �������, ���
        Value* PresToStartComp;      //REAL ������� �������� ��� ������� �����������
    };

    typedef struct T_AI_Hmi_210{
        Value* LaminPressTop;        //REAL �������� ���� ������� ��������� ����
        Value* LaminPressBot;        //REAL �������� ���� ������� ��������� ���
        Value* LAM_TE1;              //REAL ����������� ���� � �������
        Value* Za_TE3;               //REAL ����������� ���� � ����
        Value* Za_PT3;               //REAL ����������� ���� � ����
        Value* Za_TE4;               //REAL ��������
    };
    typedef struct T_HMISheetData{
        Value* WDG;                       //DINT ������� ������ �����������
        Value* WDG_toBase;                //BOOL ��� ����� ��� ����
        Value* WDG_fromBase;              //BOOL �������� ��� ����� ��� �����������
        Value* NewData;                   //BOOL ����� ���� � ������
        Value* SaveDone;                  //BOOL ����� ����� ���� � ������
        Value* CasseteIsFill;             //BOOL ������� �����������
        //Value* StartNewCassette;          //BOOL ������ ����� �������
        //Value* CassetteIsComplete;        //������� ������
        Value* Valve_1x;              //WORD ������� ����
        Value* Valve_2x;              //WORD ������ ���
        T_TopBot SpeedSection;                  //���������� ������
        T_TopBot LaminarSection1;               //���������� ������ 1
        T_TopBot LaminarSection2;               //���������� ����� 2
        //T_PlateData SheetDataIN;                //������ ����� �� ��������
        T_CassetteData Cassette;                //������ ������ �� ��������
        T_Side Top_Side;                        //������ ����������
        T_Side Bot_Side;                        //������ ����������
    };

    typedef struct T_GenSeqFromHmi{
        Value* TempSet1;                    //REAL ������� ����������� � ����
    };

    typedef struct T_GenSeqToHmi{
        Value* Seq_1_StateNo;           //INT ����� ���� ������������������ �������� � ����
        Value* Seq_2_StateNo;           //INT ����� ���� ������������������ �������� � ����
        Value* Seq_3_StateNo;           //INT ����� ���� ������������������ ��������� � ���������
        Value* HeatTime_Z1;             //REAL ����� ������� � ���� 1
        Value* HeatTime_Z2;             //REAL ����� ������� � ���� 2
        Value* CoolTimeAct;             //REAL ������� ������ ������� ����������
    };

    typedef struct T_Hmi210_1{
        Value* Htr1_1;       //REAL ����������� � ���� 1.1
        Value* Htr1_2;       //REAL ����������� � ���� 1.2
        Value* Htr1_3;       //REAL ����������� � ���� 1.3
        Value* Htr1_4;       //REAL ����������� � ���� 1.4
        Value* Htr2_1;      //REAL ����������� � ���� 2.1
        Value* Htr2_2;      //REAL ����������� � ���� 2.2
        Value* Htr2_3;      //REAL ����������� � ���� 2.3
        Value* Htr2_4;      //REAL ����������� � ���� 2.4
    };



//};

extern T_Par_Gen Par_Gen;
extern T_AI_Hmi_210 AI_Hmi_210;
extern T_HMISheetData HMISheetData;
extern T_GenSeqFromHmi GenSeqFromHmi;
extern T_GenSeqToHmi GenSeqToHmi;
extern T_Hmi210_1 Hmi210_1;

extern T_PlateData PlateData[7];

//extern std::map<int, T_PlateData*> PlateData;
