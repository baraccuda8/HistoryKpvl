#include "pch.h"
#include "main.h"
#include "win.h"
#include "Graff.h"

#include "file.h"
#include "Subscript.h"
#include "ClCodeSys.h"
#include "ValueTag.h"
#include "Pdf.h"

//#include "Graff.h"
//#include "ClCodeSys.h"

void InitGrafWindow(HWND hWnd);
void Open_GRAFF_FURN(TCassette& p);

HWND Global0 = NULL;
HWND Global1 = NULL;

HWND hWndTitl = NULL;
HWND hWndExit = NULL;
HWND hWndMaxi = NULL;
HWND hWndMini = NULL;
HWND hWndTest1 = NULL;
HWND hWndTest2 = NULL;

HWND hwndCassette = NULL;
HWND hwndSheet = NULL;

HWND hHeaderCassette = NULL;
HWND hHeaderSheet = NULL;

extern HWND hWndDebug;

extern std::deque<TSheet>AllSheet;
extern std::deque<TCassette> AllCassette;


#define DefMinTrackSizeX   1920
#define DefMinTrackSizeY   1080
#define DefCenterX DefMinTrackSizeX / 2

int MaxTrackSizeX   = DefMinTrackSizeX;
int MaxTrackSizeY   = DefMinTrackSizeY;

int borderX = 8 + 8;
int borderY = 8 + 31;
int CenterX = MaxTrackSizeX / 2 - borderX;

bool FULLSCREEN = false;
int Monitor = 1;

//#define SIZEX 32
#define SIZEX 0
#define SIZEY 20

#define PlacementName0 "GlobalServer0.dat"

#define LL0 130
#define LL1 100
#define LL2 90
#define L0 50
#define L1 60
#define L2 70

#define  XL_CX1 80
#define  XL_CX2 LL0
#define  XL_CX3 L0
#define  XL_CX4 L0
#define  XL_CX5 L0
#define  XL_CX6 L0
#define  XL_CX7 L1
#define  XL_CX8 LL0
#define  XL_CX9 LL0
#define  XL_CX10 LL0
#define  XL_CX11 LL0
#define  XL_CX12 30

#define  XL_X1 0
#define  XL_X2 (XL_CX1 + XL_X1)
#define  XL_X3 (XL_CX2 + XL_X2)
#define  XL_X4 (XL_CX3 + XL_X3)
#define  XL_X5 (XL_CX4 + XL_X4)
#define  XL_X6 (XL_CX5 + XL_X5)
#define  XL_X7 (XL_CX6 + XL_X6)
#define  XL_X8 (XL_CX7 + XL_X7)
#define  XL_X9 (XL_CX8 + XL_X8)
#define  XL_X10 (XL_CX9 + XL_X9)
#define  XL_X11 (XL_CX10 + XL_X10)
#define  XL_X12 (XL_CX11 + XL_X11)
#define  XL_X13 (XL_CX12 + XL_X12)

//События листа
std::map<std::string, std::string> NamePos ={
    //{"0", "На входе" },
    //{"1", "1-я часть печи"},
    //{"2", "2-я часть печи"},
    //{"3", "Закалка"},
    //{"4", "Охлаждение"},
    //{"5", "Кантовка"},
    //{"6", "Кантовка"},
    //{"7", "В касете"},
    //
    //{"10", "Потерян На входе" },
    //{"11", "Потерян 1-я часть печи"},
    //{"12", "Потерян 2-я часть печи"},
    //{"13", "Потерян Закалке"},
    //{"14", "Потерян Охлаждении"},
    //{"15", "Потерян Кантовке"},
    //{"16", "Потерян Кантовке"},
    //{"17", "Потерян В касете"},

};

//Черная заливка
HBRUSH TitleBrush0 = CreateSolidBrush(RGB(0, 0, 0));

//Белая заливка
HBRUSH TitleBrush1 = CreateSolidBrush(RGB(255, 255, 255));

//синяя заливка
HBRUSH TitleBrush2 = CreateSolidBrush(RGB(192, 192, 255));

//светлосиняя заливка
HBRUSH TitleBrush3 = CreateSolidBrush(RGB(224, 224, 255));

//темносиняя заливка
HBRUSH TitleBrush4 = CreateSolidBrush(RGB(0, 99, 177));

//Заголовки колонок окна листов
std::map <casSheet::cas, ListTitle> Sheet_Collumn ={
    {casSheet::NN, { "№", 30 }},
    {casSheet::ID, { "ID", 50 }},
    {casSheet::DataTime, { "Дата время\nсоздания листа", LL0 }},
    {casSheet::Start_at, { "Дата время\nзагрузки в печь", LL0 }},
    {casSheet::SecondPos_at, { "Дата время\nво второй зоне", LL0 }},
    {casSheet::Correct, { "Коррекция", LL0 }},

    //{casSheet::Pos, { "Лист\nнайден в", 100 }},
    {casSheet::Pos, { "Текущая\nпозиция", 100 }},
    {casSheet::News, { "Кантовка", L2 }},

    {casSheet::DataTime_End, { "Дата время\nвыгрузки из печи", LL0 }},
    {casSheet::DataTime_All, { "Время закалки\nмин", L1 }},
    {casSheet::TimeForPlateHeat, { "Задание Время\nокончания нагрева", L2 }},

    {casSheet::Day, { "ID листа\nДень", L1 }},
    {casSheet::Month, { "ID листа\nМесяц", L1 }},
    {casSheet::Year, { "ID листа\nГод", L1 }},
    {casSheet::CassetteNo, { "ID листа\nКасета", L1 }},
    {casSheet::SheetInCassette, { "Id листа\nНомер", L1 }},

    {casSheet::PresToStartComp, { "Задание\nДавления воды", L2 }},

    {casSheet::Alloy, { "Марка стали", L1 }},
    {casSheet::Thikness, { "Толщина\nлиста\nмм", L2 }},
    {casSheet::Melt, { "Плавка", L1 }},
    {casSheet::PartNo, { "Партия", L0 }},
    //{casSheet::Slab, { "Сляб", L0 }},
    {casSheet::Pack, { "Пачка", L0 }},
    {casSheet::Sheet, { "Номер\nлиста", L0 }},
    //{casSheet::SubSheet, { "Номер\nдодлиста", L0 }},
    {casSheet::Temper, { "Заданная\nтемпература\nС°", LL2 }},
    {casSheet::Temperature, { "Факт\nтемпературы\nС°", LL2 }},
    {casSheet::Speed, { "Скорость\nвыдачи\nмм/с", 80 }},
    {casSheet::Za_PT3, { "Давление\nводы в баке.\nбар", LL2 }},
    {casSheet::Za_TE3, { "Температура\nводы в баке.\nС°", LL1 }},
    {casSheet::LamPressTop, { "Давление\nв верхнем\nколлекторе", LL2 }},
    {casSheet::LamPressBot, { "Давление\nв нижнем\nколлекторе", LL2 }},
    {casSheet::PosClapanTop, { "Скоростная\nклапан верх", LL2 }},
    {casSheet::PosClapanBot, { "Скоростная\nклапан низ", LL2 }},
    {casSheet::Mask, { "Маскирование", 140 }},
    {casSheet::Lam1PosClapanTop, { "Ламинарная 1\nКлапан верх", LL1 }},
    {casSheet::Lam1PosClapanBot, { "Ламинарная 1\nКлапан низ", LL1 }},
    {casSheet::Lam2PosClapanTop, { "Ламинарная 2\nКлапан верх", LL1 }},
    {casSheet::Lam2PosClapanBot, { "Ламинарная 2\nКлапан низ", LL1 }},

    {casSheet::Lam_TE1, { "Температура\nводы\nв поддоне. С°", LL1 }},

    //{casSheet::Top1, { "До\nкантовки\n1", L2 }},
    //{casSheet::Top2, { "До\nкантовки\n2", L2 }},
    //{casSheet::Top3, { "До\nкантовки\n3", L2 }},
    //{casSheet::Top4, { "До\nкантовки\n4", L2 }},
    //{casSheet::Top5, { "До\nкантовки\n5", L2 }},
    //{casSheet::Top6, { "До\nкантовки\n6", L2 }},
    //{casSheet::Top7, { "До\nкантовки\n7", L2 }},
    //{casSheet::Top8, { "До\nкантовки\n8", L2 }},

    //{casSheet::Bot1, { "После\nкантовки\n1", L2 }},
    //{casSheet::Bot2, { "После\nкантовки\n2", L2 }},
    //{casSheet::Bot3, { "После\nкантовки\n3", L2 }},
    //{casSheet::Bot4, { "После\nкантовки\n4", L2 }},
    //{casSheet::Bot5, { "После\nкантовки\n5", L2 }},
    //{casSheet::Bot6, { "После\nкантовки\n6", L2 }},
    //{casSheet::Bot7, { "После\nкантовки\n7", L2 }},
    //{casSheet::Bot8, { "После\nкантовки\n8", L2 }},
};

//Заголовки колонок окна касет
std::map<int, std::string> EventCassette ={
    //{evCassete::Nul,  "Неизвестно"},
    //{evCassete::Fill, "Набирается"},
    //{evCassete::Wait, "Ожидает"},
    //{evCassete::Rel, "Отпуск"},
    //{evCassete::Error, "Авария"},
    //{evCassete::End, "Конец"},
    //{evCassete::History, "Из базы"},
};


std::vector <ListTitle> Cassette_Collumn ={
    { "№", 30 },
    {"ID", 50 },
    { "Событие", XL_CX1 },
    { "Новая кассета", XL_CX2 },
    { "Год", XL_CX3 },
    { "Месяц", XL_CX4 },
    { "День", XL_CX5 },
    { "Касета", XL_CX6 },
    { "Листов", XL_CX6 },
    //{ "Кассета закрыта", XL_CX8 },
    { "Печь",  XL_CX6},
    { "Начало оптуска", XL_CX8 },
    { "Конец оптуска", XL_CX8 },
    { "Конец процесса",  XL_CX8},
    { "Ошибка оптуска", XL_CX8 },

    { "Факт время нагрева", XL_CX4 },
    { "Факт время выдержки", XL_CX4 },
    { "Факт общее время", XL_CX4 },

};

//std::map<HWNDCLIENT, structWindow>mapWindow;

std::map<int, const char*> NamePos2 = {
    {0, "На входе в печь"},
    {1, "1-я часть печи"},
    {2, "2-я часть печи"},
    {3, "Закалка"},
    {4, "Охлаждение"},
    {5, "Выдача"},
    {6, "Кантовка"}, 
};

//Диагностика
#define XXX1 1280
#define XXC1 MaxTrackSizeX - XXX1 - 2

//Список элементов окон главного окна
std::map<HWNDCLIENT, structWindow>mapWindow = {


{hEditDiagnose, {szStat,   Stat03Flag, {0, 0, 0, 21}, "Информацмя"}},
{hEditszButt,   {szButt,   Butt5Flag, {0, 0, 21, 21}, "Информацмя"}},

#pragma region Задание: Время закалки, Давление воды
    {hGroup365, {szTem1,   Temp1Flag, {1650, 170, 215, 66}, "Задание"}},
    {hStatTimeForPlateHeat,    {szStat, Stat03Flag, { 5, 23,  129, 19}, "Время закалки"}},
    {hStatPresToStartComp,     {szStat, Stat03Flag, { 5, 43,  129, 19}, "Давление воды"}},

    {hTimeForPlateHeat,{szStat,   Stat04Flag, {135, 23, 69, 19}, "1"}},
    {hPresToStartComp, {szStat,   Stat04Flag, {135, 43, 69, 19}, "2"}},
    {hEndGroup365, {}},
#pragma endregion

#pragma region История кантовки листа
    //{hGroup06, {szTem1, Temp4Flag, {0, 530, XL_X13 + 0, 20}, ""}},
    //{hEditState_Event,      {szStat,   Stat04Flag, {XL_X1 + 0, 0,  XL_CX1 + 1, 19}, "1"}},
    //{hEditState_DataTime,   {szStat,   Stat04Flag, {XL_X2 + 2, 0,  XL_CX2 - 1, 19}, "2"} },
    //{hEditState_Year,       {szStat,   Stat04Flag, {XL_X3 + 2, 0,  XL_CX3 - 1, 19}, "3"}},
    //{hEditState_Month,      {szStat,   Stat04Flag, {XL_X4 + 2, 0,  XL_CX4 - 1, 19}, "4"}},
    //{hEditState_Day,        {szStat,   Stat04Flag, {XL_X5 + 2, 0,  XL_CX5 - 1, 19}, "5"}},
    //{hEditState_Cassette,   {szStat,   Stat04Flag, {XL_X6 + 2, 0,  XL_CX6 - 1, 19}, "6"}},
    //{hEditState_Sheets,     {szStat,   Stat04Flag, {XL_X7 + 2, 0,  XL_CX7 - 1, 19}, "7"}},
    //{hEditState_Close,      {szStat,   Stat04Flag, {XL_X8 + 2, 0,  XL_CX8 - 1, 19}, "8"}},
    //{hEditState_Rel,        {szStat,   Stat04Flag, {XL_X9 + 2, 0,  XL_CX9 - 1, 19}, "9"}},
    //{hEditState_Error,      {szStat,   Stat04Flag, {XL_X10 + 2, 0, XL_CX10 - 1, 19}, "10"}},
    //{hEditState_End,        {szStat,   Stat04Flag, {XL_X11 + 2, 0, XL_CX11 - 1, 19}, "11"}},
    //{hEditState_Count,      {szStat,   Stat04Flag, {XL_X12 + 2, 0, XL_CX12 - 2, 19}, "12"}},
    //{hEndGroup06, {}},
#pragma endregion

#pragma region Список касет и листов
    {hEdit_Cassette,{szList,   List1Flag,{0, 530, 1090, 160}, ""}},
    {hEdit_Sheet,  {szList,    List1Flag, {0, 700, 750, 130}, ""}},
#pragma endregion

#pragma region Список кассет 7 штук, для печей отпуска
    {hGroup11,{szTem1, Temp1Flag,{1100, 530, 259 , 160}, ""}},
        {hEditState_selected_cassetN, {szStat, Stat10Flag, {0,   0, 19, 19}, ""}},
        {hEditState_selected_casset1, {szStat, Stat10Flag, {0,  20, 19, 19}, "*"}},
        {hEditState_selected_casset2, {szStat, Stat10Flag, {0,  40, 19, 19}, "*"}},
        {hEditState_selected_casset3, {szStat, Stat10Flag, {0,  60, 19, 19}, "*"}},
        {hEditState_selected_casset4, {szStat, Stat10Flag, {0,  80, 19, 19}, "*"}},
        {hEditState_selected_casset5, {szStat, Stat10Flag, {0, 100, 19, 19}, "*"}},
        {hEditState_selected_casset6, {szStat, Stat10Flag, {0, 120, 19, 19}, "*"}},
        {hEditState_selected_casset7, {szStat, Stat10Flag, {0, 140, 19, 19}, "*"}},

        {hStatState_Year,       {szStat, Stat10Flag, {55 * 0 + 40,   0, 54, 19}, "Год"}},
        {hStatState_Month,      {szStat, Stat10Flag, {55 * 1 + 40,   0, 54, 19}, "Месяц"}},
        {hStatState_Day,        {szStat, Stat10Flag, {55 * 2 + 40,   0, 54, 19}, "День"}},
        {hStatState_Cassette,   {szStat, Stat10Flag, {55 * 3 + 40,   0, 54, 19}, "Кассета"}},

        {hStatState_NN,         {szStat, Stat10Flag, {20,   0, 19, 19}, "№"}},
        {hStatState_N1,         {szStat, Stat10Flag, {20,  20, 19, 19}, "1"}},
        {hStatState_N2,         {szStat, Stat10Flag, {20,  40, 19, 19}, "2"}},
        {hStatState_N3,         {szStat, Stat10Flag, {20,  60, 19, 19}, "3"}},
        {hStatState_N4,         {szStat, Stat10Flag, {20,  80, 19, 19}, "4"}},
        {hStatState_N5,         {szStat, Stat10Flag, {20, 100, 19, 19}, "5"}},
        {hStatState_N6,         {szStat, Stat10Flag, {20, 120, 19, 19}, "6"}},
        {hStatState_N7,         {szStat, Stat10Flag, {20, 140, 19, 19}, "7"}},


        {hEditState1_Year,      {szStat, Stat04Flag, {55 * 0 + 40,  20, 54, 19}, ""}},
        {hEditState1_Month,     {szStat, Stat04Flag, {55 * 1 + 40,  20, 54, 19}, ""}},
        {hEditState1_Day,       {szStat, Stat04Flag, {55 * 2 + 40,  20, 54, 19}, ""}},
        {hEditState1_Cassette,  {szStat, Stat04Flag, {55 * 3 + 40,  20, 54, 19}, ""}},

        {hEditState2_Year,      {szStat, Stat04Flag, {55 * 0 + 40,  40, 54, 19}, ""}},
        {hEditState2_Month,     {szStat, Stat04Flag, {55 * 1 + 40,  40, 54, 19}, ""}},
        {hEditState2_Day,       {szStat, Stat04Flag, {55 * 2 + 40,  40, 54, 19}, ""}},
        {hEditState2_Cassette,  {szStat, Stat04Flag, {55 * 3 + 40,  40, 54, 19}, ""}},

        {hEditState3_Year,      {szStat, Stat04Flag, {55 * 0 + 40,  60, 54, 19}, ""}},
        {hEditState3_Month,     {szStat, Stat04Flag, {55 * 1 + 40,  60, 54, 19}, ""}},
        {hEditState3_Day,       {szStat, Stat04Flag, {55 * 2 + 40,  60, 54, 19}, ""}},
        {hEditState3_Cassette,  {szStat, Stat04Flag, {55 * 3 + 40,  60, 54, 19}, ""}},


        {hEditState4_Year,      {szStat, Stat04Flag, {55 * 0 + 40,  80, 54, 19}, ""}},
        {hEditState4_Month,     {szStat, Stat04Flag, {55 * 1 + 40,  80, 54, 19}, ""}},
        {hEditState4_Day,       {szStat, Stat04Flag, {55 * 2 + 40,  80, 54, 19}, ""}},
        {hEditState4_Cassette,  {szStat, Stat04Flag, {55 * 3 + 40,  80, 54, 19}, ""}},

        {hEditState5_Year,      {szStat, Stat04Flag, {55 * 0 + 40, 100, 54, 19}, ""}},
        {hEditState5_Month,     {szStat, Stat04Flag, {55 * 1 + 40, 100, 54, 19}, ""}},
        {hEditState5_Day,       {szStat, Stat04Flag, {55 * 2 + 40, 100, 54, 19}, ""}},
        {hEditState5_Cassette,  {szStat, Stat04Flag, {55 * 3 + 40, 100, 54, 19}, ""}},

        {hEditState6_Year,      {szStat, Stat04Flag, {55 * 0 + 40, 120, 54, 19}, ""}},
        {hEditState6_Month,     {szStat, Stat04Flag, {55 * 1 + 40, 120, 54, 19}, ""}},
        {hEditState6_Day,       {szStat, Stat04Flag, {55 * 2 + 40, 120, 54, 19}, ""}},
        {hEditState6_Cassette,  {szStat, Stat04Flag, {55 * 3 + 40, 120, 54, 19}, ""}},

        {hEditState7_Year,      {szStat, Stat04Flag, {55 * 0 + 40, 140, 54, 19}, ""}},
        {hEditState7_Month,     {szStat, Stat04Flag, {55 * 1 + 40, 140, 54, 19}, ""}},
        {hEditState7_Day,       {szStat, Stat04Flag, {55 * 2 + 40, 140, 54, 19}, ""}},
        {hEditState7_Cassette,  {szStat, Stat04Flag, {55 * 3 + 40, 140, 54, 19}, ""}},

        {hEndGroup11, {}},
#pragma endregion

#pragma region Данные статистики
        {hGroup01,{szTem1,  Temp4Flag,{XXX1 + 70, 0, 540, 160}, ""}},
        {hEditState_11, {szStat,   Stat10Flag, {0,     0,  19, 19}, "1"}},
        {hEditState_21, {szStat,   Stat10Flag, {0,    20,  19, 19}, "2"}},
        {hEditState_31, {szStat,   Stat10Flag, {0,    40,  19, 19}, "3"}},

        {hEditState_12, {szStat, Stat05Flag, {20,    0, 295, 19}, "4"}},
        {hEditState_22, {szStat, Stat05Flag, {20,   20, 295, 19}, "5"}},
        {hEditState_32, {szStat, Stat05Flag, {20,   40, 295, 19}, "6"}},

        {hEditWDG,          {szStat, Stat05Flag, {320, 0, 139, 19}, "7"}},
        {hEditState_WDG,    {szStat, Stat05Flag, {320,20, 139, 19}, "8"}},
        {hEditDiagnose5,    {szStat, Stat05Flag, {320,40, 139, 19}, "9"}},

        {hStatMode,         {szStat, Stat11Flag, { 0, 70, 69, 19}, "Действие"}},
        {hStatDiagnose6,    {szStat, Stat11Flag, { 0, 90, 69, 19}, "Всего"}},
        {hStatDiagnose7,    {szStat, Stat11Flag, { 0, 110, 69, 19}, "Изменений"}},


        {hEditMode1,        {szStat, Stat04Flag, {70,  70, 230, 19}, "Mode 1"}},
        {hEditMode2,        {szStat, Stat04Flag, {300, 70, 230, 19}, "Mode 2"}},

        {hEditDiagnose6,    {szStat, Stat06Flag, {70, 90, 99, 19}, "D1"}},
        {hEditDiagnose7,    {szStat, Stat06Flag, {70, 110, 99, 19}, "D2"}},

        {hEditDiagnose9,    {szStat, Stat04Flag, {170, 90, 129, 19}, "D3"}},
        {hEditDiagnose8,    {szStat, Stat04Flag, {170, 110, 129, 19}, "D4"}},

        {hEditTime_1,       {szStat, Stat06Flag, {300, 90,  99, 19}, "1"}},
        {hEditTime_3,       {szStat, Stat06Flag, {300, 110, 100, 19}, "2"}},

        {hEditTime_4,       {szStat, Stat04Flag, {400, 90, 129, 19}, "3"}},
        {hEditTime_2,       {szStat, Stat04Flag, {400, 110, 129, 19}, "4"}},

        {hEndGroup01, {}},
#pragma endregion

#pragma region Текущее время, Скорость, Уст. Температуры
        #pragma region Текущее время
            {hGroup011, {szTem1,   Temp4Flag, {0, 0, 130, 40}, "Текущее время"}},
            {hEditTimeServer, {szStat, Stat04Flag, {0, 20, 130, 19}, "f1"}},
            {hEndGroup011, {}},
        #pragma endregion

        #pragma region Скорость
            {hGroup012, {szTem1,   Temp4Flag, {0, 45, 130, 40}, "Скорость"}},
            {hEditUnloadSpeed,  {szStat, Stat04Flag, {0, 20, 130, 19}, "f2"}},
            {hEndGroup012, {}},
        #pragma endregion

    #pragma region Уст. Температуры
        {hGroup02, {szTem1,   Temp4Flag, {0, 90, 130, 40}, "Уст. Температуры"}},
        {hEditTempSet,          {szStat, Stat04Flag,  {0,  20,  130, 19}, "f3"}},
        {hEndGroup02, {}},
    #pragma endregion
#pragma endregion

#pragma region Закалка

    {hGroup07, {szTem1,   Temp1Flag, {145, 0, 189, 150}, "Закалка"}},
    #pragma region Маска клапанов
        {hGroup072,             {szTem2,    Temp4Flag,  {5, 23, 179, 39}, ""}},
        {hEditClapTop0,          {szStat, Stat04Flag,  {  0,   0,  19, 19}, "1"}},
        {hEditClapTop1,          {szStat, Stat04Flag,  { 20,   0,  19, 19}, "2"}},
        {hEditClapTop2,          {szStat, Stat04Flag,  { 40,   0,  19, 19}, "3"}},
        {hEditClapTop3,          {szStat, Stat04Flag,  { 60,   0,  19, 19}, "4"}},
        {hEditClapTop4,          {szStat, Stat04Flag,  { 80,   0,  19, 19}, "5"}},
        {hEditClapTop5,          {szStat, Stat04Flag,  {100,   0,  19, 19}, "6"}},
        {hEditClapTop6,          {szStat, Stat04Flag,  {120,   0,  19, 19}, "7"}},
        {hEditClapTop7,          {szStat, Stat04Flag,  {140,   0,  19, 19}, "8"}},
        {hEditClapTop8,          {szStat, Stat04Flag,  {160,   0,  19, 19}, "9"}},

        {hEditClapBot0,          {szStat, Stat04Flag,  {  0,  20,  19, 19}, "1"}},
        {hEditClapBot1,          {szStat, Stat04Flag,  { 20,  20,  19, 19}, "2"}},
        {hEditClapBot2,          {szStat, Stat04Flag,  { 40,  20,  19, 19}, "3"}},
        {hEditClapBot3,          {szStat, Stat04Flag,  { 60,  20,  19, 19}, "4"}},
        {hEditClapBot4,          {szStat, Stat04Flag,  { 80,  20,  19, 19}, "5"}},
        {hEditClapBot5,          {szStat, Stat04Flag,  {100,  20,  19, 19}, "6"}},
        {hEditClapBot6,          {szStat, Stat04Flag,  {120,  20,  19, 19}, "7"}},
        {hEditClapBot7,          {szStat, Stat04Flag,  {140,  20,  19, 19}, "8"}},
        {hEditClapBot8,          {szStat, Stat04Flag,  {160,  20,  19, 19}, "9"}},
        {hEndGroup072, {}},
    #pragma endregion

    #pragma region Давления
        {hGroup071,             {szTem1,    Temp4Flag,  {5, 66, 179, 79}, ""}},
        {hStatZakPressTopSet,       {szStat, Stat11Flag, {  0,  0, 99, 19}, " P воды верх:"}},
        {hEditZakPressTopSet,       {szStat, Stat04Flag,  {100,  0, 79, 19}, "P воды "}},
        {hStatZakPressBotSet,       {szStat, Stat11Flag, {  0, 20, 99, 19}, " P воды низ:"}},
        {hEditZakPressBotSet,       {szStat, Stat04Flag,  {100, 20, 79, 19}, "P воды"}},

        {hStatSpeedTopSet,          {szStat, Stat11Flag, {  0, 40, 99, 19}, " Клапан верх:"}},
        {hEditSpeedTopSet,          {szStat, Stat04Flag, {100, 40, 79, 19}, "Клапан "}},
        {hStatSpeedBotSet,          {szStat, Stat11Flag, {  0, 60, 99, 19}, " Клапан низ:"}},
        {hEditSpeedBotSet,          {szStat, Stat04Flag, {100, 60, 79, 19}, "Клапан "}},

        {hEndGroup071, {}},
    #pragma endregion

    {hEndGroup07, {}},
#pragma endregion

#pragma region Охлаждение
    {hGroup08, {szTem1,   Temp1Flag, {430, 0, 339, 150}, "Охлаждение"}},
    #pragma region Температура воды на входе охладителя
        {hGroup081,             {szTem1,    Temp4Flag,  {5, 20, 329, 20}, ""}},
        {hStatLAM_TE1Set,          {szStat, Stat11Flag,  {  0,  0, 249, 19}, " Температура воды на входе охладителя"}},
        {hEditLAM_TE1Set,          {szStat, Stat04Flag,  {250,  0,  79, 19}, "Т воды"}},
        {hEndGroup081, {}},
    #pragma endregion

    #pragma region Температура воды после охладителя
        {hGroup082,             {szTem1,    Temp4Flag,  {5, 40, 329, 20}, ""}},
        {hStatLAM_TE3Set,          {szStat, Stat11Flag,  {  0,  0, 249, 19}, " Температура воды после охладителя"}},
        {hEditLAM_TE3Set,          {szStat, Stat04Flag,  {250,  0,  79, 19}, "Т воды"}},
        {hEndGroup082, {}},
    #pragma endregion

    #pragma region Давление воды после охладителя
        {hGroup083,             {szTem1,    Temp4Flag,  {5, 60, 329, 20}, ""}},
        {hStatLAM_PT3Set,          {szStat, Stat11Flag,  {  0,  0, 249, 19}, " Давление воды после охладителя"}},
        {hEditLAM_PT3Set,          {szStat, Stat04Flag,  {250,  0,  79, 19}, "P воды"}},
        {hEndGroup083, {}},
    #pragma endregion

    #pragma region Ламинарная 1
        {hGroup084,             {szTem1,    Temp1Flag,  { 5, 85, 163, 62}, "Ламинарная 1"}},
        {hStatLAM1_TopSet,          {szStat, Stat11Flag, { 2,  20, 84, 19}, " Клапан верх"}},
        {hEditLAM1_TopSet,          {szStat, Stat04Flag,  {87,  20, 74, 19}, "Клапан"}},

        {hStatLAM1_BotSet,          {szStat, Stat11Flag, { 2,  40, 84, 19}, " Клапан низ"}},
        {hEditLAM1_BotSet,          {szStat, Stat04Flag,  {87,  40, 74, 19}, "Клапан"}},

        {hEndGroup084, {}},
    #pragma endregion

    #pragma region Ламинарная 2
        {hGroup085,             {szTem1,    Temp1Flag,  {170, 85, 163, 62}, "Ламинарная 2"}},
        {hStatLAM2_TopSet,          {szStat, Stat11Flag, { 2,  20, 84, 19}, " Клапан верх"}},
        {hEditLAM2_TopSet,          {szStat, Stat04Flag,  {87,  20, 74, 19}, "Клапан"}},

        {hStatLAM2_BotSet,          {szStat, Stat11Flag, { 2,  40, 84, 19}, " Клапан низ"}},
        {hEditLAM2_BotSet,          {szStat, Stat04Flag,  {87,  40, 74, 19}, "Клапан"}},

        {hEndGroup085, {}},
    #pragma endregion

    {hEndGroup08, {}},
#pragma endregion

#pragma region Прохождение листов

    {hGroup03, {szTem1,   Temp1Flag, {0, 170, 855, 145}, ""}},

    #pragma region Событие
        {hGroup031, {szTem1,   Temp1Flag, {2, 2, 109, 140}, "Событие"}},
        {hStatPlate_DataZ0,             {szStat, Stat05Flag, {0,  20, 109, 19}, NamePos2[0]}},
        {hStatPlate_DataZ1,             {szStat, Stat05Flag, {0,  40, 109, 19}, NamePos2[1]}},
        {hStatPlate_DataZ2,             {szStat, Stat05Flag, {0,  60, 109, 19}, NamePos2[2]}},
        {hStatPlate_DataZ3,             {szStat, Stat05Flag, {0,  80, 109, 19}, NamePos2[3]}},
        {hStatPlate_DataZ4,             {szStat, Stat05Flag, {0, 100, 109, 19}, NamePos2[4]}},
        {hStatPlate_DataZ5,             {szStat, Stat05Flag, {0, 120, 109, 19}, NamePos2[5]}},
        {hEndGroup031, {}},
    #pragma endregion

    #pragma region Время
        {hGroup030, {szTem1,   Temp1Flag, {112, 2, 129, 140}, "Время"}},
        {hEditPlate_DataZ0_Time,   {szStat, Stat05Flag, {0,  20,  129, 19}, ""}},
        {hEditPlate_DataZ1_Time,   {szStat, Stat05Flag, {0,  40,  129, 19}, ""}},
        {hEditPlate_DataZ2_Time,   {szStat, Stat05Flag, {0,  60,  129, 19}, ""}},
        {hEditPlate_DataZ3_Time,   {szStat, Stat05Flag, {0,  80,  129, 19}, ""}},
        {hEditPlate_DataZ4_Time,   {szStat, Stat05Flag, {0, 100,  129, 19}, ""}},
        {hEditPlate_DataZ5_Time,   {szStat, Stat05Flag, {0, 120,  129, 19}, ""}},

        {hEndGroup030, {}},
    #pragma endregion

    #pragma region Марка
        {hGroup032, {szTem1,   Temp1Flag, {242, 2, 69, 140}, "Марка"}},
        {hEditPlate_DataZ0_AlloyCode,   {szStat, Stat04Flag, {0,  20,  69, 19}, ""}},
        {hEditPlate_DataZ1_AlloyCode,   {szStat, Stat04Flag, {0,  40,  69, 19}, ""}},
        {hEditPlate_DataZ2_AlloyCode,   {szStat, Stat04Flag, {0,  60,  69, 19}, ""}},
        {hEditPlate_DataZ3_AlloyCode,   {szStat, Stat04Flag, {0,  80,  69, 19}, ""}},
        {hEditPlate_DataZ4_AlloyCode,   {szStat, Stat04Flag, {0, 100,  69, 19}, ""}},
        {hEditPlate_DataZ5_AlloyCode,   {szStat, Stat04Flag, {0, 120,  69, 19}, ""}},
        {hEndGroup032, {}},
    #pragma endregion

    #pragma region Толщина
        {hGroup033, {szTem1,   Temp1Flag, {312, 2, 69, 140}, "Толщина"}},
        {hEditPlate_DataZ0_Thikness,    {szStat, Stat04Flag, {0,  20,  69, 19}, ""}},
        {hEditPlate_DataZ1_Thikness,    {szStat, Stat04Flag, {0,  40,  69, 19}, ""}},
        {hEditPlate_DataZ2_Thikness,    {szStat, Stat04Flag, {0,  60,  69, 19}, ""}},
        {hEditPlate_DataZ3_Thikness,    {szStat, Stat04Flag, {0,  80,  69, 19}, ""}},
        {hEditPlate_DataZ4_Thikness,    {szStat, Stat04Flag, {0, 100,  69, 19}, ""}},
        {hEditPlate_DataZ5_Thikness,    {szStat, Stat04Flag, {0, 120,  69, 19}, ""}},
        {hEndGroup033, {}},
    #pragma endregion

    #pragma region Плавка
        {hGroup034, {szTem1,   Temp1Flag, {382, 2, 69, 140}, "Плавка"}},
        {hEditPlate_DataZ0_Melt,        {szStat, Stat04Flag, {0,  20,  69, 19}, ""}},
        {hEditPlate_DataZ1_Melt,        {szStat, Stat04Flag, {0,  40,  69, 19}, ""}},
        {hEditPlate_DataZ2_Melt,        {szStat, Stat04Flag, {0,  60,  69, 19}, ""}},
        {hEditPlate_DataZ3_Melt,        {szStat, Stat04Flag, {0,  80,  69, 19}, ""}},
        {hEditPlate_DataZ4_Melt,        {szStat, Stat04Flag, {0, 100,  69, 19}, ""}},
        {hEditPlate_DataZ5_Melt,        {szStat, Stat04Flag, {0, 120,  69, 19}, ""}},
        {hEndGroup034, {}},
    #pragma endregion
        
    #pragma region Сляб
        {hGroup039, {szTem1,   Temp1Flag, {452, 2, 69, 140}, "Сляб"}},
        {hEditPlate_DataZ0_Slab,      {szStat, Stat04Flag, {0,  20,  69, 19}, ""}},
        {hEditPlate_DataZ1_Slab,      {szStat, Stat04Flag, {0,  40,  69, 19}, ""}},
        {hEditPlate_DataZ2_Slab,      {szStat, Stat04Flag, {0,  60,  69, 19}, ""}},
        {hEditPlate_DataZ3_Slab,      {szStat, Stat04Flag, {0,  80,  69, 19}, ""}},
        {hEditPlate_DataZ4_Slab,      {szStat, Stat04Flag, {0, 100,  69, 19}, ""}},
        {hEditPlate_DataZ5_Slab,      {szStat, Stat04Flag, {0, 120,  69, 19}, ""}},
        {hEndGroup039, {}},
    #pragma endregion

    #pragma region Партия
        {hGroup035, {szTem1,   Temp1Flag, {522, 2, 69, 140}, "Партия"}},
        {hEditPlate_DataZ0_PartNo,      {szStat, Stat04Flag, {0,  20,  69, 19}, ""}},
        {hEditPlate_DataZ1_PartNo,      {szStat, Stat04Flag, {0,  40,  69, 19}, ""}},
        {hEditPlate_DataZ2_PartNo,      {szStat, Stat04Flag, {0,  60,  69, 19}, ""}},
        {hEditPlate_DataZ3_PartNo,      {szStat, Stat04Flag, {0,  80,  69, 19}, ""}},
        {hEditPlate_DataZ4_PartNo,      {szStat, Stat04Flag, {0, 100,  69, 19}, ""}},
        {hEditPlate_DataZ5_PartNo,      {szStat, Stat04Flag, {0, 120,  69, 19}, ""}},
        {hEndGroup035, {}},
    #pragma endregion

    #pragma region Пачка
        {hGroup036, {szTem1,   Temp1Flag, {592, 2, 69, 140}, "Пачка"}},
        {hEditPlate_DataZ0_Pack,        {szStat, Stat04Flag, {0,  20,  69, 19}, ""}},
        {hEditPlate_DataZ1_Pack,        {szStat, Stat04Flag, {0,  40,  69, 19}, ""}},
        {hEditPlate_DataZ2_Pack,        {szStat, Stat04Flag, {0,  60,  69, 19}, ""}},
        {hEditPlate_DataZ3_Pack,        {szStat, Stat04Flag, {0,  80,  69, 19}, ""}},
        {hEditPlate_DataZ4_Pack,        {szStat, Stat04Flag, {0, 100,  69, 19}, ""}},
        {hEditPlate_DataZ5_Pack,        {szStat, Stat04Flag, {0, 120,  69, 19}, ""}},
        {hEndGroup036, {}},
    #pragma endregion

    #pragma region Лист
        {hGroup037, {szTem1,    Temp1Flag, {662, 2, 139, 140}, "Лист"}},
        {hEditPlate_DataZ0_Sheet,       {szStat, Stat04Flag, {0,  20,  64, 19}, ""}},
        {hEditPlate_DataZ1_Sheet,       {szStat, Stat04Flag, {0,  40,  64, 19}, ""}},
        {hEditPlate_DataZ2_Sheet,       {szStat, Stat04Flag, {0,  60,  64, 19}, ""}},
        {hEditPlate_DataZ3_Sheet,       {szStat, Stat04Flag, {0,  80,  64, 19}, ""}},
        {hEditPlate_DataZ4_Sheet,       {szStat, Stat04Flag, {0, 100,  64, 19}, ""}},
        {hEditPlate_DataZ5_Sheet,       {szStat, Stat04Flag, {0, 120,  64, 19}, ""}},

        {hEditPlate_DataZ0_SubSheet,     {szStat, Stat04Flag, {75,  20,  64, 19}, ""}},
        {hEditPlate_DataZ1_SubSheet,     {szStat, Stat04Flag, {75,  40,  64, 19}, ""}},
        {hEditPlate_DataZ2_SubSheet,     {szStat, Stat04Flag, {75,  60,  64, 19}, ""}},
        {hEditPlate_DataZ3_SubSheet,     {szStat, Stat04Flag, {75,  80,  64, 19}, ""}},
        {hEditPlate_DataZ4_SubSheet,     {szStat, Stat04Flag, {75, 100,  64, 19}, ""}},
        {hEditPlate_DataZ5_SubSheet,     {szStat, Stat04Flag, {75, 120,  64, 19}, ""}},

        {hStatPlate_DataZ0_Sheet,     {szStat, Stat10Flag, {63,  20,  13, 19}, "/"}},
        {hStatPlate_DataZ1_Sheet,     {szStat, Stat10Flag, {63,  40,  13, 19}, "/"}},
        {hStatPlate_DataZ2_Sheet,     {szStat, Stat10Flag, {63,  60,  13, 19}, "/"}},
        {hStatPlate_DataZ3_Sheet,     {szStat, Stat10Flag, {63,  80,  13, 19}, "/"}},
        {hStatPlate_DataZ4_Sheet,     {szStat, Stat10Flag, {63, 100,  13, 19}, "/"}},
        {hStatPlate_DataZ5_Sheet,     {szStat, Stat10Flag, {63, 120,  13, 19}, "/"}},

        {hEndGroup037, {}},
    #pragma endregion

    #pragma region Таймеры
        {hGroup038, {szTem1,    Temp4Flag, {802, 2, 49, 120}, ""}},
        {hEditState_13, {szStat, Stat04Flag, {0,  40, 49, 19}, ""}},
        {hEditState_23, {szStat, Stat04Flag, {0,  60, 49, 19}, ""}},
        {hEditState_34, {szStat, Stat04Flag, {0,  80, 49, 19}, ""}},
        {hEditState_33, {szStat, Stat04Flag, {0, 100, 49, 19}, ""}},
        {hEndGroup038, {}},
    #pragma endregion

    {hEndGroup03, {}},
#pragma endregion

#pragma region Кантовка
    {hGroup05, {szTem1,   Temp1Flag, {785, 0, 559, 160}, "Кантовка"}},
    #pragma region События кассеты
        {hGroup051, {szTem1,   Temp4Flag, {5, 25, 369, 20}, ""}},
        {hStatSheet_CassetteIsFull,  {szStat, Stat10Flag, {  0, 0, 149, 19}, "Кассета набирается"}},
        {hEdit_Sheet_CassetteNew,     {szStat, Stat04Flag, {150, 0,  69, 19}, ""}},
        {hButtSheet_CassetteNew,     {szStat, Stat10Flag, {220, 0, 149, 19}, "Кассета"}},
        {hEndGroup051, {}},
    #pragma endregion

    #pragma region Лист с закалки
        {hGroup052, {szTem1,   Temp4Flag, {5, 50, 549, 40}, ""}},
        {hStatSheet_AlloyCode,      {szStat, Stat10Flag, {  0,  0, 69, 19}, "Марка"}},
        {hStatSheet_Thikness,       {szStat, Stat10Flag, { 70,  0, 69, 19}, "Толщина"}},
        {hStatSheet_Melt,           {szStat, Stat10Flag, {140,  0, 69, 19}, "Плавка"}},
        {hStatSheet_Slab,           {szStat, Stat10Flag, {210,  0, 69, 19}, "Сляб"}},
        {hStatSheet_PartNo,         {szStat, Stat10Flag, {280,  0, 69, 19}, "Партия"}},
        {hStatSheet_Pack,           {szStat, Stat10Flag, {350,  0, 69, 19}, "Пачка"}},
        {hStatSheet_Sheet,          {szStat, Stat10Flag, {420,  0, 129, 19}, "Лист"}},
        {hStatSheet_SubSheet,        {szStat, Stat10Flag, {478,  20, 13, 19}, "/"}},


        {hEdit_Sheet_AlloyCode,      {szStat, Stat04Flag, {  0, 20, 69, 19}, ""}},
        {hEdit_Sheet_Thikness,       {szStat, Stat04Flag, { 70, 20, 69, 19}, ""}},
        {hEdit_Sheet_Melt,           {szStat, Stat04Flag, {140, 20, 69, 19}, ""}},
        {hEdit_Sheet_Slab,           {szStat, Stat04Flag, {210, 20, 69, 19}, ""}},
        {hEdit_Sheet_PartNo,         {szStat, Stat04Flag, {280, 20, 69, 19}, ""}},
        {hEdit_Sheet_Pack,           {szStat, Stat04Flag, {350, 20, 69, 19}, ""}},
        {hEdit_Sheet_Sheet,          {szStat, Stat04Flag, {420, 20, 59, 19}, ""}},
        {hEdit_Sheet_SubSheet,        {szStat, Stat04Flag, {490, 20, 59, 19}, ""}},
        {hEndGroup052, {}},
    #pragma endregion

    #pragma region Лист в касету
        {hGroup053, {szTem1,   Temp4Flag, {5, 95, 479, 60}, ""}},
        {hStatSheet_DataTime,       {szStat, Stat10Flag, {   0,  0, 129, 39}, "Время загрузки\nв закалочную печь"}},
        {hStatSheet_Cassette_Data,  {szStat, Stat10Flag, { 130,  0, 349, 19}, "ID листа"}},
        {hStatSheet_Cassette_Year,  {szStat, Stat10Flag, { 130, 20,  69, 19}, "Год"}},
        {hStatSheet_Cassette_Month, {szStat, Stat10Flag, { 200, 20,  69, 19}, "Месяц"}},
        {hStatSheet_Cassette_Day,   {szStat, Stat10Flag, { 270, 20,  69, 19}, "День"}},
        {hStatSheet_CassetteNo,     {szStat, Stat10Flag, { 340, 20,  69, 19}, "Кассета"}},
        {hStatSheet_InCassette,     {szStat, Stat10Flag, { 410, 20,  69, 19}, "Лист"}},
        {hEdit_Sheet_DataTime,       {szStat, Stat05Flag, {   0, 40, 129, 19}, "2023-06-11 23:56:40"}},
        {hEdit_Sheet_Cassette_Year,  {szStat, Stat04Flag, { 130, 40,  69, 19}, ""}},
        {hEdit_Sheet_Cassette_Month, {szStat, Stat04Flag, { 200, 40,  69, 19}, ""}},
        {hEdit_Sheet_Cassette_Day,   {szStat, Stat04Flag, { 270, 40,  69, 19}, ""}},
        {hEdit_Sheet_CassetteNo,     {szStat, Stat04Flag, { 340, 40,  69, 19}, ""}},
        {hEdit_Sheet_InCassette,     {szStat, Stat04Flag, { 410, 40,  69, 19}, ""}},
        {hEndGroup053, {}},
    #pragma endregion
    {hEndGroup05, {}},
#pragma endregion

#pragma region Печи отпуска

        #define TTT2 203
        {hGroup100, {szTem1,   Temp4Flag, {0, 320, 1900, TTT2}, ""}}, // 8+8 + 10

            //#define MMM 2
            ////#define FFF1 CenterX

            //#define TTT2011 70
            //#define TTT2012 55
            //#define TTT201 85
            //#define TTT203 119
            //#define TTT204 (TTT203 - TTT201)

            //#define FFF4 TTT201 * 7 + TTT204 * 2 + TTT203 + MMM * 2
            //#define FFF5 TTT201 * 13 + MMM * 2
            //#define FFF6 TTT2012 * 5 + MMM * 2 

            //#define TTT1 FFF4 + 10
            //#define TTT3 TTT2011 * 6 + TTT201 * 4 + MMM * 2 - 1

    //Отпуск.Печь №1
    #pragma region Отпуск. Печь №1
        {hGroup200,{szTem1,   Temp1Flag,{0, 0, 859, 203}, "Отпуск. Печь №1"}},
        {hEditTimeServer_1, {szStat, Stat05Flag, {  5, 2, 129, 15}, "Время 1"}},
    #pragma region ID кассеты
        {hGroup201,{szTem1,   Temp1Flag,{5, 25, 278, 62}, "ID кассеты"}},
        {RelF1_Stat_Cassette_Year,  {szStat, Stat10Flag, {55 * 0 + 2, 20, 54, 19}, "Год"}},
        {RelF1_Stat_Cassette_Month, {szStat, Stat10Flag, {55 * 1 + 2, 20, 54, 19}, "Месяц"}},
        {RelF1_Stat_Cassette_Day,   {szStat, Stat10Flag, {55 * 2 + 2, 20, 54, 19}, "День"}},
        {RelF1_Stat_CassetteNo,     {szStat, Stat10Flag, {55 * 3 + 2, 20, 54, 19}, "Кассета"}},
        {RelF1_Stat_SheetInCassette,{szStat, Stat10Flag, {55 * 4 + 2, 20, 54, 19}, "Листов"}},

        {RelF1_Edit_Cassette_Year,  {szStat, Stat04Flag, {55 * 0 + 2, 40, 54, 19}, ""}},
        {RelF1_Edit_Cassette_Month, {szStat, Stat04Flag, {55 * 1 + 2, 40, 54, 19}, ""}},
        {RelF1_Edit_Cassette_Day,   {szStat, Stat04Flag, {55 * 2 + 2, 40, 54, 19}, ""}},
        {RelF1_Edit_CassetteNo,     {szStat, Stat04Flag, {55 * 3 + 2, 40, 54, 19}, ""}},
        {RelF1_Edit_SheetInCassette,{szStat, Stat04Flag, {55 * 4 + 2, 40, 54, 19}, ""}},
        {hEndGroup201, {}},
    #pragma endregion

    #pragma region Факт времени процеса
        {hGroup20111,{szTem1,   Temp1Flag,{5, 90, 209, 40}, ""}},
        {RelF1_Stat_TimeHeatAcc,  {szStat, Stat10Flag, {70 * 0 , 0, 69, 19}, "Нагрев"}},
        {RelF1_Stat_TimeHeatWait, {szStat, Stat10Flag, {70 * 1 , 0, 69, 19}, "Выдержка"}},
        {RelF1_Stat_TimeTotal,    {szStat, Stat10Flag, {70 * 2 , 0, 69, 19}, "Общее"}},
        {RelF1_Edit_TimeHeatAcc,  {szStat, Stat04Flag, {70 * 0 , 20, 69, 19}, ""}},
        {RelF1_Edit_TimeHeatWait, {szStat, Stat04Flag, {70 * 1 , 20, 69, 19}, ""}},
        {RelF1_Edit_TimeTotal,    {szStat, Stat04Flag, {70 * 2 , 20, 69, 19}, ""}},
        {hEndGroup20111, {}},
    #pragma endregion


    // Задание
    #pragma region Задание
    {hGroup204,{szTem1,   Temp1Flag,{289, 25, 70 * 3 + 3, 62}, "Задание"}},
        {RelF1_Stat_PointTime_1,    {szStat, Stat10Flag, {70 * 0 + 2, 20, 69, 19}, "Разгона"}},
        {RelF1_Stat_PointDTime_2,   {szStat, Stat10Flag, {70 * 1 + 2, 20, 69, 19}, "Выдержка"}},
        {RelF1_Stat_PointRef_1,     {szStat, Stat10Flag, {70 * 2 + 2, 20, 69, 19}, "Уставка T"}},

        {RelF1_Edit_PointTime_1,    {szStat, Stat04Flag, {70 * 0 + 2, 40, 69, 19}, ""}},       //Время разгона
        {RelF1_Edit_PointDTime_2,   {szStat, Stat04Flag, {70 * 1 + 2, 40, 69, 19}, ""}},       //Время выдержки
        {RelF1_Edit_PointRef_1,     {szStat, Stat04Flag, {70 * 2 + 2, 40, 69, 19}, ""}},       //Уставка температуры
        {hEndGroup204, {}},
    #pragma endregion

    // Текущие параметры
    #pragma region Текущие параметры
        {hGroup203,{szTem1,   Temp1Flag,{5, 135, 848, 62}, "Текущие параметры"}},
        #pragma region События
        {hGroup2031,{szTem1,   Temp4Flag,{2, 20, 419, 40}, ""}},
            {RelF1_Stat_ProcRun,        {szStat, Stat10Flag, {70 * 0, 0, 69, 19}, "Работа"}},  //BOOL Работа
            {RelF1_Stat_ProcEnd,        {szStat, Stat10Flag, {70 * 1, 0, 69, 19}, "Конец"}},   //BOOL Окончание процесса
            {RelF1_Stat_ProcFault,      {szStat, Stat10Flag, {70 * 2, 0, 69, 19}, "Авария"}},  //BOOL Авария проц.
            {RelF1_Stat_TimeProcSet,    {szStat, Stat10Flag, {70 * 3, 0, 69, 19}, "Всего"}},
            {RelF1_Stat_ProcTimeMin,    {szStat, Stat10Flag, {70 * 4, 0, 69, 19}, "Прошло"}},
            {RelF1_Stat_TimeToProcEnd,  {szStat, Stat10Flag, {70 * 5, 0, 69, 19}, "Осталось"}},

            {RelF1_Edit_ProcRun,        {szStat, Stat04Flag, {70 * 0, 20, 69, 19}, ""}},       //BOOL Работа
            {RelF1_Edit_ProcEnd,        {szStat, Stat04Flag, {70 * 1, 20, 69, 19}, ""}},       //BOOL Окончание процесса
            {RelF1_Edit_ProcFault,      {szStat, Stat04Flag, {70 * 2, 20, 69, 19}, ""}},       //BOOL Авария проц.
            {RelF1_Edit_TimeProcSet,    {szStat, Stat04Flag, {70 * 3, 20, 69, 19}, ""}},       //Полное время процесса (уставка), мин
            {RelF1_Edit_ProcTimeMin,    {szStat, Stat04Flag, {70 * 4, 20, 69, 19}, ""}},       //Время процесса, час (0..XX)
            {RelF1_Edit_TimeToProcEnd,  {szStat, Stat04Flag, {70 * 5, 20, 69, 19}, ""}},       //Время до окончания процесса, мин
            {hEndGroup2031, {}},
        #pragma endregion

        #pragma region Данные
        {hGroup2032,{szTem1,   Temp4Flag,{422, 20, 85 * 5 - 1, 40}, ""}},
            {RelF1_Stat_TempRef,        {szStat, Stat10Flag, {85 * 0, 0, 84, 19}, "Заданние"}},
            {RelF1_Stat_TempAct,        {szStat, Stat10Flag, {85 * 1, 0, 84, 19}, "В печи"}},
            {RelF1_Stat_T1,             {szStat, Stat10Flag, {85 * 2, 0, 84, 19}, "Термопара 1"}},
            {RelF1_Stat_T2,             {szStat, Stat10Flag, {85 * 3, 0, 84, 19}, "Термопара 2"}},
            {RelF1_Stat_Proc1,          {szStat, Stat10Flag, {85 * 4, 0, 84, 19}, "Факт Т °С"}},

            {RelF1_Edit_TempRef,        {szStat, Stat04Flag, {85 * 0, 20, 84, 19}, ""}},       //Заданное значение температуры
            {RelF1_Edit_TempAct,        {szStat, Stat04Flag, {85 * 1, 20, 84, 19}, ""}},       //Фактическое значение температуры
            {RelF1_Edit_T1,             {szStat, Stat04Flag, {85 * 2, 20, 84, 19}, ""}},       //термопара 1
            {RelF1_Edit_T2,             {szStat, Stat04Flag, {85 * 3, 20, 84, 19}, ""}},       //термопара 2
            {RelF1_Edit_Proc1,          {szStat, Stat04Flag, {85 * 4, 20, 84, 19}, "0"}},      //Температура за 5 минут до конца отпуска
            {hEndGroup2032, {}},
        #pragma endregion
        {hEndGroup203, {}},
    #pragma endregion

        {hEndGroup200, {}},
    #pragma endregion

    //Отпуск.Печь №2
    #pragma region Отпуск.Печь №2
            {hGroup300,         {szTem1,   Temp1Flag,{869, 0, 859, 203}, "Отпуск. Печь №2"}},
            {hEditTimeServer_2, {szStat, Stat05Flag, {  5, 2, 129, 15}, "Время 2"}},
        #pragma region ID кассеты
        {hGroup301,{szTem1,   Temp1Flag,{5, 25, 55 * 5 + 3, 62}, "ID кассеты"}},
            {RelF2_Stat_Cassette_Year,  {szStat, Stat10Flag, {55 * 0 + 2, 20, 54, 19}, "Год"}},
            {RelF2_Stat_Cassette_Month, {szStat, Stat10Flag, {55 * 1 + 2, 20, 54, 19}, "Месяц"}},
            {RelF2_Stat_Cassette_Day,   {szStat, Stat10Flag, {55 * 2 + 2, 20, 54, 19}, "День"}},
            {RelF2_Stat_CassetteNo,     {szStat, Stat10Flag, {55 * 3 + 2, 20, 54, 19}, "Кассета"}},
            {RelF2_Stat_SheetInCassette,{szStat, Stat10Flag, {55 * 4 + 2, 20, 54, 19}, "Листов"}},

            {RelF2_Edit_Cassette_Year,  {szStat, Stat04Flag, {55 * 0 + 2, 40, 54, 19}, ""}},
            {RelF2_Edit_Cassette_Month, {szStat, Stat04Flag, {55 * 1 + 2, 40, 54, 19}, ""}},
            {RelF2_Edit_Cassette_Day,   {szStat, Stat04Flag, {55 * 2 + 2, 40, 54, 19}, ""}},
            {RelF2_Edit_CassetteNo,     {szStat, Stat04Flag, {55 * 3 + 2, 40, 54, 19}, ""}},
            {RelF2_Edit_SheetInCassette,{szStat, Stat04Flag, {55 * 4 + 2, 40, 54, 19}, ""}},
            {hEndGroup301, {}},
        #pragma endregion

        #pragma region Факт времени процеса
            {hGroup20311,{szTem1,   Temp1Flag,{5, 90, 209, 40}, ""}},
            {RelF2_Stat_TimeHeatAcc,  {szStat, Stat10Flag, {70 * 0 , 0, 69, 19}, "Нагрев"}},
            {RelF2_Stat_TimeHeatWait, {szStat, Stat10Flag, {70 * 1 , 0, 69, 19}, "Выдержка"}},
            {RelF2_Stat_TimeTotal,    {szStat, Stat10Flag, {70 * 2 , 0, 69, 19}, "Общее"}},
            {RelF2_Edit_TimeHeatAcc,  {szStat, Stat04Flag, {70 * 0 , 20, 69, 19}, ""}},
            {RelF2_Edit_TimeHeatWait, {szStat, Stat04Flag, {70 * 1 , 20, 69, 19}, ""}},
            {RelF2_Edit_TimeTotal,    {szStat, Stat04Flag, {70 * 2 , 20, 69, 19}, ""}},
            {hEndGroup20311, {}},
        #pragma endregion

        //Задание
        #pragma region Задание
        {hGroup304,{szTem1,   Temp1Flag,{55 * 5 + 14, 25, 70 * 3 + 3, 62}, "Задание"}},
            {RelF2_Stat_PointTime_1,    {szStat, Stat10Flag, {70 * 0 + 2, 20, 69, 19}, "Разгона"}},
            {RelF2_Stat_PointDTime_2,   {szStat, Stat10Flag, {70 * 1 + 2, 20, 69, 19}, "Выдержка"}},
            {RelF2_Stat_PointRef_1,     {szStat, Stat10Flag, {70 * 2 + 2, 20, 69, 19}, "Уставка T"}},


            {RelF2_Edit_PointTime_1,    {szStat, Stat04Flag, {70 * 0 + 2, 40, 69, 19}, ""}},       //Время разгона
            {RelF2_Edit_PointDTime_2,   {szStat, Stat04Flag, {70 * 1 + 2, 40, 69, 19}, ""}},       //Время выдержки
            {RelF2_Edit_PointRef_1,     {szStat, Stat04Flag, {70 * 2 + 2, 40, 69, 19}, ""}},       //Уставка температуры
            {hEndGroup304, {}},
        #pragma endregion

        // текущие параметры
        #pragma region Текущие параметры
            {hGroup303,{szTem1,   Temp1Flag,{5, 135, 70 * 6 + 85 * 5 + 3, 62}, "Текущие параметры"}},

        #pragma region События
            {hGroup3031,{szTem1,   Temp4Flag,{2, 20, 70 * 6 - 1, 40}, ""}},
            {RelF2_Stat_ProcRun,        {szStat, Stat10Flag, {70 * 0, 0, 69, 19}, "Работа"}},  //BOOL Работа
            {RelF2_Stat_ProcEnd,        {szStat, Stat10Flag, {70 * 1, 0, 69, 19}, "Конец"}},   //BOOL Окончание процесса
            {RelF2_Stat_ProcFault,      {szStat, Stat10Flag, {70 * 2, 0, 69, 19}, "Авария"}},  //BOOL Авария проц.
            {RelF2_Stat_TimeProcSet,    {szStat, Stat10Flag, {70 * 3, 0, 69, 19}, "Всего"}},
            {RelF2_Stat_ProcTimeMin,    {szStat, Stat10Flag, {70 * 4, 0, 69, 19}, "Прошло"}},
            {RelF2_Stat_TimeToProcEnd,  {szStat, Stat10Flag, {70 * 5, 0, 69, 19}, "Осталось"}},

            {RelF2_Edit_ProcRun,        {szStat, Stat04Flag, {70 * 0, 20, 69, 19}, ""}},       //BOOL Работа
            {RelF2_Edit_ProcEnd,        {szStat, Stat04Flag, {70 * 1, 20, 69, 19}, ""}},       //BOOL Окончание процесса
            {RelF2_Edit_ProcFault,      {szStat, Stat04Flag, {70 * 2, 20, 69, 19}, ""}},       //BOOL Авария проц.
            {RelF2_Edit_TimeProcSet,    {szStat, Stat04Flag, {70 * 3, 20, 69, 19}, ""}},       //Полное время процесса (уставка), мин
            {RelF2_Edit_ProcTimeMin,    {szStat, Stat04Flag, {70 * 4, 20, 69, 19}, ""}},       //Время процесса, час (0..XX)
            {RelF2_Edit_TimeToProcEnd,  {szStat, Stat04Flag, {70 * 5, 20, 69, 19}, ""}},       //Время до окончания процесса, мин
            {hEndGroup3031, {}},
        #pragma endregion

        #pragma region Данные
        {hGroup3032,{szTem1,   Temp4Flag,{70 * 6 + 2, 20, 85 * 6 - 1, 40}, ""}},
            {RelF2_Stat_TempRef,        {szStat, Stat10Flag, {85 * 0, 0, 84, 19}, "Заданние"}},
            {RelF2_Stat_TempAct,        {szStat, Stat10Flag, {85 * 1, 0, 84, 19}, "В печи"}},
            {RelF2_Stat_T1,             {szStat, Stat10Flag, {85 * 2, 0, 84, 19}, "Термопара 1"}},
            {RelF2_Stat_T2,             {szStat, Stat10Flag, {85 * 3, 0, 84, 19}, "Термопара 2"}},
            {RelF2_Stat_Proc1,          {szStat, Stat10Flag, {85 * 4, 0, 84, 19}, "Факт Т °С"}},

            {RelF2_Edit_TempRef,        {szStat, Stat04Flag, {85 * 0, 20, 84, 19}, ""}},       //Заданное значение температуры
            {RelF2_Edit_TempAct,        {szStat, Stat04Flag, {85 * 1, 20, 84, 19}, ""}},       //Фактическое значение температуры
            {RelF2_Edit_T1,             {szStat, Stat04Flag, {85 * 2, 20, 84, 19}, ""}},       //термопара 1
            {RelF2_Edit_T2,             {szStat, Stat04Flag, {85 * 3, 20, 84, 19}, ""}},       //термопара 2
            {RelF2_Edit_Proc1,          {szStat, Stat04Flag, {85 * 4, 20, 84, 19}, "0"}},      //Температура за 5 минут до конца отпуска
            {hEndGroup3032, {}},
        #pragma endregion

            {hEndGroup303, {}},
        #pragma endregion

            {hEndGroup300, {}},
    #pragma endregion


        {hEndGroup100, {}},
#pragma endregion

#pragma region Температуры печи закалки
    {hGroup345, {szTem1,   Temp1Flag, {865, 170, 290, 145}, "Температуры печи закалки"}},

    #pragma region 1-я зона печи
        {hGroup3451, {szTem1,   Temp1Flag, {5, 27, 279, 40}, ""}},
        {hStatTemp11TAct,     {szStat, Stat03Flag, { 0, 0,  69, 19}, "1.1"}},
        {hEditTemp11TAct,     {szStat, Stat04Flag, { 0, 20,  69, 19}, ""}},

        {hStatTemp12TAct,     {szStat, Stat03Flag, { 70, 0,  69, 19}, "1.2"}},
        {hEditTemp12TAct,     {szStat, Stat04Flag, { 70, 20,  69, 19}, ""}},

        {hStatTemp13TAct,     {szStat, Stat03Flag, { 140, 0,  69, 19}, "1.3"}},
        {hEditTemp13TAct,     {szStat, Stat04Flag, { 140, 20,  69, 19}, ""}},

        {hStatTemp14TAct,     {szStat, Stat03Flag, { 210, 0,  69, 19}, "1.4"}},
        {hEditTemp14TAct,     {szStat, Stat04Flag, { 210, 20,  69, 19}, ""}},
        {hEndGroup3451, {}},
    #pragma endregion

    #pragma region 2-я зона печи
        {hGroup3452, {szTem1,   Temp1Flag, {5, 77, 279, 40}, ""}},
        {hStatTemp21TAct,     {szStat, Stat03Flag, { 0, 0,  69, 19}, "2.1"}},
        {hEditTemp21TAct,     {szStat, Stat04Flag, { 0, 20,  69, 19}, ""}},

        {hStatTemp22TAct,     {szStat, Stat03Flag, { 70, 0,  69, 19}, "2.2"}},
        {hEditTemp22TAct,     {szStat, Stat04Flag, { 70, 20,  69, 19}, ""}},

        {hStatTemp23TAct,     {szStat, Stat03Flag, { 140, 0,  69, 19}, "2.3"}},
        {hEditTemp23TAct,     {szStat, Stat04Flag, { 140, 20,  69, 19}, ""}},

        {hStatTemp24TAct,     {szStat, Stat03Flag, { 210, 0,  69, 19}, "2.4"}},
        {hEditTemp24TAct,     {szStat, Stat04Flag, { 210, 20,  69, 19}, ""}},
        {hEndGroup3452, {}},
    #pragma endregion
    {hStatTempALLTAct, {szStat, Stat04Flag, { 5, 120,  69, 19}, "all"}},

    {hEndGroup053, {}},
#pragma endregion

};

//Рисуем заголовок ListBox
LRESULT OnPaintHeadListView(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);
    char szBuff[256] = "";
    int count = (int)SendMessageA(hWnd, HDM_GETITEMCOUNT, 0, 0L);
    SelectObject(hDC, TitleBrush1);
    HFONT oldfont = (HFONT)SelectObject(hDC, Font[emFont::Font10]);
    SetTextColor(hDC, RGB(0, 0, 0));
    SetBkMode(hDC, TRANSPARENT);

    for(int i= 0; i < count; i++)
    {
        SIZE siz;
        HDITEM hi;
        RECT rcItem;
        hi.mask = HDI_TEXT;
        hi.pszText = szBuff;
        hi.cchTextMax = 255;
        Header_GetItem(hWnd, i, &hi);
        Header_GetItemRect(hWnd, i, &rcItem);

        FillRect(hDC, &rcItem, TitleBrush2);
        //rcItem.left +=1;
        rcItem.left+=1;
        FillRect(hDC, &rcItem, TitleBrush4);
        FrameRect(hDC, &rcItem, TitleBrush0);

        SetTextColor(hDC, RGB(255, 255, 255));
        SetBkMode(hDC, TRANSPARENT);

        GetTextExtentPoint32A(hDC, szBuff, (int)strlen(szBuff), &siz);
        char* s = szBuff;
        int coco = 1;
        while(s && *s) { if(*(s++) == '\n')coco++; }
        coco *= siz.cy / 2;
        int y = (rcItem.bottom - rcItem.top) / 2;
        rcItem.top = y - coco;
        rcItem.bottom = y + coco;
        DrawTextA(hDC, szBuff, -1, &rcItem, DT_CENTER | DT_WORDBREAK | DT_VCENTER);
    }
    SelectObject(hDC, oldfont);
    EndPaint(hWnd, &ps);
    return 1;
}

//Устанавливаем высоту заголовка ListBox
LRESULT OnHeader_Layout(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    HDLAYOUT* hl = (HDLAYOUT*)lParam;
    LRESULT  ret = DefSubclassProc(hWnd, message, wParam, lParam);

    //hl->pwpos->flags = 0x00000014; //SWP_NOZORDER | SWP_NOACTIVATE
    hl->pwpos->cy = (int)dwRefData;
    hl->prc->top = (int)dwRefData;

    return ret;
}

//Обработека сообщений заголовка ListBpx Рисуем заголовок
LRESULT CALLBACK WndProcHeadListView(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if(message == WM_ERASEBKGND)    return 0;
    if(message == WM_PAINT)         return OnPaintHeadListView(hWnd, message, wParam, lParam, uIdSubclass, dwRefData);
    if(message == HDM_LAYOUT)       return OnHeader_Layout(hWnd, message, wParam, lParam, uIdSubclass, dwRefData);

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

//Добавить строку в ListBox
LRESULT AddItem(HWND hwndSheet, bool begin = TRUE)
{
    LRESULT iItemServ = begin ? 0 : ListView_GetItemCount(hwndSheet);
    LV_ITEM lvi ={0};
    lvi.mask = LVIF_PARAM | LVIF_TEXT;
    lvi.pszText = LPSTR_TEXTCALLBACK;// (LPSTR)name;
    lvi.cchTextMax = 255;
    lvi.iItem = (int)iItemServ;
    lvi.lParam = NULL;// param;
    LRESULT ret = ListView_InsertItem(hwndSheet, &lvi);
    return ret;
}

//Добавить колонку в ListBox
LRESULT AddColumn(HWND hwndSheet, size_t i, ListTitle* l)
{
    LV_COLUMN lvc ={0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
    lvc.cx = l->cx;
    lvc.pszText = (LPSTR)l->title.c_str();
    lvc.cchTextMax = MAX_STRING;
    lvc.fmt = LVCFMT_CENTER;

    //ListView_Get
    bool b = ListView_InsertColumn(hwndSheet, i, &lvc);
    return LRESULT(0);
}

//Инициализация истории кантовки листа
void CreateHistoriSheet()
{
    hwndSheet = winmap(hEdit_Sheet);
    hHeaderSheet = ListView_GetHeader(hwndSheet);

    SetWindowSubclass(hHeaderSheet, WndProcHeadListView, 1, 48);

    size_t i = 0;
    for(auto& a : Sheet_Collumn)
        AddColumn(hwndSheet, i++, &a.second);

    ListView_SetExtendedListViewStyle(hwndSheet, LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_REGIONAL | LVS_EX_FULLROWSELECT);
}

//Инициализация истории касеты
void CreateHistoriCassette()
{
    hwndCassette = winmap(hEdit_Cassette);
    hHeaderCassette = ListView_GetHeader(hwndCassette);

    SetWindowSubclass(hHeaderCassette, WndProcHeadListView, 2, 20);

    for(size_t i = 0; i < Cassette_Collumn.size(); i++)
        AddColumn(hwndCassette, i, &Cassette_Collumn[i]);
    ListView_SetExtendedListViewStyle(hwndCassette, LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_REGIONAL | LVS_EX_FULLROWSELECT);
}

//Создание элементов главного окна
LRESULT CreatWindowChild(HWND hWnd)
{
    try
    {
        //if(!FULLSCREEN)
        //    LoadWindowPos(hWnd, PlacementName0);

        std::vector<HWND> ListGruup;
        ListGruup.push_back(hWnd);
        for(auto& f : mapWindow)
        {
            if(!strlen(f.second.Class))
            {
                ListGruup.pop_back();
            }
            else
            {
                if(ListGruup.empty())
                    throw std::exception(__FUN(std::string("Не могу создать окно ListGruup.empty() \"") + f.second.Class + "\""));

                f.second.hParent = ListGruup.back();
                f.second.hWnd = CreateWindow(f.second.Class, f.second.Title, f.second.Flag, f.second.rc.left, f.second.rc.top, f.second.rc.right, f.second.rc.bottom, f.second.hParent, (HMENU)f.first, hInstance, nullptr);

                if(!f.second.hWnd)
                    throw std::exception(__FUN(std::string("Не могу создать окно \"") + f.second.Class + "\""));


                if(f.first == hEdit_Sheet || f.first == hEdit_Cassette)
                    SendMessage(f.second.hWnd, WM_SETFONT, (WPARAM)Font[emFont::Font09], FALSE);
                else
                    SendMessage(f.second.hWnd, WM_SETFONT, (WPARAM)Font[emFont::Font09], FALSE);

                if(!strcmp(f.second.Class, szStat))
                {
                    if((f.second.Flag & SS_EDITCONTROL) == SS_EDITCONTROL)
                    {
                        SetWindowLongPtr(f.second.hWnd, GWLP_USERDATA, (LONG_PTR)&TitleBrush1);
                    }
                    //else
                }
                if(
                    !strcmp(szTem1, f.second.Class)
                    || !strcmp(szTem2, f.second.Class)
                    )
                {
                    //hwnd = f.second.hWnd;
                    ListGruup.push_back(f.second.hWnd);
                }
            }
        }
        ListGruup.clear();
        hWndDebug = winmap(HWNDCLIENT::hEditDiagnose);
        CreateHistoriSheet();
        CreateHistoriCassette();
        //CreateHistoriListTemperature();
        //InitAlloyThiknessCode();
        //SetWindowText(winmap(hEditDiagnose5), (Server + " = " + std::to_string(MyServer)).c_str());

        //InitDataTimeTest();
    }
    catch(std::exception& exc)
    {
        WinErrorExit(NULL, exc.what());
    }
    catch(...)
    {
        WinErrorExit(NULL, "Unknown error.");
    }

    //SetCurentTimeWin("Test");
    return 0;
}

//история кантовки листа
void AddHistoriSheet(bool begin)
{
    //int item = ListView_GetItemCount(hwndSheet);
    //if(item <= CountLastSheet)
        AddItem(hwndSheet, begin);
    //else
}

//история касcет
void AddHistoriCassette(bool begin)
{
    AddItem(hwndCassette, begin);
}

void DeleteSheetBoxItem(HWND hwndSheet)
{
    LRESULT iItemServ = ListView_GetItemCount(hwndSheet);
    if(iItemServ > 0)
        ListView_DeleteItem(hwndSheet, iItemServ - 1);
}

//Вывод текста титлов
void SetTitleText(HWNDCLIENT id, std::string Data)
{
    char ss[256];
    GetWindowText(winmap(id), ss, 255);
    if(Data != ss)
    {
        SetWindowText(winmap(id), Data.c_str());
        InvalidateRect(winmap(id), NULL, FALSE);
    }
}


LRESULT onMinimize(HWND hWnd, WORD wParam)
{
    if(wParam != BN_CLICKED) return 0;

    //LONG style = GetWindowLong(hWnd, GWL_STYLE) | ;
    //SetWindowLong(hWnd, GWL_STYLE, style);
    ShowWindow(hWnd, SW_MINIMIZE);
    //InvalidateRect(hWnd, NULL, TRUE);
    //DestroyWindow(hWnd);
    return 0;
}

LRESULT onExit(HWND hWnd, WORD wParam)
{
    if(wParam != BN_CLICKED) return 0;
    DestroyWindow(hWnd);
    return 0;
}


//Отрисовка текста окна
LRESULT DrawTextStat(LPDRAWITEMSTRUCT pDIS, int TT)
{
    char szBuff[MAX_STRING] = "";
    GetWindowText(pDIS->hwndItem, szBuff, 255);
    if(strlen(szBuff))
    {
        SetBkMode(pDIS->hDC, TRANSPARENT);
        SIZE siz;
        GetTextExtentPoint32A(pDIS->hDC, szBuff, (int)strlen(szBuff), &siz);
        char* s = szBuff;
        int coco = 1;
        while(s && *s) { if(*(s++) == '\n')coco++; }
        coco *= siz.cy / 2;
        int y = (pDIS->rcItem.bottom - pDIS->rcItem.top) / 2;
        pDIS->rcItem.top = y - coco;
        pDIS->rcItem.bottom = y + coco;
        pDIS->rcItem.left++;
        pDIS->rcItem.right--;
        DrawTextA(pDIS->hDC, szBuff, -1, &pDIS->rcItem, TT | DT_WORDBREAK | DT_VCENTER);


        //GetTextExtentPoint32(pDIS->hDC, szBuff, (int)strlen(szBuff), &siz);
        //if(siz.cx < pDIS->rcItem.right - pDIS->rcItem.left)
        //{
        //    char* s = szBuff;
        //    int coco = 1;
        //    while(s && *s) { if(*(s++) == '\n')coco++; }
        //    coco *= siz.cy / 2;
        //
        //    int y = (pDIS->rcItem.bottom - pDIS->rcItem.top) / 2;
        //    pDIS->rcItem.top = y - coco;
        //    pDIS->rcItem.bottom = y + coco;
        //    pDIS->rcItem.left++;
        //    pDIS->rcItem.right--;
        //    //DrawTextA(pDIS->hDC, szBuff, -1, &pDIS->rcItem, TT | DT_WORDBREAK | DT_VCENTER);
        //}
        //SetBkMode(pDIS->hDC, TRANSPARENT);
        //DrawTextA(pDIS->hDC, szBuff, -1, &pDIS->rcItem, TT | DT_WORDBREAK | DT_VCENTER);
    }
    return 0;
}

//Получение размера HICON
ICONINFO GetIconInfo(HICON hIcon)
{
    static ICONINFO info;
    ZeroMemory(&info, sizeof(info));

    BOOL bRes = FALSE;

    bRes = GetIconInfo(hIcon, &info);

    if(info.hbmColor)
        DeleteObject(info.hbmColor);
    if(info.hbmMask)
        DeleteObject(info.hbmMask);
    return info;
}

//Прорисовка HICON
LRESULT DrawIcom(LPDRAWITEMSTRUCT pDIS, RECT& rc)
{
    int IdImage = (int)SendMessage(pDIS->hwndItem, BM_GETIMAGE, (WPARAM)IMAGE_ICON, 0);


    if(IdImage && Icon[IdImage].icon)
    {
        //ICONINFO iInfo = GetIconInfo(Icon[IdImage].icon);
        int x = rc.left;
        int y = rc.top;
        int cx1 = rc.right - rc.left;
        int cy1 = rc.bottom - rc.top;

        int cx2 = Icon[IdImage].size; //iInfo.xHotspot;// IconSize[Image];
        int cy2 = Icon[IdImage].size; //iInfo.yHotspot;// IconSize[Image];

        if(cy1 < cx1) cx2 = cy2;
        else cy2 = cx2;

        x += (cx1 - cx2) / 2;
        y += (cy1 - cy2) / 2;

        DrawIconEx(pDIS->hDC, x, y, Icon[IdImage].icon, cx2, cy2, 0, NULL, DI_NORMAL);
    }
    return 0;
}

//Отрисовка окна кнопки
LRESULT DrawButton(LPDRAWITEMSTRUCT pDIS)
{
    DWORD style = GetWindowStyle(pDIS->hwndItem);
    LRESULT state = Button_GetState(pDIS->hwndItem);
    BOOL disabled = style & WS_DISABLED;
    BOOL flat = style & BS_FLAT;
    BOOL focus =  state & BST_FOCUS;
    BOOL pushed = state & BST_PUSHED;

    if(flat)    FillRect(pDIS->hDC, &pDIS->rcItem, TitleBrush1);
    else        FillRect(pDIS->hDC, &pDIS->rcItem, TitleBrush3);
    if(pushed)  OffsetRect(&pDIS->rcItem, 1, 1);

    DrawIcom(pDIS, pDIS->rcItem);
    //SetTextColor(pDIS->hDC, RGB(0, 0, 0));
    //DrawTextStat(pDIS, DT_CENTER);
    if(pushed)  OffsetRect(&pDIS->rcItem, -1, -1);
    if(focus)
    {
        InflateRect(&pDIS->rcItem, -1, -1);
        DrawFocusRect(pDIS->hDC, &pDIS->rcItem);
    }
    return 0;
}

HPEN hpen1 = CreatePen(PS_SOLID, 5, RGB(192, 192, 192));
//Отрисовка статичного окна
LRESULT DrawStatic(LPDRAWITEMSTRUCT pDIS)
{
    HBRUSH* br = (HBRUSH*)GetWindowLongPtr(pDIS->hwndItem, GWLP_USERDATA);
    if(br)
        FillRect(pDIS->hDC, &pDIS->rcItem, *br);
    else
        FillRect(pDIS->hDC, &pDIS->rcItem, TitleBrush3);



    DWORD style = GetWindowStyle(pDIS->hwndItem);
    int Al = DT_CENTER;
    if((style & MYSS_LEFT) == MYSS_LEFT)Al = DT_LEFT;
    if((style & MYSS_RIGHT) == MYSS_RIGHT)Al = DT_RIGHT;
    //HPEN oldPen = (HPEN)SelectObject(pDIS->hDC, hpen1);
    //if((style & SS_EDITCONTROL) == SS_EDITCONTROL) FrameRect(pDIS->hDC, &pDIS->rcItem, TitleBrush0);

    SetTextColor(pDIS->hDC, RGB(0, 0, 0));
    DrawTextStat(pDIS, Al);
    //SelectObject(pDIS->hDC, oldPen);
    return 0;
}

//Проверка стиля для отрисовки окна
LRESULT DrawItem(LPDRAWITEMSTRUCT pDIS)
{
    char szBuff[MAX_STRING] = "";
    GetClassName(pDIS->hwndItem, szBuff, MAX_STRING);
    CharUpper(szBuff);
    if(boost::iequals(szButt, szBuff))
        return DrawButton(pDIS);
    if(boost::iequals(szStat, szBuff))
        return DrawStatic(pDIS);

    return 0;
}

//Ограничение размера главного окна
LRESULT GetMinMaxInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MINMAXINFO* MI = (MINMAXINFO*)lParam;
    if(MI)
    {
        MI->ptMinTrackSize.x = DefMinTrackSizeX;
        MI->ptMinTrackSize.y = DefMinTrackSizeY;
    }
    return LRESULT(0);
};

#define ELSEIF(_s, _d) else if(subItem == _s) lstrcpy(plvdi->item.pszText, _d.c_str())

LRESULT OnNotifySheet(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPNM_LISTVIEW  pnm = (LPNM_LISTVIEW)lParam;
    switch(pnm->hdr.code)
    {
        case NM_DBLCLK:
        {
            HWND hwndLV = pnm->hdr.hwndFrom;
            int item = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_FOCUSED);
            if(item >= 0 && item < (int)AllSheet.size())
            {
                TSheet p = AllSheet[item];
                //PDF::PrintPdf(p);
            }
        }
            break;
        case LVN_GETDISPINFO:
        {
            LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;

            int item = plvdi->item.iItem;
            int subItem = plvdi->item.iSubItem;
            if(item >= 0 && item < (int)AllSheet.size())
            {
                if(plvdi->item.mask & LVIF_TEXT)
                {
                    TSheet p = AllSheet[item];
                    std::string sd = "";

                    try
                    {
                        if(subItem == casSheet::NN)         lstrcpy(plvdi->item.pszText, std::to_string(item + 1).c_str());
                        ELSEIF (casSheet::ID, p.id);
                        ELSEIF (casSheet::DataTime, GetDataTimeStr(p.DataTime));
                        ELSEIF (casSheet::Start_at, GetDataTimeStr(p.Start_at));
                        ELSEIF (casSheet::SecondPos_at, GetDataTimeStr(p.SecondPos_at));
                        ELSEIF (casSheet::DataTime_End, GetDataTimeStr(p.DataTime_End));
                        ELSEIF (casSheet::Correct, GetDataTimeStr(p.Correct));

                        ELSEIF (casSheet::Pos, (NamePos[p.Pos] + " (" + p.Pos + ")"));
                        ELSEIF (casSheet::News, p.News);
                        ELSEIF (casSheet::Day, p.Day);
                        ELSEIF (casSheet::Month, p.Month);
                        ELSEIF (casSheet::Year, p.Year);
                        ELSEIF (casSheet::CassetteNo, p.CassetteNo);
                        ELSEIF (casSheet::SheetInCassette, p.SheetInCassette);

                        ELSEIF (casSheet::DataTime_All, p.DataTime_All);
                        ELSEIF (casSheet::TimeForPlateHeat, p.TimeForPlateHeat); //Время сигнализации окончания нагрева, мин

                        ELSEIF (casSheet::PresToStartComp, p.PresToStartComp);  //Уставка давления для запуска комперссора


                        ELSEIF (casSheet::Alloy, p.Alloy);
                        ELSEIF (casSheet::Thikness, p.Thikness);
                        
                        ELSEIF (casSheet::Melt, p.Melt);
                        //ELSEIF (casSheet::Slab, p.Slab);
                        ELSEIF (casSheet::PartNo, p.PartNo);
                        ELSEIF (casSheet::Pack, p.Pack);
                        ELSEIF (casSheet::Sheet, (p.Sheet + "/" + p.SubSheet));
                        //ELSEIF (casSheet::SubSheet, p.SubSheet);
                        ELSEIF (casSheet::Temper, p.Temper);
                        ELSEIF (casSheet::Temperature, p.Temperature);
                        ELSEIF (casSheet::Speed, p.Speed);
                        ELSEIF (casSheet::Za_PT3, p.Za_PT3);
                        ELSEIF (casSheet::Za_TE3, p.Za_TE3);
                        
                        ELSEIF (casSheet::LamPressTop, p.LaminPressTop);
                        ELSEIF (casSheet::LamPressBot, p.LaminPressBot);
                        
                        ELSEIF (casSheet::PosClapanTop, p.PosClapanTop);
                        ELSEIF (casSheet::PosClapanBot, p.PosClapanBot);
                        ELSEIF (casSheet::Mask, p.Mask);
                        ELSEIF (casSheet::Lam1PosClapanTop, p.Lam1PosClapanTop);
                        ELSEIF (casSheet::Lam1PosClapanBot, p.Lam1PosClapanBot);
                        ELSEIF (casSheet::Lam2PosClapanTop, p.Lam2PosClapanTop);
                        ELSEIF (casSheet::Lam2PosClapanBot, p.Lam2PosClapanBot);
                        
                        ELSEIF (casSheet::Lam_TE1, p.LAM_TE1);
                        
                        
                        //ELSEIF (casSheet::Top1, p.Top1);
                        //ELSEIF (casSheet::Top2, p.Top2);
                        //ELSEIF (casSheet::Top3, p.Top3);
                        //ELSEIF (casSheet::Top4, p.Top4);
                        //ELSEIF (casSheet::Top5, p.Top5);
                        //ELSEIF (casSheet::Top6, p.Top6);
                        //ELSEIF (casSheet::Top7, p.Top7);
                        //ELSEIF (casSheet::Top8, p.Top8);
                        
                        //ELSEIF (casSheet::Bot1, p.Bot1);
                        //ELSEIF (casSheet::Bot2, p.Bot2);
                        //ELSEIF (casSheet::Bot3, p.Bot3);
                        //ELSEIF (casSheet::Bot4, p.Bot4);
                        //ELSEIF (casSheet::Bot5, p.Bot5);
                        //ELSEIF (casSheet::Bot6, p.Bot6);
                        //ELSEIF (casSheet::Bot7, p.Bot7);
                        //ELSEIF (casSheet::Bot8, p.Bot8);
                    }
                    CATCH(AllLogger, FUNCTION_LINE_NAME + " : ");
                }
            }
        }
    }
    return 0;
}

LRESULT OnNotifyCassette(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPNM_LISTVIEW  pnm = (LPNM_LISTVIEW)lParam;
    switch(pnm->hdr.code)
    {
        case NM_RCLICK:
        {
            HWND hwndLV = pnm->hdr.hwndFrom;
            LV_ITEM lvi;
            lvi.iItem = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_FOCUSED);
            if(lvi.iItem == -1) return FALSE;
            TCassette& cassette = AllCassette[lvi.iItem];
            if(cassette.Event == "7")
                DisplayContextMenu(hWnd, IDR_MENU2);
            else if(cassette.Event == "2" || cassette.Event == "5")
                DisplayContextMenu(hWnd, IDR_MENU1);
        }
        break;
        case LVN_ITEMCHANGED:
        {
            LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
            if(pnmv->iItem >= 0 && pnmv->iItem < AllCassette.size() && pnmv->uNewState & LVIS_SELECTED)
            {
                //bool stat = pnmv->uNewState & LVIS_SELECTED;
                //TCassette p = AllCassette[pnmv->iItem];
                Open_GRAFF_FURN(AllCassette[pnmv->iItem]);

                int tt = 0;
            }
            //HWND hwndLV = pnm->hdr.hwndFrom;
            //LV_ITEM lvi;
            //lvi.iItem = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_FOCUSED);
            //if (lvi.iItem == -1) return FALSE;
        //	lvi.iSubItem = 0;
        //	lvi.mask = LVIF_PARAM;
        //	ListView_GetItem(hwndLV, &lvi);
        //	BLOCK *p = (BLOCK *)lvi.lParam;
        //	if(p && p->List2 && p->List2->hwnd != hwndLV)
        //	{
        //		char ss[1024];
        //		ListView_DeleteAllItems(p->List2->hwnd);
        //		ULONG count = p->GetAddListTagCount();
        //		ListView_SetItemCount(p->List2->hwnd, count);
        //		p->AddListTag();
        //		sprintf(ss, "%u", ListView_GetItemCount(p->List2->hwnd));
        //		p->Status->UpdateStatusBar(ss, 3, 0);
        //	}
        }
        break;
        case LVN_GETDISPINFO:
        {
            LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
            int item = plvdi->item.iItem;
            if(item >= 0 && item < AllCassette.size())
            {
                if(plvdi->item.mask & LVIF_TEXT)
                {
                    TCassette p = AllCassette[item];
                    {
                        try
                        {
                            if(plvdi->item.iSubItem == Cassete::NN)                lstrcpy(plvdi->item.pszText, std::to_string(item + 1).c_str());
                            if(plvdi->item.iSubItem == Cassete::Id)                lstrcpy(plvdi->item.pszText, p.Id.c_str());
                            if(plvdi->item.iSubItem == Cassete::Event)             lstrcpy(plvdi->item.pszText, (EventCassette[Stoi(p.Event)] + "(" + p.Event + ")").c_str());
                            if(plvdi->item.iSubItem == Cassete::Create_at)         lstrcpy(plvdi->item.pszText, GetDataTimeStr(p.Create_at).c_str());
                            if(plvdi->item.iSubItem == Cassete::Year)              lstrcpy(plvdi->item.pszText, p.Year.c_str());
                            if(plvdi->item.iSubItem == Cassete::Month)             lstrcpy(plvdi->item.pszText, p.Month.c_str());
                            if(plvdi->item.iSubItem == Cassete::Day)               lstrcpy(plvdi->item.pszText, p.Day.c_str());
                            if(plvdi->item.iSubItem == Cassete::CassetteNo)        lstrcpy(plvdi->item.pszText, p.CassetteNo.c_str());
                            if(plvdi->item.iSubItem == Cassete::SheetInCassette)   lstrcpy(plvdi->item.pszText, p.SheetInCassette.c_str());
                            //if(plvdi->item.iSubItem == Cassete::Close)             lstrcpy(plvdi->item.pszText, p.Close_at.c_str());  //Закрытие касеты
                            if(plvdi->item.iSubItem == Cassete::Run_at)            lstrcpy(plvdi->item.pszText, GetDataTimeStr(p.Run_at).c_str());      //Начало отпуска
                            if(plvdi->item.iSubItem == Cassete::Error_at)          lstrcpy(plvdi->item.pszText, GetDataTimeStr(p.Error_at).c_str());    //Ошибка отпуска
                            if(plvdi->item.iSubItem == Cassete::End_at)            lstrcpy(plvdi->item.pszText, GetDataTimeStr(p.End_at).c_str());      //Конец отпуска
                            if(plvdi->item.iSubItem == Cassete::Finish_at)         lstrcpy(plvdi->item.pszText, GetDataTimeStr(p.Finish_at).c_str());//Конец процесса
                            if(plvdi->item.iSubItem == Cassete::Peth)              lstrcpy(plvdi->item.pszText, p.Peth.c_str());       //Печь
                            if(plvdi->item.iSubItem == Cassete::HeatAcc)           lstrcpy(plvdi->item.pszText, p.HeatAcc.c_str());           //Факт время нагрева
                            if(plvdi->item.iSubItem == Cassete::HeatWait)          lstrcpy(plvdi->item.pszText, p.HeatWait.c_str());          //Факт время выдержки
                            if(plvdi->item.iSubItem == Cassete::Total)             lstrcpy(plvdi->item.pszText, p.Total.c_str());             //Факт общее время
                        }
                        CATCH(AllLogger, FUNCTION_LINE_NAME + " : ");
                    }
                }
            }
        }
        break;
    }
    return 0;
}

//ID_POP_40001
//Вывод данных листа в ListBox
LRESULT OnNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    LPNMHDR hdr = (LPNMHDR)lParam;
    if(hdr->idFrom == hEdit_Sheet)
        return OnNotifySheet(hWnd, message, wParam, lParam);

    if(hdr->idFrom == hEdit_Cassette)
        return OnNotifyCassette(hWnd, message, wParam, lParam);
    return 0;
}

//Команды групповыхо окон
LRESULT CALLBACK CommandTemp1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD command = LOWORD(wParam);
    //if(command == IDM_ABOUT) return MessageBox(hWnd, "IDM_ABOUT", "IDM_ABOUT", 0);
    //if(command == hStatSheet_Data)      	return Sheet_NewSheet(HIWORD(wParam));
    //if(command == ID_StatSheet_Data)    	return Sheet_NewSheet(HIWORD(wParam));
    //if(command == hButtSheet_CassetteNew)	return Sheet_NewCassette(HIWORD(wParam));
    //if(command == hEdit_Sheet_AlloyCode)     return SelectAlloyCodeTemp(HIWORD(wParam), (HWND)lParam);
    //if(command == ID_StatSheet_Data)     return SelectAlloyCodeTemp(HIWORD(wParam), (HWND)lParam);
    //if(command == hButtSheet_FindSheet)      return FindSheetTemp(HIWORD(wParam));
    //if(command == hEdit_Sheet_Cassette_Day)	return CreateCalendar(hWnd, HIWORD(wParam));
    //if(command == hEdit_Sheet_Cassette_Month)return CreateCalendar(hWnd, HIWORD(wParam));
    //if(command == hEdit_Sheet_Cassette_Year)	return CreateCalendar(hWnd, HIWORD(wParam));

    return DefWindowProc(hWnd, message, wParam, lParam);
}

//Прорисовка Temp2
LRESULT PaintTemp2(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);

    //FrameRect(hDC, &ps.rcPaint, TitleBrush0);
    FillRect(hDC, &ps.rcPaint, TitleBrush1);
    //FrameRect(hDC, &ps.rcPaint, TitleBrush0);


    char szBuff[MAX_STRING] = "";
    GetWindowText(hWnd, szBuff, 255);
    DWORD style = GetWindowStyle(hWnd);

    if(strlen(szBuff))
    {
        RECT rc1 = ps.rcPaint;
        RECT rc2 = ps.rcPaint;
        rc1.bottom = rc1.top + 19;
        rc2.bottom--;
        FillRect(hDC, &rc1, TitleBrush3);
        FrameRect(hDC, &rc1, TitleBrush0);
        //FrameRect(hDC, &rc2, TitleBrush0);

        SetTextColor(hDC, RGB(0, 0, 0));
        SetBkMode(hDC, TRANSPARENT);

        //HFONT oldfont = (HFONT)SelectObject(hDC, Font[emFont::Font09]);
        DrawText(hDC, szBuff, -1, &rc1, DT_CENTER | DT_SINGLELINE | DT_WORDBREAK | DT_VCENTER);
        //SelectObject(hDC, oldfont);
    }
    if((style & WS_GROUP) == WS_GROUP)
    {
        RECT rc2 = ps.rcPaint;
        rc2.bottom -= 1;
        FrameRect(hDC, &rc2, TitleBrush0);
    }

    EndPaint(hWnd, &ps);
    return 0;
}

LRESULT CALLBACK WndProcTemp1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//Сообщения групповых окон 2
LRESULT CALLBACK WndProcTemp2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_PAINT) return PaintTemp2(hWnd);
    return WndProcTemp1(hWnd, message, wParam, lParam);
}

//Прорисовка Temp2
LRESULT PaintTemp1(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);

    FillRect(hDC, &ps.rcPaint, TitleBrush2);
    //FrameRect(hDC, &ps.rcPaint, TitleBrush0);
    DWORD style = GetWindowStyle(hWnd);


    char szBuff[MAX_STRING] = "";
    GetWindowText(hWnd, szBuff, 255);
    if(strlen(szBuff))
    {
        RECT rc1 = ps.rcPaint;
        //RECT rc2 = ps.rcPaint;
        rc1.bottom = rc1.top + 19;

        //rc2.top += 18;
        //rc2.bottom -= 1;

        FillRect(hDC, &rc1, TitleBrush4);
        FrameRect(hDC, &rc1, TitleBrush0);

        SetTextColor(hDC, RGB(255, 255, 255));
        SetBkMode(hDC, TRANSPARENT);

        //HFONT oldfont = (HFONT)SelectObject(hDC, Font[emFont::Font12]);
        DrawText(hDC, szBuff, -1, &rc1, DT_CENTER | DT_SINGLELINE | DT_WORDBREAK | DT_VCENTER);
        //SelectObject(hDC, oldfont);
    }
    if((style & WS_GROUP) == WS_GROUP)
    {
        RECT rc2 = ps.rcPaint;
        rc2.bottom -= 1;
        FrameRect(hDC, &rc2, TitleBrush0);
    }
    EndPaint(hWnd, &ps);
    return 0;
}

//Команды для Temp1
LRESULT UserTemp1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(wParam == SET_CALENDAR_DATA)
    {
        SYSTEMTIME* TM = (SYSTEMTIME*)lParam;
        if(!TM) return 0;

        char sFormat[10];
        sprintf_s(sFormat, 10, "%02d", TM->wDay);
        SetWindowText(winmap(HWNDCLIENT::hEdit_Sheet_Cassette_Day), sFormat);

        sprintf_s(sFormat, 10, "%02d", TM->wMonth);
        SetWindowText(winmap(HWNDCLIENT::hEdit_Sheet_Cassette_Month), sFormat);

        sprintf_s(sFormat, 10, "%04d", TM->wYear);
        SetWindowText(winmap(HWNDCLIENT::hEdit_Sheet_Cassette_Year), sFormat);
    }
    return 0;
}

//Сообщения групповых окон 1
LRESULT CALLBACK WndProcTemp1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_USER) return UserTemp1(hWnd, message, wParam, lParam);
    if(message == WM_PAINT) return PaintTemp1(hWnd);
    //if(message == WM_ERASEBKGND) return 1;// PaintTemp1(hWnd);
    if(message == WM_DRAWITEM) return DrawItem((LPDRAWITEMSTRUCT)lParam);
    if(message == WM_COMMAND)  return CommandTemp1(hWnd, message, wParam, lParam);

    return DefWindowProc(hWnd, message, wParam, lParam);
}


//Изменение размера главного окна
LRESULT Size1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int cx = LOWORD(lParam);
    int cy = HIWORD(lParam);


    RECT rc;
    GetClientRect(winmap(HWNDCLIENT::hGroup300), &rc);
    std::string ss = "cx ";
    ss += std::to_string(rc.right - rc.left) + " cy: " + std::to_string(rc.bottom - rc.top);
    SetWindowText(winmap(HWNDCLIENT::hEditDiagnose), ss.c_str());
    
    


    POINT ptLT1 ={0, 0};
    MapWindowPoints(winmap(HWNDCLIENT::hEdit_Sheet), hWnd, &ptLT1, 1);
    int y2 = cy - ptLT1.y - 20;

    if(FULLSCREEN)
    {
        MoveWindow(winmap(HWNDCLIENT::hEditDiagnose), 0, cy - 20, cx, 20, true);
        MoveWindow(winmap(HWNDCLIENT::hEdit_Sheet), 0, ptLT1.y - 1, cx, y2, true);
        MoveWindow(winmap(HWNDCLIENT::hEditszButt), cx - 21, 0, 21, 21, false);
    }
    else
    {
        MoveWindow(winmap(HWNDCLIENT::hEditDiagnose), 0, 1007 - 20, cx, 20, false);
        MoveWindow(winmap(HWNDCLIENT::hEdit_Sheet), 0, 700, cx, 286, true);
        MoveWindow(winmap(HWNDCLIENT::hEditszButt), cx - 21, 0, 21, 21, false);
        
    }
    //SetWindowPos(winmap(HWNDCLIENT::hEdit_Sheet), HWND_TOP, 5, 0, cx - 10, y2, SWP_NOMOVE);

    return 0;
}

//Команды главного окна
void SetUpdateCassete(PGConnection& conn, TCassette& cassette, std::string update, std::string where);

LRESULT Command1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int command = LOWORD(wParam);
    // Разобрать выбор в меню:
    //if(command == IDM_ABOUT) return MessageBox(hWnd, "IDM_ABOUT", "IDM_ABOUT", 0);

    //Удаление кассеты из списка
    if(command == ID_POP_40001)
    {
        LV_ITEM lvi;
        lvi.iItem = ListView_GetNextItem(hwndCassette, -1, LVNI_ALL | LVNI_FOCUSED);
        if(lvi.iItem == -1) return FALSE;

        if(lvi.iItem < AllCassette.size())
        {
            TCassette& cassette = AllCassette[lvi.iItem];
            if(cassette.Event == "2" || cassette.Event == "5")
            {
                PGConnection conn;
                conn.connection();
                std::time_t st;
                cassette.Event = "7";
                cassette.Delete_at = GetDataTimeString(st);
                std::stringstream sd;
                sd << "UPDATE cassette SET event = 7, delete_at = now() WHERE id = " << cassette.Id;
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                SETUPDATESQL(SQLLogger, conn, sd);
            }

            //SetUpdateCassete(conn, cassette, "event = 7, delete_at = now(), event = 7 ", "");
            //MessageBox(hWnd, ss.c_str(), "ID_POP_40001", 0);
        }
    }
    if(command == ID_POP_40002)
    {
        LV_ITEM lvi;
        lvi.iItem = ListView_GetNextItem(hwndCassette, -1, LVNI_ALL | LVNI_FOCUSED);
        if(lvi.iItem == -1) return FALSE;

        if(lvi.iItem < AllCassette.size())
        {
            TCassette& cassette = AllCassette[lvi.iItem];
            if(cassette.Event == "7")
            {
                PGConnection conn;
                conn.connection();
                std::stringstream sd;
                if(cassette.Finish_at.length())
                {
                    cassette.Event = "5";
                    cassette.Delete_at = "";
                    sd << "UPDATE cassette SET event = 5, delete_at = DEFAULT WHERE id = " << cassette.Id;
                }
                else
                {
                    cassette.Event = "2";
                    cassette.Delete_at = "";
                    sd << "UPDATE cassette SET event = 2, delete_at = DEFAULT WHERE id = " << cassette.Id;
                }
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                SETUPDATESQL(SQLLogger, conn, sd);
            }

            //SetUpdateCassete(conn, cassette, "event = 2, delete_at = DEFAULT ", "");
            //MessageBox(hWnd, ss.c_str(), "ID_POP_40001", 0);
        }
    }
    if(command == 999)
    {
        //Временная кнопка для выполнения тестов
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}



LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_CREATE) return CreatWindowChild(hWnd);
    if(message == WM_COMMAND) return Command1(hWnd, message, wParam, lParam);
    if(message == WM_SIZE) return Size1(hWnd, message, wParam, lParam);
    //if(message == WM_ERASEBKGND) return 1;
    //if(message == WM_PAINT) return Paint1(hWnd);

    //if(message == WM_NOTIFY) return OnNotify(hWnd, message, wParam, lParam);


    if(message == WM_NOTIFY) return OnNotify(hWnd, message, wParam, lParam);
    if(message == WM_DRAWITEM) { return DrawItem((LPDRAWITEMSTRUCT)lParam); }
    if(message == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL bSendNewSheet = FALSE;          //Команда в PLC "Новый лист"
BOOL bSendNewCassette = FALSE;       //Команда в PLC "Новая касета"

//Команда для PLC Новый лист
LRESULT Sheet_New(HWND hWnd, WORD wParam)
{
    if(wParam != BN_CLICKED) return 0;
    if(MessageBox(hWnd, "Это тестовая команда!\nНо данные будут записаны в базу.\nВы увепены что хотите ввести новый лист кантовки?", "Внимание!", MB_SYSTEMMODAL | MB_ICONQUESTION | MB_YESNO) == IDYES)
        bSendNewSheet = TRUE;
    else
        bSendNewSheet = FALSE;
    SetFocus(NULL);

    return 0;
}


LRESULT Cassette_New(HWND hWnd, WORD wParam)
{
    if(wParam != BN_CLICKED) return 0;
    if(MessageBox(hWnd, "Это тестовая команда!\nНо данные будут записаны в базу.\nВы увепены что хотите ввести новую касету кантовки?", "Внимание!", MB_SYSTEMMODAL | MB_ICONQUESTION | MB_YESNO) == IDYES)
        bSendNewCassette = TRUE;
    else
        bSendNewCassette = FALSE;
    SetFocus(NULL);

    return 0;
}


LRESULT Command0(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int command = LOWORD(wParam);
    // Разобрать выбор в меню:
    //if(command == IDM_ABOUT) return MessageBox(hWnd, "IDM_ABOUT", "IDM_ABOUT", 0);
    if(command == IDI_MINI) return onMinimize(hWnd, HIWORD(wParam));
    if(command == IDI_EXIT) return onExit(hWnd, HIWORD(wParam));
    if(command == IDI_NEWDATA1) return Cassette_New(hWnd, HIWORD(wParam));
    if(command == IDI_NEWDATA3) return Sheet_New(hWnd, HIWORD(wParam));
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Size0(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int cx = LOWORD(lParam);
    int cy = HIWORD(lParam);

    //RECT rc;
    //GetWindowRect(hWnd, &rc);
    //std::string ss = "cx ";
    //ss += std::to_string(rc.right - rc.left) + " cy: " + std::to_string(rc.bottom - rc.top);
    //SetWindowText(winmap(HWNDCLIENT::hEditDiagnose), ss.c_str());

    if(FULLSCREEN)
    {
        //MoveWindow(Global1, 5, SIZEY, MinTrackSizeX - 10, cy - SIZEY, true);
        MoveWindow(hWndTitl, SIZEX * 2, 0, cx - SIZEX * 4 + 1, SIZEY, true);
        MoveWindow(hWndTest1, SIZEX * 0, 0, SIZEX + 1, SIZEY, true);
        MoveWindow(hWndTest2, SIZEX * 1, 0, SIZEX + 1, SIZEY, true);
        MoveWindow(hWndExit, cx - SIZEX * 1, 0, SIZEX + 1, SIZEY, true);
        MoveWindow(hWndMini, cx - SIZEX * 2, 0, SIZEX + 1, SIZEY, true);
    }
    else
    {
        
        MoveWindow(Global1, 5, 5, cx - 15, 1017 - 10, true);
        InvalidateRect(Global1, NULL, true);

        //MoveWindow(hWndTitl, SIZEX * 2, 0, cx - SIZEX * 2 + 1, cy - SIZEY, true);
        //MoveWindow(hWndTest1, SIZEX * 0, 0, cx - SIZEX + 1, cy - SIZEY, true);
        //MoveWindow(hWndTest2, SIZEX * 1, 0, cx - SIZEX + 1, cy - SIZEY, true);
    }


    SaveWindowPos(hWnd, PlacementName0);

    return 0;
}

LRESULT Move0(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //if(!FULLSCREEN)
    SaveWindowPos(hWnd, PlacementName0);

//RECT rc;
//GetWindowRect(hWnd, &rc);
//std::string ss;
//ss = " x: " + std::to_string(rc.left) + " y: " + std::to_string(rc.top) + " cx: " + std::to_string(rc.right - rc.left) + " cy: " + std::to_string(rc.bottom - rc.top);
//SetWindowText(hWnd, ss.c_str());

    return 0;
}


LRESULT CALLBACK WndProc0(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //if(message == WM_CREATE) return CreatWindowChild(hWnd);
    if(message == WM_COMMAND) return Command0(hWnd, message, wParam, lParam);
    if(message == WM_SIZE) return Size0(hWnd, message, wParam, lParam);
    if(message == WM_MOVE) return Move0(hWnd, message, wParam, lParam);
    //if(message == WM_ERASEBKGND) return 0;
    //if(message == WM_PAINT) return 0;
    if(message == WM_GETMINMAXINFO) return GetMinMaxInfo(hWnd, message, wParam, lParam);
    //if(message == WM_NOTIFY) return OnNotify(hWnd, message, wParam, lParam);


    //if(message == WM_NOTIFY) return OnNotify(hWnd, message, wParam, lParam);
    if(message == WM_DRAWITEM) { return DrawItem((LPDRAWITEMSTRUCT)lParam); }
    //if(message == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hWnd, message, wParam, lParam);
}


//Отрисовка текста окна
LRESULT DrawTextStat(HWND hWnd, PAINTSTRUCT ps, int TT)
{
    char szBuff[MAX_STRING] = "";
    GetWindowText(hWnd, szBuff, 255);
    if(strlen(szBuff))
    {
        SIZE siz;
        GetTextExtentPoint32(ps.hdc, szBuff, (int)strlen(szBuff), &siz);
        if(siz.cx < ps.rcPaint.right - ps.rcPaint.left)
        {
            char* s = szBuff;
            int coco = 1;
            while(s && *s) { if(*(s++) == '\n')coco++; }
            coco *= siz.cy / 2;

            int y = (ps.rcPaint.bottom - ps.rcPaint.top) / 2;
            ps.rcPaint.top = y - coco;
            ps.rcPaint.bottom = y + coco;
            ps.rcPaint.left++;
            ps.rcPaint.right--;
            //DrawTextA(pDIS->hDC, szBuff, -1, &pDIS->rcItem, TT | DT_WORDBREAK | DT_VCENTER);
        }
        SetBkMode(ps.hdc, TRANSPARENT);
        DrawTextA(ps.hdc, szBuff, -1, &ps.rcPaint, TT | DT_WORDBREAK | DT_VCENTER);
    }
    return 0;
}

//Прорисовка HICON
LRESULT DrawIcom(HWND hWnd, PAINTSTRUCT ps)
{
    int IdImage = (int)SendMessage(hWnd, BM_GETIMAGE, (WPARAM)IMAGE_ICON, 0);


    if(IdImage && Icon[IdImage].icon)
    {
        int x = ps.rcPaint.left;
        int y = ps.rcPaint.top;
        int cx1 = ps.rcPaint.right - ps.rcPaint.left;
        int cy1 = ps.rcPaint.bottom - ps.rcPaint.top;

        int cx2 = Icon[IdImage].size; //iInfo.xHotspot;// IconSize[Image];
        int cy2 = Icon[IdImage].size; //iInfo.yHotspot;// IconSize[Image];

        if(cy1 < cx1) cx2 = cy2;
        else cy2 = cx2;

        x += (cx1 - cx2) / 2;
        y += (cy1 - cy2) / 2;

        DrawIconEx(ps.hdc, x, y, Icon[IdImage].icon, cx2, cy2, 0, NULL, DI_NORMAL);
    }
    return 0;
}

LRESULT PaintButton(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);

    DWORD style = GetWindowStyle(hWnd);
    LRESULT state = Button_GetState(hWnd);

    BOOL icon = style & BS_ICON;
    BOOL disabled = style & WS_DISABLED;
    BOOL flat = style & BS_FLAT;
    BOOL focus =  state & BST_FOCUS;
    BOOL pushed = state & BST_PUSHED;

    if(flat) FillRect(hDC, &ps.rcPaint, TitleBrush1);
    else     FillRect(hDC, &ps.rcPaint, TitleBrush3);
    if(pushed)  OffsetRect(&ps.rcPaint, 1, 1);

    if(icon)DrawIcom(hWnd, ps);
    SetTextColor(hDC, RGB(0, 0, 0));
    DrawTextStat(hWnd, ps, DT_CENTER);
    if(pushed)  OffsetRect(&ps.rcPaint, -1, -1);
    if(focus)
    {
        InflateRect(&ps.rcPaint, -1, -1);
        DrawFocusRect(hDC, &ps.rcPaint);
    }

    EndPaint(hWnd, &ps);
    return 1;
}

static LRESULT OldWndProcButton = NULL;

//Обработека сообщений Button Рисуем заголовок
LRESULT CALLBACK WndProcButton(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if(message == WM_ERASEBKGND)    return 0;
    if(message == WM_PAINT)         return PaintButton(hWnd, message, wParam, lParam, uIdSubclass, dwRefData);
    //if(message == HDM_LAYOUT)       return OnHeader_Layout(hWnd, message, wParam, lParam, uIdSubclass, dwRefData);

    //if(OldWndProcButton)
    //    return CallWindowProc((WNDPROC)OldWndProcButton, hWnd, message, wParam, lParam);
    //else
    return DefSubclassProc(hWnd, message, wParam, lParam);
}


//Открытие главного окна
std::map<int, MONITORINFOEX>ScreenArrayInfo;

BOOL CALLBACK MyInfoEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MONITORINFOEX iMonitor;
    iMonitor.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &iMonitor);
    if(iMonitor.dwFlags != DISPLAY_DEVICE_MIRRORING_DRIVER)
    {
        std::string d = iMonitor.szDevice;
        size_t t = d.find("DISPLAY");
        if(t != std::string::npos)
            ScreenArrayInfo[atoi(&d[t + 7])] = iMonitor;
    }
    return true;
}

void enterFullscreen() {
    EnumDisplayMonitors(NULL, NULL, &MyInfoEnumProc, (LPARAM)0);

    int XV = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int YV = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int CXV = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int CYV = GetSystemMetrics(SM_CYVIRTUALSCREEN);

}

void InitInstance()
{
    int cx = 0;
    int cy = 0;
    enterFullscreen();

    int t = (int)ScreenArrayInfo.size();
    if(!t)
        FULLSCREEN = false;

    if(FULLSCREEN)
    {
        if(ScreenArrayInfo.find(Monitor) == ScreenArrayInfo.end())
            Monitor = ScreenArrayInfo.begin()->first;

        auto scr = ScreenArrayInfo[Monitor];

        cx = scr.rcMonitor.right - scr.rcMonitor.left;
        cy = scr.rcMonitor.bottom - scr.rcMonitor.top;

        Global0 = CreateWindowExA(0 /*| WS_EX_TOPMOST*/,
                                  szWindowClass0, szTitle.c_str(),
                                  //WS_OVERLAPPEDWINDOW
                                  WS_POPUP | WS_CLIPCHILDREN,
                                  scr.rcMonitor.left,
                                  scr.rcMonitor.top,
                                  cx,
                                  cy,
                                  NULL, NULL, hInstance, NULL);
        if(!Global0) throw std::exception((FUNCTION_LINE_NAME + std::string(" Ошибка создания окна : " + std::string(szWindowClass1))).c_str());
        ShowWindow(Global0, SW_MAXIMIZE);

        RECT rc;
        GetClientRect(Global0, &rc);
        cx = rc.right - rc.left;
        cy = rc.bottom - rc.top;

        RECT Rect ={0};
        AdjustWindowRectEx(&Rect, WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN, false, 0);
        borderX = abs(Rect.left) - abs(Rect.right);
        borderY = abs(Rect.top) - abs(Rect.bottom);
        MaxTrackSizeX   = cx - abs(Rect.left) - abs(Rect.right) - 10;
        MaxTrackSizeY   = cy - abs(Rect.top) - abs(Rect.bottom) - SIZEY;
        CenterX = MaxTrackSizeX / 2;


        Global1 = CreateWindowExA(0,
                                  szWindowClass1, "",
                                  WS_CHILD /*| WS_BORDER */ | WS_CLIPCHILDREN,
                                  5,
                                  SIZEY,
                                  MaxTrackSizeX,
                                  MaxTrackSizeY,
                                  Global0, (HMENU)10, hInstance, NULL);
        if(!Global1) throw std::exception((FUNCTION_LINE_NAME + std::string(" Ошибка создания окна : " + std::string(szWindowClass1))).c_str());


        hWndTitl = CreateWindow(szStat, szTitle.c_str(), Stat10Flag | DT_SINGLELINE, SIZEX * 2, 0, cx - SIZEX * 4 + 1, SIZEY, Global0, (HMENU)IDI_EXIT, hInstance, nullptr);

        hWndExit = CreateWindow(szButt, "", Butt8Flag | WS_BORDER, cx - SIZEX * 1, 0, SIZEX + 1, SIZEY, Global0, (HMENU)IDI_EXIT, hInstance, nullptr);
        SendMessage(hWndExit, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)IDI_EXIT);
        SetWindowSubclass(hWndExit, WndProcButton, 10, 0);

        hWndMini = CreateWindow(szButt, "", Butt8Flag | WS_BORDER, cx - SIZEX * 2, 0, SIZEX + 1, SIZEY, Global0, (HMENU)IDI_MINI, hInstance, nullptr);
        SendMessage(hWndMini, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)IDI_MINI);
        SetWindowSubclass(hWndMini, WndProcButton, 10, 0);
    }
    else
    {
        Global0 = CreateWindowExA(0/*WS_EX_TOPMOST*/,
                                  szWindowClass0, szTitle.c_str(),
                                  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                  0, 0, 0, 0,
                                  NULL, NULL, hInstance, NULL);


        if(!Global0) throw std::exception((FUNCTION_LINE_NAME + std::string(" Ошибка создания окна : " + std::string(szWindowClass1))).c_str());

        LoadWindowPos(Global0, PlacementName0);

        RECT rc;
        GetClientRect(Global0, &rc);
        cx = rc.right - rc.left - 10;
        cy = rc.bottom - rc.top;

        RECT Rect ={0};
        AdjustWindowRectEx(&Rect, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, false, 0);

        borderX = abs(Rect.left) + abs(Rect.right);
        borderY = abs(Rect.top) + abs(Rect.bottom);

        MaxTrackSizeX   = DefMinTrackSizeX - abs(Rect.left - Rect.right) - 5;
        MaxTrackSizeY   = DefMinTrackSizeY - abs(Rect.top - Rect.bottom) - 5;
        CenterX = MaxTrackSizeX / 2;

        Global1 = CreateWindowExA(0,
                                  szWindowClass1, "",
                                  WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER,
                                  //0, 0, 0, 0,
                                  5,
                                  5,
                                  1905, 1017 - 10,
                                  //MaxTrackSizeX,
                                  //MaxTrackSizeY,
                                  Global0, (HMENU)10, hInstance, NULL);
        if(!Global1) throw std::exception((FUNCTION_LINE_NAME + std::string(" Ошибка создания окна : " + std::string(szWindowClass1))).c_str());


        //hWndMini = CreateWindow(szButt, "", Butt8Flag | WS_BORDER, cx - SIZEX * 2, 0, SIZEX + 1, SIZEY, Global0, (HMENU)IDI_MINI, hInstance, nullptr);
        //SendMessage(hWndMini, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)IDI_MINI);
        //SetWindowSubclass(hWndMini, WndProcButton, 10, 0);

        //ShowWindow(Global0, SW_NORMAL);
        //ShowWindow(Global1, SW_NORMAL);
        InvalidateRect(Global0, NULL, TRUE);
        InvalidateRect(Global1, NULL, TRUE);

        //hWndTitl = CreateWindow(szStat, "", Stat10Flag | DT_SINGLELINE, SIZEX * 2, 0, cx - SIZEX * 2 + 1, SIZEY, Global0, (HMENU)IDI_EXIT, hInstance, nullptr);
        //if(!hWndTitl) throw std::exception((FUNCTION_LINE_NAME + std::string(" Ошибка создания окна : " + std::string(szStat))).c_str());
    }


    //if(MyServer)
    //{
        //hWndTest1 = CreateWindow(szButt, "", Butt8Flag | WS_BORDER, SIZEX * 0, 0, SIZEX + 1, SIZEY, Global0, (HMENU)IDI_NEWDATA3, hInstance, nullptr);
        //SendMessage(hWndTest1, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)IDI_NEWDATA3);
        ////OldWndProcButton = SetWindowLongPtr(hWndTest1, GWLP_WNDPROC, (LONG_PTR)WndProcButton);
        //SetWindowSubclass(hWndTest1, WndProcButton, 10, 0);
        //
        //hWndTest2 = CreateWindow(szButt, "", Butt8Flag | WS_BORDER, SIZEX * 1, 0, SIZEX + 1, SIZEY, Global0, (HMENU)IDI_NEWDATA1, hInstance, nullptr);
        //SendMessage(hWndTest2, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)IDI_NEWDATA1);
        ////SetWindowLongPtr(hWndTest2, GWLP_WNDPROC, (LONG_PTR)WndProcButton);
        //SetWindowSubclass(hWndTest2, WndProcButton, 11, 0);
    //}

    InitGrafWindow(Global1);

    ShowWindow(Global0, SW_SHOW);
    ShowWindow(Global1, SW_SHOW);
    UpdateWindow(Global0);
    UpdateWindow(Global1);


    return;
}

//Инициализация класса главного окна
void MyRegisterClass()
{
    WNDCLASSEX cex0 ={0};
    cex0.cbSize = sizeof(WNDCLASSEX);
    cex0.style          = CS_HREDRAW | CS_VREDRAW;
    cex0.lpfnWndProc    = WndProc0;
    cex0.cbClsExtra     = 0;
    cex0.cbWndExtra     = 0;
    cex0.hInstance      = hInstance;
    cex0.hIcon          = NULL;// LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    cex0.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    cex0.hbrBackground  = TitleBrush2; // (HBRUSH)(CTLCOLOR_DLG + 1);
    cex0.lpszMenuName   = NULL;// MAKEINTRESOURCE(IDC_DATACOLLACTION);
    cex0.lpszClassName  = szWindowClass0;
    cex0.hIconSm        = LoadIcon(cex0.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    WNDCLASSEX cex1 ={0};
    cex1.cbSize = sizeof(WNDCLASSEX);
    cex1.style          = CS_HREDRAW | CS_VREDRAW;
    cex1.lpfnWndProc    = WndProc1;
    cex1.cbClsExtra     = 0;
    cex1.cbWndExtra     = 0;
    cex1.hInstance      = hInstance;
    cex1.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    cex1.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    cex1.hbrBackground  = TitleBrush2; // (HBRUSH)(CTLCOLOR_DLG + 1);
    cex1.lpszMenuName   = NULL;// MAKEINTRESOURCE(IDC_DATACOLLACTION);
    cex1.lpszClassName  = szWindowClass1;
    cex1.hIconSm        = LoadIcon(cex1.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    WNDCLASSEX cex2 ={0};
    cex2.cbSize = sizeof(WNDCLASSEX);
    cex2.style          = CS_HREDRAW | CS_VREDRAW;
    cex2.lpfnWndProc    = WndProcTemp1;
    cex2.cbClsExtra     = 0;
    cex2.cbWndExtra     = 0;
    cex2.hInstance      = hInstance;
    cex2.hIcon          = NULL;
    cex2.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    cex2.hbrBackground  = TitleBrush2;
    cex2.lpszMenuName   = NULL;
    cex2.lpszClassName  = szTem1;
    cex2.hIconSm        = NULL;

    WNDCLASSEX cex3 ={0};
    cex3.cbSize = sizeof(WNDCLASSEX);
    cex3.style          = CS_HREDRAW | CS_VREDRAW;
    cex3.lpfnWndProc    = WndProcTemp2;
    cex3.cbClsExtra     = 0;
    cex3.cbWndExtra     = 0;
    cex3.hInstance      = hInstance;
    cex3.hIcon          = NULL;
    cex3.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    cex3.hbrBackground  = TitleBrush2;
    cex3.lpszMenuName   = NULL;
    cex3.lpszClassName  = szTem2;
    cex3.hIconSm        = NULL;

    WNDCLASSEX cex4 ={0};
    cex4.cbSize = sizeof(WNDCLASSEX);
    //cex4.style          = CS_HREDRAW | CS_VREDRAW;
    //cex4.lpfnWndProc    = WndProcCalendar;
    //cex4.cbClsExtra     = 0;
    //cex4.cbWndExtra     = 0;
    //cex4.hInstance      = hInstance;
    //cex4.hIcon          = NULL;
    //cex4.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //cex4.hbrBackground  = TitleBrush2;
    //cex4.lpszMenuName   = NULL;
    //cex4.lpszClassName  = szMont;
    //cex4.hIconSm        = NULL;

    if(cex0.hInstance && !RegisterClassExA(&cex0))
        throw std::exception(__FUN(std::string("Ошибка регистрации класса окна cex0: " + std::string(szWindowClass0))));

    if(cex1.hInstance && !RegisterClassExA(&cex1))
        throw std::exception(__FUN(std::string("Ошибка регистрации класса окна cex1: " + std::string(szWindowClass1))));

    if(cex2.hInstance && !RegisterClassExA(&cex2))
        throw std::exception(__FUN(std::string("Ошибка регистрации класса окна cex2: " + std::string(szTem1))));

    if(cex3.hInstance && !RegisterClassExA(&cex3))
        throw std::exception(__FUN(std::string("Ошибка регистрации класса окна cex3: " + std::string(szTem2))));

    if(cex4.hInstance && !RegisterClassExA(&cex4))
        throw std::exception(__FUN(std::string("Ошибка регистрации класса окна cex4: " + std::string(szMont))));
}


extern std::deque<Value*> AllTagKpvl;
extern std::deque<Value*> AllTagPeth;
//Создание главного окна
bool InitWindow(HINSTANCE hInst)
{
    //Пример работы атрибута [nodiscard]
    //TestNodiscard();
    MyRegisterClass();
    InitInstance();
    SetWindowText(winmap(hEditDiagnose5), GetDataTimeString().c_str());
#ifndef TESTWIN
    for(auto& val : AllTagKpvl)
        MySetWindowText(winmap(val->winId), val->GetString().c_str());

    for(auto& val : AllTagPeth)
        MySetWindowText(winmap(val->winId), val->GetString().c_str());
#endif
    return true;
}

