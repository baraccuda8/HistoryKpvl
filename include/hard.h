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
        Value* AlloyCodeText;  //��� �����
        Value* ThiknessText;   //��� �������. ������� �� ����� �����.
        Value* Melt;                //����� ������
        Value* Slab;                //����� �����
        Value* PartNo;              //����� ������
        Value* Pack;                //�����
        Value* Sheet;               //����� �����
        Value* SubSheet;            //�������� �����
    };
    typedef struct T_TopBot{
        Value* Top;
        Value* Bot;
    };
    typedef struct T_Side{
        Value* h1;               //������ ����� � �����1
        Value* h2;               //������ ����� � �����2
        Value* h3;               //������ ����� � �����3
        Value* h4;               //������ ����� � �����4
        Value* h5;               //������ ����� � �����5
        Value* h6;               //������ ����� � �����6
        Value* h7;               //������ ����� � �����7
        Value* h8;               //������ ����� � �����8
    };
    typedef struct T_CassetteData{
        Value* CassetteNo;      //����� ������� �� ����
        Value* Day;             //���� ID �����
        Value* Month;           //����� ID �����
        Value* Year;            //��� ID �����
        Value* SheetInCassette; //����� ����� � ������
    };

    typedef struct T_Par_Gen{
        Value* UnloadSpeed;          //�������� ��������
        Value* TimeForPlateHeat;     //����� ������������ ��������� �������, ���
        Value* PresToStartComp;      //������� �������� ��� ������� �����������
    };

    typedef struct T_AI_Hmi_210{
        Value* LaminPressTop;        //�������� ���� ������� ��������� ����
        Value* LaminPressBot;        //�������� ���� ������� ��������� ���
        Value* LAM_TE1;              //����������� ���� � �������
        Value* Za_TE3;               //����������� ���� � ����
        Value* Za_PT3;               //����������� ���� � ����
        Value* Za_TE4;               //��������
    };
    typedef struct T_HMISheetData{
        Value* WDG;                       //������� ������ �����������
        Value* WDG_toBase;                //��� ����� ��� ����
        Value* WDG_fromBase;              //�������� ��� ����� ��� �����������
        Value* NewData;                   //����� ���� � ������
        Value* SaveDone;                  //����� ����� ���� � ������
        Value* CasseteIsFill;             //������� �����������
        Value* StartNewCassette;          //������ ����� �������
        Value* Valve_1x;              //������� ����
        Value* Valve_2x;              //������ ���
        T_TopBot SpeedSection;                  //���������� ������
        T_TopBot LaminarSection1;               //���������� ������ 1
        T_TopBot LaminarSection2;               //���������� ����� 2
        //T_PlateData SheetDataIN;                //������ ����� �� ��������
        T_CassetteData Cassette;                //������ ������ �� ��������
        T_Side Top_Side;                        //������ ����������
        T_Side Bot_Side;                        //������ ����������
    };

    typedef struct T_GenSeqFromHmi{
        Value* TempSet1;         //������� ����������� � ����
        //T_PlateData PlateData;          //������ ����� �� ����� � ����
    };

    typedef struct T_GenSeqToHmi{
            //Value* Pos_1_Occup;          //���� 1 ������ ������ (����, ����)
            //Value* Pos_2_Occup;          //���� 2 ������ ������ (����, �����)
            //Value* Pos_3_Occup;          //���� 3 ������ ������ (�������)
            //Value* Pos_4_Occup;          //���� 4 ������ ������ (���������)
        //T_PlateData PlateData_Z1;       //������ ����� � 1-� ����� ����
        //T_PlateData PlateData_Z2;       //������ ����� �� 2-� ����� ����
        //T_PlateData PlateData_Z3;       //������ ����� � �������
        //T_PlateData PlateData_Z4;       //������ ����� � ���������
        //T_PlateData PlateData_Z5;       //������ ����� �� ������
        Value* Seq_1_StateNo;  //����� ���� ������������������ �������� � ����
        Value* Seq_2_StateNo;  //����� ���� ������������������ �������� � ����
        Value* Seq_3_StateNo;  //����� ���� ������������������ ��������� � ���������
        Value* HeatTime_Z1;      //����� ������� � ���� 1
        Value* HeatTime_Z2;      //����� ������� � ���� 2
        Value* CoolTimeAct;      //������� ������ ������� ����������
    };

    typedef struct T_Hmi210_1{
        Value* Htr_1;       //����������� � ���� 1.1
        Value* Htr_2;       //����������� � ���� 1.2
        Value* Htr_3;       //����������� � ���� 1.3
        Value* Htr_4;       //����������� � ���� 1.4
        Value* Htr2_1;      //����������� � ���� 2.1
        Value* Htr2_2;      //����������� � ���� 2.2
        Value* Htr2_3;      //����������� � ���� 2.3
        Value* Htr2_4;      //����������� � ���� 2.4
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
