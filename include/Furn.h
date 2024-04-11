#pragma once


namespace S107
{
    extern std::string URI;
    extern std::string ServerDataTime;

    namespace SQL{
        void FURN_SQL();
    };


#pragma region Функции с кассетами в базе
    bool IsCassete1(T_cassette& CD);
    bool IsCassete2(T_cassette& CD);

    void UpdateCassetteProcRun(T_ForBase_RelFurn& Furn, int Peth);
    void UpdateCassetteProcEnd(T_ForBase_RelFurn& Furn, int Peth);
    void UpdateCassetteProcError(T_ForBase_RelFurn& Furn, int Peth);
    void SetTemperCassette(T_cassette& CD, std::string teper);;

    void CloseAllCassette2(T_cassette& CD, int Peth);
#pragma endregion

    //Печ отпуска #1
    namespace Furn1{
        DWORD Data_WDG_toBase(Value* value);

        //BOOL Работа
        DWORD ProcRun(Value* value);

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value);

        //BOOL Авария процесса
        DWORD ProcError(Value* value);

        //Обнуление температуры для сравнения
        DWORD SetNull_Temper(Value* value);

        ////REAL Время до окончания процесса, мин
        //DWORD TimeToProcEnd(Value* value);

        ////REAL Время разгона
        //DWORD PointTime_1(Value* value);
        //
        ////REAL Уставка температуры
        //DWORD PointRef_1(Value* value);
        //
        ////REAL Время выдержки
        //DWORD PointDTime_2(Value* value);
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
        DWORD Data_WDG_toBase(Value* value);

        //BOOL Работа
        DWORD ProcRun(Value* value);

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value);

        //BOOL Авария процесса
        DWORD ProcError(Value* value);

        //Обнуление температуры для сравнения
        DWORD SetNull_Temper(Value* value);


        ////REAL Время до окончания процесса, мин
        //DWORD TimeToProcEnd(Value* value);

        ////REAL Время разгона
        //DWORD PointTime_1(Value* value);
        //
        ////REAL Уставка температуры
        //DWORD PointRef_1(Value* value);
        //
        ////REAL Время выдержки
        //DWORD PointDTime_2(Value* value);
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
