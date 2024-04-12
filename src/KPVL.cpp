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
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sMaxId);
            }
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
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, FilterComand.str());
            }
            PQclear(res);

            //AddHistoriSheet(true, (int)AllSheet.size());
            int t = 0;
        }
    }


//Функции обработки со сохранения истории листов
#pragma region Функции обработки со сохранения истории листов

    std::string ServerDataTime = "";

    uint32_t NextID = 1;

    void GetNextID()
    {
        std::string Test = "SELECT max(id) FROM tag";
        PGresult* res = conn_kpvl.PGexec(Test);
        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, Test);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            if(PQntuples(res))
                NextID = (uint32_t)atol(conn_kpvl.PGgetvalue(res, 0, 0).c_str());
        }
        else
        {
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, Test);
            PQclear(res);
        }
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
            PalletSheet[Pos].Pack =PlateData[Pos].Pack->GetString();
            PalletSheet[Pos].Sheet =PlateData[Pos].Sheet->GetString();
            PalletSheet[Pos].SubSheet =PlateData[Pos].SubSheet->GetString();
        }
        else
        {
            PalletSheet[Pos].Clear();
        }
    }

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
    std::string GetIdCassette(T_CassetteData& CD)
    {
        std::string id = "0";

        if(IsCassete(CD))
        {
            std::string comand = "SELECT id FROM cassette WHERE";
            comand += " Day = " + CD.Day->GetString();
            comand += " AND month = " + CD.Month->GetString();
            comand += " AND year = " + CD.Year->GetString();
            comand += " AND cassetteno = " + CD.CassetteNo->GetString();
            comand += ";";
            PGresult* res = conn_kpvl.PGexec(comand);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                if(PQntuples(res))//Линий
                {
                    id = conn_kpvl.PGgetvalue(res, 0, 0);
                }
            }
            else
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
            PQclear(res);
        }
        return id;
    }

    int GetCountSheetInCassete(T_CassetteData& CD)
    {
        int cound = 0;

        if(IsCassete(CD))
        {
            std::string comand = "SELECT count(*) FROM sheet WHERE ";
            comand += "day = '" + CD.Day->GetString() + "'";
            comand += " AND month = '" + CD.Month->GetString() + "'";
            comand += " AND year = '" + CD.Year->GetString() + "'";
            comand += " AND cassetteno = '" + CD.CassetteNo->GetString() + "'";
            PGresult* res = conn_kpvl.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            if(PQresultStatus(res) != PGRES_FATAL_ERROR && PQntuples(res))
            {
                cound = atoi(conn_kpvl.PGgetvalue(res, 0, 0).c_str());
            }
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
            PQclear(res);
        }
        return cound;
    }

    //Закрываем все не закрытые касеты кроме кассеты на кантовке
    void CloseCassete(T_CassetteData& CD)
    {
        if(IsCassete(CD))
        {
            //int count = GetCountSheetInCassete(CD);
            std::string comand = "UPDATE cassette SET";
            comand += " close_at = now(), event = 2 "; //CURRENT_TIMESTAMP
            //comand += " sheetincassette = " + std::to_string(count);
            comand += " WHERE close_at IS NULL AND NOT ";
            comand += " (day = " + CD.Day->GetString();
            comand += " AND month = " + CD.Month->GetString();
            comand += " AND year = " + CD.Year->GetString();
            comand += " AND cassetteno = " + CD.CassetteNo->GetString();
            comand += ");";

            PGresult* res = conn_kpvl.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
            PQclear(res);
        }
    }

    bool IsSheet(T_PlateData& PD)
    {
        int32_t Melt = PD.Melt->Val.As<int32_t>();
        int32_t Pack = PD.Pack->Val.As<int32_t>();
        int32_t PartNo = PD.PartNo->Val.As<int32_t>();
        int32_t Sheet = PD.Sheet->Val.As<int32_t>();
        int32_t SubSheet = PD.SubSheet->Val.As<int32_t>();
        int32_t Slab = PD.Slab->Val.As<int32_t>();
        return Melt && Pack && PartNo && Sheet/* && SubSheet*/;
    }

    bool IsSheet0(T_PlateData& PD)
    {
        int32_t Melt = GetVal<int32_t>(PD.Melt);
        int32_t Pack = GetVal<int32_t>(PD.Pack);
        int32_t PartNo = GetVal<int32_t>(PD.PartNo);
        return Melt && Pack && PartNo;
    }

    //Получаем ID листа
    std::string GetIdSheet(T_PlateData& PD)
    {
        std::string id = "0";

        if(IsSheet(PD))
        {
            std::stringstream sd;
            sd << "SELECT id FROM sheet WHERE";
            sd << " melt = " << PD.Melt->Val.As<int32_t>();
            sd << " AND pack = " << PD.Pack->Val.As<int32_t>();
            sd << " AND partno = " << PD.PartNo->Val.As<int32_t>();
            sd << " AND sheet = " << PD.Sheet->Val.As<int32_t>();
            sd << " AND subsheet = " << PD.SubSheet->Val.As<int32_t>();
            sd << " AND slab = " << PD.Slab->Val.As<int32_t>();
            std::string comand = sd.str();

            //std::string comand = "SELECT id FROM sheet WHERE";
            //comand += " melt = " + PD.Melt->Val.As<int32_t>();
            //comand += " AND pack = " + PD.Pack->GetString();
            //comand += " AND partno = " + PD.PartNo->GetString();
            //comand += " AND sheet = " + PD.Sheet->GetString();
            //comand += " AND subsheet = " + PD.SubSheet->GetString();
            //comand += " AND slab = " + PD.Slab->GetString();
            //comand += ";";
            PGresult* res = conn_kpvl.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                if(PQntuples(res))//Линий
                {
                    id = conn_kpvl.PGgetvalue(res, 0, 0);
                }
            }
            else
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
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
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                if(PQntuples(res))//Линий
                {
                    id = conn_kpvl.PGgetvalue(res, 0, 0);
                }
            }
            else
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
            PQclear(res);
        }
        return id;
    }

    //Получаем ID листа
    std::string GetIdSheet0(T_PlateData& PD)
    {
        std::string id = "0";

        if(IsSheet0(PD))
        {
            std::string comand = "SELECT id FROM sheet WHERE";
            comand += " melt = " + PD.Melt->GetString();
            comand += " AND pack = " + PD.Pack->GetString();
            comand += " AND partno = " + PD.PartNo->GetString();
            comand += ";";
            PGresult* res = conn_kpvl.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                if(PQntuples(res))//Линий
                {
                    id = conn_kpvl.PGgetvalue(res, 0, 0);
                }
            }
            else
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
            PQclear(res);
        }
        return id;
    }

        //DWORD DataAlloyCodeThikness(T_PlateData& PlateData, HWNDCLIENT hWnhAlloy, HWNDCLIENT hWnhThikness)
        //{
        //    std::string AlloyCode = GetVal<std::string>(PlateData.AlloyCodeText);
        //    int32_t Thikness =  PlateData.Thikness.Val;
        //    std::string strAlloyCode = "";
        //    std::string strThikness =  "";
        //
        //    GetAlloyThicknes(AlloyCode, Thikness, strAlloyCode, strThikness);
        //
        //    PlateData.AlloyCode.strVal = strAlloyCode;
        //    PlateData.Thikness.strVal  = strThikness;
        //
        //    SetWindowText(winmap(hWnhAlloy), strAlloyCode.c_str());
        //    SetWindowText(winmap(hWnhThikness), strThikness.c_str());
        //    return 0;
        //}

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
                {
                    std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                    LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                    LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                }
                PQclear(res);
            }
        }
    }

    //Обновлякем в базе данные по листу
    void UpdateSheetPos(T_PlateData& PD, std::string id, int Pos)
    {
        if(IsSheet(PD))
        {
            std::stringstream sd;
            sd << "UPDATE sheet SET";
            sd << " pos = "  << Pos;
            sd << " WHERE";
            sd << " id = " << id;
            //sd << " melt = " << PD.Melt->GetString();
            //sd << " AND pack = " << PD.Pack->GetString();
            //sd << " AND partno = " << PD.PartNo->GetString();
            //sd << " AND sheet = " << PD.Sheet->GetString();
            //sd << " AND subsheet = " << PD.SubSheet->GetString();
            //sd << " AND slab = " << PD.Slab->GetString();
            sd << ";";
            std::string comand = sd.str();
            PGresult* res = conn_kpvl.PGexec(comand);

            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
            PQclear(res);
        }
    };

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
                //Вывести ошибку
                std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                //SetWindowText(winmap(HWNDCLIENT::hStatPlate_DataZ0), "PGRES_FATAL_ERROR");
                PQclear(res);
                return false;
            }
            PQclear(res);
            return true;
        }
        //else
        //    SetWindowText(winmap(HWNDCLIENT::hStatPlate_DataZ0), "");

        return false;
    }

    bool SetUpdateSheet2(PGConnection* con, T_PlateData& PD, std::string update, std::string where)
    {
        if(!con) return false;
        if(IsSheet(PD))
        {
            std::stringstream sd;
            sd << "UPDATE sheet SET" << update;
            sd << " WHERE " << where;
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
                //Вывести ошибку
                std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PQclear(res);
                return false;
            }
            PQclear(res);
            return true;

        }
        return false;
    }


    bool SetUpdateSheet3(PGConnection* con, T_PlateData* PD, std::string update, std::string where)
    {
        if(!con) return NULL;
        if(IsSheet(*PD))
        {
            std::stringstream sd;
            sd << "UPDATE sheet SET" << update;
            sd << " WHERE" << where;
            sd << " melt = " << PD->Melt->Val.As<int32_t>();
            sd << " AND partno = " << PD->PartNo->Val.As<int32_t>();
            sd << " AND pack = " << PD->Pack->Val.As<int32_t>();
            sd << " AND sheet = " << PD->Sheet->Val.As<int32_t>();
            sd << " AND subsheet = " << PD->SubSheet->Val.As<int32_t>();
            sd << " AND slab = " << PD->Slab->Val.As<int32_t>();
            sd << ";";
            std::string comand = sd.str();
            PGresult* res = con->PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            {
                //Вывести ошибку
                std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PQclear(res);
                return NULL;
            }
            PQclear(res);
            return PD;
        }
        return NULL;
    }

    void SheetPos(T_PlateData& PD, int pos)
    {
        {
            std::string id = GetIdSheet(PD);

            if(id.length() && atoi(id.c_str()) != 0)
            {
                UpdateSheetPos(PD, id, pos);

                if(pos == 1 || pos == 2)
                {
                    char ss[256];
                    sprintf_s(ss, 255, " alloy = \x27%s\x27", PD.AlloyCodeText->GetString().c_str());
                    SetUpdateSheet2(&conn_kpvl, PD, ss, " alloy = \x27\x27 AND");

                    sprintf_s(ss, 255, " thikness = \x27%s\x27", PD.ThiknessText->GetString().c_str());
                    SetUpdateSheet2(&conn_kpvl, PD, ss, " thikness = \x27\x27 AND");

                    SetUpdateSheet2(&conn_kpvl, PD, " temper = " + GenSeqFromHmi.TempSet1->GetString(), " temper = 0 AND");
                    SetUpdateSheet2(&conn_kpvl, PD, " speed = " + Par_Gen.UnloadSpeed->GetString(), " speed = 0 AND");

                    SetUpdateSheet2(&conn_kpvl, PD, " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString(), "");
                    SetUpdateSheet2(&conn_kpvl, PD, " prestostartcomp = " + Par_Gen.PresToStartComp->GetString(), "");

                    SetUpdateSheet2(&conn_kpvl, PD, " posclapantop = " + HMISheetData.SpeedSection.Top->GetString(), " posclapantop = 0 AND");
                    SetUpdateSheet2(&conn_kpvl, PD, " posclapanbot = " + HMISheetData.SpeedSection.Bot->GetString(), " posclapanbot = 0 AND");
                    SetUpdateSheet2(&conn_kpvl, PD, " mask = \x27" + MaskKlapan + "\x27 ", " mask = \'\' AND");

                    SetUpdateSheet2(&conn_kpvl, PD, " lam1posclapantop = " + HMISheetData.LaminarSection1.Top->GetString(), " lam1posclapantop = 0 AND");
                    SetUpdateSheet2(&conn_kpvl, PD, " lam1posclapanbot = " + HMISheetData.LaminarSection1.Bot->GetString(), " lam1posclapanbot = 0 AND");
                    SetUpdateSheet2(&conn_kpvl, PD, " lam2posclapantop = " + HMISheetData.LaminarSection2.Top->GetString(), " lam2posclapantop = 0 AND");
                    SetUpdateSheet2(&conn_kpvl, PD, " lam2posclapanbot = " + HMISheetData.LaminarSection2.Bot->GetString(), " lam2posclapanbot = 0 AND");
                }
            }
            else
            {
                InsertSheet(PD, pos);
            }
        }
        //InsertPos(pos, "");
    }

    void InsertCassette(T_CassetteData& CD)
    {
        if(IsCassete(CD))
        {
            std::string Test = std::string("SELECT id FROM cassette WHERE") +
                " day = " + CD.Day->GetString() + " AND " +
                " month = " + CD.Month->GetString() + " AND " +
                " year = " + CD.Year->GetString() + " AND " +
                " cassetteno = " + CD.CassetteNo->GetString() + ";";
            PGresult* res = conn_kpvl.PGexec(Test);

            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                if(PQntuples(res))
                {
                    PQclear(res);
                }
                else
                {
                    PQclear(res);
            //INSERT INTO cassette (day, month, year, cassetteno, sheetincassette) VALUE (11, 12, 2023, 4, 16);
                    std::string comand = "INSERT INTO cassette ";
                    comand += "(day, month, year, cassetteno, sheetincassette, event) VALUES (";
                    comand += CD.Day->GetString() + ", ";
                    comand += CD.Month->GetString() + ", ";
                    comand += CD.Year->GetString() + ", ";
                    comand += CD.CassetteNo->GetString() + ", ";
                    comand += CD.SheetInCassette->GetString() + ", 1);";
                    res = conn_kpvl.PGexec(comand);
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    {
                        //boost::replace_all(errc, "\n", "\r\n");
                        std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                        PQclear(res);
                    }
                }
            }
            else
            {
                std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, Test);
                PQclear(res);
            }
        }
    }


    void UpdateCassette(T_CassetteData& CD, std::string id)
    {
        std::string comand = "UPDATE cassette SET";
        comand += " sheetincassette = " + CD.SheetInCassette->GetString();
        comand += ", close_at = DEFAULT, event = 1";
        comand += " WHERE id = " + id;
        comand += ";";

        PGresult* res = conn_kpvl.PGexec(comand);
        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
            //boost::replace_all(errc, "\n", "\r\n");
            std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PQclear(res);
        }

    }


    void CassettePos(T_CassetteData& CD)
    {

        if(IsCassete(CD))
        {
            std::string id = GetIdCassette(CD);
            if(id.length() && atoi(id.c_str()) != 0)
                UpdateCassette(CD, id);
            else
                InsertCassette(CD);
        }

        //Закрываем все не закрытые касеты
    }




    bool TestIDSheet(T_PlateData& PD, int& Melt, int& Pack, int& PartNo, int& Sheet, int& SubSheet, int& Slab)
    {
        if(IsSheet(PD))
        {
            Melt = GetVal<int32_t>(PD.Melt);
            Pack = GetVal<int32_t>(PD.Pack);
            PartNo = GetVal<int32_t>(PD.PartNo);
            Sheet = GetVal<int32_t>(PD.Sheet);
            SubSheet = GetVal<int32_t>(PD.SubSheet);
            Slab = GetVal<int32_t>(PD.Slab);

            return true;
        }
        return false;
    }





    void InsertSheetPos0(T_PlateData& PD)
    {
        if(IsSheet0(PD))
        {
            std::stringstream ds;
            ds << "INSERT INTO sheet ";
            ds << "(";
            ds << "pos, ";
            ds << "alloy, ";
            ds << "thikness, ";
            ds << "melt, ";
            ds << "partno, ";
            ds << "pack, ";
            ds << "sheet, ";
            ds << "subsheet, ";
            ds << "slab";
            ds << ") VALUES (0, ";
            ds << "'" << PD.AlloyCodeText->GetString() << "', ";
            ds << "'" << PD.ThiknessText->GetString() << "', ";

            ds << PD.Melt->Val.As<int32_t>() << ", ";
            ds << PD.PartNo->Val.As<int32_t>() << ", ";
            ds << PD.Pack->Val.As<int32_t>() << ", ";
            ds << PD.Sheet->Val.As<int32_t>() << ", ";
            ds << PD.SubSheet->Val.As<int32_t>() << ", ";
            ds << PD.Slab->Val.As<int32_t>() << ");";
            std::string comand = ds.str();
            PGresult* res = conn_kpvl.PGexec(comand);
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            {
                //Вывести ошибку
                std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            }
            PQclear(res);
        }
    }


    void UpdateSheetPos0(T_PlateData& PD)
    {
        if(IsSheet(PD))
        {
            std::string sId = GetIdSheet0(PD);
            if(sId == "0")
            {

                std::stringstream ds;
                ds << " pos = 0";
                ds << ", alloy = " << PD.AlloyCodeText->GetString();
                ds << ", thikness = " << PD.ThiknessText->GetString();
                ds << ", melt = " << PD.Melt->Val.As<int32_t>();
                ds << ", partno = " << PD.PartNo->Val.As<int32_t>();
                ds << ", pack = " << PD.Pack->Val.As<int32_t>();
                ds << ", sheet = " << PD.Sheet->Val.As<int32_t>();
                ds << ", subsheet = " << PD.SubSheet->Val.As<int32_t>();
                ds << ", slab = " << PD.Slab->Val.As<int32_t>();
                ds << " WHERE id = " << sId;
                ds << ";";
                std::string comand = ds.str();
                PGresult* res = conn_kpvl.PGexec(comand);
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                {
                    //Вывести ошибку
                    std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                    LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                    LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                }
                PQclear(res);
            }
        }

    }

    void UpdateSheet0()
    {
        T_PlateData& PD = PlateData[0];
        if(IsSheet(PD))
        {
            //DataAlloyCodeThikness(PD, HWNDCLIENT::hEditPlate_DataZ1_AlloyCode, HWNDCLIENT::hEditPlate_DataZ1_Thikness);
            std::string id = GetIdSheet(PD);
            if(id.length() && atoi(id.c_str()) != 0)
                UpdateSheetPos0(PD);
            else
                InsertSheetPos0(PD);
        }
    }


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

    //DWORD DataPosBool(Value* value)
    //{
    //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //    return 0;
    //}
#pragma endregion

//Зоны печи закалки
#pragma region Зоны печи закалки

    void DeleteNullSgeet(T_PlateData& PD, int Pos)
    {
        if(IsSheet(PD))
        {
            if(PD.Melt->Val.As<int32_t>() != std::stoi(PalletSheet[Pos].Melt)
               || PD.Pack->Val.As<int32_t>() != std::stoi(PalletSheet[Pos].Pack)
               || PD.PartNo->Val.As<int32_t>() != std::stoi(PalletSheet[Pos].PartNo)
               || PD.Sheet->Val.As<int32_t>() != std::stoi(PalletSheet[Pos].Sheet)
               || PD.SubSheet->Val.As<int32_t>() != std::stoi(PalletSheet[Pos].SubSheet)
               || PD.Slab->Val.As<int32_t>() != std::stoi(PalletSheet[Pos].Slab))
            {
                //std::string sId = GetIdSheet(PD);
                std::string sId = GetIdSheet(PalletSheet[Pos].Melt, PalletSheet[Pos].Pack, PalletSheet[Pos].PartNo, PalletSheet[Pos].Sheet, PalletSheet[Pos].SubSheet, PalletSheet[Pos].Slab);
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
                                std::stringstream sd ;
                                sd << "UPDATE sheet SET Pos = ";
                                sd << mod;
                                sd << " WHERE ID = " << sId;
                                comand = sd.str();
                                res = conn_kpvl.PGexec(comand);
                                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                                if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                                {
                                    //Вывести ошибку
                                    std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                                    LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                                    LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                                }
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
                //command += " 
                //command += " melt = " + PalletSheet[Pos].Melt;
                //command += " AND pack = " + PalletSheet[Pos].Pack;
                //command += " AND partno = " + PalletSheet[Pos].PartNo;
                //command += " AND sheet = " + PalletSheet[Pos].Sheet;
                //command += " AND subsheet = " + PalletSheet[Pos].SubSheet;
                //command += " AND slab = " + PalletSheet[Pos].Slab;
                //command += ";";            }
            }
        }
    }
        //Зона 0: На входе в печь
        namespace Z0{

            const int Pos = 0;
            
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.SubSheet->GetValue();
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                UpdateSheet0();
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ0_Time);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                return 0;
            }
        }

        //Зона 1: 1 зона печи 
        namespace Z1{
            const int Pos = 1;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.SubSheet->GetValue();

                MySetWindowText(winmap(value->winId), value->GetString().c_str());
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

                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                return 0;
            }
        };

        //Зона 2: 2 зона печи
        namespace Z2{
            const int Pos = 2;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.SubSheet->GetValue();
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
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
                return 0;
            }
        }

        //Зона 3: Закалка
        namespace Z3{
            const int Pos = 3;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];;
                PD.SubSheet->GetValue();
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ3_Time);

                LocSheet(PD, Pos);
                DeleteNullSgeet(PD, Pos);
                SheetPos(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                return 0;
            }
        }

        //Зона 4: Охлаждение
        namespace Z4{
            const int Pos = 4;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.SubSheet->GetValue();
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
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
                return 0;
            }
        }

        //Зона 5: Выдфча
        namespace Z5{
            const int Pos = 5;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                PD.SubSheet->GetValue();
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ5_Time);
                //SheetPos(PD, Pos);
                DeleteNullSgeet(PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(&conn_kpvl, PD, update, "");
                return 0;
            }
        }

        //Зона 6: Кантовка
        namespace Z6{
            const int Pos = 6;
            DWORD DataTime(Value* value)
            {
                T_PlateData& PD = PlateData[5];
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[5];
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[5];
                PD.SubSheet->GetValue();
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
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
                return 0;
            }


            //Вывод Номер кассеты за день
            DWORD CassetteNo(Value* value)
            {
                MySetWindowText(winmap(hEdit_Sheet_CassetteNo), value->GetString().c_str());
                MySetWindowText(winmap(hEdit_Sheet_CassetteNew), value->GetString().c_str());
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                CassettePos(HMISheetData.Cassette);
                CloseCassete(HMISheetData.Cassette);
                return 0;
            }

            //Вывод День ID листа
            DWORD InitCassette(Value* value)
            {
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                CassettePos(HMISheetData.Cassette);
                CloseCassete(HMISheetData.Cassette);
                return 0;
            }

            //Вывод Номер листа в касете
            DWORD Sheet_InCassette(Value* value)
            {
                char ss[256];
                sprintf_s(ss, 256, "%d", atoi(value->GetString().c_str()) + 1);
                MySetWindowText(winmap(value->winId), ss);
                CassettePos(HMISheetData.Cassette);

                return 0;
            }

            void SetSaveDone()
            {
                T_PlateData PD = PlateData[6];
                T_Side Top_Side = HMISheetData.Top_Side;
                T_Side Bot_Side = HMISheetData.Bot_Side;
                T_CassetteData Cassette = HMISheetData.Cassette;

                if(!IsSheet(PD))
                    PD = PlateData[5];

                if(IsSheet(PD))
                {
                    std::string id = GetIdSheet(PD);
                    if(id.length() && id != "0")
                    {
#pragma region comand = "UPDATE sheet SET"
                        std::string comand = "UPDATE sheet SET";
                        comand += " pos = 7";
                        comand += ", news = 1";
                        comand += ", top1 = " + Top_Side.h1->GetString();
                        comand += ", top2 = " + Top_Side.h2->GetString();
                        comand += ", top3 = " + Top_Side.h3->GetString();
                        comand += ", top4 = " + Top_Side.h4->GetString();
                        comand += ", top5 = " + Top_Side.h5->GetString();
                        comand += ", top6 = " + Top_Side.h6->GetString();
                        comand += ", top7 = " + Top_Side.h7->GetString();
                        comand += ", top8 = " + Top_Side.h8->GetString();
                        comand += ", bot1 = " + Bot_Side.h1->GetString();
                        comand += ", bot2 = " + Bot_Side.h2->GetString();
                        comand += ", bot3 = " + Bot_Side.h3->GetString();
                        comand += ", bot4 = " + Bot_Side.h4->GetString();
                        comand += ", bot5 = " + Bot_Side.h5->GetString();
                        comand += ", bot6 = " + Bot_Side.h6->GetString();
                        comand += ", bot7 = " + Bot_Side.h7->GetString();
                        comand += ", bot8 = " + Bot_Side.h8->GetString();
                        comand += ", day = " + Cassette.Day->GetString();
                        comand += ", month = " + Cassette.Month->GetString();
                        comand += ", year = " + Cassette.Year->GetString();
                        comand += ", cassetteno = " + Cassette.CassetteNo->GetString();
                        comand += ", sheetincassette = " + std::to_string(Cassette.SheetInCassette->Val.As<int16_t>() + 1);
                        comand += " WHERE id = " + id + ";";
#pragma endregion

                        PGresult* res = conn_kpvl.PGexec(comand);
                        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                        //LOG_INFO(HardLogger, "{:90}| SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());

                        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        {
                            std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                        }
                        PQclear(res);
                    }
                    else 
                    {
                        LOG_INFO(HardLogger, "{:90}| Not Set SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());
                    }
                }
                else
                {
                    LOG_INFO(HardLogger, "{:90}| Not Set SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());
                }

                HMISheetData.SaveDone->Set_Value(true);
            }

            //Новые лист в касету, Кассета наполяентся
            DWORD NewSheetData(Value* value)
            {
                const char* ss = WaitKant;
                if(value->Val.As<bool>())                   //Если лист новый
                {
                    LOG_INFO(SQLLogger, "{:90}| SaveDone->Set_Value(true)", FUNCTION_LINE_NAME);
                    SetSaveDone();
                    ss = WaitResv;
                }
                else
                    LOG_INFO(SQLLogger, "{:90}| NewData = false", FUNCTION_LINE_NAME);
                MySetWindowText(winmap(value->winId), ss);
                return 0;
            }
        }
    ////Начать новую кассету
    //DWORD CassetteIsFill(Value* value)
    //{
    //    const char* ss1 = "Кассета готова";
    //    const char* ss2 = "Начать кассету";
    //
    //    if(HMISheetData.Sheet.CasseteIsFill.Val)
    //    {
    //        ss1 = "Кассета набирается";
    //        ss2 = "Закрыть кассету";
    //    }
    //    MySetWindowText(winmap(hStatSheet_CassetteIsFull), ss1);
    //
    //    //MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //    return 0;
    //}
#pragma endregion


//Вспомогательные функции
#pragma region Вспомогательные функции
    //Операции в зонах
    namespace ZState{
        //bool bThreadState = false;
        DWORD WINAPI ThreadState2(LPVOID)
        {
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, 1);
            try
            {
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

                //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, 2);

                if(Melt && Pack && PartNo && Sheet)
                {
                    LOG_INFO(SQLLogger, "{:90}| Time_Z2={}, StateNo={}, Melt={}, Pack={}, PartNo={}, Sheet={}, SubSheet={}, Slab={}", FUNCTION_LINE_NAME, Time_Z2, StateNo, Melt, Pack, PartNo, Sheet, SubSheet, Slab);
                    if(Time_Z2 && StateNo == 5)
                    {
                        std::stringstream ss1;
                        ss1 << "UPDATE sheet SET ";
                        ss1 << "datatime_end = now(), ";
                        ss1 << "datatime_all = " << GenSeqToHmi.HeatTime_Z2->GetString();
                        ss1 << " WHERE";
                        ss1 << " datatime_all = 0";
                        ss1 << " AND melt = " << Melt;
                        ss1 << " AND pack = " << Pack;
                        ss1 << " AND partno = " << PartNo;
                        ss1 << " AND sheet = " << Sheet;
                        //ss << " AND subsheet = " << SubSheet;
                        //ss <<  " AND slab = " << Slab;
                        ss1 << ";";

                        PGresult* res = conn_dops.PGexec(ss1.str());
                        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss1.str());
                        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        {
                            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss1.str());
                        }
                        PQclear(res);
                    }

                    std::stringstream ss2;
                    ss2 << "UPDATE sheet SET ";
                    ss2 << "lampresstop = " + AI_Hmi_210.LaminPressTop->GetString() + ", ";
                    ss2 << "lampressbot = " + AI_Hmi_210.LaminPressBot->GetString() + " ";
                    ss2 << "WHERE ";
                    ss2 << "melt = " << Melt;
                    ss2 << " AND pack = " << Pack;
                    ss2 << " AND partno = " << PartNo;
                    ss2 << " AND sheet = " << Sheet;
                    //ss << " AND subsheet = " << SubSheet;
                    //ss <<  " AND slab = " << Slab;
                    ss2 << ";";


                    PGresult* res = conn_dops.PGexec(ss2.str());
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss2.str());
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    {
                        //Вывести ошибку
                        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss2.str());
                    }
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

                    if(Time_Z2 && StateNo == 5)
                    {
                        std::stringstream ss3;
                        ss3 << "UPDATE sheet SET ";
                        ss3 << "datatime_end = now(), ";
                        ss3 << "datatime_all = " << GenSeqToHmi.HeatTime_Z2->GetString();
                        ss3 << " WHERE";
                        ss3 << " datatime_all = 0";
                        ss3 << " AND melt = " << Melt;
                        ss3 << " AND pack = " << Pack;
                        ss3 << " AND partno = " << PartNo;
                        ss3 << " AND sheet = " << Sheet;
                        //ss << "AND SubSheet = " << SubSheet;
                        //ss << "AND Slab = " << Slab;
                        ss3 << ";";

                        PGresult* res = conn_dops.PGexec(ss3.str());
                        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss3.str());
                        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        {
                            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss3.str());
                        }
                        PQclear(res);
                    }

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
                    {
                        std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, errc);
                        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss4.str());
                    }
                    PQclear(res);
                }
                else
                {
                    LOG_INFO(SQLLogger, "{:90}| Time_Z2={}, StateNo={}, Melt={}, Pack={}, PartNo={}, Sheet={}, SubSheet={}, Slab={}", FUNCTION_LINE_NAME, Time_Z2, StateNo, Melt, Pack, PartNo, Sheet, SubSheet, Slab);
                }

                //bThreadState = false;
                hThreadState2 = NULL;
            }
            catch(...)
            {
                LOG_ERROR(SQLLogger, "{:90}| Unknown error", FUNCTION_LINE_NAME);
            };

            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, 3);
            //ExitThread(0);
            return 0;
        }

        //Операция в 1 зоне
        DWORD DataPosState_1(Value* value)
        {
            int16_t val = value->Val.As<int16_t>();
            SetWindowText(winmap(value->winId), value->GetString().c_str());
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
            SetWindowText(winmap(value->winId), value->GetString().c_str());
            SetWindowText(winmap(hEditState_22), GenSeq2[val].c_str());
            if(val == 5 || val == 6)
            {
                //if(!bThreadState/* joinable()*/)
                {
                    if(hThreadState2 == NULL)
                        hThreadState2 = CreateThread(0, 0, ThreadState2, (LPVOID)0, 0, 0);

                    //LOG_INFO(SQLLogger, "{:90}| get_id 1 = {}, ", FUNCTION_LINE_NAME, hThreadState2.get_id());
                    //hThreadState2 = std::thread(ThreadState2);
                    //LOG_INFO(SQLLogger, "{:90}| get_id 2 = {}, ", FUNCTION_LINE_NAME, hThreadState2.get_id());
                }
                
            }

            //MySetWindowText(winmap(value->winId), value->GetString().c_str());
            return 0;
        }
        //Операция в 3 зоне
        DWORD DataPosState_3(Value* value)
        {
            //char ss[256];
            //sprintf_s(ss, 256, value->format.c_str(), value->Val);
            //value->strVal = ss;
            SetWindowText(winmap(value->winId), value->GetString().c_str());
            SetWindowText(winmap(hEditState_32), GenSeq3[GetVal<int16_t>(value)].c_str());

            //SetWindowText(winmap(value->winId), ss);


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
            MySetWindowText(winmap(value->winId), value->GetString().c_str());

            if(value->Val.As<float>() > 0)
            {
                std::string update = " temper = " + value->GetString();
                SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
                SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");
            }
            return 0;
        }

        //Скорость выгрузки
        DWORD UnloadSpeed(Value* value)
        {
            //char ss[256];
            //sprintf_s(ss, 255, value->format.c_str(), value->Val.As<> * 1000.0);
            //value->strVal = ss;
            MySetWindowText(winmap(value->winId), value->GetString().c_str());

            if(value->Val.As<float>() > 0)
            {
                std::string update = " speed = " + value->GetString();
                SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
                SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");
            }
            return 0;
        }
        //Время сигнализации окончания нагрева, мин
        DWORD fTimeForPlateHeat(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());

            std::string update = " timeforplateheat = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");
            return 0;
        }
        //Уставка давления для запуска комперссора
        DWORD fPresToStartComp(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " prestostartcomp = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");

            return 0;
        }


        //Фиксируем по условию
        //state2=5
        //Давление воды закалка коллектор верх 
        //Давление в верхнем коллекторе 
        //через 5 сек после state2=5
        
        //DWORD LaminPressTop(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    return 0;
        //}

        //Давление воды закалка коллектор низ
        //Давление нижнем коллекторе
        //через 5 сек после state2=5

        //DWORD LaminPressBot(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    return 0;
        //}

        //Температура воды в поддоне
        //state2=5

        //DWORD LAM_TE1(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    return 0;
        //}

        //Температура воды в баке
        //state2=5

        //DWORD Za_TE3(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    return 0;
        //}

        //Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)
        //state2=5

        //DWORD Za_PT3(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    return 0;
        //}
    };

    ////Температуры печи
    //namespace Htr{
    ////Температура в зоне 1.1
    //    DWORD Htr_11(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //Температура в зоне 1.2
    //    DWORD Htr_12(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //Температура в зоне 1.3
    //    DWORD Htr_13(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //Температура в зоне 1.4
    //    DWORD Htr_14(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //Температура в зоне 2.1
    //    DWORD Htr_21(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //Температура в зоне 2.2
    //    DWORD Htr_22(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //Температура в зоне 2.3
    //    DWORD Htr_23(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //Температура в зоне 2.4
    //    DWORD Htr_24(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //    //Пирометр
    //    template<class T> DWORD Za_TE4(Value* value)
    //    {
    //        MySetWindowText(winmap(value->winId), value->GetString().c_str());
    //        return 0;
    //    }
    //}

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
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");

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
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, "");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, "");

            return 0;
        }
    }

    //Скоростная секция
    namespace Speed{
        //Верх
        DWORD SpeedSectionTop(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " posclapantop = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, " posclapantop = 0 AND");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, " posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD SpeedSectionBot(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " posclapanbot = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, " posclapanbot = 0 AND");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, " posclapanbot = 0 AND");
            return 0;
        }
    }
    //Ламинарная секция 1
    namespace Lam1{
        //Верх
        DWORD LaminarSection1Top(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " lam1posclapantop = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, " lam1posclapantop = 0 AND");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, " lam1posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD LaminarSection1Bot(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " lam1posclapanbot = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, " lam1posclapanbot = 0 AND");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, " lam1posclapanbot = 0 AND");
            return 0;
        }
    };
    //Ламинарная секция 2
    namespace Lam2{
        //Верх
        DWORD LaminarSection2Top(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " lam2posclapantop = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, " lam2posclapantop = 0 AND");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, " lam2posclapantop = 0 AND");
            return 0;
        }
        //Низ
        DWORD LaminarSection2Bot(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " lam2posclapanbot = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[1], update, " lam2posclapanbot = 0 AND");
            SetUpdateSheet(&conn_kpvl, PlateData[2], update, " lam2posclapanbot = 0 AND");
            return 0;
        }
    };

    //Отклонения листа на кантовке
    namespace Side{
        DWORD SheetTop1(Value* value)
        {
            std::string update = " top1 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop2(Value* value)
        {
            std::string update = " top2 = " + value->GetString();
            
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop3(Value* value)
        {
            std::string update = " top3 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop4(Value* value)
        {
            std::string update = " top4 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop5(Value* value)
        {
            std::string update = " top5 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop6(Value* value)
        {
            std::string update = " top6 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop7(Value* value)
        {
            std::string update = " top7 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetTop8(Value* value)
        {
            std::string update = " top8 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }

        DWORD SheetBot1(Value* value)
        {
            std::string update = " bot1 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot2(Value* value)
        {
            std::string update = " bot2 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot3(Value* value)
        {
            std::string update = " bot3 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot4(Value* value)
        {
            std::string update = " bot4 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot5(Value* value)
        {
            std::string update = " bot5 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot6(Value* value)
        {
            std::string update = " bot6 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot7(Value* value)
        {
            std::string update = " bot7 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
        DWORD SheetBot8(Value* value)
        {
            std::string update = " bot8 = " + value->GetString();
            SetUpdateSheet(&conn_kpvl, PlateData[6], update, "");
            return 0;
        }
    }
    //namespace TimePos{
        //DWORD TimePosState_1(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    return 0;
        //}
        //DWORD TimePosState_2(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    return 0;
        //}
        //DWORD TimePosState_3(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    return 0;
        //}
    //}

    //Бит жмизни
    namespace WDG {
        DWORD SheetData_WDG_toBase(Value* value)
        {
            if(GetVal<bool>(value))
            {
                PLC_KPVL_old_dt = time(NULL);
                //WDGSheetData_To++;
                struct tm TM;
                localtime_s(&TM, &PLC_KPVL_old_dt);
                SetWindowText(winmap(value->winId), string_time(&TM).c_str());

                //HMISheetData.WDG_fromBase->Set_Value(true);
            }
            return 0;
        }
    }

    //Кассета 
    namespace Z6{
        DWORD StartNewCassette(Value* value)
        {
            const char* ss1 = "Кассета готова";
            const char* ss2 = "Начать кассету";

            if(GetVal<bool>(value))
            {
                ss1 = "Кассета набирается";
                ss2 = "Закрыть кассету";
            }
            SetWindowText(winmap(hStatSheet_CassetteIsFull), ss1);

            return 0;
        }

        DWORD CassetteIsFill(Value* value)
        {
            const char* ss1 = "Кассета готова";
            const char* ss2 = "Начать кассету";

            if(GetVal<bool>(value))
            {
                ss1 = "Кассета набирается";
                ss2 = "Закрыть кассету";
            }
            SetWindowText(winmap(hStatSheet_CassetteIsFull), ss1);
            //SetWindowText(winmap(hStatSheet_CassetteIsFull), ss1);
            //SetWindowText(winmap(hButtSheet_CassetteNew), ss2);
            return 0;
        }
    }

#pragma endregion
};

