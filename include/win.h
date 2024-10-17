#pragma once

//Класс главного окна
#define szWindowClass0 "History Data Collection 0"
#define szWindowClass1 "History Data Collection 1"

extern HWND Global0;
extern HWND Global1;

extern HWND hwndCassette;
extern HWND hwndSheet;

extern HWND hWndDebug;

enum {
    SET_CALENDAR_DATA  = 100,
    SET_CALENDAR_DATA2  = 101,
};

namespace cas2{
    enum cas{
        NN = 0,
        DataTime = 1,
        Pos = 2,
        News,
        CassetteNo,
        SheetInCassette,
        Alloy,
        Thikness,
        Melt,
        PartNo,
        Pack,
        Sheet,

        Temper,
        Speed,

        Za_PT3,
        Za_TE3,

        LamPressTop,
        LamPressBot,
        PosClapanTop,
        PosClapanBot,
        Mask,

        Lam1PosClapanTop,
        Lam1PosClapanBot,
        Lam2PosClapanTop,
        Lam2PosClapanBot,

        Lam_TE1,

        Day,
        Month,
        Year,

        Top1,
        Top2,
        Top3,
        Top4,
        Top5,
        Top6,
        Top7,
        Top8,

        Bot1,
        Bot2,
        Bot3,
        Bot4,
        Bot5,
        Bot6,
        Bot7,
        Bot8,

    };
};

namespace evCassete
{
    enum EV{
        Nul = 0, //"Неизвестно" },
        Fill = 1, //"Набирается"},
        Wait = 2, //"Ожидает"},
        Rel = 3, //"Отпуск"},
        Error = 4, //"Авария"},
        End = 5, //"Конец"},
        History = 6, //"Конец"},
        Delete = 7, //"Удалена"},
    };
}

//Класс календаря
#define szMont "Montch"
//Класс групоывх окон
#define szTem1 "Temp1"
//Класс групоывх окон
#define szTem2 "Temp2"
//Класс закрытия групоывх окон
#define szETep "EndTemp"
//Класс окна Static
#define szStat "Static"
//Класс окна Edit
#define szEdit "Edit"
//Класс окна Button
#define szButt "Button"
//Класс окна SysListView32
#define szList "SysListView32"
//Класс окна ComboBox
#define szComb "ComboBox"

#define MYSS_LEFT   0x00010000L
#define MYSS_RIGHT  0x00020000L

#define Comb1Flag WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_HASSTRINGS | CBS_SIMPLE | CBS_AUTOHSCROLL
#define Comb2Flag WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_HASSTRINGS

//#define Stat1Flag WS_CHILD | WS_VISIBLE | DT_SINGLELINE | SS_CENTERIMAGE
//#define Stat2Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW
#define Stat03Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW | SS_CENTERIMAGE | DT_SINGLELINE
//#define Stat44Flag WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_SUNKEN | SS_EDITCONTROL
#define Stat04Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW | SS_EDITCONTROL
#define Stat05Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW | SS_EDITCONTROL | MYSS_LEFT
#define Stat06Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW | SS_EDITCONTROL | MYSS_RIGHT
#define Stat09Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW | SS_CENTER

#define Stat10Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW 
#define Stat11Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW | MYSS_LEFT
#define Stat12Flag WS_CHILD | WS_VISIBLE | WS_BORDER | SS_OWNERDRAW | MYSS_RIGHT

#define Edit1Flag WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY
#define Edit2Flag WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY | ES_CENTER
#define Edit3Flag WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY | ES_RIGHT


#define Butt1Flag WS_CHILD | WS_VISIBLE | WS_BORDER | BS_FLAT /*| BS_OWNERDRAW*/
#define Butt2Flag WS_CHILD | WS_VISIBLE | WS_BORDER /*| BS_OWNERDRAW*/
#define Butt3Flag WS_CHILD | WS_VISIBLE | BS_FLAT /*| BS_OWNERDRAW*/
#define Butt4Flag WS_CHILD | WS_VISIBLE /*| BS_OWNERDRAW*/

#define Butt5Flag WS_CHILD | WS_VISIBLE | WS_BORDER | BS_FLAT | BS_ICON /*| BS_OWNERDRAW*/
#define Butt6Flag WS_CHILD | WS_VISIBLE | WS_BORDER | BS_ICON /*| BS_OWNERDRAW*/ 
#define Butt7Flag WS_CHILD | WS_VISIBLE | BS_FLAT | BS_ICON /*| BS_OWNERDRAW*/
#define Butt8Flag WS_CHILD | WS_VISIBLE | BS_ICON /*| BS_OWNERDRAW*/



#define Temp1Flag WS_CHILD | WS_VISIBLE | WS_GROUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
#define Temp2Flag WS_CHILD | WS_VISIBLE | WS_GROUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
#define Temp3Flag WS_CHILD | WS_VISIBLE | WS_GROUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
#define Temp4Flag WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS

#define List1Flag WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_NOSORTHEADER 
#define List2Flag WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDATA

//Структура заголовков колонок окон ListBox
typedef struct {
    std::string title;
    ULONG cx;
}ListTitle;

//Структура списока элементов окон
typedef struct _structWindow{
    const char* Class = "";
    UINT Flag = 0;
    RECT rc ={0 , 0, 0, 0};
    const char* Title = "";
    HWND hParent;
    HWND hWnd = NULL;
}structWindow;

//Список ID элементов окон. Важен порядок.
enum HWNDCLIENT{
    hNull  = -1,
    hEditszButt = 999,
    hEditDiagnose  = 1000,
    hEditTimes = 1001,
    hEdit_Sheet = 1002,
    hEdit_Cassette  = 1003,

    hID1002 = 1003,
    hID1003 = 1004,
    hID1004 = 1005,
    hID1005 = 1006,
    hID1006 = 1007,
    hID1007 = 1008,
    hID1008 = 1009,


    hGroup346,
#if 346
    hEditAlloy0,
    hEditAlloy1,
    hEditAlloy2,
    hEditAlloy3,
    hEditAlloy4,
    hEditAlloy5,
    hEditAlloy6,
    hEditAlloy7,
    hEditAlloy8,
    hEditAlloy9,

    hEditThick0,
    hEditThick1,
    hEditThick2,
    hEditThick3,
    hEditThick4,
    hEditThick5,
    hEditThick6,
    hEditThick7,
    hEditThick8,
    hEditThick9,
#endif //Конец группы 346
    hEndGroup346,


    #if 365
    hGroup365,

        hStatTimeForPlateHeat,
        hStatPresToStartComp,

        hTimeForPlateHeat,
        hPresToStartComp,
    hEndGroup365,
    #endif


    #if 11
    hGroup11,
    hEditState_selected_cassetN,
    hEditState_selected_casset1,
    hEditState_selected_casset2,
    hEditState_selected_casset3,
    hEditState_selected_casset4,
    hEditState_selected_casset5,
    hEditState_selected_casset6,
    hEditState_selected_casset7,

    hStatState_Year,
    hStatState_Month,
    hStatState_Day,
    hStatState_Hour,
    hStatState_Cassette,

    hStatState_NN,
    hStatState_N1,
    hStatState_N2,
    hStatState_N3,
    hStatState_N4,
    hStatState_N5,
    hStatState_N6,
    hStatState_N7,

    hEditState1_Year,
    hEditState1_Month,
    hEditState1_Day,
    hEditState1_Hour,
    hEditState1_Cassette,

    hEditState2_Year,
    hEditState2_Month,
    hEditState2_Day,
    hEditState2_Hour,
    hEditState2_Cassette,

    hEditState3_Year,
    hEditState3_Month,
    hEditState3_Day,
    hEditState3_Hour,
    hEditState3_Cassette,

    hEditState4_Year,
    hEditState4_Month,
    hEditState4_Day,
    hEditState4_Hour,
    hEditState4_Cassette,

    hEditState5_Year,
    hEditState5_Month,
    hEditState5_Day,
    hEditState5_Hour,
    hEditState5_Cassette,

    hEditState6_Year,
    hEditState6_Month,
    hEditState6_Day,
    hEditState6_Hour,
    hEditState6_Cassette,

    hEditState7_Year,
    hEditState7_Month,
    hEditState7_Day,
    hEditState7_Hour,
    hEditState7_Cassette,


    hEndGroup11,
    #endif


    #if 6
    hGroup06,
    hEditState_Event,
    hEditState_DataTime,
    hEditState_Year,
    hEditState_Month,
    hEditState_Day,
    hEditState_Cassette,
    hEditState_Sheets,
    hEditState_Close,
    hEditState_Rel,
    hEditState_Error,
    hEditState_End,
    hEditState_Count,
    hEndGroup06,
    #endif

    #if 10
    hGroup01,
    hEditWDG,
    hEditState_11,
    hEditState_21,
    hEditState_31,
    hEditState_12,
    hEditState_22,
    hEditState_32,
    hEditState_WDG,
    hEditDiagnose5,

    hStatMode,
    hEditMode1,
    hEditMode2,

    hStatDiagnose6,
    hEditDiagnose6,

    hStatDiagnose7,
    hEditDiagnose7,

    hEditDiagnose8,
    hEditDiagnose9,

    hEditTime_1,
    hEditTime_2,
    hEditTime_3,
    hEditTime_4,

    hEndGroup01,
    #endif
    #if 11
    hGroup011,
    hStatTimeServer,
    hEditTimeServer,
    hEndGroup011,
    #endif

    #if 12
    hGroup012,
    hStatUnloadSpeed,
    hEditUnloadSpeed,
    hEndGroup012,
    #endif

    #if 20
    hGroup02,
    hEditTempSet,
    hEndGroup02,
    #endif

    #if 300
    hGroup03,
    #if 301
    hGroup0301,
    #if 30
    hGroup030,
    hEditPlate_DataZ0_Time,
    hEditPlate_DataZ1_Time,
    hEditPlate_DataZ2_Time,
    hEditPlate_DataZ3_Time,
    hEditPlate_DataZ4_Time,
    hEditPlate_DataZ5_Time,
    hEndGroup030,
    #endif

    #if 31
    hGroup031,
    hStatPlate_DataZ0,
    hStatPlate_DataZ1,
    hStatPlate_DataZ2,
    hStatPlate_DataZ3,
    hStatPlate_DataZ4,
    hStatPlate_DataZ5,
    hEndGroup031,
    #endif

    #if 32
    hGroup032,
    hStatPlate_DataZ0_AlloyCode,
    hEditPlate_DataZ0_AlloyCode,
    hEditPlate_DataZ1_AlloyCode,
    hEditPlate_DataZ2_AlloyCode,
    hEditPlate_DataZ3_AlloyCode,
    hEditPlate_DataZ4_AlloyCode,
    hEditPlate_DataZ5_AlloyCode,
    hEndGroup032,
    #endif

    #if 33
    hGroup033,
    hStatPlate_DataZ0_Thikness,
    hEditPlate_DataZ0_Thikness,
    hEditPlate_DataZ1_Thikness,
    hEditPlate_DataZ2_Thikness,
    hEditPlate_DataZ3_Thikness,
    hEditPlate_DataZ4_Thikness,
    hEditPlate_DataZ5_Thikness,
    hEndGroup033,
    #endif

    #if 34
    hGroup034,
    hStatPlate_DataZ0_Melt,
    hEditPlate_DataZ0_Melt,
    hEditPlate_DataZ1_Melt,
    hEditPlate_DataZ2_Melt,
    hEditPlate_DataZ3_Melt,
    hEditPlate_DataZ4_Melt,
    hEditPlate_DataZ5_Melt,
    hEndGroup034,
    #endif

    #if 35
    hGroup035,
    hStatPlate_DataZ0_PartNo,
    hEditPlate_DataZ0_PartNo,
    hEditPlate_DataZ1_PartNo,
    hEditPlate_DataZ2_PartNo,
    hEditPlate_DataZ3_PartNo,
    hEditPlate_DataZ4_PartNo,
    hEditPlate_DataZ5_PartNo,
    hEndGroup035,
    #endif

    #if 35
        hGroup039,
        hStatPlate_DataZ0_Slab,
        hEditPlate_DataZ0_Slab,
        hEditPlate_DataZ1_Slab,
        hEditPlate_DataZ2_Slab,
        hEditPlate_DataZ3_Slab,
        hEditPlate_DataZ4_Slab,
        hEditPlate_DataZ5_Slab,
        hEndGroup039,
    #endif

    #if 36
    hGroup036,
    hStatPlate_DataZ0_Pack,
    hEditPlate_DataZ0_Pack,
    hEditPlate_DataZ1_Pack,
    hEditPlate_DataZ2_Pack,
    hEditPlate_DataZ3_Pack,
    hEditPlate_DataZ4_Pack,
    hEditPlate_DataZ5_Pack,
    hEndGroup036,
    #endif

    #if 37
    hGroup037,
    //hStatPlate_DataZ0_Sheet,
    hEditPlate_DataZ0_Sheet,
    hEditPlate_DataZ1_Sheet,
    hEditPlate_DataZ2_Sheet,
    hEditPlate_DataZ3_Sheet,
    hEditPlate_DataZ4_Sheet,
    hEditPlate_DataZ5_Sheet,
    
    hEditPlate_DataZ0_SubSheet,
    hEditPlate_DataZ1_SubSheet,
    hEditPlate_DataZ2_SubSheet,
    hEditPlate_DataZ3_SubSheet,
    hEditPlate_DataZ4_SubSheet,
    hEditPlate_DataZ5_SubSheet,
    hStatPlate_DataZ0_Sheet,
    hStatPlate_DataZ1_Sheet,
    hStatPlate_DataZ2_Sheet,
    hStatPlate_DataZ3_Sheet,
    hStatPlate_DataZ4_Sheet,
    hStatPlate_DataZ5_Sheet,
    hEndGroup037,
    #endif

    #if 38
    hGroup038,
    hEditState_13,
    hEditState_23,
    hEditState_34,
    hEditState_33,
    hEndGroup038,
    #endif

    //#if 39
    //hGroup139,
    //hEditPlate_DataZ0_Slab,
    //hSEditPlate_DataZ1_Slab,
    //hEditPlate_DataZ2_Slab,
    //hEditPlate_DataZ3_Slab,
    //hEditPlate_DataZ4_Slab,
    //hEndGroup139,



    hEndGroup0301,
    #endif
    hEndGroup03,
    #endif

    #if 40
    hGroup04,
    hEdit_Plate_Sheet_Temper,
    hEndGroup04,
    #endif

    //Кантовка
    #if 50
    hGroup05,

        //Касета
    #if 51
    hGroup051,
    hButtSheet_CassetteNew,
    hEdit_Sheet_CassetteNew,
    hStatSheet_CassetteIsFull,
    //hStatSheet_Data,
    hEndGroup051,
    #endif

    //Лист
    #if 52
    hGroup052,
    hStatSheet_AlloyCode,
    hEdit_Sheet_AlloyCode,
    hStatSheet_Thikness,
    hEdit_Sheet_Thikness,
    hStatSheet_Melt,
    hEdit_Sheet_Melt,
    hStatSheet_Slab,
    hEdit_Sheet_Slab,
    hStatSheet_PartNo,
    hEdit_Sheet_PartNo,
    hStatSheet_Pack,
    hEdit_Sheet_Pack,
    hStatSheet_Sheet,
    hEdit_Sheet_Sheet,
    hStatSheet_SubSheet,
    hEdit_Sheet_SubSheet,
    hButtSheet_FindSheet,
    hEndGroup052,
    #endif

    //ID листа
    #if 53
    hGroup053,
    hStatSheet_DataTime,
    hEdit_Sheet_DataTime,

    hStatSheet_Cassette_Data,

    hStatSheet_Cassette_Day,
    hEdit_Sheet_Cassette_Day,

    hStatSheet_Cassette_Hour,
    hEdit_Sheet_Cassette_Hour,

    hStatSheet_Cassette_Month,
    hEdit_Sheet_Cassette_Month,

    hStatSheet_Cassette_Year,
    hEdit_Sheet_Cassette_Year,

    hStatSheet_CassetteNo,
    hEdit_Sheet_CassetteNo,

    hStatSheet_InCassette,
    hEdit_Sheet_InCassette,
    hEndGroup053,
    #endif

    //Отклонения от плоскостности листа до кантовки

    //#if 54
    //hGroup054,
    //    hStatSheetTop_Side,
    //    hStatSheetTop_Side_h1,
    //    hEdit_SheetTop_Side_h1,
    //    hStatSheetTop_Side_h2,
    //    hEdit_SheetTop_Side_h2,
    //    hStatSheetTop_Side_h3,
    //    hEdit_SheetTop_Side_h3,
    //    hStatSheetTop_Side_h4,
    //    hEdit_SheetTop_Side_h4,
    //    hStatSheetTop_Side_h5,
    //    hEdit_SheetTop_Side_h5,
    //    hStatSheetTop_Side_h6,
    //    hEdit_SheetTop_Side_h6,
    //    hStatSheetTop_Side_h7,
    //    hEdit_SheetTop_Side_h7,
    //    hStatSheetTop_Side_h8,
    //    hEdit_SheetTop_Side_h8,
    //hEndGroup054,
    //#endif

    //Отклонения от плоскостности листа после кантовки

    //#if 55
    //hGroup055,
    //    hStatSheetBottom_Side,
    //    hStatSheetBottom_Side_h1,
    //    hEdit_SheetBottom_Side_h1,
    //    hStatSheetBottom_Side_h2,
    //    hEdit_SheetBottom_Side_h2,
    //    hStatSheetBottom_Side_h3,
    //    hEdit_SheetBottom_Side_h3,
    //    hStatSheetBottom_Side_h4,
    //    hEdit_SheetBottom_Side_h4,
    //    hStatSheetBottom_Side_h5,
    //    hEdit_SheetBottom_Side_h5,
    //    hStatSheetBottom_Side_h6,
    //    hEdit_SheetBottom_Side_h6,
    //    hStatSheetBottom_Side_h7,
    //    hEdit_SheetBottom_Side_h7,
    //    hStatSheetBottom_Side_h8,
    //    hEdit_SheetBottom_Side_h8,
    //hEndGroup055,
    //#endif

    //#if 56
    //hGroup056,
    //    hStatSheet_DataNewSheet,
    //    //hStatSheet_Data,
    //hEndGroup056,
    //#endif
    hEndGroup05,
    #endif

    //#if 60
    //hGroup06,
    //hEndGroup06,
    //#endif

    //Закалка
    #if 70
    hGroup07,
    #if 71
    //hStatZakPressTopSet, 
    hGroup071,

    hStatZakPressTopSet,
    hEditZakPressTopSet,

    hStatZakPressBotSet,
    hEditZakPressBotSet,

    hStatSpeedTopSet,
    hEditSpeedTopSet,

    hStatSpeedBotSet,
    hEditSpeedBotSet,

    hEndGroup071,
    #endif

    #if 72
    hGroup072,
    hEditClapTop0,
    hEditClapTop1,
    hEditClapTop2,
    hEditClapTop3,
    hEditClapTop4,
    hEditClapTop5,
    hEditClapTop6,
    hEditClapTop7,
    hEditClapTop8,

    hEditClapBot0,
    hEditClapBot1,
    hEditClapBot2,
    hEditClapBot3,
    hEditClapBot4,
    hEditClapBot5,
    hEditClapBot6,
    hEditClapBot7,
    hEditClapBot8,
    hEndGroup072,
    #endif

    hEndGroup07,
    #endif

    #if 80
    hGroup08,
    #if 81
    hGroup081,
    hStatLAM_TE1Set,
    hEditLAM_TE1Set,
    hEndGroup081,
    #endif

    #if 82
    hGroup082,
    hStatLAM_TE3Set,
    hEditLAM_TE3Set,
    hEndGroup082,
    #endif

    #if 83
    hGroup083,
    hStatLAM_PT3Set,
    hEditLAM_PT3Set,
    hEndGroup083,
    #endif
    #if 84
    hGroup084,
    hStatLAM1_TopSet,
    hEditLAM1_TopSet,
    hStatLAM1_BotSet,
    hEditLAM1_BotSet,
    hEndGroup084,
    #endif

    #if 85
    hGroup085,
    hStatLAM2_TopSet,
    hEditLAM2_TopSet,
    hStatLAM2_BotSet,
    hEditLAM2_BotSet,
    hEndGroup085,
    #endif


    hEndGroup08,
    #endif


    //Печи отпуска
    #if 100
    hGroup100,
    #if 200
    hGroup200,
    hEditTimeServer_1,
    #if 201
    hGroup201,
    RelF1_Stat_Cassette_ID,
    RelF1_Stat_Cassette_Hour,
    RelF1_Edit_Cassette_Hour,
    RelF1_Stat_Cassette_Day,
    RelF1_Edit_Cassette_Day,
    RelF1_Stat_Cassette_Month,
    RelF1_Edit_Cassette_Month,
    RelF1_Stat_Cassette_Year,
    RelF1_Edit_Cassette_Year,
    RelF1_Stat_CassetteNo,
    RelF1_Edit_CassetteNo,
    RelF1_Stat_SheetInCassette,
    RelF1_Edit_SheetInCassette,
    hEndGroup201,

    hGroup20111,
    RelF1_Stat_TimeHeatAcc,
    RelF1_Stat_TimeHeatWait,
    RelF1_Stat_TimeTotal,
    RelF1_Edit_TimeHeatAcc,
    RelF1_Edit_TimeHeatWait,
    RelF1_Edit_TimeTotal,
    hEndGroup20111,
    #endif
    #if 202
    hGroup202,
        //RelF1_Stat_Param,
        //RelF1_Stat_P1, //Время начала
        //RelF1_Stat_PointTime_1,     //REAL Время разгона
        //RelF1_Stat_PointRef_1,      //REAL Уставка температуры
        //RelF1_Stat_PointDTime_2,    //REAL Время выдержки
        //RelF1_Stat_TimeProcSet,     //REAL Полное время процесса (уставка), мин
        //RelF1_Stat_ProcTimeMin,     //REAL Время процесса, час (0..XX)
        //RelF1_Stat_TimeToProcEnd,   //REAL Время до окончания процесса, мин
        //RefF1_Stat_TempRef,         //REAL Заданное значение температуры
        //RelF1_Stat_P7,
        //RelF1_Stat_P8,
        //
        //RelF1_Edit_P1,              //Время начала
        //RelF1_Edit_PointTime_1,     //REAL Время разгона
        //RelF1_Edit_PointRef_1,      //REAL Уставка температуры
        //RelF1_Edit_PointDTime_2,    //REAL Время выдержки
        //RelF1_Edit_TimeProcSet,     //REAL Полное время процесса (уставка), мин
        //RelF1_Edit_ProcTimeMin,     //REAL Время процесса, час (0..XX)
        //RelF1_Edit_TimeToProcEnd,   //REAL Время до окончания процесса, мин
        //RefF1_Edit_TempRef,         //REAL Заданное значение температуры
        //RelF1_Edit_P7,
        //RelF1_Edit_P8,
    hEndGroup202,
    #endif

    #if 204
    hGroup204,
    RelF1_Stat_PointTime_1,     //Время разгона
    RelF1_Stat_PointRef_1,      //Уставка температуры
    RelF1_Stat_PointDTime_2,    //Время выдержки

    RelF1_Edit_PointTime_1,     //Время разгона
    RelF1_Edit_PointRef_1,      //Уставка температуры
    RelF1_Edit_PointDTime_2,    //Время выдержки
    hEndGroup204,
    #endif

    #if 203 
    hGroup203,
    hGroup2031,
    RelF1_Stat_ProcRun,         //BOOL Работа
    RelF1_Stat_ProcEnd,         //BOOL Окончание процесса
    RelF1_Stat_ProcFault,       //BOOL Авария проц.
    RelF1_Stat_TimeProcSet,     //Полное время процесса (уставка), мин
    RelF1_Stat_ProcTimeMin,     //REAL Время процесса, час (0..XX)
    RelF1_Stat_TimeToProcEnd,   //REAL Время до окончания процесса, мин

    RelF1_Edit_ProcRun,         //BOOL Работа
    RelF1_Edit_ProcEnd,         //BOOL Окончание процесса
    RelF1_Edit_ProcFault,       //BOOL Авария проц.
    RelF1_Edit_TimeProcSet,     //Полное время процесса (уставка), мин
    RelF1_Edit_ProcTimeMin,     //REAL Время процесса, час (0..XX)
    RelF1_Edit_TimeToProcEnd,   //REAL Время до окончания процесса, мин

    hEndGroup2031,
    hGroup2032,
    RelF1_Stat_TempRef,         //Заданное значение температуры
    RelF1_Stat_TempAct,         //REAL Фактическое значение температуры
    RelF1_Stat_T1,              //REAL термопара 1
    RelF1_Stat_T2,              //REAL термопара 2
    RelF1_Stat_Proc1,           //Факт температуры за 5 минут до конца отпуска

    RelF1_Edit_TempRef,         //Заданное значение температуры
    RelF1_Edit_TempAct,         //REAL Фактическое значение температуры
    RelF1_Edit_T1,              //REAL термопара 1
    RelF1_Edit_T2,              //REAL термопара 2
    RelF1_Edit_Proc1,           //Факт температуры за 5 минут до конца отпуска
    hEndGroup2032,
    hEndGroup203,
    #endif
    hEndGroup200,
    #endif

    #if 300
    hGroup300,
    hEditTimeServer_2,
    #if 301
    hGroup301,
    RelF2_Stat_Cassette_ID,
    RelF2_Stat_Cassette_Hour,
    RelF2_Edit_Cassette_Hour,
    RelF2_Stat_Cassette_Day,
    RelF2_Edit_Cassette_Day,
    RelF2_Stat_Cassette_Month,
    RelF2_Edit_Cassette_Month,
    RelF2_Stat_Cassette_Year,
    RelF2_Edit_Cassette_Year,
    RelF2_Stat_CassetteNo,
    RelF2_Edit_CassetteNo,
    RelF2_Stat_SheetInCassette,
    RelF2_Edit_SheetInCassette,
    hEndGroup301,

    hGroup20311,
    RelF2_Stat_TimeHeatAcc,
    RelF2_Stat_TimeHeatWait,
    RelF2_Stat_TimeTotal,
    RelF2_Edit_TimeHeatAcc,
    RelF2_Edit_TimeHeatWait,
    RelF2_Edit_TimeTotal,
    hEndGroup20311,
    #endif

    #if 302
    hGroup302,
        //RelF1_Stat_Param,
            //RelF1_Stat_P1, //Время начала
            //RelF1_Stat_PointTime_1,     //REAL Время разгона
            //RelF1_Stat_PointRef_1,      //REAL Уставка температуры
            //RelF1_Stat_PointDTime_2,    //REAL Время выдержки
            //RelF1_Stat_TimeProcSet,     //REAL Полное время процесса (уставка), мин
            //RelF1_Stat_ProcTimeMin,     //REAL Время процесса, час (0..XX)
            //RelF1_Stat_TimeToProcEnd,   //REAL Время до окончания процесса, мин
            //RefF1_Stat_TempRef,         //REAL Заданное значение температуры
            //RelF1_Stat_P7,
            //RelF1_Stat_P8,
            //
            //RelF1_Edit_P1,              //Время начала
            //RelF1_Edit_PointTime_1,     //REAL Время разгона
            //RelF1_Edit_PointRef_1,      //REAL Уставка температуры
            //RelF1_Edit_PointDTime_2,    //REAL Время выдержки
            //RelF1_Edit_TimeProcSet,     //REAL Полное время процесса (уставка), мин
            //RelF1_Edit_ProcTimeMin,     //REAL Время процесса, час (0..XX)
            //RelF1_Edit_TimeToProcEnd,   //REAL Время до окончания процесса, мин
            //RefF1_Edit_TempRef,         //REAL Заданное значение температуры
            //RelF1_Edit_P7,
            //RelF1_Edit_P8,
    hEndGroup302,
    #endif

    #if 304
    hGroup304,
    RelF2_Stat_PointTime_1,     //Время разгона
    RelF2_Stat_PointRef_1,      //Уставка температуры
    RelF2_Stat_PointDTime_2,    //Время выдержки

    RelF2_Edit_PointTime_1,     //Время разгона
    RelF2_Edit_PointRef_1,      //Уставка температуры
    RelF2_Edit_PointDTime_2,    //Время выдержки
    hEndGroup304,
    #endif

    #if 303
    hGroup303,
    #if 3031 
    hGroup3031,
    RelF2_Stat_ProcRun,         //BOOL Работа
    RelF2_Stat_ProcEnd,         //BOOL Окончание процесса
    RelF2_Stat_ProcFault,       //BOOL Авария проц.
    RelF2_Stat_TimeProcSet,     //Полное время процесса (уставка), мин
    RelF2_Stat_ProcTimeMin,     //REAL Время процесса, час (0..XX)
    RelF2_Stat_TimeToProcEnd,   //REAL Время до окончания процесса, мин

    RelF2_Edit_ProcRun,         //BOOL Работа
    RelF2_Edit_ProcEnd,         //BOOL Окончание процесса
    RelF2_Edit_ProcFault,       //BOOL Авария проц.
    RelF2_Edit_TimeProcSet,     //Полное время процесса (уставка), мин
    RelF2_Edit_ProcTimeMin,     //REAL Время процесса, час (0..XX)
    RelF2_Edit_TimeToProcEnd,   //REAL Время до окончания процесса, мин
    hEndGroup3031,
    #endif
    #if 3032
    hGroup3032,
    RelF2_Stat_TempRef,         //Заданное значение температуры
    RelF2_Stat_TempAct,         //REAL Фактическое значение температуры
    RelF2_Stat_T1,              //REAL термопара 1
    RelF2_Stat_T2,              //REAL термопара 2
    RelF2_Stat_Proc1,           //Факт температуры за 5 минут до конца отпуска

    RelF2_Edit_TempRef,         //Заданное значение температуры
    RelF2_Edit_TempAct,         //REAL Фактическое значение температуры
    RelF2_Edit_T1,              //REAL термопара 1
    RelF2_Edit_T2,              //REAL термопара 2
    RelF2_Edit_Proc1,           //Факт температуры за 5 минут до конца отпуска
    hEndGroup3032,
    #endif
    hEndGroup303,
    #endif
    hEndGroup300,
    #endif

    hEndGroup100,
    #endif

    //Температуры печи
    hGroup345,
    #if 345
        hGroup3451,
            hStatTemp11TAct,
            hEditTemp11TAct,

            hStatTemp12TAct,
            hEditTemp12TAct,

            hStatTemp13TAct,
            hEditTemp13TAct,

            hStatTemp14TAct,
            hEditTemp14TAct,
        hEndGroup3451,

        hGroup3452,
            hStatTemp21TAct,
            hEditTemp21TAct,

            hStatTemp22TAct,
            hEditTemp22TAct,

            hStatTemp23TAct,
            hEditTemp23TAct,

            hStatTemp24TAct,
            hEditTemp24TAct,
        hEndGroup3452,
        hStatTempALLTAct,

        hGroup3453,
            hStatTempPerometr,
            hEditTempPerometr,
        hEndGroup3453,

    #endif //Конец группы 345
    hEndGroup345,
};

//Список элементов окон главного окна
extern std::map<HWNDCLIENT, structWindow>mapWindow;


namespace casSheet{
    enum cas{
        NN = 0,
        Cassette = 1,
        ID = 2,
        DataTime = 3,
        Start_at,
        SecondPos_at,
        DataTime_End,
        Delete_at,
        Correct,
        Pos,

        TimeForPlateHeat, //Время сигнализации окончания нагрева, мин
        DataTime_All,

        News,
        Year,
        Month,
        Day,
        Hour,
        CassetteNo,
        SheetInCassette,


        Alloy,
        Thikness,
        Melt,
        Slab,
        PartNo,
        Pack,
        Sheet,
        //SubSheet,

        Temper,
        Temperature,
        Speed,

        Za_PT3,
        Za_TE3,

        LamPressTop,
        LamPressBot,
        PosClapanTop,
        PosClapanBot,
        PresToStartComp,  //Уставка давления для запуска комперссора

        Mask,

        Lam1PosClapanTop,
        Lam1PosClapanBot,
        Lam2PosClapanTop,
        Lam2PosClapanBot,

        Lam_TE1,


        //Top1,
        //Top2,
        //Top3,
        //Top4,
        //Top5,
        //Top6,
        //Top7,
        //Top8,

        //Bot1,
        //Bot2,
        //Bot3,
        //Bot4,
        //Bot5,
        //Bot6,
        //Bot7,
        //Bot8,
    };
};

namespace cas{
    enum {
        DataTime = 0,
        Alloy = 1,
        Thikness = 2,
        Melt = 3,
        Slab,
        PartNo,
        Pack,
        Sheet,
        SubSheet,

        Temper,
        Speed,

        Za_PT3,
        Za_TE3,

        LamPressTop,
        LamPressBot,
        PosClapanTop,
        PosClapanBot,
        Mask,

        Lam1PosClapanTop,
        Lam1PosClapanBot,
        Lam2PosClapanTop,
        Lam2PosClapanBot,

        Lam_TE1,
        News,
        Top1,
        Top2,
        Top3,
        Top4,
        Top5,
        Top6,
        Top7,
        Top8,

        Bot1,
        Bot2,
        Bot3,
        Bot4,
        Bot5,
        Bot6,
        Bot7,
        Bot8,

        Day,
        Month,
        Year,
        CassetteNo,
        SheetInCassette,
        DataTime_End,
        DataTime_All,
        TimeForPlateHeat, //Время сигнализации окончания нагрева, мин
        PresToStartComp,  //Уставка давления для запуска комперссора
        Correct,
        Delete_at,
    };
};


typedef struct _TSheet{

    std::string id = "";
    std::string Cassette = "";
    std::string DataTime = "";
    std::string Start_at = "";
    std::string DataTime_End = "";
    std::string DataTime_All = "";
    std::string Alloy = "";
    std::string Thikness = "";
    std::string Melt = "";
    std::string Slab = "";
    std::string PartNo = "";
    std::string Pack = "";
    std::string Sheet = "";
    std::string SubSheet = "";
    std::string Temper = "";
    std::string Speed = "";

    std::string Za_PT3 = "";
    std::string Za_TE3 = "";

    std::string LaminPressTop = "";
    std::string LaminPressBot = "";
    std::string PosClapanTop = "";
    std::string PosClapanBot = "";
    std::string Mask = "";

    std::string Lam1PosClapanTop = "";
    std::string Lam1PosClapanBot = "";
    std::string Lam2PosClapanTop = "";
    std::string Lam2PosClapanBot = "";

    std::string LAM_TE1 = "";
    std::string News = "";
    std::string Top1 = "";
    std::string Top2 = "";
    std::string Top3 = "";
    std::string Top4 = "";
    std::string Top5 = "";
    std::string Top6 = "";
    std::string Top7 = "";
    std::string Top8 = "";

    std::string Bot1 = "";
    std::string Bot2 = "";
    std::string Bot3 = "";
    std::string Bot4 = "";
    std::string Bot5 = "";
    std::string Bot6 = "";
    std::string Bot7 = "";
    std::string Bot8 = "";

    std::string Year = "";
    std::string Month = "";
    std::string Day = "";
    std::string Hour = "";
    std::string CassetteNo = "";
    std::string SheetInCassette = "";
    std::string Pos = "";

    std::string TimeForPlateHeat = ""; //Время сигнализации окончания нагрева, мин
    std::string PresToStartComp = "";  //Уставка давления для запуска комперссора
    //std::string TempWatTankCool = "";  //Температура закалочной воды для вкл.охлаждения
    std::string Temperature = "";       //Средняя температура 2-части печи закалки
    std::string Correct = "";
    std::string SecondPos_at = "";      //Время прехода во вторую зону
    std::string Delete_at = "";      //Удален
    std::string Return_at = "";     //Вернули в список

    _TSheet()
    {
        Clear();
    };

    void Clear()
    {
        Cassette = "";
        id = "";
        DataTime = "";
        Start_at = "";
        DataTime_End = "";
        DataTime_All = "";
        Alloy = "";
        Thikness = "";
        Melt = "";
        Slab = "";
        PartNo = "";
        Pack = "";
        Sheet = "";
        SubSheet = "";
        Temper = "";
        Speed = "";

        Za_PT3 = "";
        Za_TE3 = "";

        LaminPressTop = "";
        LaminPressBot = "";
        PosClapanTop = "";
        PosClapanBot = "";
        Mask = "";

        Lam1PosClapanTop = "";
        Lam1PosClapanBot = "";
        Lam2PosClapanTop = "";
        Lam2PosClapanBot = "";

        LAM_TE1 = "";
        News = "";
        Top1 = "";
        Top2 = "";
        Top3 = "";
        Top4 = "";
        Top5 = "";
        Top6 = "";
        Top7 = "";
        Top8 = "";

        Bot1 = "";
        Bot2 = "";
        Bot3 = "";
        Bot4 = "";
        Bot5 = "";
        Bot6 = "";
        Bot7 = "";
        Bot8 = "";

        Year = "";
        Month = "";
        Day = "";
        Hour = "";
        CassetteNo = "";
        SheetInCassette = "";
        Pos = "";

        TimeForPlateHeat = ""; //Время сигнализации окончания нагрева, мин
        PresToStartComp = "";  //Уставка давления для запуска комперссора    }
        Temperature = "";
        Correct = "";
        SecondPos_at = "";
        Delete_at = "";
        Return_at = "";
    }

    //bool compare(TSheet& rhs)
    //{
    //    return (Melt == rhs.Melt &&
    //            PartNo == rhs.PartNo &&
    //            Pack == rhs.Pack &&
    //            Sheet == rhs.Sheet);
    //}
}TSheet;

namespace Cassete
{
    enum cas{
        NN,
        Id,
        Event,
        Create_at,
        Year,
        Month,
        Day,
        Hour,
        CassetteNo,
        SheetInCassette,
        Peth,
        Run_at,
        Finish_at,
        Correct,
        Pdf,
        Return_at,
        HeatAcc,           //Факт время нагрева
        HeatWait,          //Факт время выдержки
        Total,             //Факт общее время
    };
};

typedef struct Tcass{
    std::string Peth = "";
    std::string Run_at = "";
    std::string Hour = "";
    std::string Day = "";
    std::string Month = "";
    std::string Year = "";
    std::string CassetteNo = "";
    Tcass(int p)
    {
        Peth = std::to_string(p);
    }
}Tcass;

typedef struct TCassette{
    std::string Create_at = "";
    std::string Id = "";
    std::string Event = "";
    std::string Hour = "";
    std::string Day = "";
    std::string Month = "";
    std::string Year = "";
    std::string CassetteNo = "";
    std::string SheetInCassette = "";
    std::string Close_at = "";
    std::string Peth = "";
    std::string Run_at = "";
    std::string Error_at = "";
    std::string End_at = "";
    std::string Delete_at = "";
    std::string TempRef = "";           //Заданное значение температуры
    std::string PointRef_1 = "";        //Уставка температуры
    std::string PointTime_1 = "";       //Время разгона
    std::string PointTime_2 = "";      //Время выдержки
    std::string TimeProcSet = "";       //Полное время процесса (уставка), мин
    std::string facttemper = "";          //Факт температуры за 5 минут до конца отпуска
    std::string Finish_at = "";         //Завершение процесса + 15 минут
    std::string HeatAcc = "";           //Факт время нагрева
    std::string HeatWait = "";          //Факт время выдержки
    std::string Total = "";             //Факт общее время
    std::string Correct = "";           //
    std::string Pdf = "";               //
    std::string Return_at = "";     //Вернули в список

    bool compare(const TCassette& rhs)
    {
        return
            CassetteNo == rhs.CassetteNo
            && Hour == rhs.Hour
            && Day == rhs.Day
            && Month == rhs.Month
            && Year == rhs.Year;

    };

    TCassette& operator = (Tcass& cass)
    {
        Run_at = cass.Run_at;
        Peth = cass.Peth;
        Hour = cass.Hour;
        Day = cass.Day;
        Month = cass.Month;
        Year = cass.Year;
        CassetteNo = cass.CassetteNo;
        return *this;
    }
}TCassette;


//Создание главного окна
bool InitWindow(HINSTANCE hInst);

void AddHistoriSheet(bool begin);
void AddHistoriCassette(bool begin);
