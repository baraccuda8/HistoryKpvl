#pragma once

#include "Subscript.h"
#include "ClCodeSys.h"
#include "ValueTag.h"


#define WaitKant "�������� �������� �����"
#define WaitResv "�������� ������ �� ���"
#define WaitCassette "�������� �������"
#define FillCassette "������� ����������"


extern std::string MaskKlapan1;
extern std::string MaskKlapan2;
extern std::string MaskKlapan;

extern HANDLE hThreadState2;

extern time_t PLC_KPVL_old_dt;

//class ClassDataChangeKPVL: public ClassDataChange
//{
//public:
//    void DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr);
//};

class PLC_KPVL: public Client, ClassDataChange
{
public:
    void DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr);

    //ClassDataChangeKPVL DataChangeKPVL;
    PLC_KPVL(std::string uri, std::shared_ptr<spdlog::logger>& log): Client(uri, log){};
    //void Connect() override;

    void InitNodeId() override;
    void InitTag() override;
    void Run(int countconnect) override;
    void GetWD();
    bool WD();
};


//namespace {
    typedef struct T_PlateData{
        bool news = false;
        //string ��� �����
        Value* AlloyCodeText;

        //string ��� �������. ������� �� ����� �����.
        Value* ThiknessText;

        //int32_t ����� ������
        Value* Melt;

        //int32_t ����� �����
        Value* Slab;

        //int32_t ����� ������
        Value* PartNo;

        //int32_t �����
        Value* Pack;

        //int32_t ����� �����
        Value* Sheet;

        //int32_t �������� �����
        Value* SubSheet;
    };

    typedef struct T_TopBot{
        //float
        Value* Top;

        //float
        Value* Bot;
    };
    typedef struct T_Side{
        //float ������ ����� � �����1
        Value* h1;

        //float ������ ����� � �����2
        Value* h2;

        //float ������ ����� � �����3
        Value* h3;

        //float ������ ����� � �����4
        Value* h4;

        //float ������ ����� � �����5
        Value* h5;

        //float ������ ����� � �����6
        Value* h6;

        //float ������ ����� � �����7
        Value* h7;

        //float ������ ����� � �����8
        Value* h8;
    };
    typedef struct T_CassetteData{
        //int32_t ��� ID �����
        Value* Year;

        //int32_t ����� ID �����
        Value* Month;

        //int32_t ���� ID �����
        Value* Day;

        //uint16_t ��� ID �����
        Value* Hour;

        //int32_t ����� ������� �� ����
        Value* CassetteNo;

        //int16_t ����� ����� � ������
        Value* SheetInCassette;
    };

    //typedef struct O_CassetteData{
    //    //int32_t ID �������
    //    int32_t Id = 0;
    //
    //    //int32_t ��� ID �����
    //    int32_t Year = 0;
    //
    //    //int32_t ����� ID �����
    //    int32_t Month = 0;
    //
    //    //int32_t ���� ID �����
    //    int32_t Day = 0;
    //
    //    //int32_t ��� ID �����
    //    int32_t Hour = 0;
    //
    //    //int32_t ����� ������� �� ����
    //    int32_t CassetteNo = 0;
    //
    //    //int16_t ����� ����� � ������
    //    int16_t SheetInCassette = 0;
    //};


    typedef struct T_Par_Gen{
        //float �������� ��������
        Value* UnloadSpeed;

        //float ����� ������������ ��������� �������, ���
        Value* TimeForPlateHeat;

        //float ������� �������� ��� ������� �����������
        Value* PresToStartComp;
    };

    typedef struct T_AI_Hmi_210{
        //float �������� ���� ������� ��������� ����
        Value* LaminPressTop;

        //float �������� ���� ������� ��������� ���
        Value* LaminPressBot;

        //float ����������� ���� � �������
        Value* LAM_TE1;

        //float ����������� ���� � ����
        Value* Za_TE3;

        //float �������� ���� � ����
        Value* Za_PT3;

        //float ��������
        Value* Za_TE4;
    };
    typedef struct T_HMISheetData{
        //int32_t ������� ������ �����������
        Value* WDG;

        //bool ��� ����� ��� ����
        Value* WDG_toBase;

        //bool �������� ��� ����� ��� �����������
        Value* WDG_fromBase;

        //bool ����� ���� � ������
        Value* NewData;

        //bool ����� ����� ���� � ������
        Value* SaveDone;

        //bool ������� �����������
        Value* CasseteIsFill;

        ////BOOL ������ ����� �������
        //Value* StartNewCassette;          
        // 
        ////������� ������
        //Value* CassetteIsComplete;        

        //uint16_t ������� ����
        Value* Valve_1x;

        //uint16_t ������ ���
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
        //float ������� ����������� � ����
        Value* TempSet1;
    };

    typedef struct T_GenSeqToHmi{
        //int16_t ����� ���� ������������������ �������� � ����
        Value* Seq_1_StateNo;

        //int16_t ����� ���� ������������������ �������� � ����
        Value* Seq_2_StateNo;

        //int16_t ����� ���� ������������������ ��������� � ���������
        Value* Seq_3_StateNo;

        //float ����� ������� � ���� 1
        Value* HeatTime_Z1;

        //float ����� ������� � ���� 2
        Value* HeatTime_Z2;

        //float ������� ������ ������� ����������
        Value* CoolTimeAct;
    };

    typedef struct T_Hmi210_1{
        float Temperature = 0;

        //float ����������� � ���� 1.1
        Value* Htr1_1;

        //float ����������� � ���� 1.2
        Value* Htr1_2;

        //float ����������� � ���� 1.3
        Value* Htr1_3;

        //float ����������� � ���� 1.4
        Value* Htr1_4;

        //float ����������� � ���� 2.1
        Value* Htr2_1;

        //float ����������� � ���� 2.2
        Value* Htr2_2;

        //float ����������� � ���� 2.3
        Value* Htr2_3;

        //float ����������� � ���� 2.4
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

