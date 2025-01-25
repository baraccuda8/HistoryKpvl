#pragma once

namespace S107
{
    //typedef struct T_cass{
    //    int id = 0;
    //    std::string Run_at = "";
    //    std::string End_at = "";
    //    std::string Err_at = "";
    //    int Hour = 0;
    //    int Day = 0;
    //    int Month = 0;
    //    int Year = 0;
    //    int CassetteNo = 0;
    //};

    extern std::string URI;
    extern std::string ServerDataTime;

    namespace Coll{
        extern int Create_at;
        extern int Id;
        extern int Event;
        extern int Hour;
        extern int Day;
        extern int Month;
        extern int Year;
        extern int CassetteNo;
        extern int SheetInCassette;
        extern int Close_at;
        extern int Peth;
        extern int Run_at;
        extern int Error_at;
        extern int End_at;
        extern int Delete_at;
        extern int TempRef;           //Заданное значение температуры
        extern int PointTime_1;       //Время разгона
        extern int PointRef_1;        //Уставка температуры
        extern int TimeProcSet;       //Полное время процесса (уставка), мин
        extern int PointTime_2;      //Время выдержки
        extern int facttemper;          //Факт температуры за 5 минут до конца отпуска
        extern int Finish_a;
    };

#pragma region Функции с кассетами в базе
    //void GetColl(PGresult* res);
    void GetCassette(PGConnection& conn, PGresult* res, TCassette& cassette, int line);

    bool GetFinishCassete(PGConnection& conn, LOGGER Logger, TCassette& it);
    //bool IsCassette(TCassette& CD);
	template <class T>
	bool IsCassette(T& CD);
	Tcass& GetIgCassetteFurn(int Peth);
	T_ForBase_RelFurn& GetBaseRelFurn(int Peth);


    namespace SQL{

        void FURN_SQL(PGConnection& conn, std::deque<TCassette>& allCassette, LOGGER Logger);
        //bool isCasseteCant(T_CassetteData& Cassette, TCassette& CD);
        //bool isCasseteFurn(T_Fcassette& Cassette, TCassette& CD);
        //void GetIsPos(PGConnection& conn, TCassette& CD);
        //void SearthEnd_at(PGConnection& conn, TCassette& CD, int id);
        //bool GetCountSheet(PGConnection& conn, TCassette& CD);
    };


#pragma endregion

    //Печ отпуска #1
    namespace Furn1{
        extern const int nPetch;
		extern Tcass Petch;
        extern int CassetteId;
		extern T_ForBase_RelFurn& Furn;
		extern LOGGER Logger;
        DWORD Data_WDG_toBase(Value* value);

        //BOOL Работа
        DWORD ProcRun(Value* value);

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value);

        //BOOL Авария процесса
        DWORD ProcError(Value* value);

        //BOOL Возврат касеты в список
        DWORD ReturnCassetteCmd(Value* value);

        //Обнуление температуры для сравнения
        //DWORD SetNull_Temper(Value* value);

        DWORD Hour(Value* value);
        DWORD Day(Value* value);
        DWORD Month(Value* value);
        DWORD Year(Value* value);
        DWORD No(Value* value);

        DWORD TimeHeatAcc(Value* value);
        DWORD TimeHeatWait(Value* value);
        DWORD TimeTotal(Value* value);

        ////REAL Время до окончания процесса, мин
        DWORD TimeToProcEnd(Value* value);

        ////REAL Время разгона
        //DWORD PointTime_1(Value* value);
        //
        ////REAL Уставка температуры
        //DWORD PointRef_1(Value* value);
        //
        ////REAL Время выдержки
        //DWORD PointTime_2(Value* value);
        //
        ////REAL Полное время процесса (уставка), мин
        //DWORD TimeProcSet(Value* value);
        //
        ////REAL Время процесса, час (0..XX)
        //DWORD ProcTimeMin(Value* value);
        //
        ////REAL Заданное значение температуры
        //DWORD TempRef(Value* value);
        //
        ////REAL Фактическое значение температуры
        //DWORD TempAct(Value* value);
        //
        ////REAL Термопара 1
        //DWORD T1(Value* value);
        //
        ////REAL Термопара 2
        //DWORD T2(Value* value);
        //
        ////ID касеты
        //DWORD IDCassette(Value* value);
        //
        ////ID касеты месяц
        //DWORD Month(Value* value);
        //
        ////ID касеты день
        //DWORD Day(Value* value);
        //
        ////ID касеты номер
        //DWORD CassetteNo(Value* value);
        //
        ////Выбор
        //DWORD SelectedCassete(Value* value);

    }

    //Печ отпуска #2
    namespace Furn2{
        extern const int nPetch;
		extern Tcass Petch;
        extern int CassetteId;
		extern T_ForBase_RelFurn& Furn;
		extern LOGGER Logger;


        DWORD Data_WDG_toBase(Value* value);

        //BOOL Работа
        DWORD ProcRun(Value* value);

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value);

        //BOOL Авария процесса
        DWORD ProcError(Value* value);

        //BOOL Возврат касеты в список
        DWORD ReturnCassetteCmd(Value* value);

        //Обнуление температуры для сравнения
        //DWORD SetNull_Temper(Value* value);

        DWORD Hour(Value* value);
        DWORD Day(Value* value);
        DWORD Month(Value* value);
        DWORD Year(Value* value);
        DWORD No(Value* value);

        DWORD TimeHeatAcc(Value* value);
        DWORD TimeHeatWait(Value* value);
        DWORD TimeTotal(Value* value);

        ////REAL Время до окончания процесса, мин
        DWORD TimeToProcEnd(Value* value);

        ////REAL Время разгона
        //DWORD PointTime_1(Value* value);
        //
        ////REAL Уставка температуры
        //DWORD PointRef_1(Value* value);
        //
        ////REAL Время выдержки
        //DWORD PointTime_2(Value* value);
        //
        ////REAL Полное время процесса (уставка), мин
        //DWORD TimeProcSet(Value* value);
        //
        ////REAL Время процесса, час (0..XX)
        //DWORD ProcTimeMin(Value* value);
        //
        ////REAL Заданное значение температуры
        //DWORD TempRef(Value* value);
        //
        ////REAL Фактическое значение температуры
        //DWORD TempAct(Value* value);
        //
        ////REAL Термопара 1
        //DWORD T1(Value* value);
        //
        ////REAL Термопара 2
        //DWORD T2(Value* value);
        //
        ////ID касеты год
        //DWORD Year(Value* value);
        //
        ////ID касеты месяц
        //DWORD Month(Value* value);
        //
        ////ID касеты день
        //DWORD Day(Value* value);
        //
        ////ID касеты номер
        //DWORD CassetteNo(Value* value);
        //
        ////Выбор
        //DWORD SelectedCassete(Value* value);

    };

    //DWORD SelectedCassete(Value* value);
}


void SetUpdateCassete(PGConnection& conn, TCassette& cassette, std::string update, std::string where);

