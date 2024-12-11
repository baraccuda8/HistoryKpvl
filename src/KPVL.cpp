#include "pch.h"

#include "win.h"
#include "main.h"
#include "StringData.h"
#include "file.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "hard.h"
#include "term.h"
#include "Furn.h"
#include "KPVL.h"
#include "pdf.h"


extern std::shared_ptr<spdlog::logger> HardLogger;


extern PGConnection conn_dops;

TSheet PalletSheet[7];

std::deque<TSheet>AllSheet;

//Печь закалки
namespace KPVL {
    //Номер колонки в таблице листов
    namespace Coll{
        int id = 0;
        int create_at = 0;
        int start_at = 0;
        int datatime_end = 0;
        int pos = 0;
        int datatime_all = 0;
        int alloy = 0;
        int thikness = 0;
        int melt = 0;
        int slab = 0;
        int partno = 0;
        int pack = 0;
        int sheet = 0;
        int subsheet = 0;
        int temper = 0;
        int speed = 0;
        int za_pt3 = 0;
        int za_te3 = 0;
        int lampresstop = 0;
        int lampressbot = 0;
        int posclapantop  = 0;
        int posclapanbot = 0;
        int mask = 0;
        int lam1posclapantop = 0;
        int lam1posclapanbot = 0;
        int lam2posclapantop = 0;
        int lam2posclapanbot = 0;
        int lam_te1 = 0;
        int news = 0;
        int top1 = 0;
        int top2 = 0;
        int top3 = 0;
        int top4 = 0;
        int top5 = 0;
        int top6 = 0;
        int top7 = 0;
        int top8 = 0;
        int bot1 = 0;
        int bot2 = 0;
        int bot3 = 0;
        int bot4 = 0;
        int bot5 = 0;
        int bot6 = 0;
        int bot7 = 0;
        int bot8 = 0;
        int day = 0;
        int month = 0;
        int year = 0;
        int cassetteno = 0;
        int sheetincassette = 0;
        int timeforplateheat = 0;
        int prestostartcomp = 0;
        int temperature = 0;
        int correct = 0;
        int SecondPos_at = 0;
        int hour = 0;
        int delete_at = 0;
        int incant_at = 0;
        int cant_at = 0;
        int cassette = 0;
    };


    void UpdateCountSheet(PGConnection& conn, int id)
    {
        std::stringstream sd;
        sd << "SELECT updatesheetincassette(" << id << ");";
        SETUPDATESQL(HardLogger, conn, sd);
    }

    //Список последних 100 листов из базы
    namespace SQL
    {


        //Получаем список колонов в таблице sheet
        void GetCollumn(PGresult* res)
        {
            try
            {
                if(!Coll::delete_at)
                {
                    int nFields = PQnfields(res);
                    for(int j = 0; j < nFields; j++)
                    {
                        std::string l =  utf8_to_cp1251(PQfname(res, j));
                        if(l == "id") Coll::id = j;
                        else if(l == "create_at") Coll::create_at = j;
                        else if(l == "start_at") Coll::start_at = j;
                        else if(l == "datatime_end") Coll::datatime_end = j;
                        else if(l == "pos") Coll::pos = j;
                        else if(l == "datatime_all") Coll::datatime_all = j;
                        else if(l == "alloy") Coll::alloy = j;
                        else if(l == "thikness") Coll::thikness = j;
                        else if(l == "melt") Coll::melt = j;
                        else if(l == "slab") Coll::slab = j;
                        else if(l == "partno") Coll::partno = j;
                        else if(l == "pack") Coll::pack = j;
                        else if(l == "sheet") Coll::sheet = j;
                        else if(l == "subsheet") Coll::subsheet = j;
                        else if(l == "temper") Coll::temper = j;
                        else if(l == "speed") Coll::speed = j;
                        else if(l == "za_pt3") Coll::za_pt3 = j;
                        else if(l == "za_te3") Coll::za_te3 = j;
                        else if(l == "lampresstop") Coll::lampresstop = j;
                        else if(l == "lampressbot") Coll::lampressbot = j;
                        else if(l == "posclapantop") Coll::posclapantop = j;
                        else if(l == "posclapanbot") Coll::posclapanbot = j;
                        else if(l == "mask") Coll::mask = j;
                        else if(l == "lam1posclapantop") Coll::lam1posclapantop = j;
                        else if(l == "lam1posclapanbot") Coll::lam1posclapanbot = j;
                        else if(l == "lam2posclapantop") Coll::lam2posclapantop = j;
                        else if(l == "lam2posclapanbot") Coll::lam2posclapanbot = j;
                        else if(l == "lam_te1") Coll::lam_te1 = j;
                        else if(l == "news") Coll::news = j;
                        else if(l == "top1") Coll::top1 = j;
                        else if(l == "top2") Coll::top2 = j;
                        else if(l == "top3") Coll::top3 = j;
                        else if(l == "top4") Coll::top4 = j;
                        else if(l == "top5") Coll::top5 = j;
                        else if(l == "top6") Coll::top6 = j;
                        else if(l == "top7") Coll::top7 = j;
                        else if(l == "top8") Coll::top8 = j;
                        else if(l == "bot1") Coll::bot1 = j;
                        else if(l == "bot2") Coll::bot2 = j;
                        else if(l == "bot3") Coll::bot3 = j;
                        else if(l == "bot4") Coll::bot4 = j;
                        else if(l == "bot5") Coll::bot5 = j;
                        else if(l == "bot6") Coll::bot6 = j;
                        else if(l == "bot7") Coll::bot7 = j;
                        else if(l == "bot8") Coll::bot8 = j;
                        else if(l == "day") Coll::day = j;
                        else if(l == "month") Coll::month = j;
                        else if(l == "year") Coll::year = j;
                        else if(l == "hour") Coll::hour = j;
                        else if(l == "cassetteno") Coll::cassetteno = j;
                        else if(l == "sheetincassette") Coll::sheetincassette = j;
                        else if(l == "timeforplateheat") Coll::timeforplateheat = j;
                        else if(l == "prestostartcomp") Coll::prestostartcomp = j;
                        else if(l == "temperature") Coll::temperature = j;
                        else if(l == "correct") Coll::correct = j;
                        else if(l == "secondpos_at") Coll::SecondPos_at = j;
                        else if(l == "delete_at") Coll::delete_at = j;
                        else if(l == "incant_at") Coll::incant_at = j;
                        else if(l == "cant_at") Coll::cant_at = j;
                        else if(l == "cassette") Coll::cassette = j;
                        
                    }
                }
            }
            CATCH(HardLogger, "");
        }

        //Чтение листов
        void GetSheet(PGConnection& conn, PGresult* res, int l, TSheet& sheet)
        {
            try
            {
                GetCollumn(res);

                sheet.Create_at = GetStringData(conn.PGgetvalue(res, l, Coll::create_at));
                sheet.Start_at = GetStringData(conn.PGgetvalue(res, l, Coll::start_at));
                sheet.SecondPos_at = GetStringData(conn.PGgetvalue(res, l, Coll::SecondPos_at));
                sheet.DataTime_End = GetStringData(conn.PGgetvalue(res, l, Coll::datatime_end));
                sheet.InCant_at = GetStringData(conn.PGgetvalue(res, l, Coll::incant_at));
                sheet.Cant_at = GetStringData(conn.PGgetvalue(res, l, Coll::cant_at));
                sheet.Correct = GetStringData(conn.PGgetvalue(res, l, Coll::correct));
                sheet.Delete_at = GetStringData(conn.PGgetvalue(res, l, Coll::delete_at));


                sheet.Pos = conn.PGgetvalue(res, l, Coll::pos);
                sheet.id = conn.PGgetvalue(res, l, Coll::id);
                sheet.DataTime_All = conn.PGgetvalue(res, l, Coll::datatime_all);
                sheet.Alloy = conn.PGgetvalue(res, l, Coll::alloy);
                sheet.Thikness = conn.PGgetvalue(res, l, Coll::thikness);
                sheet.Melt = conn.PGgetvalue(res, l, Coll::melt);
                sheet.Slab = conn.PGgetvalue(res, l, Coll::slab);
                sheet.PartNo = conn.PGgetvalue(res, l, Coll::partno);
                sheet.Pack = conn.PGgetvalue(res, l, Coll::pack);
                sheet.Sheet = conn.PGgetvalue(res, l, Coll::sheet);
                sheet.SubSheet = conn.PGgetvalue(res, l, Coll::subsheet);
                sheet.Temper = conn.PGgetvalue(res, l, Coll::temper);
                sheet.Speed = conn.PGgetvalue(res, l, Coll::speed);

                sheet.Za_PT3 = conn.PGgetvalue(res, l, Coll::za_pt3);
                sheet.Za_TE3 = conn.PGgetvalue(res, l, Coll::za_te3);

                sheet.LaminPressTop = conn.PGgetvalue(res, l, Coll::lampresstop);
                sheet.LaminPressBot = conn.PGgetvalue(res, l, Coll::lampressbot);
                sheet.PosClapanTop = conn.PGgetvalue(res, l, Coll::posclapantop);
                sheet.PosClapanBot = conn.PGgetvalue(res, l, Coll::posclapanbot);
                sheet.Mask = conn.PGgetvalue(res, l, Coll::mask);

                sheet.Lam1PosClapanTop = conn.PGgetvalue(res, l, Coll::lam1posclapantop);
                sheet.Lam1PosClapanBot = conn.PGgetvalue(res, l, Coll::lam1posclapanbot);
                sheet.Lam2PosClapanTop = conn.PGgetvalue(res, l, Coll::lam2posclapantop);
                sheet.Lam2PosClapanBot = conn.PGgetvalue(res, l, Coll::lam2posclapanbot);

                sheet.LAM_TE1 = conn.PGgetvalue(res, l, Coll::lam_te1);
                sheet.News = conn.PGgetvalue(res, l, Coll::news);
                sheet.Top1 = conn.PGgetvalue(res, l, Coll::top1);
                sheet.Top2 = conn.PGgetvalue(res, l, Coll::top2);
                sheet.Top3 = conn.PGgetvalue(res, l, Coll::top3);
                sheet.Top4 = conn.PGgetvalue(res, l, Coll::top4);
                sheet.Top5 = conn.PGgetvalue(res, l, Coll::top5);
                sheet.Top6 = conn.PGgetvalue(res, l, Coll::top6);
                sheet.Top7 = conn.PGgetvalue(res, l, Coll::top7);
                sheet.Top8 = conn.PGgetvalue(res, l, Coll::top8);

                sheet.Bot1 = conn.PGgetvalue(res, l, Coll::bot1);
                sheet.Bot2 = conn.PGgetvalue(res, l, Coll::bot2);
                sheet.Bot3 = conn.PGgetvalue(res, l, Coll::bot3);
                sheet.Bot4 = conn.PGgetvalue(res, l, Coll::bot4);
                sheet.Bot5 = conn.PGgetvalue(res, l, Coll::bot5);
                sheet.Bot6 = conn.PGgetvalue(res, l, Coll::bot6);
                sheet.Bot7 = conn.PGgetvalue(res, l, Coll::bot7);
                sheet.Bot8 = conn.PGgetvalue(res, l, Coll::bot8);

                sheet.Hour = conn.PGgetvalue(res, l, Coll::hour);
                sheet.Day = conn.PGgetvalue(res, l, Coll::day);
                sheet.Month = conn.PGgetvalue(res, l, Coll::month);
                sheet.Year = conn.PGgetvalue(res, l, Coll::year);
                sheet.CassetteNo = conn.PGgetvalue(res, l, Coll::cassetteno);
                sheet.SheetInCassette = conn.PGgetvalue(res, l, Coll::sheetincassette);

                sheet.TimeForPlateHeat = conn.PGgetvalue(res, l, Coll::timeforplateheat);
                sheet.PresToStartComp = conn.PGgetvalue(res, l, Coll::prestostartcomp);
                sheet.Temperature = conn.PGgetvalue(res, l, Coll::temperature);
                sheet.Cassette = conn.PGgetvalue(res, l, Coll::cassette);
            }
            CATCH(HardLogger, "");
        }

        //Получаем список листов из базы
        void KPVL_SQL(PGConnection& conn, std::deque<TSheet>& allSheet)
        {
            try
            {
                allSheet.erase(allSheet.begin(), allSheet.end());

                std::time_t stop = time(NULL);
                std::time_t statr = static_cast<std::time_t>(difftime(stop, 60 * 60 * 24 * 1)); //1-е суток
                std::string start_at = GetStringOfDataTime(&statr);

                std::stringstream FilterComand;
                FilterComand << "SELECT * FROM sheet ";
                FilterComand << "WHERE create_at > '" << start_at << "'"; //delete_at IS NULL AND 
                //FilterComand << " AND id <= " << sStopId;
                FilterComand << " ORDER BY  create_at DESC, pos DESC, start_at DESC;";
                //FilterSheet();
                //bFilterData = TRUE;
                std::string command = FilterComand.str();
                if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                PGresult* res = conn.PGexec(command);
                //LOG_INFO(HardLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    int line = PQntuples(res);
                    for(int l = 0; l < line; l++)
                    {
                        TSheet sheet;
                        GetSheet(conn, res, l, sheet);
                        allSheet.push_back(sheet);
                        if(!isRun) return;
                    }

                }
                else
                    LOG_ERR_SQL(HardLogger, res, command);
                PQclear(res);

                //AddHistoriSheet(true, (int)sheet.size());
                int t = 0;
            }
            CATCH(HardLogger, "");
        }

        float GetHeatTime_Z2(PGConnection& conn, std::string enddata_at)
        {
            float out = 0;
            try
            {
                std::string sout = "";
                //std::string next_at = "";
                //if(enddata_at.length())
                //{
                //    std::stringstream co;
                //    co << "SELECT max(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at <= '" << enddata_at << "';";
                //    std::string command = co.str();
                //    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                //    PGresult* res = conn.PGexec(command);
                //    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                //    {
                //        if(PQntuples(res))
                //            next_at = conn.PGgetvalue(res, 0, 0);
                //    }
                //    else
                //        LOG_ERR_SQL(HardLogger, res, command);
                //    PQclear(res);
                //}

                //if(next_at.length())
                {
                    std::stringstream co;
                    //co << "SELECT content FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at = '" << next_at << "';";
                    co << "SELECT content FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at <= '" << enddata_at << "' ORDER BY create_at DESC LIMIT 1";
                    std::string command = co.str();
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                    PGresult* res = conn.PGexec(command);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            sout = conn.PGgetvalue(res, 0, 0);
                    }
                    else
                        LOG_ERR_SQL(HardLogger, res, command);
                    PQclear(res);
                }

                if(sout.length())
                    out = Stof(sout);
            }
            CATCH(HardLogger, "");
            return out;
        }

        void GetDataTime_All(PGConnection& conn, TSheet& TS)
        {
            try
            {
                if(Stoi(TS.Pos) < 3)return;
                if(!TS.Start_at.length())
                {
                    std::stringstream sd;
                    sd << "SELECT start_at FROM sheet WHERE id = " << TS.id;
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                    PGresult* res = conn.PGexec(sd.str());
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            TS.Start_at = GetStringData(conn.PGgetvalue(res, 0, 0));
                    }
                    else
                        LOG_ERR_SQL(HardLogger, res, sd.str());
                    PQclear(res);
                }

                if(!TS.Start_at.length())
                {
                    std::stringstream sd;
                    sd << " start_at = (SELECT MAX(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_1_StateNo->ID << " AND create_at <= '" << TS.Create_at << "' AND content = '3')";
                    std::string update = sd.str();

                    Sheet::SetUpdateSheet(conn, TS, update, ""); // " start_at IS NULL AND ");

                    std::stringstream sa;
                    sa << "SELECT MAX(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_1_StateNo->ID << " AND create_at <= '" << TS.Create_at << "' AND content = '3'";

                    std::string command = sa.str();
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                    PGresult* res = conn.PGexec(command);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            TS.Start_at = GetStringData(conn.PGgetvalue(res, 0, 0));
                    }
                    else
                        LOG_ERR_SQL(HardLogger, res, command);
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
                        //co << "SELECT min(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_2_StateNo->ID << " AND content = '3' AND create_at > '" << TS.Start_at << "';"; //GenSeqToHmi.Data.Seq_2_StateNo Номер шага последовательности выгрузки в печи
                        co << "SELECT create_at FROM todos WHERE id_name = " << GenSeqToHmi.Seq_2_StateNo->ID << " AND content = '3' AND create_at > '" << TS.Start_at << "' ORDER BY create_at LIMIT 1;"; //GenSeqToHmi.Data.Seq_2_StateNo Номер шага последовательности выгрузки в печи
                        std::string command = co.str();
                        if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                        PGresult* res = conn.PGexec(command);
                        if(PQresultStatus(res) == PGRES_TUPLES_OK)
                        {
                            if(PQntuples(res))
                                next_at = conn.PGgetvalue(res, 0, 0);
                        }
                        else
                            LOG_ERR_SQL(HardLogger, res, command);
                        PQclear(res);
                    }

                    if(next_at.length())
                    {
                        std::stringstream co;
                        co << "SELECT create_at FROM todos WHERE id_name = " << GenSeqToHmi.Seq_1_StateNo->ID << " AND content = '3' AND create_at > '" << next_at << "' ORDER BY create_at LIMIT 1;"; //GenSeqToHmi.Data.Seq_2_StateNo Номер шага последовательности выгрузки в печи
                        //co << "SELECT create_at FROM todos WHERE id_name = " << GenSeqToHmi.Seq_1_StateNo->ID << " AND content = '5' AND create_at > '" << next_at << "';"; //GenSeqToHmi.Data.Seq_2_StateNo Номер шага последовательности выгрузки в печи
                        std::string command = co.str();
                        if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                        PGresult* res = conn.PGexec(command);
                        if(PQresultStatus(res) == PGRES_TUPLES_OK)
                        {
                            if(PQntuples(res))
                                enddata_at = conn.PGgetvalue(res, 0, 0);
                        }
                        else
                            LOG_ERR_SQL(HardLogger, res, command);
                        PQclear(res);
                    }
                    if(enddata_at.length())
                    {
                        float HeatTime_Z2 = GetHeatTime_Z2(conn, enddata_at);

                        std::string Id = Sheet::GetIdSheet(conn, TS.Melt, TS.Pack, TS.PartNo, TS.Sheet, TS.SubSheet, TS.Slab);

                        std::stringstream co;
                        co << "UPDATE sheet SET datatime_end = '" << enddata_at << "', datatime_all = " << HeatTime_Z2 << " WHERE delete_at IS NULL AND id = " << Id;
                        std::string command = co.str();
                        if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                        PGresult* res = conn.PGexec(command);
                        //LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                            LOG_ERR_SQL(HardLogger, res, command);
                        PQclear(res);
                    }

                    int tt = 0;
                    //sMaxId << TS.id;
                }
            }
            CATCH(HardLogger, "");
        }

    }

    std::string ServerDataTime = "";

    //Функции обработки и со сохранения истории листов
    namespace Sheet{
#pragma region Функции обработки и со сохранения истории листов
        //Проверка на наличие листа
        bool IsSheet(T_PlateData& PD)
        {
            try
            {
                int32_t Melt = PD.Melt->GetInt();
                //int32_t Pack = PD.Pack->GetInt();
                int32_t PartNo = PD.PartNo->GetInt();
                int32_t Sheet = PD.Sheet->GetInt();
                return Melt /*&& Pack*/ && PartNo && Sheet;
            }
            CATCH(HardLogger, "");
            return false;
        }
        bool IsSheet(TSheet& PS)
        {
            try
            {
                int32_t Melt = Stoi(PS.Melt);
                //int32_t Pack = Stoi(PS.Pack);
                int32_t PartNo = Stoi(PS.PartNo);
                int32_t Sheet = Stoi(PS.Sheet);
                //int32_t SubSheet = PS.SubSheet;
                //int32_t Slab = PS.Slab;
                return Melt /*&& Pack*/ && PartNo && Sheet/* && SubSheet*/;
            }
            CATCH(HardLogger, "");
            return false;
        }

        
        std::string GetIdSheet(PGConnection& conn, std::string sMelt, std::string sPack, std::string sPartNo, std::string sSheet, std::string sSubSheet, std::string sSlab)
        {
            std::string id = "0";
            try
            {
                int32_t Melt = Stoi(sMelt);
                int32_t Pack = Stoi(sPack);
                int32_t PartNo = Stoi(sPartNo);
                int32_t Sheet = Stoi(sSheet);
                int32_t SubSheet = Stoi(sSubSheet);
                int32_t Slab = Stoi(sSlab);

                if(Melt /*&& Pack*/ && PartNo && Sheet /*&& SubSheet && Slab*/)
                {
                    std::stringstream co;
                    co << "SELECT id FROM sheet WHERE delete_at IS NULL AND ";
                    co << " melt = " << Melt;
                    co << " AND slab = " << Slab;
                    co << " AND partno = " << PartNo;
                    co << " AND pack = " << Pack;
                    co << " AND sheet = " << Sheet;
                    co << " AND subsheet = " << SubSheet;
                    co << ";";
                    std::string command = co.str();
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);

                    PGresult* res = conn.PGexec(command);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            id = conn.PGgetvalue(res, 0, 0);
                    }
                    else
                        LOG_ERR_SQL(HardLogger, res, command);
                    PQclear(res);
                }
            }
            CATCH(HardLogger, "");
            return id;
        }

        //Получаем ID листа
        std::string GetIdSheet(PGConnection& conn, int32_t Melt, int32_t Pack, int32_t PartNo, int32_t Sheet, int32_t SubSheet, int32_t Slab)
        {
            std::string id = "0";

            try
            {
                if(Melt /*&& Pack*/ && PartNo && Sheet /*&& SubSheet && Slab*/)
                {
                    std::stringstream co;
                    co << "SELECT id FROM sheet WHERE delete_at IS NULL AND ";
                    co << " melt = " << Melt;
                    co << " AND slab = " << Slab;
                    co << " AND partno = " << PartNo;
                    co << " AND pack = " << Pack;
                    co << " AND sheet = " << Sheet;
                    co << " AND subsheet = " << SubSheet;
                    co << ";";
                    std::string command = co.str();
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                    PGresult* res = conn.PGexec(command);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            id = conn.PGgetvalue(res, 0, 0);
                    }
                    else
                        LOG_ERR_SQL(HardLogger, res, command);
                    PQclear(res);
                }
            }
            CATCH(HardLogger, "");
            return id;
        }
        std::string GetIdSheet(PGConnection& conn, T_PlateData& PD)
        {
            std::string id = "0";
            try
            {
                if(IsSheet(PD))
                {
                    int32_t Melt1       = PD.Melt->Val.As<int32_t>();
                    int32_t Slab1       = PD.Slab->Val.As<int32_t>();
                    int32_t PartNo1     = PD.PartNo->Val.As<int32_t>();
                    int32_t Pack1       = PD.Pack->Val.As<int32_t>();
                    int32_t Sheet1      = PD.Sheet->Val.As<int32_t>();
                    int32_t SubSheet1   = PD.SubSheet->Val.As<int32_t>();

                    int32_t Melt2       = PD.Melt->GetValue().As<int32_t>();
                    int32_t Slab2       = PD.Slab->GetValue().As<int32_t>();
                    int32_t PartNo2     = PD.PartNo->GetValue().As<int32_t>();
                    int32_t Pack2       = PD.Pack->GetValue().As<int32_t>();
                    int32_t Sheet2      = PD.Sheet->GetValue().As<int32_t>();
                    int32_t SubSheet2   = PD.SubSheet->GetValue().As<int32_t>();

                    if(Melt1 != Melt2 || Slab1 != Slab2 || PartNo1 != PartNo2 || Pack1 != Pack2 || Sheet1 != Sheet2 || SubSheet1 != SubSheet2)
                    {
                        LOG_INFO(HardLogger, "{:90}| Запрос по ID: Melt = {}, Slab = {}, PartNo = {}, Pack = {}, Sheet = {}, SubSheet = {}", FUNCTION_LINE_NAME, Melt1, Slab1, PartNo1, Pack1, Sheet1, SubSheet1);
                        LOG_INFO(HardLogger, "{:90}| Запрос по ID: Melt = {}, Slab = {}, PartNo = {}, Pack = {}, Sheet = {}, SubSheet = {}", FUNCTION_LINE_NAME, Melt2, Slab2, PartNo2, Pack2, Sheet2, SubSheet2);
                    }

                    std::stringstream sd;
                    sd << "SELECT id FROM sheet WHERE ";
                    sd << " melt = " << Melt2;
                    sd << " AND slab = " << Slab2;
                    sd << " AND partno = " << PartNo2;
                    sd << " AND pack = " << Pack2;
                    sd << " AND sheet = " << Sheet2;
                    sd << " AND subsheet = " << SubSheet2;
                    std::string command = sd.str();
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                    PGresult* res = conn.PGexec(command);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            id = conn.PGgetvalue(res, 0, 0);
                    }
                    else
                    {
                        LOG_ERR_SQL(HardLogger, res, command);
                    }
                    PQclear(res);
                }
            }
            CATCH(HardLogger, "");
            return id;
        }


        void LocSheet(PGConnection& conn, T_PlateData& PD, int Pos)
        {
            try
            {
                if(IsSheet(PD))
                {
                    int32_t Melt = PD.Melt->Val.As<int32_t >();//(sMelt);
                    int32_t Pack = PD.Pack->Val.As<int32_t >();
                    int32_t PartNo = PD.PartNo->Val.As<int32_t >();
                    int32_t Sheet = PD.Sheet->Val.As<int32_t >();
                    int32_t SubSheet = PD.SubSheet->Val.As<int32_t >();
                    int32_t Slab = PD.Slab->Val.As<int32_t >();
                    std::string id = GetIdSheet(conn, Melt, Pack, PartNo, Sheet, SubSheet, Slab);

                    //std::string id = GetIdSheet(conn, PD);
                    PalletSheet[Pos].id = id;
                    PalletSheet[Pos].Create_at = GetStringDataTime();
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
            CATCH(HardLogger, "");
        }

        //Получаем ID листа
        //Добовление листа в базу
        void InsertSheet(PGConnection& conn, T_PlateData& PD, int Pos)
        {
            try
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
                    {
                        std::stringstream sd;
                        sd << "INSERT INTO sheet (";
#pragma region ID листа
                        sd << "start_at";
                        sd << ", alloy";
                        sd << ", thikness";
                        sd << ", melt";
                        sd << ", slab";
                        sd << ", partno";
                        sd << ", pack";
                        sd << ", sheet";
                        sd << ", subsheet";
                        sd << ", pos";
#pragma endregion

                        if(Pos == 1 || Pos == 2)
                        {
                            sd << ", temper";
                            sd << ", speed";

                            sd << ", timeforplateheat";
                            sd << ", prestostartcomp";

                            sd << ", posclapantop";
                            sd << ", posclapanbot";

                            sd << ", lam1posclapantop";
                            sd << ", lam1posclapanbot";

                            sd << ", lam2posclapantop";
                            sd << ", lam2posclapanbot";

                            sd << ", mask";
                        }

                        sd << ") VALUES (";

#pragma region ID листа
                        sd << "now()" ;
                        sd << ", '" << PD.AlloyCodeText->strVal << "'";
                        sd << ", '" << PD.ThiknessText->strVal << "'";
                        sd << ", " << PD.Melt->strVal;
                        sd << ", " << PD.Slab->strVal;
                        sd << ", " << PD.PartNo->strVal;
                        sd << ", " << PD.Pack->strVal;
                        sd << ", " << PD.Sheet->strVal;
                        sd << ", " << PD.SubSheet->strVal;
                        sd << ", " << Pos; //start_at
#pragma endregion

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

                            sd << ", "<< GenSeqFromHmi.TempSet1->strVal;
                            sd << ", "<< Par_Gen.UnloadSpeed->strVal;

                            sd << ", "<< Par_Gen.TimeForPlateHeat->strVal;
                            sd << ", "<< Par_Gen.PresToStartComp->strVal;

                            sd << ", "<< HMISheetData.SpeedSection.Top->strVal;
                            sd << ", "<< HMISheetData.SpeedSection.Bot->strVal;

                            sd << ", "<< HMISheetData.LaminarSection1.Top->strVal;
                            sd << ", "<< HMISheetData.LaminarSection1.Bot->strVal;

                            sd << ", "<< HMISheetData.LaminarSection2.Top->strVal;
                            sd << ", "<< HMISheetData.LaminarSection2.Bot->strVal;

                            sd << ", "<< "'" + MaskKlapan + "'";
                        }

                        sd << ");";

                        LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                        SETUPDATESQL(HardLogger, conn, sd);
                    }
                }
            }
            CATCH(HardLogger, "");
        }


        //Обновляем в базе данные по листу
        void SetUpdateSheet(PGConnection& conn, TSheet& TS, std::string update, std::string where)
        {
            try
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
                    SETUPDATESQL(HardLogger, conn, sd);
                }
            }
            CATCH(HardLogger, "");
        }

        //Обновлякем в базе данные по листу
        void SetUpdateSheet(PGConnection& conn, T_PlateData& PD, std::string update, std::string where)
        {
            try
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
                    SETUPDATESQL(HardLogger, conn, sd);
                }
            }
            CATCH(HardLogger, "");
        }


        //Потеряный на кантовке
        void UpdateCant(PGConnection& conn, std::string id)
        {
            try
            {
                LOG_INFO(HardLogger, "{:90}| SheetId = {}, потерян на кантовке", FUNCTION_LINE_NAME, id);
                std::stringstream sd;
                sd << "UPDATE sheet SET pos = 16 WHERE id = " << id;
                LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                SETUPDATESQL(HardLogger, conn, sd);
            }
            CATCH(HardLogger, "");
        }
        ////Установка позиции листа
        //void UpdateSQLPos(PGConnection& conn, std::string id, int Pos)
        //{
        //    try
        //    {
        //        std::stringstream sd;
        //        sd << "UPDATE sheet SET pos = " << Pos << ", correct = DEFAULT, pdf = DEFAULT";
        //        sd << " WHERE id = " << id;
        //        LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
        //        SETUPDATESQL(HardLogger, conn, sd);
        //    }
        //    CATCH(HardLogger, "");
        //}

        //Потерян на кантовке
        void UpdateIdOtherCant(PGConnection& conn, std::string id)
        {
            try
            {
                std::vector<std::string> oldid;
                std::stringstream sr;
                sr << "SELECT id FROM sheet WHERE pos = 6 AND id <> " << id << " ORDER BY id DESC";
                std::string command = sr.str();
                //LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                PGresult* res = conn.PGexec(command);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    int len = PQntuples(res);
                    for(int l = 0; l < len; l++)
                        oldid.push_back(conn.PGgetvalue(res, 0, 0));
                }
                else
                    LOG_ERR_SQL(HardLogger, res, command);
                PQclear(res);

                for(auto& a : oldid)
                    if(a != id)UpdateCant(conn, a);
            }
            CATCH(HardLogger, "");
        }

        //Обновляем позицию листа
        void UpdateSheetPos(PGConnection& conn, T_PlateData& PD, std::string id, int Pos)
        {
            try
            {
                if(IsSheet(PD))
                {
                    int pos = 0;

                    if(Stoi(id))
                    {
                        if(Pos == 6)
                            UpdateIdOtherCant(conn, id);

                        std::stringstream se;
                        se << "SELECT pos FROM sheet WHERE id = " << id;
                        std::string command = se.str();
                        if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                        PGresult* res = conn.PGexec(command);
                        if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
                            pos = Stoi(conn.PGgetvalue(res, 0, 0));
                        else
                            LOG_ERR_SQL(HardLogger, res, command);
                        PQclear(res);

                        if(pos != Pos && pos > 0 && pos < 7)
                            //UpdateSQLPos(conn, sid, Pos);
                        {
                            try
                            {
                                std::stringstream sd;
                                sd << "UPDATE sheet SET pos = " << Pos << ", correct = DEFAULT, pdf = DEFAULT WHERE id = " << id;
                                LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                                SETUPDATESQL(HardLogger, conn, sd);
                            }
                            CATCH(HardLogger, "");
                        }
                    }
                }
            }
            CATCH(HardLogger, "");
        };

        //Обновляем данные по листу если лист есть или добовляем новый
        std::string SheetPos(PGConnection& conn, T_PlateData& PD, int Pos)
        {
            std::string id = "";
            try
            {
                id = GetIdSheet(conn, PD);
                if(Stoi(id) != 0)
                {
                    UpdateSheetPos(conn, PD, id, Pos);
                    if(Pos == 1 || Pos == 2)
                    {
                        SetUpdateSheet(conn, PD, " alloy = '" + PD.AlloyCodeText->GetString() + "'", " thikness = '' AND");
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
                    InsertSheet(conn, PD, Pos);
                    id = GetIdSheet(conn, PD);
                }
            }
            CATCH(HardLogger, "");
            return id;
        }

        void DeleteNullSgeet(PGConnection& conn, T_PlateData& PD, int Pos)
        {
            try
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
                            std::string command = "SELECT pos FROM sheet WHERE id = " + sId;
                            if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                            PGresult* res = conn.PGexec(command);
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
                                        SETUPDATESQL(HardLogger, conn, sd);
                                        //command = sd.str();
                                        //if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                                        //res = conn.PGexec(command);
                                        ////LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                                        //
                                        //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                                        //    LOG_ERR_SQL(HardLogger, res, command);
                                        //PQclear(res);
                                    }

                                }
                            }
                            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                                LOG_ERR_SQL(HardLogger, res, command);
                        }

                        PalletSheet[Pos].Slab = "";
                    }
                }
            }
            CATCH(HardLogger, "");
        }

        //Вывот времени
        void OutTime(T_PlateData& PD, HWNDCLIENT chwnd)
        {
            try
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
            CATCH(HardLogger, "");

        }
#pragma endregion

        //Зона 0: На входе в печь
        namespace Z0{

            const int Pos = 0;

            DWORD DataAlloyThikn(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    PD.AlloyCodeText->GetValue();
                    PD.ThiknessText->GetValue();
                    MySetWindowText(value);
                    LocSheet(*value->Conn, PD, Pos);
                }
                CATCH(HardLogger, "");
                return 0;
            }

            DWORD Data(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);
                    //DeleteNullSgeet(conn, PD, Pos);
                    LocSheet(*value->Conn, PD, Pos);
                    OutTime(PD, HWNDCLIENT::hEditPlate_DataZ0_Time);
                }
                CATCH(HardLogger, "");
                return 0;
            }
        }

        //Зона 1: 1 зона печи 
        namespace Z1{
            const int Pos = 1;
            DWORD DataAlloyThikn(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);

                    LocSheet(*value->Conn, PD, Pos);
                    OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);

                    PD.AlloyCodeText->GetValue();
                    PD.ThiknessText->GetValue();


                    std::string update;
                    if(PD.AlloyCodeText->strVal.length())
                    {
                        update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                    if(PD.ThiknessText->strVal.length())
                    {
                        update = " thikness = '" + PD.ThiknessText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }

            DWORD Data(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);

                    LocSheet(*value->Conn, PD, Pos);
                    OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);

                    if(IsSheet(PD))
                    {
                        SheetPos(*value->Conn, PD, Pos);

                        std::string update = " temper = " + GenSeqFromHmi.TempSet1->GetString();
                        SetUpdateSheet(*value->Conn, PD, update, " temper = 0 AND");

                        update = " speed = " + Par_Gen.UnloadSpeed->GetString();
                        SetUpdateSheet(*value->Conn, PD, update, " speed = 0 AND");

                        update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                        SetUpdateSheet(*value->Conn, PD, update, " timeforplateheat = 0 AND");

                        update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                        SetUpdateSheet(*value->Conn, PD, update, " prestostartcomp = 0 AND");

                        SetUpdateSheet(*value->Conn, PD, " start_at = now() ", " start_at IS NULL AND");//" start_at IS NULL AND ");

                        DeleteNullSgeet(*value->Conn, PD, Pos);
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }
        };

        //Зона 2: 2 зона печи
        namespace Z2{
            const int Pos = 2;
            DWORD DataAlloyThikn(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);

                    LocSheet(*value->Conn, PD, Pos);
                    OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);

                    PD.AlloyCodeText->GetValue();
                    PD.ThiknessText->GetValue();

                    std::string update;
                    if(PD.AlloyCodeText->strVal.length())
                    {
                        update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                    if(PD.ThiknessText->strVal.length())
                    {
                        update = " thikness = '" + PD.ThiknessText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }

            DWORD Data(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);

                    LocSheet(*value->Conn, PD, Pos);
                    OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);

                    if(IsSheet(PD))
                    {
                        SheetPos(*value->Conn, PD, Pos);

						std::string update = " temper = " + GenSeqFromHmi.TempSet1->GetString();
                        SetUpdateSheet(*value->Conn, PD, update, " temper = 0 AND");

                        update = " speed = " + Par_Gen.UnloadSpeed->GetString();
                        SetUpdateSheet(*value->Conn, PD, update, " speed = 0 AND");

                        update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                        SetUpdateSheet(*value->Conn, PD, update, " timeforplateheat = 0 AND");

                        update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                        SetUpdateSheet(*value->Conn, PD, update, " prestostartcomp = 0 AND");

                        SetUpdateSheet(*value->Conn, PD, " secondpos_at = now() ", ""); //" secondpos_at IS NULL AND ");

                        DeleteNullSgeet(*value->Conn, PD, Pos);
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }
        };

        //Зона 3: Закалка
        namespace Z3{
            const int Pos = 3;
            DWORD DataAlloyThikn(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);

                    LocSheet(*value->Conn, PD, Pos);

                    PD.AlloyCodeText->GetValue();
                    PD.ThiknessText->GetValue();
                    std::string update;
                    if(PD.AlloyCodeText->strVal.length())
                    {
                        update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                    if(PD.ThiknessText->strVal.length())
                    {
                        update = " thikness = '" + PD.ThiknessText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }
            DWORD Data(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];;
                    MySetWindowText(value);

                    LocSheet(*value->Conn, PD, Pos);

                    OutTime(PD, HWNDCLIENT::hEditPlate_DataZ3_Time);

                    if(IsSheet(PD))
                    {
                        SheetPos(*value->Conn, PD, Pos);

                        SetUpdateSheet(*value->Conn, PD, " datatime_end = now() ", " datatime_end IS NULL AND"); //" secondpos_at IS NULL AND ");

                        DeleteNullSgeet(*value->Conn, PD, Pos);
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }
        };

        //Зона 4: Охлаждение
        namespace Z4{
            const int Pos = 4;
            DWORD DataAlloyThikn(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);

                    LocSheet(*value->Conn, PD, Pos);

                    PD.AlloyCodeText->GetValue();
                    PD.ThiknessText->GetValue();

                    std::string update;
                    if(PD.AlloyCodeText->strVal.length())
                    {
                        update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                    if(PD.ThiknessText->strVal.length())
                    {
                        update = " thikness = '" + PD.ThiknessText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }

            DWORD Data(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);

                    LocSheet(*value->Conn, PD, Pos);
                    OutTime(PD, HWNDCLIENT::hEditPlate_DataZ4_Time);

                    if(IsSheet(PD))
                    {
                        SheetPos(*value->Conn, PD, Pos);
                        SetUpdateSheet(*value->Conn, PD, " datatime_end = now() ", " datatime_end IS NULL AND ");
                        DeleteNullSgeet(*value->Conn, PD, Pos);
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }
        };

        //Зона 5: Выдфча
        namespace Z5{
            const int Pos = 5;
            DWORD DataAlloyThikn(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    MySetWindowText(value);
                    LocSheet(*value->Conn, PD, Pos);
                }
                CATCH(HardLogger, "");
                return 0;
            }

            DWORD Data(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    PD.SubSheet->GetValue();
                    MySetWindowText(value);
                    LocSheet(*value->Conn, PD, Pos);
                    OutTime(PD, HWNDCLIENT::hEditPlate_DataZ5_Time);

                    SetUpdateSheet(*value->Conn, PD, " datatime_end = now() ", " datatime_end IS NULL AND ");
                }
                CATCH(HardLogger, "");
                return 0;
            }

        }

        //Зона 6: Кантовка
        namespace Z6{
            const int Pos = 6;
            DWORD DataTime(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];
                    //if(!IsSheet(PD))
                    //    PD = PlateData[Pos - 1];

                    MySetWindowText(value);
                    LocSheet(*value->Conn, PD, Pos);
                }
                CATCH(HardLogger, "");
                return 0;
            }

            DWORD DataAlloyThikn(Value* value)
            {
                try
                {
                    T_PlateData& PD = PlateData[Pos];

                    MySetWindowText(value);
                    LocSheet(*value->Conn, PD, Pos);

                    PD.AlloyCodeText->GetValue();
                    PD.ThiknessText->GetValue();

                    std::string update;
                    if(PD.AlloyCodeText->strVal.length())
                    {
                        update = " alloy = '" + PD.AlloyCodeText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                    if(PD.ThiknessText->strVal.length())
                    {
                        update = " thikness = '" + PD.ThiknessText->strVal + "'";
                        SetUpdateSheet(*value->Conn, PD, update, "");
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }

            //int16_t Old_SheetInCassette = 0;
            //int16_t getSheetInCassette(int16_t New_SheetInCassette)
            //{
            //    if(!Old_SheetInCassette || Old_SheetInCassette + 1 != New_SheetInCassette)
            //        New_SheetInCassette++;
            //    Old_SheetInCassette = New_SheetInCassette;
            //    return New_SheetInCassette;
            //}
            void UpdateInCant(PGConnection& conn)
            {
                T_PlateData PD = PlateData[Pos];
                T_CassetteData Cassette = HMISheetData.Cassette;

                int32_t id = Stoi(SheetPos(conn, PD, Pos));

                int32_t  CasseteId = Cassette::CassettePos(conn, Cassette);
                //std::string DataTime = GetStringDataTime();

                std::stringstream co;
                co << "UPDATE sheet SET";
                co << " pos = 6";
                co << ", cassette = " << CasseteId;
                co << ", year = " << Cassette.Year->Val.As<int32_t>();
                co << ", month = " << Cassette.Month->Val.As<int32_t>();
                co << ", day = " << Cassette.Day->Val.As<int32_t>();
                co << ", hour = " << Cassette.Hour->Val.As<uint16_t>();
                co << ", cassetteno = " << Cassette.CassetteNo->Val.As<int32_t>();
                co << ", sheetincassette = " << (Cassette.SheetInCassette->Val.As<int16_t>() + 1);
                co << ", incant_at = now(), correct = DEFAULT, pdf = DEFAULT";
                co << " WHERE";
                if(id != 0)
                {
                    co << " id = " << id;
                } else
                {
                    std::stringstream sd;
                    sd << " melt = " << PD.Melt->GetInt();
                    sd << " AND slab = " << PD.Slab->GetInt();
                    sd << " AND pack = " << PD.Pack->GetInt();
                    sd << " AND partno = " << PD.PartNo->GetInt();
                    sd << " AND sheet = " << PD.Sheet->GetInt();
                    sd << " AND subsheet = " << PD.SubSheet->GetInt();
                    LOG_INFO(HardLogger, "{:90}| Not find Sheet {}", sd.str());
                    co << sd.str();
                }
                SETUPDATESQL(HardLogger, conn, co);
            }

            DWORD Data(Value* value)
            {
                try
                {
                    T_PlateData PD = PlateData[Pos];

                    //if(!IsSheet(PD))
                    //    PD = PlateData[5];

                    MySetWindowText(value);
                    LocSheet(*value->Conn, PD, Pos);

                    if(IsSheet(PD))
                    {
                        //std::string update = " incant_at = '" + GetStringDataTime() + "'";
                        //SetUpdateSheet(conn_kpvl, PD, update, ""); //" incant_at IS NULL  AND");
						SetUpdateSheet(*value->Conn, PD, " datatime_end = now() ", " datatime_end IS NULL AND ");

						UpdateInCant(*value->Conn);
                        DeleteNullSgeet(*value->Conn, PD, Pos);
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }

            DWORD SetSaveDone(LPVOID)
            {
                try
                {
                    PGConnection conn;
                    CONNECTION1(conn, HardLogger);
                    if(HMISheetData.NewData->Val.As<bool>())
                    {
                        try
                        {
                            T_Side Top_Side = HMISheetData.Top_Side;
                            T_Side Bot_Side = HMISheetData.Bot_Side;
                            T_CassetteData Cassette = HMISheetData.Cassette;
                            T_PlateData PD = PlateData[Pos];

                            if(IsSheet(PD))
                            {
                                #pragma region InitTag

                                int32_t id = Stoi(SheetPos(conn, PD, Pos));
                                std::string DataTime = GetStringDataTime();

                                int32_t Year = Cassette.Year->GetValue().As<int32_t>();
                                int32_t Month = Cassette.Month->GetValue().As<int32_t>();
                                int32_t Day = Cassette.Day->GetValue().As<int32_t>();
                                uint16_t Hour = Cassette.Hour->GetValue().As<uint16_t>();
                                int32_t CassetteNo = Cassette.CassetteNo->GetValue().As<int32_t>();
                                int16_t SheetInCassette = Cassette.SheetInCassette->GetValue().As<int16_t>() + 1;

                                int Melt = PD.Melt->GetInt();
                                int Slab = PD.Slab->GetInt();
                                int Pack = PD.Pack->GetInt();
                                int PartNo = PD.PartNo->GetInt();
                                int Sheet = PD.Sheet->GetInt();
                                int SubSheet = PD.SubSheet->GetInt();

                                int32_t  CasseteId = Cassette::CassettePos(conn, HMISheetData.Cassette);

                                #pragma endregion

                                #pragma region co = "UPDATE sheet SET"
                                std::stringstream co;
                                co << "UPDATE sheet SET pos = 7, news = 1, cant_at = now(), correct = DEFAULT, pdf = DEFAULT ";
                                co << ", cassette = " << CasseteId;
                                co << ", top1 = " << Top_Side.h1->Val.As<float>();  // GetFloat();
                                co << ", top2 = " << Top_Side.h2->Val.As<float>();  // GetFloat();
                                co << ", top3 = " << Top_Side.h3->Val.As<float>();  // GetFloat();
                                co << ", top4 = " << Top_Side.h4->Val.As<float>();  // GetFloat();
                                co << ", top5 = " << Top_Side.h5->Val.As<float>();  // GetFloat();
                                co << ", top6 = " << Top_Side.h6->Val.As<float>();  // GetFloat();
                                co << ", top7 = " << Top_Side.h7->Val.As<float>();  // GetFloat();
                                co << ", top8 = " << Top_Side.h8->Val.As<float>();  // GetFloat();
                                co << ", bot1 = " << Bot_Side.h1->Val.As<float>();  // GetFloat();
                                co << ", bot2 = " << Bot_Side.h2->Val.As<float>();  // GetFloat();
                                co << ", bot3 = " << Bot_Side.h3->Val.As<float>();  // GetFloat();
                                co << ", bot4 = " << Bot_Side.h4->Val.As<float>();  // GetFloat();
                                co << ", bot5 = " << Bot_Side.h5->Val.As<float>();  // GetFloat();
                                co << ", bot6 = " << Bot_Side.h6->Val.As<float>();  // GetFloat();
                                co << ", bot7 = " << Bot_Side.h7->Val.As<float>();  // GetFloat();
                                co << ", bot8 = " << Bot_Side.h8->Val.As<float>();  // GetFloat();
                                co << ", year = " << Year;
                                co << ", month = " << Month;
                                co << ", day = " << Day;
                                co << ", hour = " << Hour;
                                co << ", cassetteno = " << CassetteNo;
                                co << ", sheetincassette = " << SheetInCassette;
                                co << " WHERE";
                                #pragma endregion

                                #pragma region WHERE ...
                                if(id != 0)
                                {
                                    co << " id = " << id;
                                } 
                                else
                                {
                                    std::stringstream sd;
                                    sd << " melt = " << Melt;
                                    sd << " AND slab = " << Slab;
                                    sd << " AND pack = " << Pack;
                                    sd << " AND partno = " << PartNo;
                                    sd << " AND sheet = " << Sheet;
                                    sd << " AND subsheet = " << SubSheet;
                                    LOG_INFO(HardLogger, "{:90}| Not find Sheet {}", sd.str());
                                    co << sd.str();
                                }
                                #pragma endregion

                                LOG_INFO(HardLogger, "{:90}| Set SaveDone->Set_Value(true), {}\r\n", FUNCTION_LINE_NAME, co.str());
                                SETUPDATESQL(HardLogger, conn, co);
                                //LOG_INFO(HardLogger, "{:90}| Set SaveDone->Set_Value(true), CasseteId={}, Id={}, Melt={}, Slab={},PartNo={}, Pack={}, Sheet={}, SubSheet={}\r\n", FUNCTION_LINE_NAME, CasseteId, id, PD.Melt->GetString(), PD.Slab->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString(), PD.SubSheet->GetString());
                                UpdateCountSheet(conn, CasseteId);
                                MySetWindowText(winmap(hEdit_Sheet_DataTime), DataTime);
                                std::this_thread::sleep_for(std::chrono::seconds(1));
                            } 
                            else
                            {
                                LOG_INFO(HardLogger, "{:90}| Not Set SaveDone->Set_Value(true), Melt={}, PartNo={}, Pack={}, Sheet={}\r\n", FUNCTION_LINE_NAME, PD.Melt->GetString(), PD.PartNo->GetString(), PD.Pack->GetString(), PD.Sheet->GetString());
                            }
                        }
                        CATCH(HardLogger, "");
                        HMISheetData.SaveDone->Set_Value(true);
                        PalletSheet[Pos - 1].Clear();
                        PalletSheet[Pos].Clear();
                        PDF::CorrectSheet(0);
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }

            //Новые лист в касету, Кассета наполяентся
            DWORD NewSheetData(Value* value)
            {
                try
                {
                    bool b = value->GetBool();// Val.As<bool>();
                    MySetWindowText(winmap(value->winId), CantTextOut[b]);
                    if(b) //Если лист новый
                    {
                        T_PlateData PD = PlateData[Pos];
                        if(IsSheet(PD))
                        {
                            SetUpdateSheet(*value->Conn, PD, " incant_at = now() ", " incant_at IS NULL AND ");
                            CreateThread(0, 0, SetSaveDone, (LPVOID)0, 0, 0);
                        }
                        //Коррекция листа
                        //CreateThread(0, 0, PDF::CorrectSheet, (LPVOID)0, 0, 0);
                    }
                }
                CATCH(HardLogger, "");
                return 0;
            }
        }
    }

    //Операции в зонах
    namespace ZState{
        DWORD WINAPI ThreadState2(LPVOID)
        {
            //LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, 1);
            try
            {
                PGresult* res = NULL;
                int32_t Melt     = Stoi(PalletSheet[2].Melt);
                int32_t Pack     = Stoi(PalletSheet[2].Pack);
                int32_t PartNo   = Stoi(PalletSheet[2].PartNo);
                int32_t Sheet    = Stoi(PalletSheet[2].Sheet);
                int32_t SubSheet = Stoi(PalletSheet[2].SubSheet);
                int32_t Slab     = Stoi(PalletSheet[2].Slab);

                std::string lam_te1 = AI_Hmi_210.LAM_TE1->GetString();
                std::string za_te3 = AI_Hmi_210.Za_TE3->GetString();
                std::string za_pt3 = AI_Hmi_210.Za_PT3->GetString();

                float Time_Z2 = GenSeqToHmi.HeatTime_Z2->GetFloat();
                int16_t StateNo = GenSeqToHmi.Seq_2_StateNo->GetInt();

                if(Time_Z2 == 0)
                {
                    time_t st;
                    std::string datatimeend_at = GetStringOfDataTime(st);
                    if(datatimeend_at.length())
                        Time_Z2 = SQL::GetHeatTime_Z2(conn_dops, datatimeend_at);
                }

                if(Melt /*&& Pack*/ && PartNo && Sheet)
                {
                    int Id = Stoi(Sheet::GetIdSheet(conn_dops, Melt, Pack, PartNo, Sheet, SubSheet, Slab));

                    std::stringstream ss1;
                    ss1 << "UPDATE sheet SET ";
                    ss1 << "datatime_end = now()";
                    if(Time_Z2)
                        ss1 << ", datatime_all = " << Time_Z2;

                    ss1 << " WHERE datatime_end IS NULL AND"; //delete_at IS NULL AND 
                    ss1 << " id = " << Id;

                    std::string command = ss1.str();
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);

                    res = conn_dops.PGexec(command);
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(HardLogger, res, command);
                    PQclear(res);

                    
                    int r = 4;
                    while(isRun && --r)  //5 секунд
                    {
                        SetWindowText(winmap(hEditState_34), std::to_string(r).c_str());
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }

                    std::string LaminTop  = AI_Hmi_210.LaminPressTop->GetString();
                    std::string LaminBot = AI_Hmi_210.LaminPressBot->GetString();

                    std::stringstream ss4;
                    ss4 << "UPDATE sheet SET ";
                    ss4 << "lam_te1 = " << lam_te1 << ", ";
                    ss4 << "za_te3 = " << za_te3 << ", ";
                    ss4 << "za_pt3 = " << za_pt3 << ", ";
                    ss4 << "lampresstop = " << LaminTop << ", ";
                    ss4 << "lampressbot = " << LaminBot << " ";

                    ss4 << "WHERE";
                    ss4 << " id = " << Id;

                    command = ss4.str();
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                    res = conn_dops.PGexec(command);
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(HardLogger, res, command);
                    PQclear(res);
                }
            }
            CATCH(HardLogger, "");

            hThreadState2 = NULL;
            return 0;
        }

        //Операция в 1 зоне
        DWORD DataPosState_1(Value* value)
        {
            try
            {
                int16_t val = value->GetInt();
                MySetWindowText(value);
                SetWindowText(winmap(hEditState_12), GenSeq1[val].c_str());
                if(val == 3)
                {

                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Операция в 2 зоне
        DWORD DataPosState_2(Value* value)
        {
            try
            {
                int16_t val = value->GetInt();
                MySetWindowText(value);
                SetWindowText(winmap(hEditState_22), GenSeq2[val].c_str());
                if(val == 5 /*|| val == 6*/)
                {
                    if(hThreadState2 == NULL)
                        hThreadState2 = CreateThread(0, 0, ThreadState2, (LPVOID)0, 0, 0);

                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Операция в 3 зоне
        DWORD DataPosState_3(Value* value)
        {
            try
            {
                MySetWindowText(value);
                SetWindowText(winmap(hEditState_32), GenSeq3[value->GetInt()].c_str());

                MySetWindowText(winmap(value->winId), value->GetString().c_str());
            }
            CATCH(HardLogger, "");
            return 0;
        }
    }

    //Функции обработки и со сохранения истории кассет
    namespace Cassette{

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
                //int16_t SheetInCassette = CD.SheetInCassette->GetInt();

                return Day && Month && Year && CassetteNo /*&& SheetInCassette*/;
            }
            CATCH(HardLogger, "");
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
                    co << " year = " << CD.Year->GetInt();
                    co << " AND month = " << CD.Month->GetInt();
                    co << " AND day = " << CD.Day->GetInt();
                    co << " AND hour = " << CD.Hour->GetInt();
                    co << " AND cassetteno = " << CD.CassetteNo->GetInt();
                    co << " LIMIT 1;";
                    std::string command = co.str();
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                    PGresult* res = conn.PGexec(command);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        if(PQntuples(res))
                            id = Stoi(conn.PGgetvalue(res, 0, 0));
                    }
                    else
                        LOG_ERR_SQL(HardLogger, res, command);
                    PQclear(res);
                }
            }
            CATCH(HardLogger, "");
            return id;
        }

        //Добовление кассеты в базу
        int32_t InsertCassette(PGConnection& conn, T_CassetteData& CD)
        {
            int32_t id = 0;
            try
            {
                //21:57:43.225 | Час кассеты за день = 21
                //21:57:41.318 | Кассета наполяентся = true
                //21:57:38.305 | Номер кассеты за день = 4
                //21:57:26

                if(IsCassette(CD))
                {
                    //int hour = CD.Hour->GetValue().As<uint16_t>();// GetInt();
                    
                    std::stringstream co;
                    co << "INSERT INTO cassette ";
                    co << "(event, year, month, day, cassetteno, hour, sheetincassette) VALUES (1, ";
                    co << CD.Year->GetInt() << ", ";
                    co << CD.Month->GetInt() << ", ";
                    co << CD.Day->GetInt() << ", ";
                    co << CD.CassetteNo->GetInt() << ", ";
                    co << CD.Hour->GetInt() << ", ";

                    int16_t count = CD.SheetInCassette->Val.As<int16_t>();
                    if(!count) count = -1;
                    co << count << ");";

                    LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, co.str());
                    SETUPDATESQL(HardLogger, conn, co);

                    id = GetIdCassette(conn, CD);
                }
            }
            CATCH(HardLogger, "");
            return id;
        }

        //Обновляем данные по кассете если кассета есть или добовляем новую
        int32_t CassettePos(PGConnection& conn, T_CassetteData& CD)
        {
            int32_t id = 0;
            try
            {
                CD.Year->GetValue();
                CD.Month->GetValue();
                CD.Day->GetValue();
                CD.CassetteNo->GetValue();
                CD.SheetInCassette->GetValue();
                CD.Hour->GetValue();

                if(IsCassette(CD))
                {
                    //UpdateCassette(conn, CD, id);

                    id = GetIdCassette(conn, CD);
                    if(!id)
                    {
                        id = InsertCassette(conn, CD);
                    }
                    //else
                    //    SetOldCassette(CD, id);
                }
            }
            CATCH(HardLogger, "");
            return id;
        }

        //Вывод Номер листа в касете
        DWORD Sheet_InCassette(Value* value)
        {
            try
            {
                //char ss[256];
                //int InCassette = value->GetInt();
                //sprintf_s(ss, 256, "%d", InCassette);
                //MySetWindowText(winmap(value->winId), ss);
                //if(!Sheet::Z6::Old_SheetInCassette)
                //    Sheet::Z6::Old_SheetInCassette = value->GetInt();
                MySetWindowText(value);
                MySetWindowText(winmap(hEdit_Sheet_DataTime), GetStringDataTime());
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Вывод Номер кассеты за день
        DWORD CassetteNo(Value* value)
        {
            try
            {
                MySetWindowText(winmap(hEdit_Sheet_CassetteNo), value->GetString().c_str());
                MySetWindowText(winmap(hEdit_Sheet_CassetteNew), value->GetString().c_str());
                MySetWindowText(winmap(hEdit_Sheet_DataTime), GetStringDataTime());
                MySetWindowText(value);
                if(HMISheetData.CasseteIsFill->GetValue().As<bool>())
                {
                    int32_t id = CassettePos(*value->Conn, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteNo = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), id);
                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Вывод Год ID листа
        DWORD CassetteYear(Value* value)
        {
            try
            {
                MySetWindowText(value);
                MySetWindowText(winmap(hEdit_Sheet_DataTime), GetStringDataTime());
                if(HMISheetData.CasseteIsFill->GetValue().As<bool>())
                {
                    int32_t id = CassettePos(*value->Conn, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteYear = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), id);
                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Вывод Месяц ID листа
        DWORD CassetteMonth(Value* value)
        {
            try
            {
                MySetWindowText(value);
                MySetWindowText(winmap(hEdit_Sheet_DataTime), GetStringDataTime());
                if(HMISheetData.CasseteIsFill->GetValue().As<bool>())
                {
                    int32_t id = CassettePos(*value->Conn, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteMonth = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), id);
                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Вывод День ID листа
        DWORD CassetteDay(Value* value)
        {
            try
            {
                MySetWindowText(value);
                MySetWindowText(winmap(hEdit_Sheet_DataTime), GetStringDataTime());
                if(HMISheetData.CasseteIsFill->GetValue().As<bool>())
                {
                    int32_t id = CassettePos(*value->Conn, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteDay = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), id);
                }
            }
            CATCH(HardLogger, "");
            return 0;
        }


        //Вывод Час ID листа
        DWORD CassetteHour(Value* value)
        {
            try
            {
                MySetWindowText(value);
                MySetWindowText(winmap(hEdit_Sheet_DataTime), GetStringDataTime());
                if(HMISheetData.CasseteIsFill->GetValue().As<bool>())
                {
                    int32_t id = CassettePos(*value->Conn, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CassetteHour = {} Id = {}", FUNCTION_LINE_NAME, value->GetInt(), id);
                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //кассета наполяентся 
        // false = закраваем касету
        // true = создаем касету
        DWORD CasseteIsFill(Value* value)
        {
            try
            {
                MySetWindowText(winmap(hEdit_Sheet_DataTime), GetStringDataTime());
                bool b = value->Val.As<bool>();
                SetWindowText(winmap(value->winId), CassetTextOut[b].c_str());
                
                if(b)
                {
                    int32_t id = CassettePos(*value->Conn, HMISheetData.Cassette);
                    LOG_INFO(HardLogger, "{:90}| CasseteIsFill = true, Id = {}", FUNCTION_LINE_NAME, id);
                    
                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

    }


#pragma region Вспомогательные функции
    //Аналоги
    namespace An{

       //Скорость выгрузки, Уставка температуры, Давление, Температура воды
        //Уставка температуры

        DWORD TempSet1(Value* value)
        {
            try
            {
                MySetWindowText(value);

                if(value->GetFloat() > 0)
                {
                    std::string update = " temper = " + value->GetString();
                    Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, "");
                    Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, "");
                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Скорость выгрузки
        DWORD UnloadSpeed(Value* value)
        {
            try
            {
                MySetWindowText(value);

                if(value->GetFloat() > 0)
                {
                    std::string update = " speed = " + value->GetString();
                    Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, " speed = 0 AND");
                    Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, " speed = 0 AND");
                }
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Время сигнализации окончания нагрева, мин
        DWORD fTimeForPlateHeat(Value* value)
        {
            try
            {
                MySetWindowText(value);

                std::string update = " timeforplateheat = " + value->GetString();
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, "");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, " timeforplateheat = 0 AND");
            }
            CATCH(HardLogger, "");
            return 0;
        }

        //Уставка давления для запуска комперссора
        DWORD fPresToStartComp(Value* value)
        {
            try
            {
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                std::string update = " prestostartcomp = " + value->GetString();
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, "");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, "");
            }
            CATCH(HardLogger, "");

            return 0;
        }

    };

    //Режим работы клапана
    //State_1 = 3 
    //Маска клапанов
    namespace Mask{
        DWORD DataMaskKlapan1(Value* value)
        {
            try
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
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, "");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, "");
            }
            CATCH(HardLogger, "");
            return 0;
        }
        DWORD DataMaskKlapan2(Value* value)
        {
            try
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
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, "");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, "");
            }
            CATCH(HardLogger, "");
            return 0;
        }
    }

    //Скоростная секция
    namespace Speed{
        //Верх
        DWORD SpeedSectionTop(Value* value)
        {
            try
            {
                MySetWindowText(value);
                std::string update = " posclapantop = " + value->GetString();
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, " posclapantop = 0 AND");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, " posclapantop = 0 AND");
            }
            CATCH(HardLogger, "");
            return 0;
        }
        //Низ
        DWORD SpeedSectionBot(Value* value)
        {
            try
            {
                MySetWindowText(value);
                std::string update = " posclapanbot = " + value->GetString();
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, " posclapanbot = 0 AND");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, " posclapanbot = 0 AND");
            }
            CATCH(HardLogger, "");
            return 0;
        }
    }

    //Ламинарная секция 1
    namespace Lam1{
        //Верх
        DWORD LaminarSection1Top(Value* value)
        {
            try
            {
                MySetWindowText(value);
                std::string update = " lam1posclapantop = " + value->GetString();
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, " lam1posclapantop = 0 AND");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, " lam1posclapantop = 0 AND");
            }
            CATCH(HardLogger, "");
            return 0;
        }
        //Низ
        DWORD LaminarSection1Bot(Value* value)
        {
            try
            {
                MySetWindowText(value);
                std::string update = " lam1posclapanbot = " + value->GetString();
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, " lam1posclapanbot = 0 AND");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, " lam1posclapanbot = 0 AND");
            }
            CATCH(HardLogger, "");
            return 0;
        }
    };

    //Ламинарная секция 2
    namespace Lam2{
        //Верх
        DWORD LaminarSection2Top(Value* value)
        {
            try
            {
                MySetWindowText(value);
                std::string update = " lam2posclapantop = " + value->GetString();
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, " lam2posclapantop = 0 AND");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, " lam2posclapantop = 0 AND");
            }
            CATCH(HardLogger, "");
            return 0;
        }
        //Низ
        DWORD LaminarSection2Bot(Value* value)
        {
            try
            {
                MySetWindowText(value);
                std::string update = " lam2posclapanbot = " + value->GetString();
                Sheet::SetUpdateSheet(*value->Conn, PlateData[1], update, " lam2posclapanbot = 0 AND");
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, " lam2posclapanbot = 0 AND");
            }
            CATCH(HardLogger, "");
            return 0;
        }
    };

    //Отклонения листа на кантовке
    DWORD Side(Value* value)
    {
        try
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
                    if(DEB)LOG_INFO(HardLogger, "{:90}| {}", FUNCTION_LINE_NAME, up.str());
                    Sheet::SetUpdateSheet(*value->Conn, PlateData[6], up.str(), "");
                }
            }
        }
        CATCH(HardLogger, "");
        return 0;
    }
    
    DWORD Temperature(Value* value)
    {
        try
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
                Sheet::SetUpdateSheet(*value->Conn, PlateData[2], update, "");
            }
        }
        CATCH(HardLogger, "");
        return 0;
    }

    //Бит жмизни
    DWORD SheetData_WDG_toBase(Value* value)
    {
        try
        {
            if(value->GetBool())
            {
                PLC_KPVL_old_dt = time(NULL);
                struct tm TM;
                localtime_s(&TM, &PLC_KPVL_old_dt);
                HMISheetData.WDG_fromBase->Set_Value(true);
                SetWindowText(winmap(value->winId), string_time(&TM).c_str());
            }
        }
        CATCH(HardLogger, "");
        return 0;
    }


#pragma endregion
};

