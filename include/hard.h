#pragma once

#include "Subscript.h"
#include "ClCodeSys.h"
#include "ValueTag.h"


#define WaitKant "Ожидание кантовки листа"
#define WaitResv "Ожидание ответа от ПЛС"
#define WaitCassette "Ожидание кассеты"
#define FillCassette "Кассета набирается"


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
        //string Код марки
        Value* AlloyCodeText;

        //string код толщины. Зависит от марки стали.
        Value* ThiknessText;

        //int32_t номер плавки
        Value* Melt;

        //int32_t Номер сляба
        Value* Slab;

        //int32_t номер партии
        Value* PartNo;

        //int32_t Пачка
        Value* Pack;

        //int32_t номер листа
        Value* Sheet;

        //int32_t подномер листа
        Value* SubSheet;
    };

    typedef struct T_TopBot{
        //float
        Value* Top;

        //float
        Value* Bot;
    };
    typedef struct T_Side{
        //float высота листа в точке1
        Value* h1;

        //float высота листа в точке2
        Value* h2;

        //float высота листа в точке3
        Value* h3;

        //float высота листа в точке4
        Value* h4;

        //float высота листа в точке5
        Value* h5;

        //float высота листа в точке6
        Value* h6;

        //float высота листа в точке7
        Value* h7;

        //float высота листа в точке8
        Value* h8;
    };
    typedef struct T_CassetteData{
        //int32_t Год ID листа
        Value* Year;

        //int32_t Месяц ID листа
        Value* Month;

        //int32_t День ID листа
        Value* Day;

        //uint16_t Час ID листа
        Value* Hour;

        //int32_t Номер кассеты за день
        Value* CassetteNo;

        //int16_t Номер листа в касете
        Value* SheetInCassette;
    };

    //typedef struct O_CassetteData{
    //    //int32_t ID кассеты
    //    int32_t Id = 0;
    //
    //    //int32_t Год ID листа
    //    int32_t Year = 0;
    //
    //    //int32_t Месяц ID листа
    //    int32_t Month = 0;
    //
    //    //int32_t День ID листа
    //    int32_t Day = 0;
    //
    //    //int32_t Час ID листа
    //    int32_t Hour = 0;
    //
    //    //int32_t Номер кассеты за день
    //    int32_t CassetteNo = 0;
    //
    //    //int16_t Номер листа в касете
    //    int16_t SheetInCassette = 0;
    //};


    typedef struct T_Par_Gen{
        //float Скорость выгрузки
        Value* UnloadSpeed;

        //float Время сигнализации окончания нагрева, мин
        Value* TimeForPlateHeat;

        //float Уставка давления для запуска комперссора
        Value* PresToStartComp;
    };

    typedef struct T_AI_Hmi_210{
        //float Давление воды закалка коллектор верх
        Value* LaminPressTop;

        //float Давление воды закалка коллектор низ
        Value* LaminPressBot;

        //float Температура воды в поддоне
        Value* LAM_TE1;

        //float Температура воды в баке
        Value* Za_TE3;

        //float Давление воды в баке
        Value* Za_PT3;

        //float Пирометр
        Value* Za_TE4;
    };
    typedef struct T_HMISheetData{
        //int32_t Счетчик циклов контроллера
        Value* WDG;

        //bool Бит жизни для базы
        Value* WDG_toBase;

        //bool Обратный бит жизни для контроллера
        Value* WDG_fromBase;

        //bool Новые лист в касету
        Value* NewData;

        //bool Ответ Новые лист в касету
        Value* SaveDone;

        //bool кассета наполяентся
        Value* CasseteIsFill;

        ////BOOL начать новую кассету
        //Value* StartNewCassette;          
        // 
        ////кассета готова
        //Value* CassetteIsComplete;        

        //uint16_t Кларана верх
        Value* Valve_1x;

        //uint16_t Клапна низ
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
        //float Уставки температуры в печи
        Value* TempSet1;
    };

    typedef struct T_GenSeqToHmi{
        //int16_t Номер шага последовательности загрузки в печь
        Value* Seq_1_StateNo;

        //int16_t Номер шага последовательности выгрузки в печи
        Value* Seq_2_StateNo;

        //int16_t Номер шага последовательности обработки в ламинарке
        Value* Seq_3_StateNo;

        //float время нагрева в зоне 1
        Value* HeatTime_Z1;

        //float время нагрева в зоне 2
        Value* HeatTime_Z2;

        //float Сколько прошло времени охлаждения
        Value* CoolTimeAct;
    };

    typedef struct T_Hmi210_1{
        float Temperature = 0;

        //float Температура в зоне 1.1
        Value* Htr1_1;

        //float Температура в зоне 1.2
        Value* Htr1_2;

        //float Температура в зоне 1.3
        Value* Htr1_3;

        //float Температура в зоне 1.4
        Value* Htr1_4;

        //float Температура в зоне 2.1
        Value* Htr2_1;

        //float Температура в зоне 2.2
        Value* Htr2_2;

        //float Температура в зоне 2.3
        Value* Htr2_3;

        //float Температура в зоне 2.4
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

