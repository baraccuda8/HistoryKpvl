#include "pch.h"

#include "win.h"
#include "main.h"
#include "file.h"
#include "SQL.h"
#include "ValueTag.h"
#include "term.h"
#include "hard.h"
#include "Graff.h"
#include "KPVL.h"
#include "Furn.h"
#include "Pdf.h"

extern std::shared_ptr<spdlog::logger> PethLogger;
extern std::string FORMATTIME;
extern HANDLE hAllPlf;

namespace S107
{
    //������ ������� � �������
    namespace Coll{
        int Create_at = 0;
        int Id = 0;
        int Event = 0;
        int Day = 0;
        int Month = 0;
        int Year = 0;
        int CassetteNo = 0;
        int SheetInCassette = 0;
        int Close_at = 0;
        int Peth = 0;
        int Run_at = 0;
        int Error_at = 0;
        int End_at = 0;
        int Delete_at = 0;
        int TempRef = 0;           //�������� �������� �����������
        int PointTime_1 = 0;       //����� �������
        int PointRef_1 = 0;        //������� �����������
        int TimeProcSet = 0;       //������ ����� �������� (�������), ���
        int PointDTime_2 = 0;      //����� ��������
        int f_temper = 0;          //���� ����������� �� 5 ����� �� ����� �������
        int Finish_at = 0;
        int HeatAcc = 0;           //���� ����� �������
        int HeatWait = 0;          //���� ����� ��������
        int Total = 0;             //���� ����� �����

    };

    //��������� ������ �������
    void GetColl(PGresult* res)
    {
        if(!Coll::Total)
        {
            int nFields = PQnfields(res);
            for(int j = 0; j < nFields; j++)
            {
                std::string l =  utf8_to_cp1251(PQfname(res, j));
                if(l == "create_at") Coll::Create_at = j;
                else if(l == "id") Coll::Id = j;
                else if(l == "event") Coll::Event = j;
                else if(l == "day") Coll::Day = j;
                else if(l == "month") Coll::Month = j;
                else if(l == "year") Coll::Year = j;
                else if(l == "cassetteno") Coll::CassetteNo = j;
                else if(l == "sheetincassette") Coll::SheetInCassette = j;
                else if(l == "close_at") Coll::Close_at = j;
                else if(l == "peth") Coll::Peth = j;
                else if(l == "run_at") Coll::Run_at = j;
                else if(l == "error_at") Coll::Error_at = j;
                else if(l == "end_at") Coll::End_at = j;
                else if(l == "delete_at") Coll::Delete_at = j;
                else if(l == "tempref") Coll::TempRef = j;
                else if(l == "pointtime_1") Coll::PointTime_1 = j;
                else if(l == "pointref_1") Coll::PointRef_1 = j;
                else if(l == "timeprocset") Coll::TimeProcSet = j;
                else if(l == "pointdtime_2") Coll::PointDTime_2 = j;
                else if(l == "facttemper") Coll::f_temper = j;
                else if(l == "finish_at") Coll::Finish_at = j;
                else if(l == "heatacc")Coll::HeatAcc = j;
                else if(l == "heatwait")Coll::HeatWait = j;
                else if(l == "total")Coll::Total = j;

            }
        }
    }

    //������ �������
    void GetCassette(PGresult* res, TCassette& cassette, int line)
    {
        cassette.Create_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::Create_at));
        cassette.Id = conn_spis.PGgetvalue(res, line, Coll::Id);
        cassette.Event = conn_spis.PGgetvalue(res, line, Coll::Event);
        cassette.Day = conn_spis.PGgetvalue(res, line, Coll::Day);
        cassette.Month = conn_spis.PGgetvalue(res, line, Coll::Month);
        cassette.Year = conn_spis.PGgetvalue(res, line, Coll::Year);
        cassette.CassetteNo = conn_spis.PGgetvalue(res, line, Coll::CassetteNo);
        cassette.SheetInCassette = conn_spis.PGgetvalue(res, line, Coll::SheetInCassette);
        cassette.Close_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::Close_at));
        cassette.Peth = conn_spis.PGgetvalue(res, line, Coll::Peth);
        cassette.Run_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::Run_at));
        cassette.Error_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::Error_at));
        cassette.End_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::End_at));
        cassette.Delete_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::Delete_at));
        cassette.TempRef = conn_spis.PGgetvalue(res, line, Coll::TempRef);           //�������� �������� �����������
        cassette.PointTime_1 = conn_spis.PGgetvalue(res, line, Coll::PointTime_1);       //����� �������
        cassette.PointRef_1 = conn_spis.PGgetvalue(res, line, Coll::PointRef_1);        //������� �����������
        cassette.TimeProcSet = conn_spis.PGgetvalue(res, line, Coll::TimeProcSet);       //������ ����� �������� (�������), ���
        cassette.PointDTime_2 = conn_spis.PGgetvalue(res, line, Coll::PointDTime_2);      //����� ��������
        cassette.f_temper = conn_spis.PGgetvalue(res, line, Coll::f_temper);          //���� ����������� �� 5 ����� �� ����� �������
        cassette.Finish_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::Finish_at)); //���������� �������� + 15 �����
        cassette.HeatAcc = conn_spis.PGgetvalue(res, line, Coll::HeatAcc); //���������� �������� + 15 �����
        cassette.HeatWait = conn_spis.PGgetvalue(res, line, Coll::HeatWait); //���������� �������� + 15 �����
        cassette.Total = conn_spis.PGgetvalue(res, line, Coll::Total); //���������� �������� + 15 �����
    }

    std::string URI = "opc.tcp://192.168.9.40:4840";
    std::string ServerDataTime = "";

    namespace SQL{
        void FURN_SQL(PGConnection& conn, std::deque<TCassette>& allCassette)
        {
            AppFurn1.Cassette.Day->GetValue();
            AppFurn1.Cassette.Month->GetValue();
            AppFurn1.Cassette.Year->GetValue();
            AppFurn1.Cassette.CassetteNo->GetValue();
            AppFurn2.Cassette.Day->GetValue();
            AppFurn2.Cassette.Month->GetValue();
            AppFurn2.Cassette.Year->GetValue();
            AppFurn2.Cassette.CassetteNo->GetValue();

            std::tm TM_End ={0};
            std::tm TM_Beg ={0};

            time_t tTemp = 60 * 60 * 24 * 7; //7 �����
            time_t tStop = time(NULL);
            time_t tStart = (time_t)difftime(tStop, tTemp);

            localtime_s(&TM_Beg, &tStart);

            std::stringstream ss;
            ss << "SELECT * FROM cassette WHERE create_at > '";
            ss << boost::format("%|04|-") % (TM_Beg.tm_year + 1900);
            ss << boost::format("%|02|-") % (TM_Beg.tm_mon + 1);
            ss << boost::format("%|02| ") % TM_Beg.tm_mday;
            ss << boost::format("%|02|:") % TM_Beg.tm_hour;
            ss << boost::format("%|02|:") % TM_Beg.tm_min;
            ss << boost::format("%|02|'") % TM_Beg.tm_sec;
            ss << " ORDER BY event ASC, create_at DESC;"; //DESC
            std::string comand = ss.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, sMaxId);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {

                GetColl(res);
                allCassette.erase(allCassette.begin(), allCassette.end());
                int line =  PQntuples(res);
                for(int l = 0; l < line; l++)
                {
                    TCassette cassette;
                    GetCassette(res, cassette, l);
                    allCassette.push_back(cassette);
                }
            }
            else
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);

            for(auto iCD = allCassette.begin(); iCD != allCassette.end();)
            {
                if(GetCountSheet(conn, *iCD))
                    iCD = allCassette.erase(iCD);
                else
                {
                    GetIsPos(conn, *iCD);
                    ++iCD;
                }
            }
        }

        bool GetCountSheet(PGConnection& conn, TCassette& CD)
        {
            if(IsCassete(CD))
            {
                std::stringstream ss;
                ss << "SELECT count(*) FROM sheet WHERE";
                ss << " day = '" << CD.Day << "'";
                ss << " AND month = '" << CD.Month << "'";
                ss << " AND year = '" << CD.Year << "'";
                ss << " AND cassetteno = " << CD.CassetteNo;

                std::string comand = ss.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);
                //LOG_INFO(SQLLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, sMaxId);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    int count = 0;
                    int line =  PQntuples(res);
                    if(line)
                    {
                        count = std::stoi(conn.PGgetvalue(res, 0, 0));
                        PQclear(res);
                    }

                    if(line)
                    {
                        if(!count)
                        {
                            std::stringstream ss;
                            ss << "DELETE FROM cassette WHERE";
                            ss << " day = " << CD.Day;
                            ss << " AND month = " << CD.Month;
                            ss << " AND year = " << CD.Year;
                            ss << " AND cassetteno = " << CD.CassetteNo;
                            SETUPDATESQL(conn, ss);
                            return true;
                        }
                        else if(std::stoi(CD.SheetInCassette) != count)
                        {
                            std::stringstream ss;
                            ss << "UPDATE cassette SET";
                            ss << " sheetincassette = " << count;
                            ss << " WHERE";
                            ss << " day = " << CD.Day;
                            ss << " AND month = " << CD.Month;
                            ss << " AND year = " << CD.Year;
                            ss << " AND cassetteno = " << CD.CassetteNo;
                            std::string comand = ss.str();
                            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                            PGresult* res = conn_spic.PGexec(comand);
                            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                                LOG_ERR_SQL(SQLLogger, res, comand);
                            PQclear(res);
                            return false;
                        }
                    }
                }
                else
                {
                    LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
            }
            return false;
        }

        bool isCasseteFurn(T_cassette& Cassette, TCassette& CD)
        {
            if(IsCassete(CD))
            {
                int32_t FDay = Cassette.Day->Val.As<int32_t>();
                int32_t FMonth = Cassette.Month->Val.As<int32_t>();
                int32_t FYear = Cassette.Year->Val.As<int32_t>();
                int32_t FCassetteNo = Cassette.CassetteNo->Val.As<int32_t>();
                int32_t CDay = std::stoi(CD.Day);
                int32_t CMonth = std::stoi(CD.Month);
                int32_t CYear = std::stoi(CD.Year);
                int32_t CCassetteNo = std::stoi(CD.CassetteNo);
                return (FDay && FMonth && FYear && FCassetteNo && FDay == CDay && FMonth == CMonth && FYear == CYear && FCassetteNo == CCassetteNo);
            }
            return false;
        }

        bool isCasseteCant(T_CassetteData& Cassette, TCassette& CD)
        {
            if(KPVL::Cassette::IsCassete(HMISheetData.Cassette))
            {
                int32_t FDay = Cassette.Day->Val.As<int32_t>();
                int32_t FMonth = Cassette.Month->Val.As<int32_t>();
                int32_t FYear = Cassette.Year->Val.As<int32_t>();
                int32_t FCassetteNo = Cassette.CassetteNo->Val.As<int32_t>();
                int32_t CDay = std::stoi(CD.Day);
                int32_t CMonth = std::stoi(CD.Month);
                int32_t CYear = std::stoi(CD.Year);
                int32_t CCassetteNo = std::stoi(CD.CassetteNo);
                return (FDay && FMonth && FYear && FCassetteNo && FDay == CDay && FDay == CDay && FMonth == CMonth && FYear == CYear && FCassetteNo == CCassetteNo);
            }
            return false;
        }

        int GetId(PGConnection& conn, TCassette& CD)
        {
            int id = 0;
            std::stringstream ss;
            ss << "SELECT id FROM cassette WHERE";
            ss << " day = " << CD.Day;
            ss << " AND month = " << CD.Month;
            ss << " AND year = " << CD.Year;
            ss << " AND cassetteno = " << CD.CassetteNo;
            std::string comand = ss.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn_spic.PGexec(comand);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                int nFields = PQnfields(res);
                id = std::stoi(conn_spis.PGgetvalue(res, 0, 0));
            }
            else
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);
            return id;
        }

        void FinishPdf(PGConnection& conn, TCassette& CD, int Peth, int id)
        {
            PrintPdfAuto(CD);
        }

        void SearthEnd_at(PGConnection& conn, TCassette& CD, int id)
        {
            std::stringstream sd;
            int Peth = std::stoi(CD.Peth);
            int ID = 0;

            if(Peth == 1) ID = AppFurn1.ProcRun->ID;
            else if(Peth == 2) ID = AppFurn2.ProcRun->ID;

            if(ID)
            {
                //����� �������
                std::stringstream sd;
                sd << "SELECT min(create_at) FROM todos WHERE create_at >= '" << CD.Run_at << "' AND content = 'false' AND id_name = " << ID;
                std::string comand = sd.str();
                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn.PGexec(comand);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {
                    int nFields = PQnfields(res);
                    CD.End_at = GetStringData(conn_spis.PGgetvalue(res, 0, 0));
                    PQclear(res);

                    if(CD.End_at.length())
                    {
                        std::stringstream sf;
                        sf << "UPDATE cassette SET end_at = '" << CD.End_at << "' WHERE id = " << id;
                        SETUPDATESQL(conn, sf);
                            
                        //���� ������ ������
                        if(!CD.Finish_at.length())
                        {
                            #pragma region ������ Finish_at  �� End_at
                                time_t tt;
                                std::tm TM;
                                DataTimeOfString(CD.End_at, FORMATTIME, TM);
                                TM.tm_year -= 1900;
                                TM.tm_mon -= 1;
                                tt = mktime(&TM);
                                tt += 15 * 60; //���� 15 �����
                                localtime_s(&TM, &tt);
                                CD.Finish_at = GetDataTimeString(TM);
                            #pragma endregion

                            if(CD.Finish_at.length())
                            {
                                std::stringstream sf;
                                sf << "UPDATE cassette SET event = 5, finish_at = '" << CD.Finish_at << "' WHERE id = " << id;
                                SETUPDATESQL(conn, sf);

                                PrintPdfAuto(CD);
                                //FinishPdf(conn, CD, Peth, id);
                            }
                        }
                        ////���� ������ ����
                        //else if(CD.Error_at.length())
                        //{
                        //    std::stringstream sf;
                        //    sf << "UPDATE cassette SET event = 4 WHERE id = " << id;
                        //    SETUPDATESQL(conn, sf);
                        //}
                    }
                }
                else
                {
                    LOG_ERR_SQL(SQLLogger, res, comand);
                    PQclear(res);
                }
            }
        }


        void GetIsPos(PGConnection& conn, TCassette& CD)
        {
            if(IsCassete(CD))
            {
                int id = GetId(conn, CD);


                if(id)
                {
                     //������ �� 1� - ����
                    if(isCasseteFurn(AppFurn1.Cassette, CD))
                    {
                        if(CD.Event != "3" || CD.Peth != "1")
                        {

                            std::stringstream ss;
                            ss << "UPDATE cassette SET event = 3, peth = 1, end_at = DEFAULT, finish_at = DEFAULT WHERE id = " << id;
                            SETUPDATESQL(conn, ss);
                            CD.Event = 3;
                            CD.Peth = 1;
                            CD.End_at = "";
                            CD.Finish_at = "";
                        }
                    }
                    //������ �� 2� - ����
                    else if(isCasseteFurn(AppFurn2.Cassette, CD)) //������ �� 2� - ����
                    {
                        if(CD.Event != "3" || CD.Peth != "2")
                        {
                            std::stringstream ss;
                            ss << "UPDATE cassette SET event = 3, peth = 2, end_at = DEFAULT, finish_at = DEFAULT WHERE id = " << id;
                            SETUPDATESQL(conn, ss);
                            CD.Event = 3;
                            CD.Peth = 2;
                            CD.End_at = "";
                            CD.Finish_at = "";
                        }
                    }
                    //������ �� ��������
                    else if(isCasseteCant(HMISheetData.Cassette, CD))
                    {
                        if(CD.Event != "1")
                        {
                            std::stringstream ss;
                            ss << "UPDATE cassette SET event = 1, peth = 0, run_at = DEFAULT, error_at = DEFAULT, end_at = DEFAULT, finish_at = DEFAULT WHERE id = " << id;
                            SETUPDATESQL(conn, ss);
                            CD.Event = 1;
                            CD.Peth = 1;
                            CD.Run_at = "";
                            CD.Error_at = "";
                            CD.End_at = "";
                            CD.Finish_at = "";
                        }
                    }
                    //���� ��� ����� ������� 
                    else if(CD.Run_at.length())
                    {
                        //���� ����� ������� ��� ������
                        if(!CD.End_at.length() && !CD.Error_at.length())
                        {
                            //����� ����� ��������
                            SearthEnd_at(conn, CD, id);
                        }

                        int ID = 0;
                        int Peth = std::stoi(CD.Peth);
                        if(Peth == 1) ID = AppFurn1.ProcFault->ID;
                        else if(Peth == 2) ID = AppFurn2.ProcFault->ID;

                        //���� ��� ������� ������
                        if(ID && !CD.Error_at.length())
                        {
                            std::stringstream sd;
                            sd << "SELECT min(create_at) FROM todos WHERE create_at >= '" << CD.Run_at << "'";
                            if(CD.End_at.length()) sd << " AND create_at <= '" << CD.End_at << "'";
                            sd << " AND content = 'true' AND id_name = " << ID;
                            std::string comand = sd.str();
                            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                            PGresult* res = conn.PGexec(comand);
                            if(PQresultStatus(res) == PGRES_TUPLES_OK)
                            {
                                int nFields = PQnfields(res);
                                CD.Error_at = GetStringData(conn_spis.PGgetvalue(res, 0, 0));
                            }
                            else
                                LOG_ERR_SQL(SQLLogger, res, comand);
                            PQclear(res);

                            if(CD.Error_at.length())
                            {
                                std::stringstream sf;
                                sf << "UPDATE cassette SET event = 4, error_at = '" << CD.Error_at << "' WHERE id = " << id;
                                SETUPDATESQL(conn, sf);
                            }
                        }

                    }
                    else
                    {
                        if(CD.Delete_at.length())
                        {
                            std::stringstream sf;
                            sf << "UPDATE cassette SET event = 7 WHERE id = " << id;
                            SETUPDATESQL(conn, sf);
                        }
                        else if(CD.Event != "2" && !CD.Delete_at.length())
                        {
                            std::stringstream sf;
                            sf << "UPDATE cassette SET event = 2 WHERE id = " << id;
                            SETUPDATESQL(conn, sf);
                        }
                    }
                }
            }
        }
    };

#pragma region ������� � ��������� � ����

    bool IsCassete(TCassette& CD)
    {
        int32_t Day = std::stoi(CD.Day);
        int32_t Month = std::stoi(CD.Month);
        int32_t Year = std::stoi(CD.Year);
        int32_t CassetteNo = std::stoi(CD.CassetteNo);
        return Day && Month && Year && CassetteNo;
    }

    bool IsCassete1(T_cassette& CD)
    {
        int32_t Day = GetVal<int32_t>(CD.Day);
        int32_t Month = GetVal<int32_t>(CD.Month);
        int32_t Year = GetVal<int32_t>(CD.Year);
        int32_t CassetteNo = GetVal<int32_t>(CD.CassetteNo);
        return Day && Month && Year && CassetteNo;
    }
    bool IsCassete2(T_cassette& CD)
    {
        int32_t Day = GetVal<int32_t>(CD.Day);
        int32_t Month = GetVal<int32_t>(CD.Month);
        int32_t Year = GetVal<int32_t>(CD.Year);
        int32_t CassetteNo = GetVal<int32_t>(CD.CassetteNo);
        return (Day && Month && Year && CassetteNo) || (!Day && !Month && !Year && !CassetteNo);
    }

    void UpdateCassetteProcRun(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        //T_cassette& CD = Furn.Cassette;
        if(IsCassete1(Furn.Cassette))
        {
            //CloseAllCassette(CD);
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "peth = " << Peth;
            sd << ", run_at = now()";

            sd << ", event = 3";
            sd << ", tempref = " << Furn.TempRef->Val.As<float>(); //GetString();
            sd << ", pointtime_1 = " << Furn.PointTime_1->Val.As<float>(); //GetString();
            sd << ", pointref_1 = " << Furn.PointRef_1->Val.As<float>(); //GetString();
            sd << ", timeprocset = " << Furn.TimeProcSet->Val.As<float>(); //GetString();
            sd << ", pointdtime_2 = " << Furn.PointDTime_2->Val.As<float>(); //GetString();

            sd << " WHERE";
            //sd << " run_at IS NULL";
            sd << " day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";
            SETUPDATESQL(conn, sd);
            
            sd = std::stringstream("");
            sd << "UPDATE cassette SET ";
            sd << "error_at = DEFAULT, ";
            sd << "end_at = DEFAULT";
            sd << " WHERE";
            sd << " day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";
            SETUPDATESQL(conn, sd);
            LOG_INFO(PethLogger, "{:90}| run_at, Peth = {}, Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.CassetteNo->GetString());
        }
        else
            LOG_INFO(PethLogger, "{:90}| Not run_at, Peth = {}, Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.CassetteNo->GetString());

    }
    void UpdateCassetteProcEnd(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        if(IsCassete1(Furn.Cassette))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "end_at = now()";
            sd << ", event = 5";
            sd << " WHERE";
            //sd << " end_at IS NULL";
            sd << " peth = " << Peth;
            sd << " AND day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";
            SETUPDATESQL(conn, sd);
            LOG_INFO(PethLogger, "{:90}| end_at, Peth = {}, Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.CassetteNo->GetString());
        }
        else
            LOG_INFO(PethLogger, "{:90}| Not end_at, Peth = {}, Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.CassetteNo->GetString());
    }
    void UpdateCassetteProcError(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        if(IsCassete1(Furn.Cassette))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "run_at = DEFAULT";
            sd << ", error_at = now()";
            sd << ", event = 4";
            sd << " WHERE";
            //sd << " error_at IS NULL";
            sd << " peth = " << Peth;
            sd << " AND day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";
            SETUPDATESQL(conn, sd);
            LOG_INFO(PethLogger, "{:90}| error_at, Peth = {}, Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.CassetteNo->GetString());
        }
        else
            LOG_INFO(PethLogger, "{:90}| Not error_at, Peth = {}, Melt={}, PartNo={}, Pack={}, Sheet={}", FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.CassetteNo->GetString());
    }

    void SetTemperCassette(PGConnection& conn, T_cassette& CD, std::string teper)
    {
        if(teper != CD.f_temper)
            if(IsCassete1(CD))
            {
                CD.f_temper = teper;
                std::stringstream sd;
                //std::string comand = "UPDATE cassette SET";
                sd << "UPDATE cassette SET";
                sd << " facttemper = " << teper;
                sd << " WHERE ";
                //comand += " facttemper = 0.0 AND";
                sd << " day = " << CD.Day->Val.As<int32_t>();
                sd << " AND month = " << CD.Month->Val.As<int32_t>();
                sd << " AND year = " << CD.Year->Val.As<int32_t>();
                sd << " AND cassetteno = " << CD.CassetteNo->Val.As<int32_t>();
                sd << ";";
                SETUPDATESQL(conn, sd);
            }
    }

    void CloseAllCassette2(PGConnection& conn, T_cassette& CD, int Peth)
    {
        if(IsCassete2(CD))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << " event = 5,";
            sd << " end_at = now()";
            sd << " WHERE end_at IS NULL";
            sd << " AND event = 3";
            sd << " AND peth = " << Peth;
            if(IsCassete1(CD))
            {
                sd << " AND day <> " << CD.Day->Val.As<int32_t>(); //GetString();
                sd << " AND month <> " << CD.Month->Val.As<int32_t>(); //GetString();
                sd << " AND year <> " << CD.Year->Val.As<int32_t>(); //GetString();
                sd << " AND cassetteno <> " << CD.CassetteNo->Val.As<int32_t>(); //GetString();
            }
            sd << ";";

            SETUPDATESQL(conn, sd);
        }
    }

    void SetUpdateCassete(PGConnection& conn, T_ForBase_RelFurn& Furn, std::string update)
    {
        if(IsCassete1(Furn.Cassette))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << update;
            sd << " WHERE error_at IS NULL";
            sd << " AND day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";
            SETUPDATESQL(conn, sd);
        }
    }

#pragma endregion

    //��� ������� #1
    namespace Furn1{
        const int Peth = 1;
        DWORD Data_WDG_toBase(Value* value)
        {
            if(value->Val.As<bool>())
            {
                AppFurn1.Furn_old_dt = time(NULL);
                try
                {
                    //if(!MyServer)
                    {
                        AppFurn1.WDG_fromBase->Set_Value(true);
                        //TAG_PLC_SPK1.Application.ForBase_RelFurn_1.Data;
                    }
                }
                catch(std::exception& exc)
                {
                    LOG_ERROR(PethLogger, "{:90}| ������ �������� ������ ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value", FUNCTION_LINE_NAME, exc.what());
                }
                catch(...)
                {
                    LOG_ERROR(PethLogger, "{:90}| ������ �������� ������ ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value", FUNCTION_LINE_NAME);
                };


                struct tm TM;
                localtime_s(&TM, &AppFurn1.Furn_old_dt);

                SetWindowText(winmap(value->winId), string_time(&TM).c_str());
            }

            //MySetWindowText(winmap(value->winId), value->GetString().c_str());
            return 0;
        }

        //BOOL ������
        DWORD ProcRun(Value* value)
        {
            std::string out = "�������";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcRun(conn_temp, AppFurn1, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value)
        {
            std::string out = "";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcEnd(conn_temp, AppFurn1, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL ������ ��������
        DWORD ProcError(Value* value)
        {
            std::string out = "";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcError(conn_temp, AppFurn1, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //��������� ����������� ��� ���������
        DWORD SetNull_Temper(Value* value)
        {
            AppFurn1.Cassette.f_temper = "0";
            MySetWindowText(value);
            return 0;
        }


        ////REAL ����� �� ��������� ��������, ���
        //DWORD TimeToProcEnd(Value* value)
        //{
        //    MySetWindowText(value);
        //    float time = GetVal<float>(value);
        //    if(time <= 5.0 && time >= 4.9)
        //    {
        //        MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.TempAct->GetString().c_str());
        //        SetTemperCassette(conn_temp, AppFurn1.Cassette, AppFurn1.TempAct->GetString());
        //    }
        //    else if(GetVal<float>(value) >= GetVal<float>(AppFurn1.TimeProcSet))
        //    {
        //        AppFurn1.Cassette.f_temper = "0";
        //        MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.Cassette.f_temper.c_str());
        //    }
        //    return 0;
        //}

        DWORD TimeHeatAcc(Value* value)
        {
            MySetWindowText(value);
            if(value->Val.As<float>())
                SetUpdateCassete(conn_temp, AppFurn1, "heatacc = " + value->GetString() );
            return 0;
        }
        DWORD TimeHeatWait(Value* value)
        {
            MySetWindowText(value);
            if(value->Val.As<float>())
                SetUpdateCassete(conn_temp, AppFurn1, "heatwait = " + value->GetString());
            return 0;
        }
        DWORD TimeTotal(Value* value)
        {
            MySetWindowText(value);
            if(value->Val.As<float>())
                SetUpdateCassete(conn_temp, AppFurn1, "total = " + value->GetString());
            return 0;
        }

        //REAL ����� �� ��������� ��������, ���
        DWORD TimeToProcEnd(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            float f = value->Val.As<float>();
            if(f <= 5.0 && f >= 4.9)
            {
                MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.TempAct->GetString().c_str());
                SetTemperCassette(conn_temp, AppFurn1.Cassette, AppFurn1.TempAct->GetString());
            }
            else if(f >= AppFurn1.TimeProcSet->Val.As<float>())
            {
                AppFurn1.Cassette.f_temper = "0";
                MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.Cassette.f_temper.c_str());

            }
            return 0;
        }


    }

    //��� ������� #2
    namespace Furn2{
        const int Peth = 2;
        DWORD Data_WDG_toBase(Value* value)
        {
            if(value->Val.As<bool>())
            {
                AppFurn2.Furn_old_dt = time(NULL);
                try
                {
                    //if(!MyServer)
                    {
                        AppFurn2.WDG_fromBase->Set_Value(true);
                    }
                }
                catch(std::exception& exc)
                {
                    LOG_ERROR(PethLogger, "{:90}| ������ �������� ������ ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value {}", FUNCTION_LINE_NAME, exc.what());
                }
                catch(...)
                {
                    LOG_ERROR(PethLogger, "Unknown ������ �������� ������ ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value ", FUNCTION_LINE_NAME);
                };


                struct tm TM;
                localtime_s(&TM, &AppFurn2.Furn_old_dt);

                SetWindowText(winmap(value->winId), string_time(&TM).c_str());
            }

            //MySetWindowText(winmap(value->winId), value->GetString().c_str());
            return 0;
        }

        //BOOL ������
        DWORD ProcRun(Value* value)
        {
            std::string out = "�������";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcRun(conn_temp, AppFurn2, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value)
        {
            std::string out = "";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcEnd(conn_temp, AppFurn2, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL ������ ��������
        DWORD ProcError(Value* value)
        {
            std::string out = "";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcError(conn_temp, AppFurn2, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //��������� ����������� ��� ���������
        DWORD SetNull_Temper(Value* value)
        {
            AppFurn2.Cassette.f_temper = "0";
            MySetWindowText(value);
            return 0;
        }

        ////REAL ����� �� ��������� ��������, ���
        //DWORD TimeToProcEnd(Value* value)
        //{
        //    MySetWindowText(value);
        //    float time = GetVal<float>(value);
        //    if(time <= 5.0 && time >= 4.9)
        //    {
        //        MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.TempAct->GetString().c_str());
        //        SetTemperCassette(conn_temp, AppFurn2.Cassette, AppFurn2.TempAct->GetString());
        //    }
        //    else if(GetVal<float>(value) >= GetVal<float>(AppFurn2.TimeProcSet))
        //    {
        //        AppFurn2.Cassette.f_temper = "0";
        //        MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.Cassette.f_temper.c_str());
        //    }
        //    return 0;
        //}

        DWORD TimeHeatAcc(Value* value)
        {
            MySetWindowText(value);
            if(value->Val.As<float>())
                SetUpdateCassete(conn_temp, AppFurn2, "heatacc = " + value->GetString());
            return 0;
        }
        DWORD TimeHeatWait(Value* value)
        {
            MySetWindowText(value);
            if(value->Val.As<float>())
                SetUpdateCassete(conn_temp, AppFurn2, "heatwait = " + value->GetString());
            return 0;
        }
        DWORD TimeTotal(Value* value)
        {
            MySetWindowText(value);
            if(value->Val.As<float>())
                SetUpdateCassete(conn_temp, AppFurn2, "total = " + value->GetString());
            return 0;
        }

        //REAL ����� �� ��������� ��������, ���
        DWORD TimeToProcEnd(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            float f = value->Val.As<float>();
            if(f <= 5.0 && f >= 4.9)
            {
                MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.TempAct->GetString().c_str());
                SetTemperCassette(conn_temp, AppFurn2.Cassette, AppFurn2.TempAct->GetString());
            }
            else if(f >= AppFurn2.TimeProcSet->Val.As<float>())
            {
                AppFurn2.Cassette.f_temper = "0";
                MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.Cassette.f_temper.c_str());

            }
            return 0;
        }

    };

}


void SetUpdateCassete(PGConnection& conn, TCassette& cassette, std::string update, std::string where)
{
    if(S107::IsCassete(cassette))
    {
        std::stringstream sd;
        sd << "UPDATE cassette SET ";
        sd << update;
        sd << " WHERE " + where;
        sd << " id = " << cassette.Id;
        sd << ";";
        SETUPDATESQL(conn, sd);
    }
}

//DWORD WINAPI AllPdf(LPVOID)
//{
//    PGConnection conn_pdf;
//    conn_pdf.connection();
//    std::deque<TCassette>Cassette;
//    //KPVL::SQL::KPVL_SQL(conn_pdf, Sheet);
//
//    std::deque<TCassette> all;
//    S107::SQL::FURN_SQL(conn_pdf, all);
//
//    LOG_INFO(AllLogger, "{:90}| Start PrintPdfAuto, Sheet.size = {}", FUNCTION_LINE_NAME, all.size());
//    SetWindowText(hWndDebug, "Start PrintPdfAuto");
//    for(auto TS : all)
//    {
//        if(!isRun)
//        {
//            hAllPlf = NULL;
//            return 0;
//        }
//        //KPVL::SQL::GetDataTime_All(conn_pdf, TS);
//        PrintPdfAuto(TS);
//    }
//    LOG_INFO(AllLogger, "{:90}| Stop PrintPdfAuto", FUNCTION_LINE_NAME);
//    SetWindowText(hWndDebug, "Stop PrintPdfAuto");
//    hAllPlf = NULL;
//    return 0;
//}
