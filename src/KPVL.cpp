#include "pch.h"

#include "win.h"
#include "main.h"
#include "file.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "hard.h"
#include "term.h"
#include "Furn.h"
#include "KPVL.h"
#include "pdf.h"


//extern const std::string FORMATTIME;

extern std::shared_ptr<spdlog::logger> HardLogger;

std::map<int, std::string> GenSeq1 ={
    {0, "Отключено"},
    {1, "Подготовка"},
    {2, "Прогрев"},
    {3, "Открыть входную дверь"},
    {4, "Загрузка в печь"},
    {5, "Закрыть входную дверь"},
    {6, "Нагрев листа"},
    {7, "Передача на 2 рольганг"},
    {8, "Передача на 2-й рольганг печи"},
};
std::map<int, std::string> GenSeq2 ={
    {0, "Отключено"},
    {1, "Подготовка"},
    {2, "Прогрев"},
    {3, "Прием заготовки с 1-го рольганга печи"},
    {4, "Осциляция. Нагрев Листа"},
    {5, "Открыть выходную дверь"},
    {6, "Выдача в линию закалки"},
    {7, "Закрыть выходную дверь"},
};
std::map<int, std::string> GenSeq3 ={
    {0, "Отключено"},
    {1, "Ожидание листа"},
    {2, "Осциляция. Охл.листа."},
    {3, "Выдача заготовки"},
    {4, "Окончание цикла обработки"},
};


TSheet PalletSheet[7];

std::deque<TSheet>AllSheet;

//Собираем данные по 2 секции через 5 секунд после State_2 = 5
//Номер колонки в таблице листов
#pragma region //Номер колонки в таблице листов
        //enum{
int Col_Sheet_id = 0;
int Col_Sheet_create_at = 0;
int Col_Sheet_start_at = 0;
int Col_Sheet_datatime_end = 0;
int Col_Sheet_pos = 0;
int Col_Sheet_datatime_all = 0;
int Col_Sheet_alloy = 0;
int Col_Sheet_thikness = 0;
int Col_Sheet_melt = 0;
int Col_Sheet_slab = 0;
int Col_Sheet_partno = 0;
int Col_Sheet_pack = 0;
int Col_Sheet_sheet = 0;
int Col_Sheet_subsheet = 0;
int Col_Sheet_temper = 0;
int Col_Sheet_speed = 0;
int Col_Sheet_za_pt3 = 0;
int Col_Sheet_za_te3 = 0;
int Col_Sheet_lampresstop = 0;
int Col_Sheet_lampressbot = 0;
int Col_Sheet_posclapantop  = 0;
int Col_Sheet_posclapanbot = 0;
int Col_Sheet_mask = 0;
int Col_Sheet_lam1posclapantop = 0;
int Col_Sheet_lam1posclapanbot = 0;
int Col_Sheet_lam2posclapantop = 0;
int Col_Sheet_lam2posclapanbot = 0;
int Col_Sheet_lam_te1 = 0;
int Col_Sheet_news = 0;
int Col_Sheet_top1 = 0;
int Col_Sheet_top2 = 0;
int Col_Sheet_top3 = 0;
int Col_Sheet_top4 = 0;
int Col_Sheet_top5 = 0;
int Col_Sheet_top6 = 0;
int Col_Sheet_top7 = 0;
int Col_Sheet_top8 = 0;
int Col_Sheet_bot1 = 0;
int Col_Sheet_bot2 = 0;
int Col_Sheet_bot3 = 0;
int Col_Sheet_bot4 = 0;
int Col_Sheet_bot5 = 0;
int Col_Sheet_bot6 = 0;
int Col_Sheet_bot7 = 0;
int Col_Sheet_bot8 = 0;
int Col_Sheet_day = 0;
int Col_Sheet_month = 0;
int Col_Sheet_year = 0;
int Col_Sheet_cassetteno = 0;
int Col_Sheet_sheetincassette = 0;
int Col_Sheet_timeforplateheat = 0;
int Col_Sheet_prestostartcomp = 0;
int Col_Sheet_temperature = 0;
int Col_Sheet_correct = 0;
int Col_Sheet_SecondPos_at = 0;
int Col_Sheet_hour = 0;
int Col_Sheet_delete_at = 0;
//};
#pragma endregion

//Печь закалки
namespace KPVL {

    //Список последних 100 листов из базы
    namespace SQL
    {


        //Получаем список колонов в таблице sheet
        void GetCollumn(PGresult* res)
        {
            if(!Col_Sheet_delete_at)
            {
                int nFields = PQnfields(res);
                for(int j = 0; j < nFields; j++)
                {
                    std::string l =  utf8_to_cp1251(PQfname(res, j));
                    if(l == "id") Col_Sheet_id = j;
                    else if(l == "create_at") Col_Sheet_create_at = j;
                    else if(l == "start_at") Col_Sheet_start_at = j;
                    else if(l == "datatime_end") Col_Sheet_datatime_end = j;
                    else if(l == "pos") Col_Sheet_pos = j;
                    else if(l == "datatime_all") Col_Sheet_datatime_all = j;
                    else if(l == "alloy") Col_Sheet_alloy = j;
                    else if(l == "thikness") Col_Sheet_thikness = j;
                    else if(l == "melt") Col_Sheet_melt = j;
                    else if(l == "slab") Col_Sheet_slab = j;
                    else if(l == "partno") Col_Sheet_partno = j;
                    else if(l == "pack") Col_Sheet_pack = j;
                    else if(l == "sheet") Col_Sheet_sheet = j;
                    else if(l == "subsheet") Col_Sheet_subsheet = j;
                    else if(l == "temper") Col_Sheet_temper = j;
                    else if(l == "speed") Col_Sheet_speed = j;
                    else if(l == "za_pt3") Col_Sheet_za_pt3 = j;
                    else if(l == "za_te3") Col_Sheet_za_te3 = j;
                    else if(l == "lampresstop") Col_Sheet_lampresstop = j;
                    else if(l == "lampressbot") Col_Sheet_lampressbot = j;
                    else if(l == "posclapantop") Col_Sheet_posclapantop = j;
                    else if(l == "posclapanbot") Col_Sheet_posclapanbot = j;
                    else if(l == "mask") Col_Sheet_mask = j;
                    else if(l == "lam1posclapantop") Col_Sheet_lam1posclapantop = j;
                    else if(l == "lam1posclapanbot") Col_Sheet_lam1posclapanbot = j;
                    else if(l == "lam2posclapantop") Col_Sheet_lam2posclapantop = j;
                    else if(l == "lam2posclapanbot") Col_Sheet_lam2posclapanbot = j;
                    else if(l == "lam_te1") Col_Sheet_lam_te1 = j;
                    else if(l == "news") Col_Sheet_news = j;
                    else if(l == "top1") Col_Sheet_top1 = j;
                    else if(l == "top2") Col_Sheet_top2 = j;
                    else if(l == "top3") Col_Sheet_top3 = j;
                    else if(l == "top4") Col_Sheet_top4 = j;
                    else if(l == "top5") Col_Sheet_top5 = j;
                    else if(l == "top6") Col_Sheet_top6 = j;
                    else if(l == "top7") Col_Sheet_top7 = j;
                    else if(l == "top8") Col_Sheet_top8 = j;
                    else if(l == "bot1") Col_Sheet_bot1 = j;
                    else if(l == "bot2") Col_Sheet_bot2 = j;
                    else if(l == "bot3") Col_Sheet_bot3 = j;
                    else if(l == "bot4") Col_Sheet_bot4 = j;
                    else if(l == "bot5") Col_Sheet_bot5 = j;
                    else if(l == "bot6") Col_Sheet_bot6 = j;
                    else if(l == "bot7") Col_Sheet_bot7 = j;
                    else if(l == "bot8") Col_Sheet_bot8 = j;
                    else if(l == "day") Col_Sheet_day = j;
                    else if(l == "month") Col_Sheet_month = j;
                    else if(l == "year") Col_Sheet_year = j;
                    else if(l == "hour") Col_Sheet_hour = j;
                    else if(l == "cassetteno") Col_Sheet_cassetteno = j;
                    else if(l == "sheetincassette") Col_Sheet_sheetincassette = j;
                    else if(l == "timeforplateheat") Col_Sheet_timeforplateheat = j;
                    else if(l == "prestostartcomp") Col_Sheet_prestostartcomp = j;
                    else if(l == "temperature") Col_Sheet_temperature = j;
                    else if(l == "correct") Col_Sheet_correct = j;
                    else if(l == "secondpos_at") Col_Sheet_SecondPos_at = j;
                    else if(l == "delete_at") Col_Sheet_delete_at = j;
                    
                    
                    
                }
            }
        }

                //Чтение листов
        void GetSheet(PGConnection& conn, PGresult* res, std::deque<TSheet>& Sheet)
        {
            int line = PQntuples(res);
            for(int l = 0; l < line; l++)
            {
                TSheet sheet;
                sheet.DataTime = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_create_at));
                sheet.Pos = conn.PGgetvalue(res, l, Col_Sheet_pos);
                sheet.id = conn.PGgetvalue(res, l, Col_Sheet_id);
                sheet.DataTime_End = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_datatime_end));
                sheet.DataTime_All = conn.PGgetvalue(res, l, Col_Sheet_datatime_all);
                sheet.Alloy = conn.PGgetvalue(res, l, Col_Sheet_alloy);
                sheet.Thikness = conn.PGgetvalue(res, l, Col_Sheet_thikness);
                sheet.Melt = conn.PGgetvalue(res, l, Col_Sheet_melt);
                sheet.Slab = conn.PGgetvalue(res, l, Col_Sheet_slab);
                sheet.PartNo = conn.PGgetvalue(res, l, Col_Sheet_partno);
                sheet.Pack = conn.PGgetvalue(res, l, Col_Sheet_pack);
                sheet.Sheet = conn.PGgetvalue(res, l, Col_Sheet_sheet);
                sheet.SubSheet = conn.PGgetvalue(res, l, Col_Sheet_subsheet);
                sheet.Temper = conn.PGgetvalue(res, l, Col_Sheet_temper);
                sheet.Speed = conn.PGgetvalue(res, l, Col_Sheet_speed);

                sheet.Za_PT3 = conn.PGgetvalue(res, l, Col_Sheet_za_pt3);
                sheet.Za_TE3 = conn.PGgetvalue(res, l, Col_Sheet_za_te3);

                sheet.LaminPressTop = conn.PGgetvalue(res, l, Col_Sheet_lampresstop);
                sheet.LaminPressBot = conn.PGgetvalue(res, l, Col_Sheet_lampressbot);
                sheet.PosClapanTop = conn.PGgetvalue(res, l, Col_Sheet_posclapantop);
                sheet.PosClapanBot = conn.PGgetvalue(res, l, Col_Sheet_posclapanbot);
                sheet.Mask = conn.PGgetvalue(res, l, Col_Sheet_mask);

                sheet.Lam1PosClapanTop = conn.PGgetvalue(res, l, Col_Sheet_lam1posclapantop);
                sheet.Lam1PosClapanBot = conn.PGgetvalue(res, l, Col_Sheet_lam1posclapanbot);
                sheet.Lam2PosClapanTop = conn.PGgetvalue(res, l, Col_Sheet_lam2posclapantop);
                sheet.Lam2PosClapanBot = conn.PGgetvalue(res, l, Col_Sheet_lam2posclapanbot);

                sheet.LAM_TE1 = conn.PGgetvalue(res, l, Col_Sheet_lam_te1);
                sheet.News = conn.PGgetvalue(res, l, Col_Sheet_news);
                sheet.Top1 = conn.PGgetvalue(res, l, Col_Sheet_top1);
                sheet.Top2 = conn.PGgetvalue(res, l, Col_Sheet_top2);
                sheet.Top3 = conn.PGgetvalue(res, l, Col_Sheet_top3);
                sheet.Top4 = conn.PGgetvalue(res, l, Col_Sheet_top4);
                sheet.Top5 = conn.PGgetvalue(res, l, Col_Sheet_top5);
                sheet.Top6 = conn.PGgetvalue(res, l, Col_Sheet_top6);
                sheet.Top7 = conn.PGgetvalue(res, l, Col_Sheet_top7);
                sheet.Top8 = conn.PGgetvalue(res, l, Col_Sheet_top8);

                sheet.Bot1 = conn.PGgetvalue(res, l, Col_Sheet_bot1);
                sheet.Bot2 = conn.PGgetvalue(res, l, Col_Sheet_bot2);
                sheet.Bot3 = conn.PGgetvalue(res, l, Col_Sheet_bot3);
                sheet.Bot4 = conn.PGgetvalue(res, l, Col_Sheet_bot4);
                sheet.Bot5 = conn.PGgetvalue(res, l, Col_Sheet_bot5);
                sheet.Bot6 = conn.PGgetvalue(res, l, Col_Sheet_bot6);
                sheet.Bot7 = conn.PGgetvalue(res, l, Col_Sheet_bot7);
                sheet.Bot8 = conn.PGgetvalue(res, l, Col_Sheet_bot8);

                sheet.Hour = conn.PGgetvalue(res, l, Col_Sheet_hour);
                sheet.Day = conn.PGgetvalue(res, l, Col_Sheet_day);
                sheet.Month = conn.PGgetvalue(res, l, Col_Sheet_month);
                sheet.Year = conn.PGgetvalue(res, l, Col_Sheet_year);
                sheet.CassetteNo = conn.PGgetvalue(res, l, Col_Sheet_cassetteno);
                sheet.SheetInCassette = conn.PGgetvalue(res, l, Col_Sheet_sheetincassette);

                sheet.Start_at = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_start_at));
                sheet.TimeForPlateHeat = conn.PGgetvalue(res, l, Col_Sheet_timeforplateheat);
                sheet.PresToStartComp = conn.PGgetvalue(res, l, Col_Sheet_prestostartcomp);
                sheet.Temperature = conn.PGgetvalue(res, l, Col_Sheet_temperature);
                sheet.Correct = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_correct));
                sheet.SecondPos_at = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_SecondPos_at));
                sheet.Delete_at = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_delete_at));
                Sheet.push_back(sheet);
            }
        }

        //Получаем список листов из базы
        void KPVL_SQL(PGConnection& conn, std::deque<TSheet>& Sheet)
        {
            Sheet.erase(Sheet.begin(), Sheet.end());

            std::time_t stop = time(NULL);
            std::time_t statr = static_cast<std::time_t>(difftime(stop, 60 * 60 * 24 * 10)); //7-е суток
            std::string start_at = GetDataTimeString(&statr);

            std::stringstream FilterComand;
            FilterComand << "SELECT * FROM sheet ";
            FilterComand << "WHERE create_at > '" << start_at << "'"; //delete_at IS NULL AND 
            //FilterComand << " AND id <= " << sStopId;
            FilterComand << " ORDER BY  create_at DESC, pos DESC, start_at DESC;";
            //FilterSheet();
            //bFilterData = TRUE;
            std::string comand = FilterComand.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                GetCollumn(res);
                GetSheet(conn, res, Sheet);

            }
            else
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);

            //AddHistoriSheet(true, (int)sheet.size());
            int t = 0;
        }

        float GetHeatTime_Z2(PGConnection& conn, std::string enddata_at)
        {
            float out = 0;
            std::string sout = "";
            //std::string next_at = "";
            //if(enddata_at.length())
            //{
            //    std::stringstream co;
            //    co << "SELECT max(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at <= '" << enddata_at << "';";
            //    std::string comand = co.str();
            //    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            //    PGresult* res = conn.PGexec(comand);
            //    if(PQresultStatus(res) == PGRES_TUPLES_OK)
            //    {
            //        if(PQntuples(res))
            //            next_at = conn.PGgetvalue(res, 0, 0);
            //    }
            //    else
            //        LOG_ERR_SQL(SQLLogger, res, comand);
            //    PQclear(res);
            //}

            //if(next_at.length())
            {
                std::stringstream co;
                //co << "SELECT content FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at = '" << next_at << "';";
                co << "SELECT content FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at <= '" << enddata_at << "' ORDER BY id DESC LIMIT 1";
                std::string comand = co.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))
                        sout = conn.PGgetvalue(res, 0, 0);
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }

            if(sout.length())
                out = Stof(sout);
            return out;
        }

        void GetDataTime_All(PGConnection& conn, TSheet& TS)
        {

            if(Stoi(TS.Pos) < 3)return;
            if(!TS.Start_at.length())
            {
                std::stringstream sd;
                sd << "SELECT start_at FROM sheet WHERE id = " << TS.id;
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                PGresult* res = conn.PGexec(sd.str());
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))
                        TS.Start_at = GetStringData(conn.PGgetvalue(res, 0, 0));
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, sd.str());
                PQclear(res);
            }

            if(!TS.Start_at.length())
            {
                std::stringstream sd;
                sd << " start_at = (SELECT MAX(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_1_StateNo->ID << " AND create_at <= '" << TS.DataTime << "' AND content = '3')";
                std::string update = sd.str();

                Sheet::SetUpdateSheet(conn, TS, update, ""); // " start_at IS NULL AND ");

                std::stringstream sa;
                sa << "SELECT MAX(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_1_StateNo->ID << " AND create_at <= '" << TS.DataTime << "' AND content = '3'";

                std::string comand = sa.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))
                        TS.Start_at = GetStringData(conn.PGgetvalue(res, 0, 0));
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }

            if(Stoi(TS.DataTime_All) == 0 || !TS.DataTime_End.length())
            {
                //TS.Start_at
                std::string next_at = "";
                std::string enddata_at = "";
                std::string HeatTime_Z2 = "";


                if(TS.Start_at.length())
                {
                    std::stringstream co;
                    co << "SELECT min(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_2_StateNo->ID << " AND content = '3' AND create_at > '" << TS.Start_at << "';"; //GenSeqToHmi.Data.Seq_2_StateNo Номер шага последовательности выгрузки в печи
                    std::string comand = co.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    PGresult* res = conn.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            next_at = conn.PGgetvalue(res, 0, 0);
                    }
                    else
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }

                if(next_at.length())
                {
                    std::stringstream co;
                    co << "SELECT min(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_1_StateNo->ID << " AND content = '5' AND create_at > '" << next_at << "';"; //GenSeqToHmi.Data.Seq_2_StateNo Номер шага последовательности выгрузки в печи
                    std::string comand = co.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    PGresult* res = conn.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            enddata_at = conn.PGgetvalue(res, 0, 0);
                    }
                    else
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
                if(enddata_at.length())
                {
                    float HeatTime_Z2 = GetHeatTime_Z2(conn, enddata_at);

                    std::string Id = Sheet::GetIdSheet(conn, TS.Melt, TS.Pack, TS.PartNo, TS.Sheet, TS.SubSheet, TS.Slab);

                    std::stringstream co;
                    co << "UPDATE sheet SET datatime_end = '" << enddata_at << "', datatime_all = " << HeatTime_Z2 << " WHERE delete_at IS NULL AND id = " << Id;
                    std::string comand = co.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    PGresult* res = conn.PGexec(comand);
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }

                int tt = 0;
                //sMaxId << TS.id;
            }
        }

    }


    std::string ServerDataTime = "";

    //ШВ листа в локальных данных по листу
    //uint32_t NextID = 1;

//Функции обработки и со сохранения истории листов
#pragma region Функции обработки и со сохранения истории листов
    //Локальные данные по листу
    namespace Sheet{

        //Проверка на наличие листа
        bool IsSheet(T_PlateData& PD)
        {
            //PD.Melt->GetValue();
            //PD.Pack->GetValue();
            //PD.PartNo->GetValue();
            //PD.Sheet->GetValue();

            int32_t Melt = PD.Melt->GetInt();
            int32_t Pack = PD.Pack->GetInt();
            int32_t PartNo = PD.PartNo->GetInt();
            int32_t Sheet = PD.Sheet->GetInt();
            //int32_t SubSheet = PD.SubSheet->GetInt();
            //int32_t Slab = PD.Slab->GetInt();
            return Melt && Pack && PartNo && Sheet/* && SubSheet*/;
        }
        bool IsSheet(TSheet& PS)
        {
            int32_t Melt = Stoi(PS.Melt);
            int32_t Pack = Stoi(PS.Pack);
            int32_t PartNo = Stoi(PS.PartNo);
            int32_t Sheet = Stoi(PS.Sheet);
            //int32_t SubSheet = PS.SubSheet;
            //int32_t Slab = PS.Slab;
            return Melt && Pack && PartNo && Sheet/* && SubSheet*/;
        }

                //Получаем ID листа
        std::string GetIdSheet(PGConnection& conn, T_PlateData& PD)
        {
            std::string id = "0";

            if(IsSheet(PD))
            {
                PD.SubSheet->GetValue();
                PD.Slab->GetValue();

                std::stringstream sd;
                sd << "SELECT id FROM sheet WHERE ";
                sd << " melt = " << PD.Melt->GetInt();
                sd << " AND pack = " << PD.Pack->GetInt();
                sd << " AND partno = " << PD.PartNo->GetInt();
                sd << " AND sheet = " << PD.Sheet->GetInt();
                sd << " AND subsheet = " << PD.SubSheet->GetInt();
                sd << " AND slab = " << PD.Slab->GetInt();
                std::string comand = sd.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                PGresult* res = conn.PGexec(comand);

                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))//Линий
                    {
                        id = conn.PGgetvalue(res, 0, 0);
                    }
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
            return id;
        }


        void LocSheet(PGConnection& conn, T_PlateData& PD, int Pos)
        {
            if(IsSheet(PD))
            {
                std::string id = GetIdSheet(conn, PD);
                PalletSheet[Pos].id = id;
                PalletSheet[Pos].DataTime = GetDataTimeString();
                PalletSheet[Pos].Pos = std::to_string(Pos);
                PalletSheet[Pos].Alloy = PD.AlloyCodeText->GetString();
                PalletSheet[Pos].Thikness = PD.ThiknessText->GetString();
                PalletSheet[Pos].Melt = PD.Melt->GetString();
                PalletSheet[Pos].Slab = PD.Slab->GetString();
                PalletSheet[Pos].PartNo = PD.PartNo->GetString();
                PalletSheet[Pos].Pack = PD.Pack->GetString();
                PalletSheet[Pos].Sheet = PD.Sheet->GetString();
                PalletSheet[Pos].SubSheet = PD.SubSheet->GetString();
            }
            else
            {
                PalletSheet[Pos].Clear();
            }
        }

        //Получаем ID листа
        std::string GetIdSheet(PGConnection& conn, std::string sMelt, std::string sPack, std::string sPartNo, std::string sSheet, std::string sSubSheet, std::string sSlab)
        {
            std::string id = "0";

            int32_t Melt = Stoi(sMelt);
            int32_t Pack = Stoi(sPack);
            int32_t PartNo = Stoi(sPartNo);
            int32_t Sheet = Stoi(sSheet);
            int32_t SubSheet = Stoi(sSubSheet);
            int32_t Slab = Stoi(sSlab);

            if(Melt && Pack && PartNo && Sheet /*&& SubSheet && Slab*/)
            {
                std::stringstream co;
                co << "SELECT id FROM sheet WHERE delete_at IS NULL AND ";
                co << " melt = " << Melt;
                co << " AND pack = " << Pack;
                co << " AND partno = " << PartNo;
                co << " AND sheet = " << Sheet;
                co << " AND subsheet = " << SubSheet;
                co << " AND slab = " << Slab;
                co << ";";
                std::string comand = co.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                PGresult* res = conn.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
                    id = conn.PGgetvalue(res, 0, 0);
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
            return id;
        }

                //Получаем ID листа
        std::string GetIdSheet(PGConnection& conn, int32_t Melt, int32_t Pack, int32_t PartNo, int32_t Sheet, int32_t SubSheet, int32_t Slab)
        {
            std::string id = "0";

            if(Melt && Pack && PartNo && Sheet /*&& SubSheet && Slab*/)
            {
                std::stringstream co;
                co << "SELECT id FROM sheet WHERE delete_at IS NULL AND ";
                co << " melt = " << Melt;
                co << " AND pack = " << Pack;
                co << " AND partno = " << PartNo;
                co << " AND sheet = " << Sheet;
                co << " AND subsheet = " << SubSheet;
                co << " AND slab = " << Slab;
                co << ";";
                std::string comand = co.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
                    id = conn.PGgetvalue(res, 0, 0);
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
            return id;
        }

        //Добовление листа в базу
        void InsertSheet(PGConnection& conn, T_PlateData& PD, int Pos)
        {
            PD.Melt->GetValue();
            PD.PartNo->GetValue();
            PD.Pack->GetValue();
            PD.Slab->GetValue();
            PD.Sheet->GetValue();
            PD.SubSheet->GetValue();
            PD.AlloyCodeText->GetValue();
            PD.ThiknessText->GetValue();

            if(IsSheet(PD))
            {
                std::string id = GetIdSheet(conn, PD);
                if(!Stoi(id)) //!id.length() || id == "" || id == "0")
                {
                    std::stringstream sd;
                    sd << "INSERT INTO sheet ";
                    sd << "(";
                    sd << "start_at, ";
#pragma region MyRegion
                    sd << "alloy, ";
                    sd << "thikness, ";
                    sd << "melt, ";
                    sd << "slab, ";
                    sd << "partno, ";
                    sd << "pack, ";
                    sd << "sheet, ";
                    sd << "subsheet, ";
#pragma endregion

#pragma region MyRegion
                    if(Pos == 1 || Pos == 2)
                    {
                        sd << "temper, ";
                        sd << "speed, ";

                        sd << "timeforplateheat, ";
                        sd << "prestostartcomp, ";

                        sd << "posclapantop, ";
                        sd << "posclapanbot, ";

                        sd << "lam1posclapantop, ";
                        sd << "lam1posclapanbot, ";

                        sd << "lam2posclapantop, ";
                        sd << "lam2posclapanbot, ";

                        sd << "mask, ";
                    }
#pragma endregion

#pragma region MyRegion
                    sd << " pos";
                    sd << ") VALUES (now(), ";
                    sd << "'" << PD.AlloyCodeText->GetString() << "', ";
                    sd << "'" << PD.ThiknessText->GetString() << "', ";

                    sd << PD.Melt->GetInt() << ", ";
                    sd << PD.Slab->GetInt() << ", ";
                    sd << PD.PartNo->GetInt() << ", ";
                    sd << PD.Pack->GetInt() << ", ";
                    sd << PD.Sheet->GetInt() << ", ";
                    sd << PD.SubSheet->GetInt() << ", ";
#pragma endregion

#pragma region MyRegion
                    if(Pos == 1 || Pos == 2)
                    {
                        GenSeqFromHmi.TempSet1->GetValue();
                        Par_Gen.UnloadSpeed->GetValue();
                        Par_Gen.TimeForPlateHeat->GetValue();
                        Par_Gen.PresToStartComp->GetValue();
                        HMISheetData.SpeedSection.Top->GetValue();
                        HMISheetData.SpeedSection.Bot->GetValue();
                        HMISheetData.LaminarSection1.Top->GetValue();
                        HMISheetData.LaminarSection1.Bot->GetValue();
                        HMISheetData.LaminarSection2.Top->GetValue();
                        HMISheetData.LaminarSection2.Bot->GetValue();

                        sd << GenSeqFromHmi.TempSet1->GetFloat() << ", ";
                        sd << Par_Gen.UnloadSpeed->GetFloat() << ", ";

                        sd << Par_Gen.TimeForPlateHeat->GetFloat() << ", ";
                        sd << Par_Gen.PresToStartComp->GetFloat() << ", ";

                        sd << HMISheetData.SpeedSection.Top->GetFloat() << ", ";
                        sd << HMISheetData.SpeedSection.Bot->GetFloat() << ", ";

                        sd << HMISheetData.LaminarSection1.Top->GetFloat() << ", ";
                        sd << HMISheetData.LaminarSection1.Bot->GetFloat() << ", ";

                        sd << HMISheetData.LaminarSection2.Top->GetFloat() << ", ";
                        sd << HMISheetData.LaminarSection2.Bot->GetFloat() << ", ";

                        sd << "'" + MaskKlapan + "', ";
                    }
#pragma endregion

                    sd << Pos << ");";

                    LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                    SETUPDATESQL(HardLogger, conn, sd);
                    //std::string comand = sd.str();
                    //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    //PGresult* res = conn.PGexec(comand);
                    ////LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    //
                    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    //    LOG_ERR_SQL(SQLLogger, res, comand);
                    //PQclear(res);
                }
            }
        }


        //Обновляем в базе данные по листу
        void SetUpdateSheet(PGConnection& conn, TSheet& TS, std::string update, std::string where)
        {
            bool ret = true;
            if(IsSheet(TS))
            {
                std::stringstream sd;
                sd << "UPDATE sheet SET" << update;
                sd << " WHERE" << where;
                sd << " melt = " << TS.Melt;
                sd << " AND partno = " << TS.PartNo;
                sd << " AND pack = " << TS.Pack;
                sd << " AND sheet = " << TS.Sheet;
                sd << " AND subsheet = " << TS.SubSheet;
                sd << " AND slab = " << TS.Slab;
                sd << ";";
                SETUPDATESQL(SQLLogger, conn, sd);

                //std::string comand = sd.str();
                //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                //PGresult* res = conn.PGexec(comand);
                ////LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                //
                //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                //{
                //    LOG_ERR_SQL(SQLLogger, res, comand);
                //}
                //else
                //{
                //    ret = true;
                //}
                //PQclear(res);
            }
            //return ret;
        }

        //Обновлякем в базе данные по листу
        void SetUpdateSheet(PGConnection& conn, T_PlateData& PD, std::string update, std::string where)
        {
            bool ret = true;
            if(IsSheet(PD))
            {
                std::stringstream sd;
                sd << "UPDATE sheet SET" << update;
                sd << " WHERE" << where;
                sd << " melt = " << PD.Melt->GetInt();
                sd << " AND partno = " << PD.PartNo->GetInt();
                sd << " AND pack = " << PD.Pack->GetInt();
                sd << " AND sheet = " << PD.Sheet->GetInt();
                sd << " AND subsheet = " << PD.SubSheet->GetInt();
                sd << " AND slab = " << PD.Slab->GetInt();
                sd << ";";
                SETUPDATESQL(SQLLogger, conn, sd);

                //std::string comand = sd.str();
                //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                //PGresult* res = conn.PGexec(comand);
                ////LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                //
                //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                //{
                //    LOG_ERR_SQL(SQLLogger, res, comand);
                //}
                //else
                //{
                //    ret = true;
                //}
                //PQclear(res);
            }
            //return ret;
        }


        //Потеряный на кантовке
        void UpdateCant(PGConnection& conn, int id)
        {
            LOG_INFO(HardLogger, "{:90}| SheetId={}, потерян на кантовке", FUNCTION_LINE_NAME, id);

            std::stringstream sd;
            sd << "UPDATE sheet SET pos = 16 WHERE pos = 6 AND id <> " << id;
            SETUPDATESQL(SQLLogger, conn, sd);
        }
        //Установка позиции листа
        void UpdateSQLPos(PGConnection& conn, int id, int pos, int Pos)
        {
            LOG_INFO(HardLogger, "{:90}| SheetId={}, OldPos={} --> NewPos={}", FUNCTION_LINE_NAME, id, pos, Pos);

            std::stringstream sd;
            sd << "UPDATE sheet SET pos = " << Pos;
            sd << " WHERE id = " << id;
            SETUPDATESQL(SQLLogger, conn, sd);
        }
        //Обновляем позицию листа
        void UpdateSheetPos(PGConnection& conn, T_PlateData& PD, std::string sid, int Pos)
        {
            if(IsSheet(PD))
            {
                int pos = 0;
                int id = Stoi(sid);
                if(id)
                {
                    std::stringstream se;
                    se << "SELECT pos FROM sheet WHERE id = " << id;
                    std::string comand = se.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    PGresult* res = conn.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
                        pos = Stoi(conn.PGgetvalue(res, 0, 0));
                    else
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);

                    if(Pos == 6)UpdateCant(conn, id);

                    if(pos != Pos && pos > 0 && pos < 7)
                        UpdateSQLPos(conn, id, pos, Pos);
                }
            }
        };

        //Обновляем данные по листу если лист есть или добовляем новый
        void SheetPos(PGConnection& conn, T_PlateData& PD, int pos)
        {
            std::string id = GetIdSheet(conn, PD);

            if(Stoi(id) != 0)
            {
                UpdateSheetPos(conn, PD, id, pos);

                if(pos == 1 || pos == 2)
                {
                    //std::stringstream sAlloy;
                    //sAlloy << " alloy = '" << PD.AlloyCodeText->GetString() << "'";
                    //SetUpdateSheet(&conn, PD, sAlloy.str(), " alloy = '' AND");
                    SetUpdateSheet(conn, PD, " alloy = '" + PD.AlloyCodeText->GetString() + "'", " thikness = '' AND");

                    //std::stringstream sThikness;
                    //sThikness << " thikness = '" + PD.ThiknessText->GetString() + "'";
                    //SetUpdateSheet(&conn, PD, sThikness.str(), " thikness = '' AND");
                    SetUpdateSheet(conn, PD, " thikness = '" + PD.ThiknessText->GetString() + "'", " thikness = '' AND");

                    SetUpdateSheet(conn, PD, " temper = " + GenSeqFromHmi.TempSet1->GetString(), " temper = 0 AND");
                    SetUpdateSheet(conn, PD, " speed = " + Par_Gen.UnloadSpeed->GetString(), " speed = 0 AND");

                    SetUpdateSheet(conn, PD, " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString(), "");
                    SetUpdateSheet(conn, PD, " prestostartcomp = " + Par_Gen.PresToStartComp->GetString(), "");

                    SetUpdateSheet(conn, PD, " posclapantop = " + HMISheetData.SpeedSection.Top->GetString(), " posclapantop = 0 AND");
                    SetUpdateSheet(conn, PD, " posclapanbot = " + HMISheetData.SpeedSection.Bot->GetString(), " posclapanbot = 0 AND");
                    SetUpdateSheet(conn, PD, " mask = '" + MaskKlapan + "' ", " mask = '' AND");

                    SetUpdateSheet(conn, PD, " lam1posclapantop = " + HMISheetData.LaminarSection1.Top->GetString(), " lam1posclapantop = 0 AND");
                    SetUpdateSheet(conn, PD, " lam1posclapanbot = " + HMISheetData.LaminarSection1.Bot->GetString(), " lam1posclapanbot = 0 AND");
                    SetUpdateSheet(conn, PD, " lam2posclapantop = " + HMISheetData.LaminarSection2.Top->GetString(), " lam2posclapantop = 0 AND");
                    SetUpdateSheet(conn, PD, " lam2posclapanbot = " + HMISheetData.LaminarSection2.Bot->GetString(), " lam2posclapanbot = 0 AND");
                }
            }
            else
            {
                InsertSheet(conn, PD, pos);
            }
        }

        void DeleteNullSgeet(PGConnection& conn, T_PlateData& PD, int Pos)
        {
            TSheet& TS = PalletSheet[Pos];
            if(Pos != 5 && IsSheet(PD) && IsSheet(TS))
            {
                if(PD.Melt->GetInt() != Stoi(TS.Melt)
                   || PD.Pack->GetInt() != Stoi(TS.Pack)
                   || PD.PartNo->GetInt() != Stoi(TS.PartNo)
                   || PD.Sheet->GetInt() != Stoi(TS.Sheet)
                   || PD.SubSheet->GetInt() != Stoi(TS.SubSheet)
                   || PD.Slab->GetInt() != Stoi(TS.Slab)
                   )
                {
                    //std::string sId = GetIdSheet(PD);
                    std::string sId = GetIdSheet(conn, TS.Melt, TS.Pack, TS.PartNo, TS.Sheet, TS.SubSheet, TS.Slab);
                    if(atoi(sId.c_str()))
                    {
                        int iPos = -1;
                        std::string comand = "SELECT pos FROM sheet WHERE id = " + sId;
                        if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                        PGresult* res = conn.PGexec(comand);
                        if(PQresultStatus(res) == PGRES_TUPLES_OK)
                        {
                            if(PQntuples(res))
                            {
                                std::string pos = conn.PGgetvalue(res, 0, 0);
                                if(pos.length())
                                    iPos = atoi(pos.c_str());
                                PQclear(res);

                                if(iPos >= 0)
                                {
                                    iPos = iPos % 10;
                                    iPos += 10;
                                    //int mod = (int)(std::fmod)(iPos + 10, 30);
                                    std::stringstream sd;
                                    sd << "UPDATE sheet SET pos = " << iPos;
                                    sd << " WHERE news <> 1 AND id = " << sId;
                                    SETUPDATESQL(SQLLogger, conn, sd);
                                    //comand = sd.str();
                                    //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                                    //res = conn.PGexec(comand);
                                    ////LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                                    //
                                    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                                    //    LOG_ERR_SQL(SQLLogger, res, comand);
                                    //PQclear(res);
                                }

                            }
                        }
                        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                            LOG_ERR_SQL(SQLLogger, res, comand);
                    }

                    PalletSheet[Pos].Slab = "";
                }
            }
        }

        //Вывот времени
        void OutTime(T_PlateData& PD, HWNDCLIENT chwnd)
        {
            if(IsSheet(PD))
            {
                time_t st = time(0);
                std::tm TM;
                localtime_s(&TM, &st);
                char sFormat[1024];
                sprintf_s(sFormat, 50, "%04d-%02d-%02d %02d:%02d:%02d", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
                SetWindowText(winmap(chwnd), sFormat);
            }
            else
            {
                SetWindowText(winmap(chwnd), "");
            }
        }
    }

#pragma endregion


//Функции обработки и со сохранения истории кассет

#pragma region Зоны печи закалки

    //Зоны
    namespace Sheet{
    //Зона 0: На входе в печь
        namespace Z0{

            const int Pos = 0;

            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();
                MySetWindowText(value);
                LocSheet(conn_kpvl, PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);
                //DeleteNullSgeet(conn, PD, Pos);
                LocSheet(conn_kpvl, PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ0_Time);
                return 0;
            }
        }

        //Зона 1: 1 зона печи 
        namespace Z1{
            const int Pos = 1;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);

                LocSheet(conn_kpvl, PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);

                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();


                std::string update;
                if(PD.AlloyCodeText->strVal.length())
                {
                    update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }
                if(PD.ThiknessText->strVal.length())
                {
                    update = " thikness = '" + PD.ThiknessText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);

                LocSheet(conn_kpvl, PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);

                if(IsSheet(PD))
                {
                    SheetPos(conn_kpvl, PD, Pos);

                    std::string update = " temper = " + GenSeqFromHmi.TempSet1->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    update = " speed = " + Par_Gen.UnloadSpeed->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    //update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                    //SetUpdateSheet(conn_kpvl, PD, update, "");

                    update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    
                    SetUpdateSheet(conn_kpvl, PD, " start_at = (SELECT MAX(create_at) FROM todos WHERE id_name = " + std::to_string(GenSeqToHmi.Seq_1_StateNo->ID) + " AND create_at <= now() AND (content = '3' OR content = '4' OR content = '5'))", " start_at IS NULL AND ");

                    DeleteNullSgeet(conn_kpvl, PD, Pos);
                }
                return 0;
            }
        };

        //Зона 2: 2 зона печи
        namespace Z2{
            const int Pos = 2;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);

                LocSheet(conn_kpvl, PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);

                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();

                std::string update;
                if(PD.AlloyCodeText->strVal.length())
                {
                    update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }
                if(PD.ThiknessText->strVal.length())
                {
                    update = " thikness = '" + PD.ThiknessText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }


                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);

                LocSheet(conn_kpvl, PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);

                if(IsSheet(PD))
                {
                    SheetPos(conn_kpvl, PD, 2);

                    std::string update = " temper = " + GenSeqFromHmi.TempSet1->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    update = " speed = " + Par_Gen.UnloadSpeed->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    //update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                    //SetUpdateSheet(conn_kpvl, PD, update, "");

                    update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    //update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                    //SetUpdateSheet(conn_kpvl, PD, update, "");

                    update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                    SetUpdateSheet(conn_kpvl, PD, update, "");

                    SetUpdateSheet(conn_kpvl, PD, " secondpos_at = now() ", " secondpos_at IS NULL AND ");

                    DeleteNullSgeet(conn_kpvl, PD, Pos);
                }
                return 0;
            }
        };

        //Зона 3: Закалка
        namespace Z3{
            const int Pos = 3;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);

                LocSheet(conn_kpvl, PD, Pos);

                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();
                std::string update;
                if(PD.AlloyCodeText->strVal.length())
                {
                    update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }
                if(PD.ThiknessText->strVal.length())
                {
                    update = " thikness = '" + PD.ThiknessText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];;
                MySetWindowText(value);

                LocSheet(conn_kpvl, PD, Pos);

                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ3_Time);

                if(IsSheet(PD))
                {
                    SheetPos(conn_kpvl, PD, Pos);
                    DeleteNullSgeet(conn_kpvl, PD, Pos);
                }
                return 0;
            }
        };

        //Зона 4: Охлаждение
        namespace Z4{
            const int Pos = 4;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);

                LocSheet(conn_kpvl, PD, Pos);

                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();

                std::string update;
                if(PD.AlloyCodeText->strVal.length())
                {
                    update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }
                if(PD.ThiknessText->strVal.length())
                {
                    update = " thikness = '" + PD.ThiknessText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }

                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);

                LocSheet(conn_kpvl, PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ4_Time);

                if(IsSheet(PD))
                {
                    SheetPos(conn_kpvl, PD, Pos);
                    DeleteNullSgeet(conn_kpvl, PD, Pos);
                }

                return 0;
            }
        };

        //Зона 5: Выдфча
        namespace Z5{
            const int Pos = 5;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);
                LocSheet(conn_kpvl, PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.SubSheet->GetValue();
                MySetWindowText(value);
                LocSheet(conn_kpvl, PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ5_Time);

                return 0;
            }

        }

        //Зона 6: Кантовка
        namespace Z6{
            const int Pos = 6;
            DWORD DataTime(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                if(!IsSheet(PD))
                    PD = PlateData[Pos - 1];

                MySetWindowText(value);
                LocSheet(conn_kpvl, PD, Pos);
                return 0;
            }

            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];

                MySetWindowText(value);
                LocSheet(conn_kpvl, PD, Pos);

                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();

                std::string update;
                if(PD.AlloyCodeText->strVal.length())
                {
                    update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }
                if(PD.ThiknessText->strVal.length())
                {
                    update = " thikness = '" + PD.ThiknessText->strVal + "'";
                    SetUpdateSheet(conn_kpvl, PD, update, "");
                }
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData PD = PlateData[Pos];

                if(!IsSheet(PD))
                    PD = PlateData[5];

                MySetWindowText(value);
                LocSheet(conn_kpvl, PD, Pos);

                if(IsSheet(PD))
                {
                    SheetPos(conn_kpvl, PD, Pos);
                    DeleteNullSgeet(conn_kpvl, PD, Pos);
                }
                return 0;
            }

            void SetSaveDone(PGConnection& conn)
            {
                try
                {
                    T_Side Top_Side = HMISheetData.Top_Side;
                    T_Side Bot_Side = HMISheetData.Bot_Side;
                    T_CassetteData Cassette = HMISheetData.Cassette;

                    T_PlateData PD = PlateData[Pos];
                    //if(!IsSheet(PD))
                    //    PD = PlateData[5];
                    //LOG_INFO(HardLogger, "{:90}", FUNCTION_LINE_NAME);

                    if(HMISheetData.NewData->Val.As<bool>()) //GetBool())
                    {
                        if(IsSheet(PD))
                        {
                            std::string id = GetIdSheet(conn, PD);
                            if(id.length() && Stoi(id) != 0)
                            {

#pragma region comand = "UPDATE sheet SET"
                                std::stringstream co;
                                co << "UPDATE sheet SET";
                                co << " pos = 7";
                                co << ", news = 1";
                                co << ", top1 = " << Top_Side.h1->GetFloat();
                                co << ", top2 = " << Top_Side.h2->GetFloat();
                                co << ", top3 = " << Top_Side.h3->GetFloat();
                                co << ", top4 = " << Top_Side.h4->GetFloat();
                                co << ", top5 = " << Top_Side.h5->GetFloat();
                                co << ", top6 = " << Top_Side.h6->GetFloat();
                                co << ", top7 = " << Top_Side.h7->GetFloat();
                                co << ", top8 = " << Top_Side.h8->GetFloat();
                                co << ", bot1 = " << Bot_Side.h1->GetFloat();
                                co << ", bot2 = " << Bot_Side.h2->GetFloat();
                                co << ", bot3 = " << Bot_Side.h3->GetFloat();
                                co << ", bot4 = " << Bot_Side.h4->GetFloat();
                                co << ", bot5 = " << Bot_Side.h5->GetFloat();
                                co << ", bot6 = " << Bot_Side.h6->GetFloat();
                                co << ", bot7 = " << Bot_Side.h7->GetFloat();
                                co << ", bot8 = " << Bot_Side.h8->GetFloat();
                                co << ", hour = " << Stoi(Cassette.Hour->GetString());// ->GetInt();
                                co << ", day = " << Cassette.Day->GetInt();
                                co << ", month = " << Cassette.Month->GetInt();
                                co << ", year = " << Cassette.Year->GetInt();
                                co << ", cassetteno = " << Cassette.CassetteNo->GetInt();
                                co << ", sheetincassette = " << (Cassette.SheetInCassette->GetInt() + 1);
                                co << " WHERE delete_at IS NULL AND id = " << id << ";";
#pragma endregion
                                SETUPDATESQL(SQLLogger, conn, co);

                                LOG_INFO(HardLogger, "{:90}| Set SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());
                            }
                            else
                                LOG_INFO(HardLogger, "{:90}| Not Set SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());
                        }
                        else
                            LOG_INFO(HardLogger, "{:90}| Not Set SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());

                        LOG_INFO(HardLogger, "{:90}| SaveDone->Set_Value(true)", FUNCTION_LINE_NAME);
                        HMISheetData.SaveDone->Set_Value(true);
                    }
                }
                CATCH(HardLogger, "");
            }

            //Новые лист в касету, Кассета наполяентся
            DWORD NewSheetData(Value* value)
            {
                const char* ss = WaitKant;
                if(value->Val.As<bool>()) //GetBool())                   //Если лист новый
                {
                    try
                    {
                        Cassette::CassettePos(conn_kpvl, HMISheetData.Cassette);
                        SetSaveDone(conn_kpvl);
                        ss = WaitResv;
                        PalletSheet[Pos - 1].Clear();
                        PalletSheet[Pos].Clear();
                    }
                    CATCH(HardLogger, "");
                    
                    //Коррекция листа
                    CreateThread(0, 0, PDF::CorrectSheet, (LPVOID)0, 0, 0);
                }
                MySetWindowText(winmap(value->winId), ss);
                
                return 0;
            }
        }
    }

    namespace Cassette{
        O_CassetteData OldCassette;
        void SetOldCassette(T_CassetteData& CD, int32_t id)
        {
            try
            {
                OldCassette.Id = id;
                OldCassette.Year = CD.Year->GetInt();
                OldCassette.Month = CD.Month->GetInt();
                OldCassette.Day = CD.Day->GetInt();
                OldCassette.Hour = CD.Hour->GetInt();
                OldCassette.CassetteNo = CD.CassetteNo->GetInt();
                OldCassette.SheetInCassette = CD.SheetInCassette->GetInt();
            }CATCH(HardLogger, "");
        }

        //Проверка на наличие кассеты
        bool IsCassette(O_CassetteData& CD)
        {
            return CD.Day && CD.Month && CD.Year && CD.CassetteNo;
        }

        //Получаем ID кассеты
        int32_t GetIdCassette(PGConnection& conn, O_CassetteData& CD)
        {
            int32_t id = 0;

            if(IsCassette(CD))
            {
                std::stringstream co;
                co << "SELECT id FROM cassette WHERE";
                co << " hour = " << CD.Hour;
                co << " AND day = " << CD.Day;
                co << " AND month = " << CD.Month;
                co << " AND year = " << CD.Year;
                co << " AND cassetteno = " << CD.CassetteNo;
                co << ";";
                std::string comand = co.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))//Линий
                    {
                        id = Stoi(conn.PGgetvalue(res, 0, 0));
                    }
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
            return id;

        }

        //Проверка на наличие кассеты
        bool IsCassette(T_CassetteData& CD)
        {
            try
            {
                //int32_t Hour = CD.Hour->GetInt(); //Stoi(CD.Hour->GetString());// 
                int32_t Day = CD.Day->GetInt();
                int32_t Month = CD.Month->GetInt();
                int32_t Year = CD.Year->GetInt();
                int32_t CassetteNo = CD.CassetteNo->GetInt();
                int16_t SheetInCassette = CD.SheetInCassette->GetInt();

                return Day && Month && Year && CassetteNo && SheetInCassette;
            }CATCH(HardLogger, "");
            return false;
        }

        //Получаем ID кассеты по листу Из касеты
        int32_t GetIdCassette(PGConnection& conn, T_CassetteData& CD)
        {
            int32_t id = 0;
            try
            {
                if(IsCassette(CD))
                {
                    std::stringstream co;
                    co << "SELECT id FROM cassette WHERE";
                    //if(!CD.Hour->Val.IsNul())
                    try
                    {
                        co << " hour = " << CD.Hour->GetInt() << " AND";
                    }CATCH(HardLogger, "");
                    try
                    {
                        co << " day = " << CD.Day->GetInt();
                    }CATCH(HardLogger, "");
                    try
                    {
                        co << " AND month = " << CD.Month->GetInt();
                    }CATCH(HardLogger, "");
                    try
                    {
                        co << " AND year = " << CD.Year->GetInt();
                    }CATCH(HardLogger, "");
                    try
                    {
                        co << " AND cassetteno = " << CD.CassetteNo->GetInt();
                    }CATCH(HardLogger, "");
                    co << " LIMIT 1;";
                    std::string comand = co.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    PGresult* res = conn.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
                        id = Stoi(conn.PGgetvalue(res, 0, 0));
                    else
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
            }CATCH(HardLogger, "");
            return id;
        }

        //Добовление кассеты в базу
        int32_t InsertCassette(PGConnection& conn, T_CassetteData& CD)
        {
            int32_t id = 0;
            try
            {
                if(IsCassette(CD))
                {
                    std::stringstream co;
                    co << "INSERT INTO cassette ";
                    co << "(event, year, month, day, hour, cassetteno, sheetincassette) VALUES (1, ";
                    co << CD.Year->GetInt() << ", ";
                    co << CD.Month->GetInt() << ", ";
                    co << CD.Day->GetInt() << ", ";
                    co << CD.Hour->GetInt() << ", ";    //CD.Hour->GetInt() << ", ";
                    co << CD.CassetteNo->GetInt() << ", ";
                    co << CD.SheetInCassette->GetInt() << ");";
                    std::string comand = co.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    PGresult* res = conn.PGexec(comand);
                    LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, co.str());

                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);

                    id = GetIdCassette(conn, CD);

                    SetOldCassette(CD, id);
                }
            }CATCH(HardLogger, "");
            return id;
        }

        //Обновляем в базе данные по кассете
        void UpdateCassette(PGConnection& conn, T_CassetteData& CD, int32_t id)
        {
            try
            {

                std::stringstream co;
                co << "UPDATE cassette SET";
                co << " year = " << CD.Year->GetInt() << ", ";
                co << " month = " << CD.Month->GetInt() << ", ";
                co << " day = " << CD.Day->GetInt() << ", ";
                co << " hour = " << CD.Hour->GetInt();//CD.Hour->GetInt() << ", ";
                co << " cassetteno = " << CD.CassetteNo->GetInt() << ", ";
                co << " sheetincassette = " << CD.SheetInCassette->GetInt() << ",";
                co << " close_at = DEFAULT, event = 1";
                co << " WHERE id = " << id;
                co << ";";

                std::string comand = co.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);

                if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
                SetOldCassette(CD, id);
            }CATCH(HardLogger, "");
        }

        //Обновляем данные по кассете если кассета есть или добовляем новую
        int32_t CassettePos(PGConnection& conn, T_CassetteData& CD)
        {
            int32_t id = 0;
            try
            {
                if(IsCassette(CD))
                {
                    //UpdateCassette(conn, CD, id);

                    id = GetIdCassette(conn, CD);
                    if(id)
                        SetOldCassette(CD, id);
                    else
                        id = InsertCassette(conn, CD);

                }
            }CATCH(HardLogger, "");
            return id;
        }

        //Закрываем все не закрытые касеты кроме кассеты на кантовке
        void CloseCassete(PGConnection& conn, O_CassetteData& CD)
        {
            try
            {
                if(IsCassette(CD))
                {
                    int32_t id = GetIdCassette(conn, CD);
                    std::stringstream co;
                    co << "UPDATE cassette SET close_at = now(), event = 2 WHERE close_at IS NULL AND id = " << id;

                    std::string comand = co.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    PGresult* res = conn.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
            }CATCH(HardLogger, "");
        }



        //Вывод Номер листа в касете
        DWORD Sheet_InCassette(Value* value)
        {
            try
            {
                char ss[256];
                int d = value->GetInt();
                sprintf_s(ss, 256, "%d", d + 1);
                MySetWindowText(winmap(value->winId), ss);
                //if(!HMISheetData.CasseteIsFill->Val.IsNul())
                bool b = HMISheetData.CasseteIsFill->Val.As<bool>();
                if(b) // GetBool())
                {
                    int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                    //LOG_INFO(HardLogger, "{:90}| Sheet_InCassette = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, d/*GetInt()*/, OldCassette.Id, id);
                }
                //LOG_INFO(HardLogger, "{:90} {}", FUNCTION_LINE_NAME, b);
            }CATCH(HardLogger, "");
            return 0;
        }

        //Вывод Номер кассеты за день
        DWORD CassetteNo(Value* value)
        {
            try
            {
                MySetWindowText(winmap(hEdit_Sheet_CassetteNo), value->GetString().c_str());
                MySetWindowText(winmap(hEdit_Sheet_CassetteNew), value->GetString().c_str());
                MySetWindowText(value);
                if(HMISheetData.CasseteIsFill->GetBool())
                {
                    int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteNo = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), OldCassette.Id, id);
                }
            }CATCH(HardLogger, "");
            return 0;
        }

        //Вывод Час ID листа
        DWORD CassetteHour(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(HMISheetData.CasseteIsFill->GetBool())
                {
                    int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteHour = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), OldCassette.Id, id);
                }
            }CATCH(HardLogger, "");
            return 0;
        }

        //Вывод День ID листа
        DWORD CassetteDay(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(HMISheetData.CasseteIsFill->GetBool())
                {
                    int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteDay = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), OldCassette.Id, id);
                }
            }CATCH(HardLogger, "");
            return 0;
        }

        //Вывод Месяц ID листа
        DWORD CassetteMonth(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(HMISheetData.CasseteIsFill->GetBool())
                {
                    int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteMonth = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), OldCassette.Id, id);
                }
            }CATCH(HardLogger, "");
            return 0;
        }

        //Вывод Год ID листа
        DWORD CassetteYear(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(HMISheetData.CasseteIsFill->GetBool())
                {
                    int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteYear = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), OldCassette.Id, id);
                }
            }CATCH(HardLogger, "");
            return 0;
        }

        //кассета наполяентся 
        // false = закраваем касету
        // true = создаем касету
        DWORD CasseteIsFill(Value* value)
        {
            try
            {
                MySetWindowText(value);
                bool b = value->Val.As<bool>();// GetBool();
                if(b)
                {
                    int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CasseteIsFill = true, OldCassette.Id = {}, Id = {}", FUNCTION_LINE_NAME, OldCassette.Id, id);
                }
                else
                {
                    if(OldCassette.Id)
                    {
                        LOG_INFO(HardLogger, "{:90}| OldCassette.Id = {}, OldCassette.Year = {}, OldCassette.Month = {}, OldCassette.Day = {}, OldCassette.Hour = {}, OldCassette.CassetteNo = {}, OldCassette.SheetInCassette = {}", FUNCTION_LINE_NAME,
                                 OldCassette.Id, OldCassette.Year, OldCassette.Month, OldCassette.Day, OldCassette.Hour, OldCassette.CassetteNo, OldCassette.SheetInCassette);
                        CloseCassete(conn_kpvl, OldCassette);
                        OldCassette = O_CassetteData();
                    }
                }

            }CATCH(HardLogger, "");
            return 0;
        }

        ////кассета готова
        //DWORD CassetteIsComplete(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    LOG_INFO(HardLogger, "{:90}| CassetteIsComplete = {} Id = {}", FUNCTION_LINE_NAME, value->GetBool(), OldCassette.Id);
        //    return 0;
        //}
        //
        ////начать новую кассету
        //DWORD StartNewCassette(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    LOG_INFO(HardLogger, "{:90}| StartNewCassette = {} Id = {}", FUNCTION_LINE_NAME, value->GetBool(), OldCassette.Id);
        //    return 0;
        //}
    }
#pragma endregion


#pragma region Вспомогательные функции
    //Операции в зонах
    namespace ZState{
        DWORD WINAPI ThreadState2(LPVOID)
        {
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, 1);
            try
            {
                PGresult* res = NULL;
                int32_t Melt     = Stoi(PalletSheet[2].Melt);
                int32_t Pack     = Stoi(PalletSheet[2].Pack);
                int32_t PartNo   = Stoi(PalletSheet[2].PartNo);
                int32_t Sheet    = Stoi(PalletSheet[2].Sheet);
                int32_t SubSheet = Stoi(PalletSheet[2].SubSheet);
                int32_t Slab     = Stoi(PalletSheet[2].Slab);


                float Time_Z2 = GenSeqToHmi.HeatTime_Z2->GetFloat();
                int16_t StateNo = GenSeqToHmi.Seq_2_StateNo->GetInt();

                if(Time_Z2 == 0)
                {
                    time_t st;
                    std::string datatimeend_at = GetDataTimeString(st);
                    if(datatimeend_at.length())
                        Time_Z2 = SQL::GetHeatTime_Z2(conn_dops, datatimeend_at);
                }

                if(Melt && Pack && PartNo && Sheet)
                {
                    int Id = Stoi(Sheet::GetIdSheet(conn_dops, Melt, Pack, PartNo, Sheet, SubSheet, Slab));

                    std::stringstream ss1;
                    ss1 << "UPDATE sheet SET ";
                    ss1 << "datatime_end = now()";
                    if(Time_Z2)
                        ss1 << ", datatime_all = " << Time_Z2;

                    ss1 << " WHERE datatime_end IS NULL AND"; //delete_at IS NULL AND 
                    ss1 << " id = " << Id;

                    std::string comand = ss1.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                    res = conn_dops.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);

                    std::stringstream ss2;
                    ss2 << "UPDATE sheet SET ";
                    ss2 << "lampresstop = " + AI_Hmi_210.LaminPressTop->GetString() + ", ";
                    ss2 << "lampressbot = " + AI_Hmi_210.LaminPressBot->GetString() + " ";
                    ss2 << "WHERE";
                    ss2 << " id = " << Id;

                    comand = ss2.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                    res = conn_dops.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);

                    int r = 5;
                    while(isRun && --r)  //5 секунд
                    {
                        SetWindowText(winmap(hEditState_34), std::to_string(r).c_str());
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }

                    std::string lam_te1 = AI_Hmi_210.LAM_TE1->GetString();
                    std::string za_te3 = AI_Hmi_210.Za_TE3->GetString();
                    std::string za_pt3 = AI_Hmi_210.Za_PT3->GetString();

                    std::stringstream ss4;
                    ss4 << "UPDATE sheet SET ";
                    ss4 << "lam_te1 = " << lam_te1 << ", ";
                    ss4 << "za_te3 = " << za_te3 << ", ";
                    ss4 << "za_pt3 = " << za_pt3 << " ";
                    ss4 << "WHERE";
                    ss4 << " id = " << Id;

                    comand = ss4.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    res = conn_dops.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
            }
            CATCH(SQLLogger, "");

            hThreadState2 = NULL;
            return 0;
        }

        //Операция в 1 зоне
        DWORD DataPosState_1(Value* value)
        {
            int16_t val = value->GetInt();
            MySetWindowText(value);
            SetWindowText(winmap(hEditState_12), GenSeq1[val].c_str());
            if(val == 3)
            {

            }
            return 0;
        }

        //Операция в 2 зоне
        DWORD DataPosState_2(Value* value)
        {
            int16_t val = value->GetInt();
            MySetWindowText(value);
            SetWindowText(winmap(hEditState_22), GenSeq2[val].c_str());
            if(val == 5 || val == 6)
            {
                if(hThreadState2 == NULL)
                    hThreadState2 = CreateThread(0, 0, ThreadState2, (LPVOID)0, 0, 0);

            }
            return 0;
        }

        //Операция в 3 зоне
        DWORD DataPosState_3(Value* value)
        {
            MySetWindowText(value);
            SetWindowText(winmap(hEditState_32), GenSeq3[value->GetInt()].c_str());

            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            return 0;
        }
    }

    //Аналоги
    namespace An{

       //Скорость выгрузки, Уставка температуры, Давление, Температура воды
        //Уставка температуры

        DWORD TempSet1(Value* value)
        {
            MySetWindowText(value);

            if(value->GetFloat() > 0)
            {
                std::string update = " temper = " + value->GetString();
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");
            }
            return 0;
        }

        //Скорость выгрузки
        DWORD UnloadSpeed(Value* value)
        {
            MySetWindowText(value);

            if(value->GetFloat() > 0)
            {
                std::string update = " speed = " + value->GetString();
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");
            }
            return 0;
        }

        //Время сигнализации окончания нагрева, мин
        DWORD fTimeForPlateHeat(Value* value)
        {
            MySetWindowText(value);

            std::string update = " timeforplateheat = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");
            return 0;
        }

        //Уставка давления для запуска комперссора
        DWORD fPresToStartComp(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " prestostartcomp = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");

            return 0;
        }

    };

    //Режим работы клапана
    //State_1 = 3 
    //Маска клапанов
    namespace Mask{
        DWORD DataMaskKlapan1(Value* value)
        {
            int16_t v = value->GetInt();
            for(int i = 0; i < 9; i++)
            {
                if((1 << i) & v)
                {
                    MySetWindowText(mapWindow[HWNDCLIENT(value->winId + i)].hWnd, "1");
                    MaskKlapan1[i] = '1';
                }
                else
                {
                    MySetWindowText(winmap(HWNDCLIENT(value->winId + i)), "0");
                    MaskKlapan1[i] = '0';
                }
            }

            MaskKlapan = MaskKlapan1 + " " + MaskKlapan2;

            std::string update = " mask = '" + MaskKlapan + "'";
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");

            return 0;
        }
        DWORD DataMaskKlapan2(Value* value)
        {
            int16_t v = value->GetInt();
            for(int i = 0; i < 9; i++)
            {
                if((1 << i) & v)
                {
                    MySetWindowText(mapWindow[HWNDCLIENT(value->winId + i)].hWnd, "1");
                    MaskKlapan2[i] = '1';
                }
                else
                {
                    MySetWindowText(winmap(HWNDCLIENT(value->winId + i)), "0");
                    MaskKlapan2[i] = '0';
                }
            }

            MaskKlapan = MaskKlapan1 + " " + MaskKlapan2;
            std::string update = " mask = '" + MaskKlapan + "'";
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");

            return 0;
        }
    }

    //Скоростная секция
    namespace Speed{
        //Верх
        DWORD SpeedSectionTop(Value* value)
        {
            MySetWindowText(value);
            std::string update = " posclapantop = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " posclapantop = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD SpeedSectionBot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " posclapanbot = 0 AND");
            return 0;
        }
    }

    //Ламинарная секция 1
    namespace Lam1{
        //Верх
        DWORD LaminarSection1Top(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam1posclapantop = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " lam1posclapantop = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " lam1posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD LaminarSection1Bot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam1posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " lam1posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " lam1posclapanbot = 0 AND");
            return 0;
        }
    };

    //Ламинарная секция 2
    namespace Lam2{
        //Верх
        DWORD LaminarSection2Top(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam2posclapantop = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " lam2posclapantop = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " lam2posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD LaminarSection2Bot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam2posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " lam2posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " lam2posclapanbot = 0 AND");
            return 0;
        }
    };

    //Отклонения листа на кантовке
    DWORD Side(Value* value)
    {
        std::map <std::string, std::string> find ={
            {" top1", "Top_Side.h1"},
            {" top2", "Top_Side.h2"},
            {" top3", "Top_Side.h3"},
            {" top4", "Top_Side.h4"},
            {" top5", "Top_Side.h5"},
            {" top6", "Top_Side.h6"},
            {" top7", "Top_Side.h7"},
            {" top8", "Top_Side.h8"},
            {" bot1", "Bottom_Side.h1"},
            {" bot2", "Bottom_Side.h2"},
            {" bot3", "Bottom_Side.h3"},
            {" bot4", "Bottom_Side.h4"},
            {" bot5", "Bottom_Side.h5"},
            {" bot6", "Bottom_Side.h6"},
            {" bot7", "Bottom_Side.h7"},
            {" bot8", "Bottom_Side.h8"},
        };
        for(auto f : find)
        {
            size_t l = value->Patch.find(f.second);
            if(l != std::string::npos)
            {
                std::stringstream up;
                up << f.first << " = " << value->GetFloat();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, up.str());
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[6], up.str(), "");
            }
        }


        //else
        //{
        //    std::string find = "Bottom_Side.h";
        //    size_t l = value->Patch.find(find);
        //    if(l != std::string::npos)
        //    {
        //        char s = value->Patch[l + 13];
        //        std::stringstream up;
        //        up << " top" << value->Patch[l + find.length()] << " = " << value->GetFloat();
        //        LOG_INFO(SQLLogger, "{:90}| {}", up.str());
        //        Sheet::SetUpdateSheet(conn_kpvl, PlateData[6], up.str(), "");
        //    }
        //}

        //std::string update = " top1 = " + value->GetString();
        //Sheet::SetUpdateSheet(conn_kpvl, PlateData[6], update, "");
        return 0;
    }
    
    DWORD Temperature(Value* value)
    {
        MySetWindowText(value);

        if(
            Hmi210_1.Htr2_1->Val.IsNul() ||
            Hmi210_1.Htr2_2->Val.IsNul() ||
            Hmi210_1.Htr2_3->Val.IsNul() ||
            Hmi210_1.Htr2_4->Val.IsNul()
            )return 0;

        float f1 = Hmi210_1.Htr2_1->GetFloat();
        float f2 = Hmi210_1.Htr2_2->GetFloat();
        float f3 = Hmi210_1.Htr2_3->GetFloat();
        float f4 = Hmi210_1.Htr2_4->GetFloat();
        if(f1 && f2 && f3 && f4)
        {
            Hmi210_1.Temperature = (f1 + f2 + f3 + f4) / 4.0f;
            std::stringstream ss;
            ss << std::setprecision(0) << std::fixed << Hmi210_1.Temperature;
            MySetWindowText(winmap(hStatTempALLTAct), ss.str());
            std::string update = " temperature = " + ss.str();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");
        }
        return 0;
    }

    //Бит жмизни
    DWORD SheetData_WDG_toBase(Value* value)
    {
        if(value->GetBool())
        {
            PLC_KPVL_old_dt = time(NULL);
            struct tm TM;
            localtime_s(&TM, &PLC_KPVL_old_dt);
            HMISheetData.WDG_fromBase->Set_Value(true);
            SetWindowText(winmap(value->winId), string_time(&TM).c_str());
        }
        return 0;
    }


#pragma endregion
};


//void KPVL_SQL_Cassette(PGConnection& conn, std::deque<TSheet>& Sheet, std::string start_at, std::string stop_at)
//{
//    Sheet.erase(Sheet.begin(), Sheet.end());
//
//    //std::time_t stop = time(NULL);
//    //std::time_t statr = static_cast<std::time_t>(difftime(stop, 60 * 60 * 24 * 10)); //7-е суток
//    //std::string start_at = "";  //GetDataTimeString(&statr);
//    //std::string stop_at = "";
//
//    std::stringstream FilterComand;
//    FilterComand << "SELECT * FROM sheet ";
//    FilterComand << "WHERE  create_at > '" << start_at << "'";
//    FilterComand << " AND id <= " << stop_at;
//    FilterComand << " ORDER BY ORDER BY id;";
//    //FilterSheet();
//    //bFilterData = TRUE;
//    std::string comand = FilterComand.str();
//    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
//    PGresult* res = conn.PGexec(comand);
//    //LOG_INFO(SQLLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
//    if(PQresultStatus(res) == PGRES_TUPLES_OK)
//    {
//        KPVL::SQL::GetCollumn(res);
//
//        int line = PQntuples(res);
//        for(int l = 0; l < line; l++)
//        {
//            TSheet sheet;
//            sheet.DataTime = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_create_at));
//            sheet.Pos = conn.PGgetvalue(res, l, Col_Sheet_pos);
//            sheet.id = conn.PGgetvalue(res, l, Col_Sheet_id);
//            sheet.DataTime_End = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_datatime_end));
//            sheet.DataTime_All = conn.PGgetvalue(res, l, Col_Sheet_datatime_all);
//            sheet.Alloy = conn.PGgetvalue(res, l, Col_Sheet_alloy);
//            sheet.Thikness = conn.PGgetvalue(res, l, Col_Sheet_thikness);
//            sheet.Melt = conn.PGgetvalue(res, l, Col_Sheet_melt);
//            sheet.Slab = conn.PGgetvalue(res, l, Col_Sheet_slab);
//            sheet.PartNo = conn.PGgetvalue(res, l, Col_Sheet_partno);
//            sheet.Pack = conn.PGgetvalue(res, l, Col_Sheet_pack);
//            sheet.Sheet = conn.PGgetvalue(res, l, Col_Sheet_sheet);
//            sheet.SubSheet = conn.PGgetvalue(res, l, Col_Sheet_subsheet);
//            sheet.Temper = conn.PGgetvalue(res, l, Col_Sheet_temper);
//            sheet.Speed = conn.PGgetvalue(res, l, Col_Sheet_speed);
//
//            sheet.Za_PT3 = conn.PGgetvalue(res, l, Col_Sheet_za_pt3);
//            sheet.Za_TE3 = conn.PGgetvalue(res, l, Col_Sheet_za_te3);
//
//            sheet.LaminPressTop = conn.PGgetvalue(res, l, Col_Sheet_lampresstop);
//            sheet.LaminPressBot = conn.PGgetvalue(res, l, Col_Sheet_lampressbot);
//            sheet.PosClapanTop = conn.PGgetvalue(res, l, Col_Sheet_posclapantop);
//            sheet.PosClapanBot = conn.PGgetvalue(res, l, Col_Sheet_posclapanbot);
//            sheet.Mask = conn.PGgetvalue(res, l, Col_Sheet_mask);
//
//            sheet.Lam1PosClapanTop = conn.PGgetvalue(res, l, Col_Sheet_lam1posclapantop);
//            sheet.Lam1PosClapanBot = conn.PGgetvalue(res, l, Col_Sheet_lam1posclapanbot);
//            sheet.Lam2PosClapanTop = conn.PGgetvalue(res, l, Col_Sheet_lam2posclapantop);
//            sheet.Lam2PosClapanBot = conn.PGgetvalue(res, l, Col_Sheet_lam2posclapanbot);
//
//            sheet.LAM_TE1 = conn.PGgetvalue(res, l, Col_Sheet_lam_te1);
//            sheet.News = conn.PGgetvalue(res, l, Col_Sheet_news);
//            sheet.Top1 = conn.PGgetvalue(res, l, Col_Sheet_top1);
//            sheet.Top2 = conn.PGgetvalue(res, l, Col_Sheet_top2);
//            sheet.Top3 = conn.PGgetvalue(res, l, Col_Sheet_top3);
//            sheet.Top4 = conn.PGgetvalue(res, l, Col_Sheet_top4);
//            sheet.Top5 = conn.PGgetvalue(res, l, Col_Sheet_top5);
//            sheet.Top6 = conn.PGgetvalue(res, l, Col_Sheet_top6);
//            sheet.Top7 = conn.PGgetvalue(res, l, Col_Sheet_top7);
//            sheet.Top8 = conn.PGgetvalue(res, l, Col_Sheet_top8);
//
//            sheet.Bot1 = conn.PGgetvalue(res, l, Col_Sheet_bot1);
//            sheet.Bot2 = conn.PGgetvalue(res, l, Col_Sheet_bot2);
//            sheet.Bot3 = conn.PGgetvalue(res, l, Col_Sheet_bot3);
//            sheet.Bot4 = conn.PGgetvalue(res, l, Col_Sheet_bot4);
//            sheet.Bot5 = conn.PGgetvalue(res, l, Col_Sheet_bot5);
//            sheet.Bot6 = conn.PGgetvalue(res, l, Col_Sheet_bot6);
//            sheet.Bot7 = conn.PGgetvalue(res, l, Col_Sheet_bot7);
//            sheet.Bot8 = conn.PGgetvalue(res, l, Col_Sheet_bot8);
//
//            sheet.Day = conn.PGgetvalue(res, l, Col_Sheet_day);
//            sheet.Month = conn.PGgetvalue(res, l, Col_Sheet_month);
//            sheet.Year = conn.PGgetvalue(res, l, Col_Sheet_year);
//            sheet.CassetteNo = conn.PGgetvalue(res, l, Col_Sheet_cassetteno);
//            sheet.SheetInCassette = conn.PGgetvalue(res, l, Col_Sheet_sheetincassette);
//
//            sheet.Start_at = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_start_at));
//            sheet.TimeForPlateHeat = conn.PGgetvalue(res, l, Col_Sheet_timeforplateheat);
//            sheet.PresToStartComp = conn.PGgetvalue(res, l, Col_Sheet_prestostartcomp);
//            sheet.Temperature = conn.PGgetvalue(res, l, Col_Sheet_temperature);
//
//
//            Sheet.push_back(sheet);
//        }
//
//    }
//    else
//        LOG_ERR_SQL(SQLLogger, res, comand);
//    PQclear(res);
//
//    //AddHistoriSheet(true, (int)sheet.size());
//    int t = 0;
//}


