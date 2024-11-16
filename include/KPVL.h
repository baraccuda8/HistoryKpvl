#pragma once

//Номер колонки в таблице листов
#pragma region //Номер колонки в таблице листов
extern int Col_Sheet_id;
extern int Col_Sheet_create_at;
extern int Col_Sheet_start_at;
extern int Col_Sheet_datatime_end;
extern int Col_Sheet_pos;
extern int Col_Sheet_datatime_all;
extern int Col_Sheet_alloy;
extern int Col_Sheet_thikness;
extern int Col_Sheet_melt;
extern int Col_Sheet_slab;
extern int Col_Sheet_partno;
extern int Col_Sheet_pack;
extern int Col_Sheet_sheet;
extern int Col_Sheet_subsheet;
extern int Col_Sheet_temper;
extern int Col_Sheet_speed;
extern int Col_Sheet_za_pt3;
extern int Col_Sheet_za_te3;
extern int Col_Sheet_lampresstop;
extern int Col_Sheet_lampressbot;
extern int Col_Sheet_posclapantop;
extern int Col_Sheet_posclapanbot;
extern int Col_Sheet_mask;
extern int Col_Sheet_lam1posclapantop;
extern int Col_Sheet_lam1posclapanbot;
extern int Col_Sheet_lam2posclapantop;
extern int Col_Sheet_lam2posclapanbot;
extern int Col_Sheet_lam_te1;
extern int Col_Sheet_news;
extern int Col_Sheet_top1;
extern int Col_Sheet_top2;
extern int Col_Sheet_top3;
extern int Col_Sheet_top4;
extern int Col_Sheet_top5;
extern int Col_Sheet_top6;
extern int Col_Sheet_top7;
extern int Col_Sheet_top8;
extern int Col_Sheet_bot1;
extern int Col_Sheet_bot2;
extern int Col_Sheet_bot3;
extern int Col_Sheet_bot4;
extern int Col_Sheet_bot5;
extern int Col_Sheet_bot6;
extern int Col_Sheet_bot7;
extern int Col_Sheet_bot8;
extern int Col_Sheet_day;
extern int Col_Sheet_month;
extern int Col_Sheet_year;
extern int Col_Sheet_cassetteno;
extern int Col_Sheet_sheetincassette;
extern int Col_Sheet_timeforplateheat;
extern int Col_Sheet_prestostartcomp;
extern int Col_Sheet_correct;
//};
#pragma endregion


//Печь закалки
namespace KPVL {
    extern std::string ServerDataTime;

    void UpdateCountSheet(PGConnection& conn, int id);

    namespace SQL
    {
        //Получаем список колонов в таблице sheet
        void GetCollumn(PGresult* res);
        //Чтение листов
        void GetSheet(PGConnection& conn, PGresult* res, std::deque<TSheet>& Sheet);


        //Получаем список листов из базы
        //void KPVL_SQL(PGConnection& conn);
        void KPVL_SQL(PGConnection& conn, std::deque<TSheet>& Sheet);
        void GetDataTime_All(PGConnection& conn, TSheet& TS);
    };

    namespace Sheet{
        //Получаем ID листа
        std::string GetIdSheet(PGConnection& conn, std::string sMelt, std::string sPack, std::string sPartNo, std::string sSheet, std::string sSubSheet, std::string sSlab);
        std::string GetIdSheet(PGConnection& conn, int32_t Melt, int32_t Pack, int32_t PartNo, int32_t Sheet, int32_t SubSheet, int32_t Slab);

        ////Обновляем в базе данные по листу
        void SetUpdateSheet(PGConnection& conn, T_PlateData& PD, std::string update, std::string where);
        void SetUpdateSheet(PGConnection& conn, TSheet& TS, std::string update, std::string where);

        //Зона 0 на входе в печь
        namespace  Z0{
            extern const int Pos;
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //Зона 1 зона печи
        namespace  Z1{
            extern const int Pos;
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        };

        //Зона 2 зона печи
        namespace Z2{
            extern const int Pos;

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //Зона 3 закалка
        namespace Z3{
            extern const int Pos;

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //Зона 4 Охлаждение
        namespace Z4{
            extern const int Pos;

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //Зона 5 Выход
        namespace Z5{
            extern const int Pos;

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //Зона 6 (кантовка)
        namespace Z6{
            extern const int Pos;

            DWORD DataTime(Value* value);

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);

            //Новые лист в касету, Кассета наполяентся
            DWORD NewSheetData(Value* value);

            void SetSaveDone(PGConnection& conn);

        }
    }

    namespace Cassette{

        //Проверка на наличие кассеты
        bool IsCassette(T_CassetteData& CD);

        //Получаем ID Кассеты
        int32_t GetIdCassette(PGConnection& conn, T_CassetteData& CD);

        //Обновляем данные по кассете если кассета есть или добовляем новую
        int32_t CassettePos(PGConnection& conn, T_CassetteData& CD);

        //Вывод Номер листа в касете
        DWORD Sheet_InCassette(Value* value);

        //Вывод Номер кассеты за день
        DWORD CassetteNo(Value* value);

        //Вывод Час ID листа
        DWORD CassetteHour(Value* value);

        //Вывод День ID листа
        DWORD CassetteDay(Value* value);

        //Вывод Месяц ID листа
        DWORD CassetteMonth(Value* value);

        //Вывод Год ID листа
        DWORD CassetteYear(Value* value);

        //кассета наполяентся
        DWORD CasseteIsFill(Value* value);
    }

    //Операция в зонах
    namespace ZState{

        ////5-ти секундная задержка на запись данных по листу во 2- зоне
        //DWORD WINAPI ThreadState2(LPVOID);
        //Операция в 1 зоне
        DWORD DataPosState_1(Value* value);

        //Операция в 2 зоне
        DWORD DataPosState_2(Value* value);

        //Операция в 3 зоне
        DWORD DataPosState_3(Value* value);
    }


    //Аналоги
    namespace An{
    //Уставка температуры
        DWORD TempSet1(Value* value);

        //Скорость выгрузки
        DWORD UnloadSpeed(Value* value);
        //Время сигнализации окончания нагрева, мин
        DWORD fTimeForPlateHeat(Value* value);
        //Уставка давления для запуска комперссора
        DWORD fPresToStartComp(Value* value);


        //Фиксируем по условию
        //state2=5
            //Давление воды закалка коллектор верх 
            //Давление в верхнем коллекторе 
            //через 5 сек после state2=5
        //DWORD LaminPressTop(Value* value);
        //
        //Давление воды закалка коллектор низ
        //Давление нижнем коллекторе
        //через 5 сек после state2=5
        //DWORD LaminPressBot(Value* value);
        //
        //Температура воды в поддоне
        //state2=5
        //DWORD LAM_TE1(Value* value);
        //
        //Температура воды в баке
        //state2=5
        //DWORD Za_TE3(Value* value);
        //
        //Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)
        //state2=5
        //DWORD Za_PT3(Value* value);
    };

    //Режим работы клапана
    //State_1 = 3 
    //Маска клапанов
    namespace Mask{
        DWORD DataMaskKlapan1(Value* value);
        DWORD DataMaskKlapan2(Value* value);
    }
    //Скоростная секция
    namespace Speed{
        //Верх
        DWORD SpeedSectionTop(Value* value);
        //Низ
        DWORD SpeedSectionBot(Value* value);
    }
    //Ламинарная секция 1
    namespace Lam1{
        //Верх
        DWORD LaminarSection1Top(Value* value);
        //Низ
        DWORD LaminarSection1Bot(Value* value);
    };
    //Ламинарная секция 2
    namespace Lam2{
        //Верх
        DWORD LaminarSection2Top(Value* value);
        //Низ
        DWORD LaminarSection2Bot(Value* value);
    };

    //Отклонения листа на кантовке
    DWORD Side(Value* value);

    DWORD Temperature(Value* value);

    //Битовый вачтог
    DWORD SheetData_WDG_toBase(Value* value);

}


extern std::deque<TSheet>AllSheet;


