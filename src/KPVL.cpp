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
    {0, "���������"},
    {1, "����������"},
    {2, "�������"},
    {3, "������� ������� �����"},
    {4, "�������� � ����"},
    {5, "������� ������� �����"},
    {6, "������ �����"},
    {7, "�������� �� 2 ��������"},
    {8, "�������� �� 2-� �������� ����"},
};
std::map<int, std::string> GenSeq2 ={
    {0, "���������"},
    {1, "����������"},
    {2, "�������"},
    {3, "����� ��������� � 1-�� ��������� ����"},
    {4, "���������. ������ �����"},
    {5, "������� �������� �����"},
    {6, "������ � ����� �������"},
    {7, "������� �������� �����"},
};
std::map<int, std::string> GenSeq3 ={
    {0, "���������"},
    {1, "�������� �����"},
    {2, "���������. ���.�����."},
    {3, "������ ���������"},
    {4, "��������� ����� ���������"},
};


TSheet PalletSheet[7];

std::deque<TSheet>AllSheet;

//�������� ������ �� 2 ������ ����� 5 ������ ����� State_2 = 5
//����� ������� � ������� ������
#pragma region //����� ������� � ������� ������
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
//};
#pragma endregion

//���� �������
namespace KPVL {

    //������ ��������� 100 ������ �� ����
    namespace SQL
    {


        //�������� ������ ������� � ������� sheet
        void GetCollumn(PGresult* res)
        {
            if(!Col_Sheet_SecondPos_at)
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
                    else if(l == "cassetteno") Col_Sheet_cassetteno = j;
                    else if(l == "sheetincassette") Col_Sheet_sheetincassette = j;
                    else if(l == "timeforplateheat") Col_Sheet_timeforplateheat = j;
                    else if(l == "prestostartcomp") Col_Sheet_prestostartcomp = j;
                    else if(l == "temperature") Col_Sheet_temperature = j;
                    else if(l == "correct") Col_Sheet_correct = j;
                    else if(l == "secondpos_at") Col_Sheet_SecondPos_at = j;
                    
                    
                }
            }
        }

                //������ ������
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

                sheet.Hour = conn.PGgetvalue(res, l, Col_Sheet_day);
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

                Sheet.push_back(sheet);
            }
        }

        //�������� ������ ������ �� ����
        void KPVL_SQL(PGConnection& conn, std::deque<TSheet>& Sheet)
        {
            Sheet.erase(Sheet.begin(), Sheet.end());

            std::time_t stop = time(NULL);
            std::time_t statr = static_cast<std::time_t>(difftime(stop, 60 * 60 * 24 * 10)); //7-� �����
            std::string start_at = GetDataTimeString(&statr);

            std::stringstream FilterComand;
            FilterComand << "SELECT * FROM sheet ";
            FilterComand << "WHERE  create_at > '" << start_at << "'";
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
            std::string next_at = "";
            std::string sout = "";
            if(enddata_at.length())
            {
                std::stringstream co;
                co << "SELECT max(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at <= '" << enddata_at << "';";
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
                co << "SELECT content FROM todos WHERE id_name = " << GenSeqToHmi.HeatTime_Z2->ID << " AND create_at = '" << next_at << "';";
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
                    co << "SELECT min(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_2_StateNo->ID << " AND content = '3' AND create_at > '" << TS.Start_at << "';"; //GenSeqToHmi.Data.Seq_2_StateNo ����� ���� ������������������ �������� � ����
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
                    co << "SELECT min(create_at) FROM todos WHERE id_name = " << GenSeqToHmi.Seq_1_StateNo->ID << " AND content = '5' AND create_at > '" << next_at << "';"; //GenSeqToHmi.Data.Seq_2_StateNo ����� ���� ������������������ �������� � ����
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
                    co << "UPDATE sheet SET datatime_end = '" << enddata_at << "', datatime_all = " << HeatTime_Z2 << " WHERE id = " << Id;
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

    //�� ����� � ��������� ������ �� �����
    uint32_t NextID = 1;

//������� ��������� � �� ���������� ������� ������
#pragma region ������� ��������� � �� ���������� ������� ������
    //��������� ������ �� �����
    namespace Sheet{

        //�������� �� ������� �����
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
            int32_t Melt = Stoi(PS.Melt);
            int32_t Pack = Stoi(PS.Pack);
            int32_t PartNo = Stoi(PS.PartNo);
            int32_t Sheet = Stoi(PS.Sheet);
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

        //�������� ID �����
        std::string GetIdSheet(PGConnection& conn, T_PlateData& PD)
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
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                PGresult* res = conn.PGexec(comand);

                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    if(PQntuples(res))//�����
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

        //�������� ID �����
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
                co << "SELECT id FROM sheet WHERE";
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

                //�������� ID �����
        std::string GetIdSheet(PGConnection& conn, int32_t Melt, int32_t Pack, int32_t PartNo, int32_t Sheet, int32_t SubSheet, int32_t Slab)
        {
            std::string id = "0";

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

        //���������� ����� � ����
        void InsertSheet(PGConnection& conn, T_PlateData& PD, int Pos)
        {
            if(IsSheet(PD))
            {
                std::string id = GetIdSheet(conn, PD);
                if(!id.length() || id == "" || id == "0")
                {
                    std::stringstream sd;
                    sd << "INSERT INTO sheet ";
                    sd << "(";
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
#pragma endregion

#pragma region MyRegion
                    if(Pos == 1 || Pos == 2)
                    {
                        sd << GenSeqFromHmi.TempSet1->Val.As<float>() << ", ";
                        sd << Par_Gen.UnloadSpeed->Val.As<float>() << ", ";

                        sd << Par_Gen.TimeForPlateHeat->Val.As<float>() << ", ";
                        sd << Par_Gen.PresToStartComp->Val.As<float>() << ", ";

                        sd << HMISheetData.SpeedSection.Top->Val.As<float>() << ", ";
                        sd << HMISheetData.SpeedSection.Bot->Val.As<float>() << ", ";

                        sd << HMISheetData.LaminarSection1.Top->Val.As<float>() << ", ";
                        sd << HMISheetData.LaminarSection1.Bot->Val.As<float>() << ", ";

                        sd << HMISheetData.LaminarSection2.Top->Val.As<float>() << ", ";
                        sd << HMISheetData.LaminarSection2.Bot->Val.As<float>() << ", ";

                        sd << "'" + MaskKlapan + "', ";
                    }
#pragma endregion

                    sd << Pos << ");";

                    SETUPDATESQL(SQLLogger, conn, sd);
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


        //���������� � ���� ������ �� �����
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

        //���������� � ���� ������ �� �����
        void SetUpdateSheet(PGConnection& conn, T_PlateData& PD, std::string update, std::string where)
        {
            bool ret = true;
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

        //��������� ������� �����
        void UpdateSheetPos(PGConnection& conn, T_PlateData& PD, std::string id, int Pos)
        {
            if(IsSheet(PD))
            {
                int pos = 0;
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

                if(pos != Pos && pos > 0 && pos < 7)
                {
                    LOG_INFO(HardLogger, "{:90}| SheetId={}, OldPos={} --> NewPos={}", FUNCTION_LINE_NAME, id, pos, Pos);
                    std::stringstream sd;
                    sd << "UPDATE sheet SET";
                    sd << " pos = " << Pos;
                    sd << " WHERE";
                    sd << " id = " << id;
                    sd << ";";
                    SETUPDATESQL(SQLLogger, conn, sd);
                    //std::string comand = sd.str();
                    //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                    //PGresult* res = conn.PGexec(comand);
                    //LOG_INFO(SQLLogger, "{:90}| SheetId={}, OldPos={} --> NewPos={}", FUNCTION_LINE_NAME, id, pos, Pos);
                    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    //    LOG_ERR_SQL(SQLLogger, res, comand);
                    //PQclear(res);
                }
            }
        };

        //��������� ������ �� ����� ���� ���� ���� ��� ��������� �����
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
                if(PD.Melt->Val.As<int32_t>() != Stoi(TS.Melt)
                   || PD.Pack->Val.As<int32_t>() != Stoi(TS.Pack)
                   || PD.PartNo->Val.As<int32_t>() != Stoi(TS.PartNo)
                   || PD.Sheet->Val.As<int32_t>() != Stoi(TS.Sheet)
                   || PD.SubSheet->Val.As<int32_t>() != Stoi(TS.SubSheet)
                   || PD.Slab->Val.As<int32_t>() != Stoi(TS.Slab))
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

        //����� �������
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


//������� ��������� � �� ���������� ������� ������

#pragma region ���� ���� �������

    //����
    namespace Sheet{
    //���� 0: �� ����� � ����
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
                //DeleteNullSgeet(conn, PD, Pos);
                LocSheet(PD, Pos);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ0_Time);
                return 0;
            }
        }

        //���� 1: 1 ���� ���� 
        namespace Z1{
            const int Pos = 1;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
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
                MySetWindowText(value);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];

                MySetWindowText(value);
                SheetPos(conn_kpvl, PD, Pos);

                std::string update = " temper = " + GenSeqFromHmi.TempSet1->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " speed = " + Par_Gen.UnloadSpeed->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                SetUpdateSheet(conn_kpvl, PD, " start_at = (SELECT MAX(create_at) FROM todos WHERE id_name = " + std::to_string(GenSeqToHmi.Seq_1_StateNo->ID) + " AND create_at <= now() AND content = '3')", " start_at IS NULL AND ");

                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ1_Time);
                DeleteNullSgeet(conn_kpvl, PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        };

        //���� 2: 2 ���� ����
        namespace Z2{
            const int Pos = 2;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
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


                MySetWindowText(value);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                MySetWindowText(value);
                SheetPos(conn_kpvl, PD, 2);

                std::string update = " temper = " + GenSeqFromHmi.TempSet1->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " speed = " + Par_Gen.UnloadSpeed->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " timeforplateheat = " + Par_Gen.TimeForPlateHeat->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                update = " prestostartcomp = " + Par_Gen.PresToStartComp->GetString();
                SetUpdateSheet(conn_kpvl, PD, update, "");

                SetUpdateSheet(conn_kpvl, PD, " secondpos_at = now() ", " secondpos_at IS NULL AND ");

                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ2_Time);
                DeleteNullSgeet(conn_kpvl, PD, Pos);
                LocSheet(PD, Pos);

                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        };

        //���� 3: �������
        namespace Z3{
            const int Pos = 3;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
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

                MySetWindowText(value);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD Data(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];;
                MySetWindowText(value);
                OutTime(PD, HWNDCLIENT::hEditPlate_DataZ3_Time);
                SheetPos(conn_kpvl, PD, Pos);
                DeleteNullSgeet(conn_kpvl, PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        };

        //���� 4: ����������
        namespace Z4{
            const int Pos = 4;
            DWORD DataAlloyThikn(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
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
                SheetPos(conn_kpvl, PD, Pos);
                DeleteNullSgeet(conn_kpvl, PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData& PD = PlateData[Pos];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        };

        //���� 5: ������
        namespace Z5{
            const int Pos = 5;
            DWORD DataAlloyThikn(Value* value)
            {
                //T_PlateData& PD = PlateData[Pos];
                //PD.AlloyCodeText->GetValue();
                //PD.ThiknessText->GetValue();
                //std::string update = " alloy = '" + PD.AlloyCodeText->strVal + "', thikness = '" + PD.ThiknessText->strVal + "'";
                //SetUpdateSheet(conn_kpvl, PD, update, "");
                //
                MySetWindowText(value);
                //LocSheet(PD, Pos);
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
                SetUpdateSheet(conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }
        }

        //���� 6: ��������
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
                //if(!IsSheet(PD))
                //    PD = PlateData[5];
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


                MySetWindowText(value);
                LocSheet(PD, Pos);

                return 0;
            }

            DWORD Data(Value* value)
            {
                MySetWindowText(value);

                T_PlateData PD = PlateData[Pos];
                if(!IsSheet(PD))
                    PD = PlateData[5];

                SheetPos(conn_kpvl, PD, Pos);
                DeleteNullSgeet(conn_kpvl, PD, Pos);
                LocSheet(PD, Pos);
                return 0;
            }

            DWORD DataSlab(Value* value)
            {
                T_PlateData PD = PlateData[Pos];
                //if(!IsSheet(PD))
                //    PD = PlateData[5];
                std::string update = " slab = " + std::to_string(PD.Slab->Val.As<int32_t>());
                SetUpdateSheet(conn_kpvl, PD, update, "");
                MySetWindowText(value);
                return 0;
            }

            void SetSaveDone(PGConnection& conn)
            {
                T_Side Top_Side = HMISheetData.Top_Side;
                T_Side Bot_Side = HMISheetData.Bot_Side;
                T_CassetteData Cassette = HMISheetData.Cassette;

                T_PlateData PD = PlateData[Pos];
                //if(!IsSheet(PD))
                //    PD = PlateData[5];

                if(HMISheetData.NewData->Val.As<bool>())
                {
                    if(IsSheet(PD))
                    {
                        std::string id = GetIdSheet(conn, PD);
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
                            co << ", hour = " << Stoi(Cassette.Hour->GetString());// ->Val.As<int32_t>();
                            co << ", day = " << Cassette.Day->Val.As<int32_t>();
                            co << ", month = " << Cassette.Month->Val.As<int32_t>();
                            co << ", year = " << Cassette.Year->Val.As<int32_t>();
                            co << ", cassetteno = " << Cassette.CassetteNo->Val.As<int32_t>();
                            co << ", sheetincassette = " << (Cassette.SheetInCassette->Val.As<int16_t>() + 1);
                            co << " WHERE id = " << id << ";";
#pragma endregion
                            SETUPDATESQL(SQLLogger, conn, co);

                            //std::string comand = co.str();
                            //if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                            //PGresult* res = conn.PGexec(comand);
                            //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                            //    LOG_ERR_SQL(SQLLogger, res, comand);
                            //PQclear(res);

                            //PlateData[5].Sheet->Set_Value((int32_t)0);
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

            //����� ���� � ������, ������� �����������
            DWORD NewSheetData(Value* value)
            {
                const char* ss = WaitKant;
                if(value->Val.As<bool>())                   //���� ���� �����
                {
                    Cassette::CassettePos(conn_kpvl, HMISheetData.Cassette);
                    SetSaveDone(conn_kpvl);
                    ss = WaitResv;
                }
                //else
                //    LOG_INFO(SQLLogger, "{:90}| NewData = false", FUNCTION_LINE_NAME);
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
            OldCassette.Hour = Stoi(CD.Hour->GetString());// CD.Hour->Val.As<uint16_t>();
            OldCassette.Day = CD.Day->Val.As<int32_t>();
            OldCassette.CassetteNo = CD.CassetteNo->Val.As<int32_t>();
            OldCassette.SheetInCassette = CD.SheetInCassette->Val.As<int16_t>();
        }

        //�������� �� ������� �������
        bool IsCassette(O_CassetteData& CD)
        {
            return CD.Day && CD.Month && CD.Year && CD.CassetteNo;
        }

        //�������� ID �������
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
                    if(PQntuples(res))//�����
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

        //�������� �� ������� �������
        bool IsCassette(T_CassetteData& CD)
        {
            //auto tt = CD.Hour->GetType();
            //uint32_t Hour = 0;// 
            //if(tt == OpcUa::VariantType::UINT16)
            //{
            //    Hour = GetVal<uint16_t>(CD.Hour); //Stoi(CD.Hour->GetString());// 
            //}
            //else if(tt == OpcUa::VariantType::UINT32)
            //{
            //    Hour = GetVal<uint32_t>(CD.Hour); //Stoi(CD.Hour->GetString());// 
            //}

            int32_t Hour = Stoi(CD.Hour->GetString()); //Stoi(CD.Hour->GetString());// 
            int32_t Day = Stoi(CD.Day->GetString());
            int32_t Month = Stoi(CD.Month->GetString());
            int32_t Year = Stoi(CD.Year->GetString());
            int32_t CassetteNo = Stoi(CD.CassetteNo->GetString());
            int16_t SheetInCassette = 0;
            auto t = CD.SheetInCassette->GetType();
            if(t == OpcUa::VariantType::INT16)
                SheetInCassette = Stoi(CD.SheetInCassette->GetString());
            else
                if(t == OpcUa::VariantType::INT32)
                    SheetInCassette = Stoi(CD.SheetInCassette->GetString());
            return Day && Month && Year && CassetteNo && SheetInCassette;
        }

        //�������� ID ������� �� ����� �� ������
        int32_t GetIdCassette(PGConnection& conn, T_CassetteData& CD)
        {
            int32_t id = 0;

            if(IsCassette(CD))
            {
                std::stringstream co;
                co << "SELECT id FROM cassette WHERE";
                co << " hour = " << Stoi(CD.Hour->GetString());//CD.Hour->Val.As<uint32_t>();
                co << " day = " << CD.Day->Val.As<int32_t>();
                co << " AND month = " << CD.Month->Val.As<int32_t>();
                co << " AND year = " << CD.Year->Val.As<int32_t>();
                co << " AND cassetteno = " << CD.CassetteNo->Val.As<int32_t>();
                co << " AND hour <> -1";
                co << ";";
                std::string comand = co.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
                        id = Stoi(conn.PGgetvalue(res, 0, 0));
                else
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
            return id;
        }

        //���������� ������� � ����
        void InsertCassette(PGConnection& conn, T_CassetteData& CD)
        {
            if(IsCassette(CD))
            {
                std::stringstream co;
                co << "INSERT INTO cassette ";
                co << "(event, year, month, day, hour, cassetteno, sheetincassette) VALUES (1, ";
                co << CD.Year->Val.As<int32_t>() << ", ";
                co << CD.Month->Val.As<int32_t>() << ", ";
                co << CD.Day->Val.As<int32_t>() << ", ";
                co << Stoi(CD.Hour->GetString());//CD.Hour->Val.As<int32_t>() << ", ";
                co << CD.CassetteNo->Val.As<int32_t>() << ", ";
                co << CD.SheetInCassette->Val.As<int16_t>() << ");";
                std::string comand = co.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);
                //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, co.str());

                if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);

                int32_t id = GetIdCassette(conn, CD);

                SetOldCassette(CD, id);
            }
        }

        //��������� � ���� ������ �� �������
        void UpdateCassette(PGConnection& conn, T_CassetteData& CD, int32_t id)
        {
            std::stringstream co;
            co << "UPDATE cassette SET";
            co << " year = " << CD.Year->Val.As<int32_t>() << ", ";
            co << " month = " << CD.Month->Val.As<int32_t>() << ", ";
            co << " day = " << CD.Day->Val.As<int32_t>() << ", ";
            co << " hour = " << Stoi(CD.Hour->GetString());//CD.Hour->Val.As<int32_t>() << ", ";
            co << " cassetteno = " << CD.CassetteNo->Val.As<int32_t>() << ", ";
            co << " sheetincassette = " << CD.SheetInCassette->Val.As<int16_t>() << ",";
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

        }

        //��������� ������ �� ������� ���� ������� ���� ��� ��������� �����
        int32_t CassettePos(PGConnection& conn, T_CassetteData& CD) 
        {
            int32_t id = 0;
            if(IsCassette(CD))
            {
                //UpdateCassette(conn, CD, id);
                InsertCassette(conn, CD);
                id = GetIdCassette(conn, CD);
                if(id)
                    SetOldCassette(CD, id);
            }
            return id;
        }

        //��������� ��� �� �������� ������ ����� ������� �� ��������
        void CloseCassete(PGConnection& conn, O_CassetteData& CD)
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
        }



        //����� ����� ����� � ������
        DWORD Sheet_InCassette(Value* value)
        {
            char ss[256];
            sprintf_s(ss, 256, "%d", value->Val.As<int16_t>() + 1);
            MySetWindowText(winmap(value->winId), ss);
            if(HMISheetData.CasseteIsFill->Val.As<bool>())
            {
                int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                LOG_INFO(HardLogger, "{:90}| Sheet_InCassette = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int16_t>(), OldCassette.Id, id);
            }
            return 0;
        }

        //����� ����� ������� �� ����
        DWORD CassetteNo(Value* value)
        {
            MySetWindowText(winmap(hEdit_Sheet_CassetteNo), value->GetString().c_str());
            MySetWindowText(winmap(hEdit_Sheet_CassetteNew), value->GetString().c_str());
            MySetWindowText(value);
            if(HMISheetData.CasseteIsFill->Val.As<bool>())
            {
                int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                LOG_INFO(HardLogger, "{:90}| CassetteNo = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id, id);
            }
            return 0;
        }

        //����� ��� ID �����
        DWORD CassetteHour(Value* value)
        {
            MySetWindowText(value);
            if(HMISheetData.CasseteIsFill->Val.As<bool>())
            {
                int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                LOG_INFO(HardLogger, "{:90}| CassetteHour = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id, id);
            }
            return 0;
        }

        //����� ���� ID �����
        DWORD CassetteDay(Value* value)
        {
            MySetWindowText(value);
            if(HMISheetData.CasseteIsFill->Val.As<bool>())
            {
                int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                LOG_INFO(HardLogger, "{:90}| CassetteDay = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id, id);
            }
            return 0;
        }

        //����� ����� ID �����
        DWORD CassetteMonth(Value* value)
        {
            MySetWindowText(value);
            if(HMISheetData.CasseteIsFill->Val.As<bool>())
            {
                int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                LOG_INFO(HardLogger, "{:90}| CassetteMonth = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id, id);
            }
            return 0;
        }

        //����� ��� ID �����
        DWORD CassetteYear(Value* value)
        {
            MySetWindowText(value);
            if(HMISheetData.CasseteIsFill->Val.As<bool>())
            {
                int32_t id = CassettePos(conn_kpvl, HMISheetData.Cassette);
                LOG_INFO(HardLogger, "{:90}| CassetteYear = {} OldCassette.Id = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<int32_t>(), OldCassette.Id, id);
            }
            return 0;
        }

        //������� ����������� 
        // false = ��������� ������
        // true = ������� ������
        DWORD CasseteIsFill(Value* value)
        {
            MySetWindowText(value);
            bool b = value->Val.As<bool>();
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


            return 0;
        }

        ////������� ������
        //DWORD CassetteIsComplete(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    LOG_INFO(HardLogger, "{:90}| CassetteIsComplete = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<bool>(), OldCassette.Id);
        //    return 0;
        //}
        //
        ////������ ����� �������
        //DWORD StartNewCassette(Value* value)
        //{
        //    MySetWindowText(winmap(value->winId), value->GetString().c_str());
        //    LOG_INFO(HardLogger, "{:90}| StartNewCassette = {} Id = {}", FUNCTION_LINE_NAME, value->Val.As<bool>(), OldCassette.Id);
        //    return 0;
        //}
    }
#pragma endregion


#pragma region ��������������� �������
    //�������� � �����
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


                float Time_Z2 = GenSeqToHmi.HeatTime_Z2->GetVal<float>();
                int16_t StateNo = GenSeqToHmi.Seq_2_StateNo->GetVal<int16_t>();

                if(Time_Z2 == 0)
                {
                    time_t st;
                    std::string datatimeend_at = GetDataTimeString(st);
                    if(datatimeend_at.length())
                        Time_Z2 = SQL::GetHeatTime_Z2(conn_dops, datatimeend_at);
                }
                //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, 2);

                if(Melt && Pack && PartNo && Sheet)
                {
                    //LOG_INFO(SQLLogger, "{:90}| Time_Z2={}, StateNo={}, Melt={}, Pack={}, PartNo={}, Sheet={}, SubSheet={}, Slab={}", FUNCTION_LINE_NAME, Time_Z2, StateNo, Melt, Pack, PartNo, Sheet, SubSheet, Slab);
                    int Id = Stoi(Sheet::GetIdSheet(conn_dops, Melt, Pack, PartNo, Sheet, SubSheet, Slab));

                    std::stringstream ss1;
                    ss1 << "UPDATE sheet SET ";
                    ss1 << "datatime_end = now()";
                    if(Time_Z2)
                        ss1 << ", datatime_all = " << Time_Z2;

                    ss1 << " WHERE datatime_end IS NULL AND";
                    ss1 << " id = " << Id;
                    //ss1 << " AND melt = " << Melt;
                    //ss1 << " AND pack = " << Pack;
                    //ss1 << " AND partno = " << PartNo;
                    //ss1 << " AND sheet = " << Sheet;
                    //ss1 << " AND subsheet = " << SubSheet;
                    //ss1 <<  " AND slab = " << Slab;
                    //ss1 << ";";

                    std::string comand = ss1.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                    res = conn_dops.PGexec(comand);
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss1.str());
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);

                    std::stringstream ss2;
                    ss2 << "UPDATE sheet SET ";
                    ss2 << "lampresstop = " + AI_Hmi_210.LaminPressTop->GetString() + ", ";
                    ss2 << "lampressbot = " + AI_Hmi_210.LaminPressBot->GetString() + " ";
                    ss2 << "WHERE";
                    ss2 << " id = " << Id;
                    //ss2 << " melt = " << Melt;
                    //ss2 << " AND pack = " << Pack;
                    //ss2 << " AND partno = " << PartNo;
                    //ss2 << " AND sheet = " << Sheet;
                    //ss2 << " AND subsheet = " << SubSheet;
                    //ss2 <<  " AND slab = " << Slab;
                    //ss2 << ";";

                    comand = ss2.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                    res = conn_dops.PGexec(comand);
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss2.str());
                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);

                    int r = 0;
                    //5 ������
                    while(isRun && r++ < 5) 
                    {
                        SetWindowText(winmap(hEditState_34), std::to_string(r).c_str());
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }

                    std::string lam_te1 = AI_Hmi_210.LAM_TE1->GetString();
                    std::string za_te3 = AI_Hmi_210.Za_TE3->GetString();
                    std::string za_pt3 = AI_Hmi_210.Za_PT3->GetString();

                    //LOG_INFO(SQLLogger, "{:90}| lam_te1={}, za_te3={}, za_pt3={}, StateNo={}, Melt={}, Pack={}, PartNo={}, Sheet={}, SubSheet={}, Slab={}", FUNCTION_LINE_NAME, lam_te1, za_te3, za_pt3, StateNo, Melt, Pack, PartNo, Sheet, SubSheet, Slab);

                    std::stringstream ss4;
                    ss4 << "UPDATE sheet SET ";
                    ss4 << "lam_te1 = " << lam_te1 << ", ";
                    ss4 << "za_te3 = " << za_te3 << ", ";
                    ss4 << "za_pt3 = " << za_pt3 << " ";
                    ss4 << "WHERE";
                    ss4 << " id = " << Id;
                    //ss4 << " melt = " << Melt;
                    //ss4 << " AND pack = " << Pack;
                    //ss4 << " AND partno = " << PartNo;
                    //ss4 << " AND sheet = " << Sheet;
                    //ss4 << " AND subsheet = " << SubSheet;
                    //ss4 << " AND slab = " << Slab; 
                    //ss4 << ";";


                    comand = ss4.str();
                    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);

                    res = conn_dops.PGexec(comand);
                    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, ss4.str());

                    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                        LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
                //else
                //    LOG_INFO(SQLLogger, "{:90}| Time_Z2={}, StateNo={}, Melt={}, Pack={}, PartNo={}, Sheet={}, SubSheet={}, Slab={}", FUNCTION_LINE_NAME, Time_Z2, StateNo, Melt, Pack, PartNo, Sheet, SubSheet, Slab);

            }
            catch(...)
                LOG_ERROR(SQLLogger, "{:89}| Unknown error", FUNCTION_LINE_NAME);

            hThreadState2 = NULL;
            return 0;
        }

        //�������� � 1 ����
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

        //�������� � 2 ����
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

        //�������� � 3 ����
        DWORD DataPosState_3(Value* value)
        {
            MySetWindowText(value);
            SetWindowText(winmap(hEditState_32), GenSeq3[GetVal<int16_t>(value)].c_str());

            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            return 0;
        }
    }

    //�������
    namespace An{

       //�������� ��������, ������� �����������, ��������, ����������� ����
        //������� �����������

        DWORD TempSet1(Value* value)
        {
            MySetWindowText(value);

            if(value->Val.As<float>() > 0)
            {
                std::string update = " temper = " + value->GetString();
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");
            }
            return 0;
        }

        //�������� ��������
        DWORD UnloadSpeed(Value* value)
        {
            MySetWindowText(value);

            if(value->Val.As<float>() > 0)
            {
                std::string update = " speed = " + value->GetString();
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
                Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");
            }
            return 0;
        }

        //����� ������������ ��������� �������, ���
        DWORD fTimeForPlateHeat(Value* value)
        {
            MySetWindowText(value);

            std::string update = " timeforplateheat = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");
            return 0;
        }

        //������� �������� ��� ������� �����������
        DWORD fPresToStartComp(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            std::string update = " prestostartcomp = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");

            return 0;
        }

    };

    //����� ������ �������
    //State_1 = 3 
    //����� ��������
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
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");

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
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, "");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, "");

            return 0;
        }
    }

    //���������� ������
    namespace Speed{
        //����
        DWORD SpeedSectionTop(Value* value)
        {
            MySetWindowText(value);
            std::string update = " posclapantop = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " posclapantop = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " posclapantop = 0 AND");
            return 0;
        }
        //���
        DWORD SpeedSectionBot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " posclapanbot = 0 AND");
            return 0;
        }
    }

    //���������� ������ 1
    namespace Lam1{
        //����
        DWORD LaminarSection1Top(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam1posclapantop = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " lam1posclapantop = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " lam1posclapantop = 0 AND");
            return 0;
        }
        //���
        DWORD LaminarSection1Bot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam1posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " lam1posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " lam1posclapanbot = 0 AND");
            return 0;
        }
    };

    //���������� ������ 2
    namespace Lam2{
        //����
        DWORD LaminarSection2Top(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam2posclapantop = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " lam2posclapantop = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " lam2posclapantop = 0 AND");
            return 0;
        }
        //���
        DWORD LaminarSection2Bot(Value* value)
        {
            MySetWindowText(value);
            std::string update = " lam2posclapanbot = " + value->GetString();
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[1], update, " lam2posclapanbot = 0 AND");
            Sheet::SetUpdateSheet(conn_kpvl, PlateData[2], update, " lam2posclapanbot = 0 AND");
            return 0;
        }
    };

    //���������� ����� �� ��������
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
                up << f.first << " = " << value->Val.As<float>();
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, up.str());
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
        //        up << " top" << value->Patch[l + find.length()] << " = " << value->Val.As<float>();
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
        if(
            Hmi210_1.Htr2_1->Val.IsNul() ||
            Hmi210_1.Htr2_2->Val.IsNul() ||
            Hmi210_1.Htr2_3->Val.IsNul() ||
            Hmi210_1.Htr2_4->Val.IsNul()
            )return 0;

        float f1 = Hmi210_1.Htr2_1->Val.As<float>();
        float f2 = Hmi210_1.Htr2_2->Val.As<float>();
        float f3 = Hmi210_1.Htr2_3->Val.As<float>();
        float f4 = Hmi210_1.Htr2_4->Val.As<float>();
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

    //��� ������
    DWORD SheetData_WDG_toBase(Value* value)
    {
        if(GetVal<bool>(value))
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
//    //std::time_t statr = static_cast<std::time_t>(difftime(stop, 60 * 60 * 24 * 10)); //7-� �����
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


