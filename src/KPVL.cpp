#include "pch.h"

#include "win.h"
#include "main.h"
#include "file.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "hard.h"
#include "KPVL.h"

extern std::string FORMATTIME;

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
const int Col_Sheet_id = 0;
const int Col_Sheet_create_at = 1;
const int Col_Sheet_start_at = 2;
const int Col_Sheet_datatime_end = 3;
const int Col_Sheet_pos = 4;
const int Col_Sheet_datatime_all = 5;
const int Col_Sheet_alloy = 6;
const int Col_Sheet_thikness = 7;
const int Col_Sheet_melt = 8;
const int Col_Sheet_slab = 9;
const int Col_Sheet_partno = 10;
const int Col_Sheet_pack = 11;
const int Col_Sheet_sheet = 12;
const int Col_Sheet_subsheet = 13;
const int Col_Sheet_temper = 14;
const int Col_Sheet_speed = 15;
const int Col_Sheet_za_pt3 = 16;
const int Col_Sheet_za_te3 = 17;
const int Col_Sheet_lampresstop = 18;
const int Col_Sheet_lampressbot = 19;
const int Col_Sheet_posclapantop  =20;
const int Col_Sheet_posclapanbot = 21;
const int Col_Sheet_mask = 22;
const int Col_Sheet_lam1posclapantop = 23;
const int Col_Sheet_lam1posclapanbot = 24;
const int Col_Sheet_lam2posclapantop = 25;
const int Col_Sheet_lam2posclapanbot = 26;
const int Col_Sheet_lam_te1 = 27;
const int Col_Sheet_news = 28;
const int Col_Sheet_top1 = 29;
const int Col_Sheet_top2 = 30;
const int Col_Sheet_top3 = 31;
const int Col_Sheet_top4 = 32;
const int Col_Sheet_top5 = 33;
const int Col_Sheet_top6 = 34;
const int Col_Sheet_top7 = 35;
const int Col_Sheet_top8 = 36;
const int Col_Sheet_bot1 = 37;
const int Col_Sheet_bot2 = 38;
const int Col_Sheet_bot3 = 39;
const int Col_Sheet_bot4 = 40;
const int Col_Sheet_bot5 = 41;
const int Col_Sheet_bot6 = 42;
const int Col_Sheet_bot7 = 43;
const int Col_Sheet_bot8 = 44;
const int Col_Sheet_day = 45;
const int Col_Sheet_month = 46;
const int Col_Sheet_year = 47;
const int Col_Sheet_cassetteno = 48;
const int Col_Sheet_sheetincassette = 49;
const int Col_Sheet_timeforplateheat = 50;
const int Col_Sheet_prestostartcomp = 51;
//};
#pragma endregion

//Печь закалки
namespace KPVL {

    //Список последних 100 листов из базы
    namespace SQL
    {
        //Получаем список листов из базы
        void KPVL_SQL()
        {
            //AllSheet.erase(AllSheet.begin(), AllSheet.end());
            std::string sStartId = "0";
            std::string sStopId = "0";

            std::string sMaxId = "SELECT max(id) FROM sheet";
            PGresult* res = conn_spis.PGexec(sMaxId);
            //LOG_INFO(SQLLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, sMaxId);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                int line = PQntuples(res);
                if(line)
                    sStopId = conn_kpvl.PGgetvalue(res, line - 1, 0);
            }
            else
                LOG_ERR_SQL(SQLLogger, res, sMaxId);
            PQclear(res);

            AllSheet.erase(AllSheet.begin(), AllSheet.end());

            if(!sStopId.length())sStopId = "0";
            long TempId = atoi(sStopId.c_str());
            TempId = TempId > 100 ? TempId - 100 : 0;
            sStartId = std::to_string(TempId);

            std::stringstream FilterComand;
            FilterComand << "SELECT * FROM sheet ";
            FilterComand << "WHERE id > " << sStartId;
            FilterComand << " AND id <= " << sStopId;
            FilterComand << " ORDER BY  create_at DESC, pos DESC, start_at DESC;";
            //FilterSheet();
            //bFilterData = TRUE;
            res = conn_spis.PGexec(FilterComand.str());
            //LOG_INFO(SQLLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                int line = PQntuples(res);
                for(int l = 0; l < line; l++)
                {
                    TSheet sheet;
                    sheet.DataTime = GetStringData(conn_kpvl.PGgetvalue(res, l, Col_Sheet_create_at));
                    sheet.Pos = conn_kpvl.PGgetvalue(res, l, Col_Sheet_pos);
                    sheet.id = conn_kpvl.PGgetvalue(res, l, Col_Sheet_id);
                    sheet.DataTime_End = GetStringData(conn_kpvl.PGgetvalue(res, l, Col_Sheet_datatime_end));
                    sheet.DataTime_All = conn_kpvl.PGgetvalue(res, l, Col_Sheet_datatime_all);
                    sheet.Alloy = conn_kpvl.PGgetvalue(res, l, Col_Sheet_alloy);
                    sheet.Thikness = conn_kpvl.PGgetvalue(res, l, Col_Sheet_thikness);
                    sheet.Melt = conn_kpvl.PGgetvalue(res, l, Col_Sheet_melt);
                    sheet.Slab = conn_kpvl.PGgetvalue(res, l, Col_Sheet_slab);
                    sheet.PartNo = conn_kpvl.PGgetvalue(res, l, Col_Sheet_partno);
                    sheet.Pack = conn_kpvl.PGgetvalue(res, l, Col_Sheet_pack);
                    sheet.Sheet = conn_kpvl.PGgetvalue(res, l, Col_Sheet_sheet);
                    sheet.SubSheet = conn_kpvl.PGgetvalue(res, l, Col_Sheet_subsheet);
                    sheet.Temper = conn_kpvl.PGgetvalue(res, l, Col_Sheet_temper);
                    sheet.Speed = conn_kpvl.PGgetvalue(res, l, Col_Sheet_speed);

                    sheet.Za_PT3 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_za_pt3);
                    sheet.Za_TE3 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_za_te3);

                    sheet.LaminPressTop = conn_kpvl.PGgetvalue(res, l, Col_Sheet_lampresstop);
                    sheet.LaminPressBot = conn_kpvl.PGgetvalue(res, l, Col_Sheet_lampressbot);
                    sheet.PosClapanTop = conn_kpvl.PGgetvalue(res, l, Col_Sheet_posclapantop);
                    sheet.PosClapanBot = conn_kpvl.PGgetvalue(res, l, Col_Sheet_posclapanbot);
                    sheet.Mask = conn_kpvl.PGgetvalue(res, l, Col_Sheet_mask);

                    sheet.Lam1PosClapanTop = conn_kpvl.PGgetvalue(res, l, Col_Sheet_lam1posclapantop);
                    sheet.Lam1PosClapanBot = conn_kpvl.PGgetvalue(res, l, Col_Sheet_lam1posclapanbot);
                    sheet.Lam2PosClapanTop = conn_kpvl.PGgetvalue(res, l, Col_Sheet_lam2posclapantop);
                    sheet.Lam2PosClapanBot = conn_kpvl.PGgetvalue(res, l, Col_Sheet_lam2posclapanbot);

                    sheet.LAM_TE1 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_lam_te1);
                    sheet.News = conn_kpvl.PGgetvalue(res, l, Col_Sheet_news);
                    sheet.Top1 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_top1);
                    sheet.Top2 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_top2);
                    sheet.Top3 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_top3);
                    sheet.Top4 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_top4);
                    sheet.Top5 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_top5);
                    sheet.Top6 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_top6);
                    sheet.Top7 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_top7);
                    sheet.Top8 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_top8);

                    sheet.Bot1 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_bot1);
                    sheet.Bot2 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_bot2);
                    sheet.Bot3 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_bot3);
                    sheet.Bot4 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_bot4);
                    sheet.Bot5 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_bot5);
                    sheet.Bot6 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_bot6);
                    sheet.Bot7 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_bot7);
                    sheet.Bot8 = conn_kpvl.PGgetvalue(res, l, Col_Sheet_bot8);

                    sheet.Day = conn_kpvl.PGgetvalue(res, l, Col_Sheet_day);
                    sheet.Month = conn_kpvl.PGgetvalue(res, l, Col_Sheet_month);
                    sheet.Year = conn_kpvl.PGgetvalue(res, l, Col_Sheet_year);
                    sheet.CassetteNo = conn_kpvl.PGgetvalue(res, l, Col_Sheet_cassetteno);
                    sheet.SheetInCassette = conn_kpvl.PGgetvalue(res, l, Col_Sheet_sheetincassette);

                    sheet.Start_at = GetStringData(conn_kpvl.PGgetvalue(res, l, Col_Sheet_start_at));
                    sheet.TimeForPlateHeat = conn_kpvl.PGgetvalue(res, l, Col_Sheet_timeforplateheat);
                    sheet.PresToStartComp = conn_kpvl.PGgetvalue(res, l, Col_Sheet_prestostartcomp);

                    AllSheet.push_back(sheet);
                }

            }
            else
                LOG_ERR_SQL(SQLLogger, res, FilterComand.str());
            PQclear(res);

            //AddHistoriSheet(true, (int)AllSheet.size());
            int t = 0;
        }

        float GetHeatTime_Z2(std::string enddata_at)
        {
            float out = 0;
            std::string next_at = "";
            std::string sout = "";
            if(enddata_at.length())
            {
                std::stringstream co;
                co << "SELECT max(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at <= '" << enddata_at << "';";
                std::string comand = co.str();
                PGresult* res = conn_spis.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))
                        next_at = conn_kpvl.PGgetvalue(res, 0, 0);
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }

            if(next_at.length())
            {
                std::stringstream co;
                co << "SELECT content FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at = '" << next_at << "';";
                std::string comand = co.str();
                PGresult* res = conn_spis.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))
                        sout = conn_kpvl.PGgetvalue(res, 0, 0);
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
            if(sout.length())
                out = std::stof(sout);
            return out;
        }

        void GetDataTime_All(TSheet& TS)
        {
            if(std::stoi(TS.DataTime_All) == 0)
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
                    PGresult* res = conn_spis.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            next_at = conn_kpvl.PGgetvalue(res, 0, 0);
                    }
                    else
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }

                if(next_at.length())
                {
                    std::stringstream co;
                    co << "SELECT min(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_2_StateNo->ID << " AND content = '5' AND create_at > '" << next_at << "';"; //GenSeqToHmi.Data.Seq_2_StateNo Номер шага последовательности выгрузки в печи
                    std::string comand = co.str();
                    PGresult* res = conn_spis.PGexec(comand);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            enddata_at = conn_kpvl.PGgetvalue(res, 0, 0);
                    }
                    else
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
                if(enddata_at.length())
                {
                    float HeatTime_Z2 = GetHeatTime_Z2(enddata_at);

                    std::string Id = Sheet::GetIdSheet(TS.Melt, TS.Pack, TS.PartNo, TS.Sheet, TS.SubSheet, TS.Slab);

                    std::stringstream co;
                    co << "UPDATE sheet SET datatime_end = '" << enddata_at << "', datatime_all = " << HeatTime_Z2 << " WHERE id = " << Id;
                    std::string comand = co.str();
                    PGresult* res = conn_kpvl.PGexec(comand);
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
    uint32_t NextID = 1;

//Функции обработки и со сохранения истории листов
#pragma region Функции обработки и со сохранения истории листов
    //Локальные данные по листу
    namespace Sheet{

        //Проверка на наличие листа
        bool IsSheet(T_PlateData& PD)
        {
            int32_t Melt = PD.Melt->Val.As<int32_t>();
            int32_t Pack = PD.Pack->Val.As<int32_t>();
            int32_t PartNo = PD.PartNo->Val.As<int32_t>();
            int32_t Sheet = PD.Sheet->Val.As<int32_t>();
            //int32_t SubSheet = PD.SubSheet->Val.As<int32_t>();
            //int32_t Slab = PD.Slab->Val.As<int32_t>();
            return Melt && Pack && PartNo && Sheet/* && SubSheet*/;
        }
        bool IsSheet(TSheet& PS)
        {
            int32_t Melt = std::stoi(PS.Melt);
            int32_t Pack = std::stoi(PS.Pack);
            int32_t PartNo = std::stoi(PS.PartNo);
            int32_t Sheet = std::stoi(PS.Sheet);
            //int32_t SubSheet = PS.SubSheet;
            //int32_t Slab = PS.Slab;
            return Melt && Pack && PartNo && Sheet/* && SubSheet*/;
        }

        void LocSheet(T_PlateData& PD, int Pos)
        {
            if(IsSheet(PD))
            {
                PalletSheet[Pos].id = std::to_string(++NextID);
                PalletSheet[Pos].DataTime = GetDataTimeString();
                PalletSheet[Pos].Pos = std::to_string(Pos);
                PalletSheet[Pos].Alloy = PlateData[Pos].AlloyCodeText->GetString();
                PalletSheet[Pos].Thikness = PlateData[Pos].ThiknessText->GetString();
                PalletSheet[Pos].Melt = PlateData[Pos].Melt->GetString();
                PalletSheet[Pos].Slab = PlateData[Pos].Slab->GetString();
                PalletSheet[Pos].PartNo = PlateData[Pos].PartNo->GetString();
                PalletSheet[Pos].Pack = PlateData[Pos].Pack->GetString();
                PalletSheet[Pos].Sheet = PlateData[Pos].Sheet->GetString();
                PalletSheet[Pos].SubSheet =PlateData[Pos].SubSheet->GetString();
            }
            else
            {
                PalletSheet[Pos].Clear();
            }
        }

        //Получаем ID листа
        std::string GetIdSheet(T_PlateData& PD)
        {
            std::string id = "0";

            if(IsSheet(PD))
            {
                PD.SubSheet->GetValue();

                std::stringstream sd;
                sd << "SELECT id FROM sheet WHERE";
                sd << " melt = " << PD.Melt->Val.As<int32_t>();
                sd << " AND pack = " << PD.Pack->Val.As<int32_t>();
                sd << " AND partno = " << PD.PartNo->Val.As<int32_t>();
                sd << " AND sheet = " << PD.Sheet->Val.As<int32_t>();
                sd << " AND subsheet = " << PD.SubSheet->Val.As<int32_t>();
                sd << " AND slab = " << PD.Slab->Val.As<int32_t>();
                std::string comand = sd.str();
                PGresult* res = conn_kpvl.PGexec(comand);

                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))//Линий
                    {
                        id = conn_kpvl.PGgetvalue(res, 0, 0);
                    }
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
            return id;
        }

        //Получаем ID листа
        std::string GetIdSheet(std::string sMelt, std::string sPack, std::string sPartNo, std::string sSheet, std::string sSubSheet, std::string sSlab)
        {
            std::string id = "0";

            int32_t Melt = std::stoi(sMelt);
            int32_t Pack = std::stoi(sPack);
            int32_t PartNo = std::stoi(sPartNo);
            int32_t Sheet = std::stoi(sSheet);
            int32_t SubSheet = std::stoi(sSubSheet);
            int32_t Slab = std::stoi(sSlab);

            if(Melt && Pack && PartNo && Sheet /*&& SubSheet && Slab*/)
            {
                std::stringstream co;
                co << "SELECT id FROM sheet WHERE";
                co << " melt = " << Melt;
                co << " AND pack = " << Pack;
                co << " AND partno = " << PartNo;
                co << " AND sheet = " << Sheet;
                co << " AND subsheet = " << SubSheet;
                co << " AND slab = " << Slab;
                co << ";";
                std::string comand = co.str();
                PGresult* res = conn_kpvl.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
                    id = conn_kpvl.PGgetvalue(res, 0, 0);
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
            return id;
        }

        //Добовление листа в базу
        void InsertSheet(T_PlateData& PD, int Pos)
        {
            if(IsSheet(PD))
            {
                std::string id = GetIdSheet(PD);
                if(!id.length() || id == "" || id == "0")
                {
                    std::stringstream sd;
                    sd << "INSERT INTO sheet ";
                    sd << "(";
                    sd << "alloy, ";
                    sd << "thikness, ";
                    sd << "melt, ";
                    sd << "slab, ";
                    sd << "partno, ";
                    sd << "pack, ";
                    sd << "sheet, ";
                    sd << "subsheet, ";
                    if(Pos == 1 || Pos == 2)
                    {
                        sd << "temper, ";
                        sd << "speed, ";

                        sd << "timeforplateheat, ";
                        sd << "prestostartcomp, ";

                        sd << "posclapantop, ";
                        sd << "posclapanbot, ";
                        sd << "mask, ";
                        sd << "lam1posclapantop, ";
                        sd << "lam1posclapanbot, ";
                        sd << "lam2posclapantop, ";
                        sd << "lam2posclapanbot, ";
                    }
                    sd << " pos";
                    sd << ") VALUES (";
                    sd << "'" << PD.AlloyCodeText->GetString() << "', ";
                    sd << "'" << PD.ThiknessText->GetString() << "', ";
                    //char ss[256];
                    //sprintf_s(ss, 255, "\x27%s\x27, ", PD.AlloyCodeText->GetString().c_str());
                    //sd << += ss; // "'" + PD.AlloyCodeText->GetString() + "', ";
                    //sprintf_s(ss, 255, "\x27%s\x27, ", PD.ThiknessText->GetString().c_str());
                    //comand += ss; // "'" + PD.ThiknessText->GetString() + "', ";
                    sd << PD.Melt->Val.As<int32_t>() << ", ";
                    sd << PD.Slab->Val.As<int32_t>() << ", ";
                    sd << PD.PartNo->Val.As<int32_t>() << ", ";
                    sd << PD.Pack->Val.As<int32_t>() << ", ";
                    sd << PD.Sheet->Val.As<int32_t>() << ", ";
                    sd << PD.SubSheet->Val.As<int32_t>() << ", ";

                    if(Pos == 1 || Pos == 2)
                    {
                        sd << GenSeqFromHmi.TempSet1->Val.As<float>() << ", ";
                        //LOG_INFO(HardLogger, "{:90} Speed = {}", FUNCTION_LINE_NAME, Par_Gen.UnloadSpeed->GetString());
                        sd << Par_Gen.UnloadSpeed->Val.As<float>() << ", ";
                        sd << Par_Gen.TimeForPlateHeat->Val.As<float>() << ", ";
                        sd << Par_Gen.PresToStartComp->Val.As<float>() << ", ";

                        //comand += Par_Gen.Par.UnloadSpeed.GetString() + ", ";

                        sd << HMISheetData.SpeedSection.Top->Val.As<float>() << ", ";
                        sd << HMISheetData.SpeedSection.Bot->Val.As<float>() << ", ";
                        sd << "'" + MaskKlapan + "', ";
                        sd << HMISheetData.LaminarSection1.Top->Val.As<float>() << ", ";
                        sd << HMISheetData.LaminarSection1.Bot->Val.As<float>() << ", ";
                        sd << HMISheetData.LaminarSection2.Top->Val.As<float>() << ", ";
                        sd << HMISheetData.LaminarSection2.Bot->Val.As<float>() << ", ";
                    }
                    sd << Pos << ");";

                    std::string comand = sd.str();
                    PGresult* res = conn_kpvl.PGexec(comand);
                    //LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
            }
        }

        //Обновлякем в базе данные по листу
        bool SetUpdateSheet(PGConnection* con, T_PlateData& PD, std::string update, std::string where)
        {
            if(!con) return false;
            if(IsSheet(PD))
            {
                std::stringstream sd;
                sd << "UPDATE sheet SET" << update;
                sd << " WHERE" << where;
                sd << " melt = " << PD.Melt->Val.As<int32_t>();
                sd << " AND partno = " << PD.PartNo->Val.As<int32_t>();
                sd << " AND pack = " << PD.Pack->Val.As<int32_t>();
                sd << " AND sheet = " << PD.Sheet->Val.As<int32_t>();
                sd << " AND subsheet = " << PD.SubSheet->Val.As<int32_t>();
                sd << " AND slab = " << PD.Slab->Val.As<int32_t>();
                sd << ";";

                std::string comand = sd.str();
                PGresult* res = con->PGexec(comand);
                //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                {
                    LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                    return false;
                }
                PQclear(res);
                return true;
            }
            return false;
        }

        //Обновляем позицию листа
        void UpdateSheetPos(T_PlateData& PD, std::string id, int Pos)
        {
            if(IsSheet(PD))
            {
                int pos = 0;
                std::stringstream se;
                se << "SELECT pos FROM sheet WHERE id = " << id;
                PGresult* res = conn_kpvl.PGexec(se.str());
                if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
                    pos = std::stoi(conn_kpvl.PGgetvalue(res, 0, 0));
                else
                    LOG_ERR_SQL(SQLLogger, res, se.str());
                PQclear(res);

                if(pos != Pos && pos > 0 && pos < 7)
                {
                    std::stringstream sd;
                    sd << "UPDATE sheet SET";
                    sd << " pos = " << Pos;
                    sd << " WHERE";
                    sd << " id = " << id;
                    sd << ";";
                    //std::string comand = sd.str();
                    PGresult* res = conn_kpvl.PGexec(sd.str());
                    LOG_INFO(SQLLogger, "{:90}| SheetId={}, OldPos={} --> NewPos={}", FUNCTION_LINE_NAME, id, pos, Pos);
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, sd.str());
                    PQclear(res);
                }
            }
        };

        //Обновляем данные по листу если лист есть или добовляем новый
        void SheetPos(T_PlateData& PD, int pos)
        {
            std::string id = GetIdSheet(PD);

            if(id.length() && std::stoi(id) != 0)
            {
                UpdateSheetPos(PD, id, pos);

                if(pos == 1 || pos == 2)
                {
                    //std::stringstream sAlloy;
                    //sAlloy << " alloy = '" << PD.AlloyCodeText->GetString() << "'";
                    //SetUpdateSheet(&conn_kpvl, PD, sAlloy.str(), " alloy = '' AND");
                    SetUpdateSheet(&conn_kpvl, PD, " alloy = '" + PD.AlloyCodeText->GetString() + "'", " thikness = '' AND");

                    //std::stringstream sThikness;
                    //sThikness << " thikness = '" + PD.ThiknessText->GetString() + "'";
                    //SetUpdateSheet(&conn_kpvl, PD, sThikness.str(), " thikness = '' AND");
                    SetUpdateSheet(&conn_kpvl, PD, " thikness = '" + PD.ThiknessText->GetString() + "'", " thikness = '' AND");

                    SetUpdateSheet(&conn_kpvl, PD, " temper = " + GenSeqFromHmi.TempSet1->GetString(), " temper = 0 AND");
                    SetUpdateSheet(&conn_kpvl, PD, " speed = " + Par_Gen.UnloadSpeed->GetString(), " speed = 0 AND");

                    SetUpdateSheet(&conn_kpvl, PD, " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString(), "");
                    SetUpdateSheet(&conn_kpvl, PD, " prestostartcomp = " + Par_Gen.PresToStartComp->GetString(), "");

                    SetUpdateSheet(&conn_kpvl, PD, " posclapantop = " + HMISheetData.SpeedSection.Top->GetString(), " posclapantop = 0 AND");
                    SetUpdateSheet(&conn_kpvl, PD, " posclapanbot = " + HMISheetData.SpeedSection.Bot->GetString(), " posclapanbot = 0 AND");
                    SetUpdateSheet(&conn_kpvl, PD, " mask = '" + MaskKlapan + "' ", " mask = '' AND");

                    SetUpdateSheet(&conn_kpvl, PD, " lam1posclapantop = " + HMISheetData.LaminarSection1.Top->GetString(), " lam1posclapantop = 0 AND");
                    SetUpdateSheet(&conn_kpvl, PD, " lam1posclapanbot = " + HMISheetData.LaminarSection1.Bot->GetString(), " lam1posclapanbot = 0 AND");
                    SetUpdateSheet(&conn_kpvl, PD, " lam2posclapantop = " + HMISheetData.LaminarSection2.Top->GetString(), " lam2posclapantop = 0 AND");
                    SetUpdateSheet(&conn_kpvl, PD, " lam2posclapanbot = " + HMISheetData.LaminarSection2.Bot->GetString(), " lam2posclapanbot = 0 AND");
                }
            }
            else
            {
                InsertSheet(PD, pos);
            }
        }

        void DeleteNullSgeet(T_PlateData& PD, int Pos)
        {
            TSheet& TS = PalletSheet[Pos];
            if(Pos != 5 && IsSheet(PD) && IsSheet(TS))
            {
                if(PD.Melt->Val.As<int32_t>() != std::stoi(TS.Melt)
                   || PD.Pack->Val.As<int32_t>() != std::stoi(TS.Pack)
                   || PD.PartNo->Val.As<int32_t>() != std::stoi(TS.PartNo)
                   || PD.Sheet->Val.As<int32_t>() != std::stoi(TS.Sheet)
                   || PD.SubSheet->Val.As<int32_t>() != std::stoi(TS.SubSheet)
                   || PD.Slab->Val.As<int32_t>() != std::stoi(TS.Slab))
                {
                    //std::string sId = GetIdSheet(PD);
                    std::string sId = GetIdSheet(TS.Melt, TS.Pack, TS.PartNo, TS.Sheet, TS.SubSheet, TS.Slab);
                    if(atoi(sId.c_str()))
                    {
                        int iPos = -1;
                        std::string comand = "SELECT pos FROM sheet WHERE id = " + sId;
                        PGresult* res = conn_kpvl.PGexec(comand);
                        if(PQresultStatus(res) == PGRES_TUPLES_OK)
                        {
                            if(PQntuples(res))
                            {
                                std::string pos = conn_spis.PGgetvalue(res, 0, 0);
                                if(pos.length())
                                    iPos = atoi(pos.c_str());
                                PQclear(res);

                                if(iPos >= 0)
                                {
                                    int mod = (int)(std::fmod)(iPos + 10, 30);
                                    std::stringstream sd;
                                    sd << "UPDATE sheet SET Pos = ";
                                    sd << mod;
                                    sd << " WHERE news <> 1 AND ID = " << sId;
                                    comand = sd.str();
                                    res = conn_kpvl.PGexec(comand);
                                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                                        LOG_ERR_SQL(SQLLogger, res, comand);
                                    PQclear(res);
                                }

                            }
                        }
                        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        {
                            //Вывести ошибку
                            std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                            PQclear(res);
                        }

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
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
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
                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();

                std::string update = " alloy = '" + PD.AlloyCodeText->strVal + "', thikness = '" + PD.ThiknessText->strVal + "'";
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                MySetWindowText(value);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];

                MySetWindowText(value);
                SheetPos(PD, Pos);

                std::string update = " temper = " + GenSeqFromHmi.TempSet1->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " speed = " + Par_Gen.UnloadSpeed->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                SetUpdateSheet(&conn_kpvl, PD, " start_at = now()", "");

                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        };

        //Зона 2: 2 зона печи
        namespace Z2{
            const int Pos = 2;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();
                std::string update = " alloy = '" + PD.AlloyCodeText->strVal + "', thikness = '" + PD.ThiknessText->strVal + "'";
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                MySetWindowText(value);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);
                SheetPos(PD, 2);

                std::string update = " temper = " + GenSeqFromHmi.TempSet1->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " speed = " + Par_Gen.UnloadSpeed->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);

                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        };

        //Зона 3: Закалка
        namespace Z3{
            const int Pos = 3;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();
                std::string update = " alloy = '" + PD.AlloyCodeText->strVal + "', thikness = '" + PD.ThiknessText->strVal + "'";
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                MySetWindowText(value);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];;
                MySetWindowText(value);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ3_Time);
                SheetPos(PD, Pos);
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        };

        //Зона 4: Охлаждение
        namespace Z4{
            const int Pos = 4;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();
                std::string update = " alloy = '" + PD.AlloyCodeText->strVal + "', thikness = '" + PD.ThiknessText->strVal + "'";
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                MySetWindowText(value);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.SubSheet->GetValue();
                MySetWindowText(value);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ4_Time);
                SheetPos(PD, Pos);
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        };

        //Зона 5: Выдфча
        namespace Z5{
            const int Pos = 5;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();
                std::string update = " alloy = '" + PD.AlloyCodeText->strVal + "', thikness = '" + PD.ThiknessText->strVal + "'";
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                MySetWindowText(value);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.SubSheet->GetValue();
                MySetWindowText(value);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ5_Time);
                //SheetPos(PD, Pos);
                //DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        }

        //Зона 6: Кантовка
        namespace Z6{
            const int Pos = 6;
            DWORD DataTime(Value* value)
            {
                T_PlateData& PD = PlateData[6];
                if(!IsSheet(PD))
                    PD = PlateData[5];
                MySetWindowText(value);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[6];
                if(!IsSheet(PD))
                    PD = PlateData[5];
                PD.AlloyCodeText->GetValue();
                PD.ThiknessText->GetValue();
                std::string update = " alloy = '" + PD.AlloyCodeText->strVal + "', thikness = '" + PD.ThiknessText->strVal + "'";
                SetUpdateSheet(&conn_kpvl, PD, update, "");

                MySetWindowText(value);
                LocSheet(PD, Pos);

                return 0;
            }

            DWORD Data(Value* value)
            {
                MySetWindowText(value);

                T_PlateData PD = PlateData[6];
                if(!IsSheet(PD))
                    PD = PlateData[5];

                SheetPos(PD, Pos);
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData PD = PlateData[6];
                if(!IsSheet(PD))
                    PD = PlateData[5];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }

            void SetSaveDone()
            {
                T_Side Top_Side = HMISheetData.Top_Side;
                T_Side Bot_Side = HMISheetData.Bot_Side;
                T_CassetteData Cassette = HMISheetData.Cassette;

                T_PlateData PD = PlateData[6];
                if(!IsSheet(PD))
                    PD = PlateData[5];

                if(IsSheet(PD))
                {
                    std::string id = GetIdSheet(PD);
                    if(id.length() && id != "0")
                    {

#pragma region comand = "UPDATE sheet SET"
                        std::stringstream co;
                        co << "UPDATE sheet SET";
                        co << " pos = 7";
                        co << ", news = 1";
                        co << ", top1 = " << Top_Side.h1->Val.As<float>();
                        co << ", top2 = " << Top_Side.h2->Val.As<float>();
                        co << ", top3 = " << Top_Side.h3->Val.As<float>();
                        co << ", top4 = " << Top_Side.h4->Val.As<float>();
                        co << ", top5 = " << Top_Side.h5->Val.As<float>();
                        co << ", top6 = " << Top_Side.h6->Val.As<float>();
                        co << ", top7 = " << Top_Side.h7->Val.As<float>();
                        co << ", top8 = " << Top_Side.h8->Val.As<float>();
                        co << ", bot1 = " << Bot_Side.h1->Val.As<float>();
                        co << ", bot2 = " << Bot_Side.h2->Val.As<float>();
                        co << ", bot3 = " << Bot_Side.h3->Val.As<float>();
                        co << ", bot4 = " << Bot_Side.h4->Val.As<float>();
                        co << ", bot5 = " << Bot_Side.h5->Val.As<float>();
                        co << ", bot6 = " << Bot_Side.h6->Val.As<float>();
                        co << ", bot7 = " << Bot_Side.h7->Val.As<float>();
                        co << ", bot8 = " << Bot_Side.h8->Val.As<float>();
                        co << ", day = " << Cassette.Day->Val.As<int32_t>();
                        co << ", month = " << Cassette.Month->Val.As<int32_t>();
                        co << ", year = " << Cassette.Year->Val.As<int32_t>();
                        co << ", cassetteno = " << Cassette.CassetteNo->Val.As<int32_t>();
                        co << ", sheetincassette = " << (Cassette.SheetInCassette->Val.As<int16_t>() + 1);
                        co << " WHERE id = " << id << ";";
#pragma endregion

                        PGresult* res = conn_kpvl.PGexec(co.str());
                        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                            LOG_ERR_SQL(SQLLogger, res, co.str());
                        PQclear(res);
                        LOG_INFO(SQLLogger, "{:90}| SaveDone->Set_Value(true)", FUNCTION_LINE_NAME);
                        HMISheetData.SaveDone->Set_Value(true);
                        //PlateData[5].Sheet->Set_Value((int32_t)0);
                    }
                    else
                        LOG_INFO(HardLogger, "{:90}| Not Set SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());
                }
                else
                    LOG_INFO(HardLogger, "{:90}| Not Set SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());
            }

            //Новые лист в касету, Кассета наполяентся
            DWORD NewSheetData(Value* value)
            {
                const char* ss = WaitKant;
                if(value->Val.As<bool>())                   //Если лист новый
                {
                    Cassette::CassettePos(HMISheetData.Cassette);
                    SetSaveDone();
                    ss = WaitResv;
                }
                else
                    LOG_INFO(SQLLogger, "{:90}| NewData = false", FUNCTION_LINE_NAME);
                MySetWindowText(winmap(value->winId), ss);
                return 0;
            }
        }
    }

    namespace Cassette{
        O_CassetteData OldCassette;
        void SetOldCassette(T_CassetteData& CD, int32_t id)
        {
            OldCassette.Id = id;
            OldCassette.Year = CD.Year->Val.As<int32_t>();
            OldCassette.Month = CD.Month->Val.As<int32_t>();
            OldCassette.Day = CD.Day->Val.As<int32_t>();
            OldCassette.CassetteNo = CD.CassetteNo->Val.As<int32_t>();
            OldCassette.SheetInCassette = CD.SheetInCassette->Val.As<int16_t>();
        }

        //Проверка на наличие кассеты
        bool IsCassete(O_CassetteData& CD)
        {
            return CD.Day && CD.Month && CD.Year && CD.CassetteNo;
        }

        //Получаем ID кассеты
        int32_t GetIdCassette(O_CassetteData& CD)
        {
            int32_t id = 0;

            if(IsCassete(CD))
            {
                std::stringstream co;
                co << "SELECT id FROM cassette WHERE";
                co << " Day = " << CD.Day;
                co << " AND month = " << CD.Month;
                co << " AND year = " << CD.Year;
                co << " AND cassetteno = " << CD.CassetteNo;
                co << ";";
                PGresult* res = conn_kpvl.PGexec(co.str());
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))//Линий
                    {
                        id = std::stoi(conn_kpvl.PGgetvalue(res, 0, 0));
                    }
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, co.str());
                PQclear(res);
            }
            return id;

        }

        //Проверка на наличие кассеты
        bool IsCassete(T_CassetteData& CD)
        {
            int32_t Day = GetVal<int32_t>(CD.Day);
            int32_t Month = GetVal<int32_t>(CD.Month);
            int32_t Year = GetVal<int32_t>(CD.Year);
            int32_t CassetteNo = GetVal<int32_t>(CD.CassetteNo);
            int16_t SheetInCassette = GetVal<int16_t>(CD.SheetInCassette);
            return Day && Month && Year && CassetteNo && SheetInCassette;
        }

        //Получаем ID кассеты по листу Из касеты
        int32_t GetIdCassette(T_CassetteData& CD)
        {
            int32_t id = 0;

            if(IsCassete(CD))
            {
                std::stringstream co;
                co << "SELECT id FROM cassette WHERE";
                co << " Day = " << CD.Day->Val.As<int32_t>();
                co << " AND month = " << CD.Month->Val.As<int32_t>();
                co << " AND year = " << CD.Year->Val.As<int32_t>();
                co << " AND cassetteno = " << CD.CassetteNo->Val.As<int32_t>();
                co << ";";
                PGresult* res = conn_kpvl.PGexec(co.str());
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))//Линий
                    {
                        id = std::stoi(conn_kpvl.PGgetvalue(res, 0, 0));
                    }
                }
                else
                    LOG_ERR_SQL(SQLLogger, res, co.str());
                PQclear(res);
            }
            return id;
        }

        //Добовление кассеты в базу
        void InsertCassette(T_CassetteData& CD)
        {
            if(IsCassete(CD))
            {
                std::stringstream co;
                co << "INSERT INTO cassette ";
                co << "(event, year, month, day, cassetteno, sheetincassette) VALUES (1, ";
                co << CD.Year->Val.As<int32_t>() << ", ";
                co << CD.Month->Val.As<int32_t>() << ", ";
                co << CD.Day->Val.As<int32_t>() << ", ";
                co << CD.CassetteNo->Val.As<int32_t>() << ", ";
                co << CD.SheetInCassette->Val.As<int16_t>() << ");";

                PGresult* res = conn_kpvl.PGexec(co.str());
                //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, co.str());

                if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    LOG_ERR_SQL(SQLLogger, res, co.str());
                PQclear(res);

                int32_t id = GetIdCassette(CD);

                SetOldCassette(CD, id);
            }
        }

        //Обновляем в базе данные по кассете
        void UpdateCassette(T_CassetteData& CD, int32_t id)
        {
            std::stringstream co;
            co << "UPDATE cassette SET";
            co << " year = " << CD.Year->Val.As<int32_t>() << ", ";
            co << " month = " << CD.Month->Val.As<int32_t>() << ", ";
            co << " day = " << CD.Day->Val.As<int32_t>() << ", ";
            co << " cassetteno = " << CD.CassetteNo->Val.As<int32_t>() << ", ";
            co << " sheetincassette = " << CD.SheetInCassette->Val.As<int16_t>() << ",";
            co << " close_at = DEFAULT, event = 1";
            co << " WHERE id = " << id;
            co << ";";

            PGresult* res = conn_kpvl.PGexec(co.str());
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, co.str());

            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                LOG_ERR_SQL(SQLLogger, res, co.str());
            PQclear(res);
            SetOldCassette(CD, id);

        }

        //Обновляем данные по кассете если кассета есть или добовляем новую
        void CassettePos(T_CassetteData& CD)
        {
            if(IsCassete(CD))
            {
                int32_t id = GetIdCassette(CD);
                if(id)
                    UpdateCassette(CD, id);
                else
                    InsertCassette(CD);
            }
        }

        //Закрываем все не закрытые касеты кроме кассеты на кантовке
        void CloseCassete(O_CassetteData& CD)
        {
            if(IsCassete(CD))
            {
                int32_t id = GetIdCassette(CD);
                std::stringstream co;
                co << "UPDATE cassette SET close_at = now(), event = 2 WHERE close_at IS NULL AND id = " << id;

                PGresult* res = conn_kpvl.PGexec(co.str());
                if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    LOG_ERR_SQL(SQLLogger, res, co.str());
                PQclear(res);
            }
        }



        //Вывод Номер листа в касете
        DWORD Sheet_InCassette(Value* value)
        {
            char ss[256];
            sprintf_s(ss, 256, "%d", value->Val.As<int16_t>() + 1);
            MySetWindowText(winmap(value->winId), ss);
            CassettePos(HMISheetData.Cassette);
            LOG_INFO(HardLogger, "{:90}| Sheet_InCassette = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int16_t>(), OldCassette.Id);
            return 0;
        }

        //Вывод Номер кассеты за день
        DWORD CassetteNo(Value* value)
        {
            MySetWindowText(winmap(hEdit_Sheet_CassetteNo), value->GetString().c_str());
            MySetWindowText(winmap(hEdit_Sheet_CassetteNew), value->GetString().c_str());
            MySetWindowText(value);
            CassettePos(HMISheetData.Cassette);
            LOG_INFO(HardLogger, "{:90}| CassetteNo = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id);
            return 0;
        }

        //Вывод День ID листа
        DWORD CassetteDay(Value* value)
        {
            MySetWindowText(value);
            CassettePos(HMISheetData.Cassette);
            LOG_INFO(HardLogger, "{:90}| CassetteDay = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id);
            return 0;
        }

        //Вывод Месяц ID листа
        DWORD CassetteMonth(Value* value)
        {
            MySetWindowText(value);
            CassettePos(HMISheetData.Cassette);
            LOG_INFO(HardLogger, "{:90}| CassetteMonth = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id);
            return 0;
        }

        //Вывод Год ID листа
        DWORD CassetteYear(Value* value)
        {
            MySetWindowText(value);
            CassettePos(HMISheetData.Cassette);
            LOG_INFO(HardLogger, "{:90}| CassetteYear = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id);
            return 0;
        }

        //кассета наполяентся 
        // false = закраваем касету
        // true = создаем касету
        DWORD CasseteIsFill(Value* value)
        {
            MySetWindowText(value);
            bool b = value->Val.As<bool>();
            if(b)
            {
                int32_t id = GetIdCassette(HMISheetData.Cassette);

                if(!id)
                    SetOldCassette(HMISheetData.Cassette, id);
                else
                    InsertCassette(HMISheetData.Cassette);
                LOG_INFO(HardLogger, "{:90}| CasseteIsFill = true, Id = {}", FUNCTION_LINE_NAME, OldCassette.Id);
            }
            else
            {
                CloseCassete(OldCassette);
                OldCassette = O_CassetteData();
                LOG_INFO(HardLogger, "{:90}| Id = {}, Year = {}, Month = {}, Day = {}, CassetteNo = {}, SheetInCassette = {}", FUNCTION_LINE_NAME, 
                         OldCassette.Id, OldCassette.Year, OldCassette.Month, OldCassette.Day, OldCassette.CassetteNo, OldCassette.SheetInCassette);
                LOG_INFO(HardLogger, "{:90}| CasseteIsFill = false, Id = {}", FUNCTION_LINE_NAME, OldCassette.Id);
            }


            return 0;
        }

        ////кассета готова
        //DWORD CassetteIsComplete(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    LOG_INFO(HardLogger, "{:90}| CassetteIsComplete = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<bool>(), OldCassette.Id);
        //    return 0;
        //}
        //
        ////начать новую кассету
        //DWORD StartNewCassette(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    LOG_INFO(HardLogger, "{:90}| StartNewCassette = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<bool>(), OldCassette.Id);
        //    return 0;
        //}
    }
#pragma endregion


#pragma region Вспомогательные функции
    //Операции в зонах
    namespace ZState{
        //bool bThreadState = false;
        DWORD WINAPI ThreadState2(LPVOID)
        {
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, 1);
            try
            {
                PGresult* res = NULL;
                int32_t Melt = 0;
                int32_t Pack = 0;
                int32_t PartNo = 0;
                int32_t Sheet = 0;
                int32_t SubSheet = 0;
                int32_t Slab = 0;

                if(PalletSheet[2].Melt.length())Melt = atoi(PalletSheet[2].Melt.c_str());
                if(PalletSheet[2].Pack.length())Pack = atoi(PalletSheet[2].Pack.c_str());
                if(PalletSheet[2].PartNo.length())PartNo = atoi(PalletSheet[2].PartNo.c_str());
                if(PalletSheet[2].Sheet.length())Sheet = atoi(PalletSheet[2].Sheet.c_str());
                if(PalletSheet[2].SubSheet.length())SubSheet = atoi(PalletSheet[2].SubSheet.c_str());
                if(PalletSheet[2].Slab.length())Slab = atoi(PalletSheet[2].Slab.c_str());


                float Time_Z2 = GenSeqToHmi.HeatTime_Z2->GetVal<float>();
                int16_t StateNo = GenSeqToHmi.Seq_2_StateNo->GetVal<int16_t>();

                if(Time_Z2 = 0)
                {
                    time_t st;
                    std::string datatimeend_at = GetDataTimeString(st);
                    Time_Z2 = SQL::GetHeatTime_Z2(datatimeend_at);
                }
                //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, 2);

                if(Melt && Pack && PartNo && Sheet)
                {
                    LOG_INFO(SQLLogger, "{:90}| Time_Z2={}, StateNo={}, Melt={}, Pack={}, PartNo={}, Sheet={}, SubSheet={}, Slab={}", FUNCTION_LINE_NAME, Time_Z2, StateNo, Melt, Pack, PartNo, Sheet, SubSheet, Slab);
                    std::stringstream ss1;
                    ss1 << "UPDATE sheet SET ";
                    ss1 << "datatime_end = now()";
                    if(Time_Z2)
                        ss1 << ", datatime_all = " << Time_Z2;

                    ss1 << " WHERE";
                    ss1 << " datatime_end IS NULL";
                    ss1 << " AND melt = " << Melt;
                    ss1 << " AND pack = " << Pack;
                    ss1 << " AND partno = " << PartNo;
                    ss1 << " AND sheet = " << Sheet;
                    //ss << " AND subsheet = " << SubSheet;
                    //ss <<  " AND slab = " << Slab;
                    ss1 << ";";

                    res = conn_dops.PGexec(ss1.str());
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss1.str());
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, ss1.str());
                    PQclear(res);

                    std::stringstream ss2;
                    ss2 << "UPDATE sheet SET ";
                    ss2 << "lampresstop = " + AI_Hmi_210.LaminPressTop->GetString() + ", ";
                    ss2 << "lampressbot = " + AI_Hmi_210.LaminPressBot->GetString() + " ";
                    ss2 << "WHERE";
                    ss2 << " melt = " << Melt;
                    ss2 << " AND pack = " << Pack;
                    ss2 << " AND partno = " << PartNo;
                    ss2 << " AND sheet = " << Sheet;
                    //ss << " AND subsheet = " << SubSheet;
                    //ss <<  " AND slab = " << Slab;
                    ss2 << ";";


                    res = conn_dops.PGexec(ss2.str());
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss2.str());
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, ss2.str());
                    PQclear(res);

                    int r = 0;
                    while(isRun && r++ < 5) //5 секунд
                    {
                        SetWindowText(winmap(hEditState_34), std::to_string(r).c_str());
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }

                    std::string lam_te1 = AI_Hmi_210.LAM_TE1->GetString();
                    std::string za_te3 = AI_Hmi_210.Za_TE3->GetString();
                    std::string za_pt3 = AI_Hmi_210.Za_PT3->GetString();

                    //Time_Z2 = GenSeqToHmi.HeatTime_Z2->GetVal<float>();
                    //StateNo = GenSeqToHmi.Seq_2_StateNo->GetVal<int16_t>();

                    LOG_INFO(SQLLogger, "{:90}| lam_te1={}, za_te3={}, za_pt3={}, StateNo={}, Melt={}, Pack={}, PartNo={}, Sheet={}, SubSheet={}, Slab={}", FUNCTION_LINE_NAME, lam_te1, za_te3, za_pt3, StateNo, Melt, Pack, PartNo, Sheet, SubSheet, Slab);

                    std::stringstream ss4;
                    ss4 << "UPDATE sheet SET ";
                    ss4 << "lam_te1 = " << lam_te1 << ", ";
                    ss4 << "za_te3 = " << za_te3 << ", ";
                    ss4 << "za_pt3 = " << za_pt3 << " ";
                    ss4 << "WHERE ";
                    ss4 << "melt = " << Melt;
                    ss4 << " AND pack = " << Pack;
                    ss4 << " AND partno = " << PartNo;
                    ss4 << " AND sheet = " << Sheet;
                    //ss4 << " AND subsheet = " << SubSheet;
                    //ss4 << " AND slab = " << Slab; 
                    ss4 << ";";


                    res = conn_dops.PGexec(ss4.str());
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss4.str());

                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, ss4.str());
                    PQclear(res);
                }
                else
                    LOG_INFO(SQLLogger, "{:90}| Time_Z2={}, StateNo={}, Melt={}, Pack={}, PartNo={}, Sheet={}, SubSheet={}, Slab={}", FUNCTION_LINE_NAME, Time_Z2, StateNo, Melt, Pack, PartNo, Sheet, SubSheet, Slab);

                hThreadState2 = NULL;
            }
            catch(...)
                LOG_ERROR(SQLLogger, "{:90}| Unknown error", FUNCTION_LINE_NAME);

            return 0;
        }

        //Операция в 1 зоне
        DWORD DataPosState_1(Value* value)
        {
            int16_t val = value->Val.As<int16_t>();
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
            int16_t val = value->Val.As<int16_t>();
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
            SetWindowText(winmap(hEditState_32), GenSeq3[GetVal<int16_t>(value)].c_str());

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

            if(value->Val.As<float>() > 0)
            {
                std::string update = " temper = " + value->GetString();
                Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
                Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");
            }
            return 0;
        }

        //Скорость выгрузки
        DWORD UnloadSpeed(Value* value)
        {
            MySetWindowText(value);

            if(value->Val.As<float>() > 0)
            {
                std::string update = " speed = " + value->GetString();
                Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
                Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");
            }
            return 0;
        }

        //Время сигнализации окончания нагрева, мин
        DWORD fTimeForPlateHeat(Value* value)
        {
            MySetWindowText(value);

            std::string update = " timeforplateheat = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");
            return 0;
        }

        //Уставка давления для запуска комперссора
        DWORD fPresToStartComp(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " prestostartcomp = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");

            return 0;
        }

    };

    //Режим работы клапана
    //State_1 = 3 
    //Маска клапанов
    namespace Mask{
        DWORD DataMaskKlapan1(Value* value)
        {
            int16_t v = value->Val.As<uint16_t>();
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
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");

            return 0;
        }
        DWORD DataMaskKlapan2(Value* value)
        {
            int16_t v = value->Val.As<uint16_t>();
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
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");

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
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, " posclapantop = 0 AND");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, " posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD SpeedSectionBot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, " posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, " posclapanbot = 0 AND");
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
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, " lam1posclapantop = 0 AND");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, " lam1posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD LaminarSection1Bot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam1posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, " lam1posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, " lam1posclapanbot = 0 AND");
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
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, " lam2posclapantop = 0 AND");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, " lam2posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD LaminarSection2Bot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam2posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[1], update, " lam2posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[2], update, " lam2posclapanbot = 0 AND");
            return 0;
        }
    };

    //Отклонения листа на кантовке
    namespace Side{
        DWORD SheetTop1(Value* value)
        {
            std::string update = " top1 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop2(Value* value)
        {
            std::string update = " top2 = " + value->GetString();
            
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop3(Value* value)
        {
            std::string update = " top3 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop4(Value* value)
        {
            std::string update = " top4 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop5(Value* value)
        {
            std::string update = " top5 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop6(Value* value)
        {
            std::string update = " top6 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop7(Value* value)
        {
            std::string update = " top7 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop8(Value* value)
        {
            std::string update = " top8 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }

        DWORD SheetBot1(Value* value)
        {
            std::string update = " bot1 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot2(Value* value)
        {
            std::string update = " bot2 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot3(Value* value)
        {
            std::string update = " bot3 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot4(Value* value)
        {
            std::string update = " bot4 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot5(Value* value)
        {
            std::string update = " bot5 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot6(Value* value)
        {
            std::string update = " bot6 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot7(Value* value)
        {
            std::string update = " bot7 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot8(Value* value)
        {
            std::string update = " bot8 = " + value->GetString();
            Sheet::SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
    }
    
    //Бит жмизни
    namespace WDG {
        DWORD SheetData_WDG_toBase(Value* value)
        {
            if(GetVal<bool>(value))
            {
                PLC_KPVL_old_dt = time(NULL);
                struct tm TM;
                localtime_s(&TM, &PLC_KPVL_old_dt);
                SetWindowText(winmap(value->winId), string_time(&TM).c_str());
            }
            return 0;
        }
    }


#pragma endregion
};

