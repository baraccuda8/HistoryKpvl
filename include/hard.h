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
        Value* AlloyCodeText;       //Char Код марки
        Value* ThiknessText;        //Char код толщины. Зависит от марки стали.
        Value* Melt;                //DINT номер плавки
        Value* Slab;                //DINT Номер сляба
        Value* PartNo;              //DINT номер партии
        Value* Pack;                //DINT Пачка
        Value* Sheet;               //DINT номер листа
        Value* SubSheet;            //DINT подномер листа
    };
    typedef struct T_TopBot{
        Value* Top;                  //REAL
        Value* Bot;                  //REAL
    };
    typedef struct T_Side{
        Value* h1; //REAL высота листа в точке1
        Value* h2; //REAL высота листа в точке2
        Value* h3; //REAL высота листа в точке3
        Value* h4; //REAL высота листа в точке4
        Value* h5; //REAL высота листа в точке5
        Value* h6; //REAL высота листа в точке6
        Value* h7; //REAL высота листа в точке7
        Value* h8; //REAL высота листа в точке8
    };
    typedef struct T_CassetteData{
        Value* Year; //DINT Год ID листа
        Value* Month; //DINT Месяц ID листа
        Value* Day; //DINT День ID листа
        Value* CassetteNo; //DINT Номер кассеты за день
        Value* SheetInCassette; //INT Номер листа в касете
    };

    typedef struct O_CassetteData{
        int32_t Id = 0; //DINT Номер кассеты за день
        int32_t Year = 0; //DINT Год ID листа
        int32_t Month = 0; //DINT Месяц ID листа
        int32_t Day = 0; //DINT День ID листа
        int32_t CassetteNo = 0; //DINT Номер кассеты за день
        int16_t SheetInCassette = 0; //INT Номер листа в касете
    };


    typedef struct T_Par_Gen{
        Value* UnloadSpeed;          //REAL Скорость выгрузки
        Value* TimeForPlateHeat;     //REAL Время сигнализации окончания нагрева, мин
        Value* PresToStartComp;      //REAL Уставка давления для запуска комперссора
    };

    typedef struct T_AI_Hmi_210{
        Value* LaminPressTop;        //REAL Давление воды закалка коллектор верх
        Value* LaminPressBot;        //REAL Давление воды закалка коллектор низ
        Value* LAM_TE1;              //REAL Температура воды в поддоне
        Value* Za_TE3;               //REAL Температура воды в баке
        Value* Za_PT3;               //REAL Температура воды в баке
        Value* Za_TE4;               //REAL Пирометр
    };
    typedef struct T_HMISheetData{
        Value* WDG;                       //DINT Счетчик циклов контроллера
        Value* WDG_toBase;                //BOOL Бит жизни для базы
        Value* WDG_fromBase;              //BOOL Обратный бит жизни для контроллера
        Value* NewData;                   //BOOL Новые лист в касету
        Value* SaveDone;                  //BOOL Ответ Новые лист в касету
        Value* CasseteIsFill;             //BOOL кассета наполяентся
        //Value* StartNewCassette;          //BOOL начать новую кассету
        //Value* CassetteIsComplete;        //кассета готова
        Value* Valve_1x;              //WORD Кларана верх
        Value* Valve_2x;              //WORD Клапна низ
        T_TopBot SpeedSection;                  //Скоростная секция
        T_TopBot LaminarSection1;               //Ламинарная секция 1
        T_TopBot LaminarSection2;               //Ламинарная секия 2
        //T_PlateData SheetDataIN;                //Данные листа на кантовке
        T_CassetteData Cassette;                //Данные касеты на кантовке
        T_Side Top_Side;                        //Замеры отклонения
        T_Side Bot_Side;                        //Замеры отклонения
    };

    typedef struct T_GenSeqFromHmi{
        Value* TempSet1;                    //REAL Уставки температуры в печи
    };

    typedef struct T_GenSeqToHmi{
        Value* Seq_1_StateNo;           //INT Номер шага последовательности загрузки в печь
        Value* Seq_2_StateNo;           //INT Номер шага последовательности выгрузки в печи
        Value* Seq_3_StateNo;           //INT Номер шага последовательности обработки в ламинарке
        Value* HeatTime_Z1;             //REAL время нагрева в зоне 1
        Value* HeatTime_Z2;             //REAL время нагрева в зоне 2
        Value* CoolTimeAct;             //REAL Сколько прошло времени охлаждения
    };

    typedef struct T_Hmi210_1{
        Value* Htr1_1;       //REAL Температура в зоне 1.1
        Value* Htr1_2;       //REAL Температура в зоне 1.2
        Value* Htr1_3;       //REAL Температура в зоне 1.3
        Value* Htr1_4;       //REAL Температура в зоне 1.4
        Value* Htr2_1;      //REAL Температура в зоне 2.1
        Value* Htr2_2;      //REAL Температура в зоне 2.2
        Value* Htr2_3;      //REAL Температура в зоне 2.3
        Value* Htr2_4;      //REAL Температура в зоне 2.4
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
