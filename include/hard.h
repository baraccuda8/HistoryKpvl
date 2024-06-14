#pragma once

#include "Subscript.h"
#include "ClCodeSys.h"
#include "ValueTag.h"


#define WaitKant "Ожидание кантовки листа"
#define WaitResv "Ожидание ответа от ПЛС"

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
        //Char Код марки
        Value* AlloyCodeText;

        //Char код толщины. Зависит от марки стали.
        Value* ThiknessText;

        //DINT номер плавки
        Value* Melt;

        //DINT Номер сляба
        Value* Slab;

        //DINT номер партии
        Value* PartNo;

        //DINT Пачка
        Value* Pack;

        //DINT номер листа
        Value* Sheet;

        //DINT подномер листа
        Value* SubSheet;
    };
    typedef struct T_TopBot{
        Value* Top;                  //REAL
        Value* Bot;                  //REAL
    };
    typedef struct T_Side{
        //REAL высота листа в точке1
        Value* h1;

        //REAL высота листа в точке2
        Value* h2;

        //REAL высота листа в точке3
        Value* h3;

        //REAL высота листа в точке4
        Value* h4;

        //REAL высота листа в точке5
        Value* h5;

        //REAL высота листа в точке6
        Value* h6;

        //REAL высота листа в точке7
        Value* h7;

        //REAL высота листа в точке8
        Value* h8;
    };
    typedef struct T_CassetteData{
        //DINT Год ID листа
        Value* Year;

        //DINT Месяц ID листа
        Value* Month;

        //DINT День ID листа
        Value* Day;

        //DINT Час ID листа
        Value* Hour;

        //DINT Номер кассеты за день
        Value* CassetteNo;

        //INT Номер листа в касете
        Value* SheetInCassette;
    };

    typedef struct O_CassetteData{
        //DINT ID кассеты
        int32_t Id = 0;

        //DINT Год ID листа
        int32_t Year = 0;

        //DINT Месяц ID листа
        int32_t Month = 0;

        //DINT День ID листа
        int32_t Day = 0;

        //DINT Час ID листа
        int32_t Hour = 0;


        //DINT Номер кассеты за день
        int32_t CassetteNo = 0;

        //INT Номер листа в касете
        int16_t SheetInCassette = 0;
    };


    typedef struct T_Par_Gen{
        //REAL Скорость выгрузки
        Value* UnloadSpeed;

        //REAL Время сигнализации окончания нагрева, мин
        Value* TimeForPlateHeat;

        //REAL Уставка давления для запуска комперссора
        Value* PresToStartComp;
    };

    typedef struct T_AI_Hmi_210{
        //REAL Давление воды закалка коллектор верх
        Value* LaminPressTop;

        //REAL Давление воды закалка коллектор низ
        Value* LaminPressBot;

        //REAL Температура воды в поддоне
        Value* LAM_TE1;

        //REAL Температура воды в баке
        Value* Za_TE3;

        //REAL Температура воды в баке
        Value* Za_PT3;

        //REAL Пирометр
        Value* Za_TE4;
    };
    typedef struct T_HMISheetData{
        //DINT Счетчик циклов контроллера
        Value* WDG;

        //BOOL Бит жизни для базы
        Value* WDG_toBase;

        //BOOL Обратный бит жизни для контроллера
        Value* WDG_fromBase;

        //BOOL Новые лист в касету
        Value* NewData;

        //BOOL Ответ Новые лист в касету
        Value* SaveDone;

        //BOOL кассета наполяентся
        Value* CasseteIsFill;

        ////BOOL начать новую кассету
        //Value* StartNewCassette;          
        // 
        ////кассета готова
        //Value* CassetteIsComplete;        

        //WORD Кларана верх
        Value* Valve_1x;

        //WORD Клапна низ
        Value* Valve_2x;

        //Скоростная секция
        T_TopBot SpeedSection;

        //Ламинарная секция 1
        T_TopBot LaminarSection1;

        //Ламинарная секия 2
        T_TopBot LaminarSection2;

        ////Данные листа на кантовке
        //T_PlateData SheetDataIN;              
        
        //Данные касеты на кантовке  
        T_CassetteData Cassette;                

        //Замеры отклонения до кантовки
        T_Side Top_Side;

        //Замеры отклонения после кантовки
        T_Side Bot_Side;
    };

    typedef struct T_GenSeqFromHmi{
        //REAL Уставки температуры в печи
        Value* TempSet1;
    };

    typedef struct T_GenSeqToHmi{
        //INT Номер шага последовательности загрузки в печь
        Value* Seq_1_StateNo;

        //INT Номер шага последовательности выгрузки в печи
        Value* Seq_2_StateNo;

        //INT Номер шага последовательности обработки в ламинарке
        Value* Seq_3_StateNo;

        //REAL время нагрева в зоне 1
        Value* HeatTime_Z1;

        //REAL время нагрева в зоне 2
        Value* HeatTime_Z2;

        //REAL Сколько прошло времени охлаждения
        Value* CoolTimeAct;
    };

    typedef struct T_Hmi210_1{
        float Temperature = 0;

        //REAL Температура в зоне 1.1
        Value* Htr1_1;

        //REAL Температура в зоне 1.2
        Value* Htr1_2;

        //REAL Температура в зоне 1.3
        Value* Htr1_3;

        //REAL Температура в зоне 1.4
        Value* Htr1_4;

        //REAL Температура в зоне 2.1
        Value* Htr2_1;

        //REAL Температура в зоне 2.2
        Value* Htr2_2;

        //REAL Температура в зоне 2.3
        Value* Htr2_3;

        //REAL Температура в зоне 2.4
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

