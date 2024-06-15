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
//extern const std::string FORMATTIME;


namespace S107
{
    //Номера колонок в кассете
    namespace Coll{
        int Create_at = 0;
        int Id = 0;
        int Event = 0;
        int Hour = 0;
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
        int TempRef = 0;           //Заданное значение температуры
        int PointTime_1 = 0;       //Время разгона
        int PointRef_1 = 0;        //Уставка температуры
        int TimeProcSet = 0;       //Полное время процесса (уставка), мин
        int PointDTime_2 = 0;      //Время выдержки
        int facttemper = 0;          //Факт температуры за 5 минут до конца отпуска
        int Finish_at = 0;
        int HeatAcc = 0;           //Факт время нагрева
        int HeatWait = 0;          //Факт время выдержки
        int Total = 0;             //Факт общее время
        int Correct = 0;           //
        int Pdf = 0;               //

    };

    //Получение номера колонки
    void GetColl(PGresult* res)
    {
        if(!Coll::Hour)
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
                else if(l == "facttemper") Coll::facttemper = j;
                else if(l == "finish_at") Coll::Finish_at = j;
                else if(l == "heatacc")Coll::HeatAcc = j;
                else if(l == "heatwait")Coll::HeatWait = j;
                else if(l == "total")Coll::Total = j;
                else if(l == "correct")Coll::Total = j;
                else if(l == "pdf")Coll::Total = j;
                else if(l == "hour")Coll::Hour = j;

            }
        }
    }

    //Чтение кассеты
    void GetCassette(PGresult* res, TCassette& cassette, int line)
    {
        cassette.Create_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::Create_at));
        cassette.Id = conn_spis.PGgetvalue(res, line, Coll::Id);
        cassette.Event = conn_spis.PGgetvalue(res, line, Coll::Event);
        cassette.Hour = conn_spis.PGgetvalue(res, line, Coll::Hour);
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
        cassette.TempRef = conn_spis.PGgetvalue(res, line, Coll::TempRef);           //Заданное значение температуры
        cassette.PointTime_1 = conn_spis.PGgetvalue(res, line, Coll::PointTime_1);       //Время разгона
        cassette.PointRef_1 = conn_spis.PGgetvalue(res, line, Coll::PointRef_1);        //Уставка температуры
        cassette.TimeProcSet = conn_spis.PGgetvalue(res, line, Coll::TimeProcSet);       //Полное время процесса (уставка), мин
        cassette.PointDTime_2 = conn_spis.PGgetvalue(res, line, Coll::PointDTime_2);      //Время выдержки
        cassette.facttemper = conn_spis.PGgetvalue(res, line, Coll::facttemper);          //Факт температуры за 5 минут до конца отпуска
        cassette.Finish_at = GetStringData(conn_spis.PGgetvalue(res, line, Coll::Finish_at)); //Завершение процесса + 15 минут
        cassette.HeatAcc = conn_spis.PGgetvalue(res, line, Coll::HeatAcc); //Завершение процесса + 15 минут
        cassette.HeatWait = conn_spis.PGgetvalue(res, line, Coll::HeatWait); //Завершение процесса + 15 минут
        cassette.Total = conn_spis.PGgetvalue(res, line, Coll::Total); //Завершение процесса + 15 минут
        cassette.Correct = conn_spis.PGgetvalue(res, line, Coll::Correct); //Завершение процесса + 15 минут
        cassette.Pdf = conn_spis.PGgetvalue(res, line, Coll::Pdf); //Завершение процесса + 15 минут

    }

    std::string URI = "opc.tcp://192.168.9.40:4840";
    std::string ServerDataTime = "";

    namespace SQL{
        void FURN_SQL(PGConnection& conn, std::deque<TCassette>& allCassette)
        {
            AppFurn1.Cassette.Hour->GetValue();
            AppFurn1.Cassette.Day->GetValue();
            AppFurn1.Cassette.Month->GetValue();
            AppFurn1.Cassette.Year->GetValue();
            AppFurn1.Cassette.CassetteNo->GetValue();
            AppFurn2.Cassette.Hour->GetValue();
            AppFurn2.Cassette.Day->GetValue();
            AppFurn2.Cassette.Month->GetValue();
            AppFurn2.Cassette.Year->GetValue();
            AppFurn2.Cassette.CassetteNo->GetValue();

            std::tm TM_End ={0};
            std::tm TM_Beg ={0};

            time_t tTemp = 60 * 60 * 24 * 7; //7 суток
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
            if(IsCassette(CD))
            {
                std::stringstream ss;
                ss << "SELECT count(*) FROM sheet WHERE";
                ss << " hour = '" << CD.Hour << "'";
                ss << " AND day = '" << CD.Day << "'";
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
                        count = Stoi(conn.PGgetvalue(res, 0, 0));
                        PQclear(res);
                    }

                    if(line)
                    {
                        if(!count && !Stoi(CD.Peth))
                        {
                            CD.Event = "7";
                            std::time_t st;
                            CD.Delete_at = GetDataTimeString(st);
                            std::stringstream sd;
                            sd << "UPDATE cassette SET event = 7, delete_at = now() WHERE ";
                            sd << " hour = " << CD.Hour;
                            sd << " AND day = " << CD.Day;
                            sd << " AND month = " << CD.Month;
                            sd << " AND year = " << CD.Year;
                            sd << " AND cassetteno = " << CD.CassetteNo;
                            SETUPDATESQL(SQLLogger, conn, ss);
                            return true;
                        }
                        else
                        if(Stoi(CD.SheetInCassette) != count)
                        {
                            std::stringstream ss;
                            ss << "UPDATE cassette SET";
                            ss << " sheetincassette = " << count;
                            ss << " WHERE";
                            ss << " hour = " << CD.Hour;
                            ss << " AND day = " << CD.Day;
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
            if(IsCassette(CD))
            {
                int32_t FHour = Cassette.Hour->GetInt();// ->GetInt();
                int32_t FDay = Cassette.Day->GetInt();// ->GetInt();
                int32_t FMonth = Cassette.Month->GetInt();// ->GetInt();
                int32_t FYear = Cassette.Year->GetInt();// ->GetInt();
                int32_t FCassetteNo = Cassette.CassetteNo->GetInt();// ->GetInt();
                int32_t CHour = Stoi(CD.Hour);
                int32_t CDay = Stoi(CD.Day);
                int32_t CMonth = Stoi(CD.Month);
                int32_t CYear = Stoi(CD.Year);
                int32_t CCassetteNo = Stoi(CD.CassetteNo);
                return (FDay && FMonth && FYear && FCassetteNo && FHour == CHour && FDay == CDay && FMonth == CMonth && FYear == CYear && FCassetteNo == CCassetteNo);
            }
            return false;
        }

        bool isCasseteCant(T_CassetteData& Cassette, TCassette& CD)
        {
            if(KPVL::Cassette::IsCassette(HMISheetData.Cassette))
            {
                int32_t FHour = Cassette.Hour->GetInt();//Cassette.Hour->GetInt();
                int32_t FDay = Cassette.Day->GetInt();// ->GetInt();
                int32_t FMonth = Cassette.Month->GetInt();// ->GetInt();
                int32_t FYear = Cassette.Year->GetInt();// ->GetInt();
                int32_t FCassetteNo = Cassette.CassetteNo->GetInt();// ->GetInt();
                int32_t CHour = Stoi(CD.Hour);
                int32_t CDay = Stoi(CD.Day);
                int32_t CMonth = Stoi(CD.Month);
                int32_t CYear = Stoi(CD.Year);
                int32_t CCassetteNo = Stoi(CD.CassetteNo);
                return (FDay && FMonth && FYear && FCassetteNo && FHour == CHour && FDay == CDay && FDay == CDay && FMonth == CMonth && FYear == CYear && FCassetteNo == CCassetteNo);
            }
            return false;
        }

        int GetId(PGConnection& conn, TCassette& CD)
        {
            int id = 0;
            std::stringstream ss;
            ss << "SELECT id FROM cassette WHERE";
            ss << " hour = " << CD.Hour;
            ss << " AND day = " << CD.Day;
            ss << " AND month = " << CD.Month;
            ss << " AND year = " << CD.Year;
            ss << " AND cassetteno = " << CD.CassetteNo;
            std::string comand = ss.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn_spic.PGexec(comand);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                int nFields = PQnfields(res);
                id = Stoi(conn_spis.PGgetvalue(res, 0, 0));
            }
            else
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);
            return id;
        }

        void SearthEnd_at(PGConnection& conn, TCassette& CD, int id)
        {
            std::stringstream sd;
            int Peth = Stoi(CD.Peth);
            int ID = 0;

            if(Peth == 1) ID = AppFurn1.ProcRun->ID;
            else if(Peth == 2) ID = AppFurn2.ProcRun->ID;

            if(ID)
            {
                //Конец отпуска
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
                        SETUPDATESQL(SQLLogger, conn, sf);
                            
                        //Если небыло финиша
                        if(!CD.Finish_at.length())
                        {
                            #pragma region Считае Finish_at  по End_at
                                time_t tt1, tt2;
                                std::tm TM;
                                DataTimeOfString(CD.End_at, TM);
                                TM.tm_year -= 1900;
                                TM.tm_mon -= 1;
                                tt1 = mktime(&TM);
                                tt1 += 15 * 60; //плюс 15 минут
                                localtime_s(&TM, &tt1);
                                tt2 = time(NULL);
                                if(difftime(tt2, tt1) > 15 * 60)
                                {
                                    localtime_s(&TM, &tt2);
                                    CD.Finish_at = GetDataTimeString(TM);
                                    sf << "UPDATE cassette SET Finish_at = '" << CD.Finish_at << "' WHERE id = " << id;
                                }
                            #pragma endregion

                            if(CD.Finish_at.length())
                            {
                                std::stringstream sf;
                                sf << "UPDATE cassette SET event = 5, finish_at = '" << CD.Finish_at << "' WHERE id = " << id;
                                SETUPDATESQL(SQLLogger, conn, sf);
                            }
                        }
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
            if(IsCassette(CD))
            {
                int id = GetId(conn, CD);

                if(id)
                {
                    //Отпуск на 1й - печи
                    if(isCasseteFurn(AppFurn1.Cassette, CD))
                    {
                        if(CD.Event != "3" || CD.Peth != "1")
                        {

                            std::stringstream ss;
                            ss << "UPDATE cassette SET event = 3, peth = 1, end_at = DEFAULT, finish_at = DEFAULT WHERE id = " << id;
                            SETUPDATESQL(SQLLogger, conn, ss);
                            CD.Event = 3;
                            CD.Peth = 1;
                            CD.End_at = "";
                            CD.Finish_at = "";
                        }
                    }
                    //Отпуск на 2й - печи
                    else if(isCasseteFurn(AppFurn2.Cassette, CD)) //Отпуск на 2й - печи
                    {
                        if(CD.Event != "3" || CD.Peth != "2")
                        {
                            std::stringstream ss;
                            ss << "UPDATE cassette SET event = 3, peth = 2, end_at = DEFAULT, finish_at = DEFAULT WHERE id = " << id;
                            SETUPDATESQL(SQLLogger, conn, ss);
                            CD.Event = 3;
                            CD.Peth = 2;
                            CD.End_at = "";
                            CD.Finish_at = "";
                        }
                    }
                    //Касета на контовке
                    else if(isCasseteCant(HMISheetData.Cassette, CD))
                    {
                        if(CD.Event != "1")
                        {
                            std::stringstream ss;
                            ss << "UPDATE cassette SET event = 1, peth = 0, run_at = DEFAULT, error_at = DEFAULT, end_at = DEFAULT, finish_at = DEFAULT WHERE id = " << id;
                            SETUPDATESQL(SQLLogger, conn, ss);
                            CD.Event = 1;
                            CD.Peth = 1;
                            CD.Run_at = "";
                            CD.Error_at = "";
                            CD.End_at = "";
                            CD.Finish_at = "";
                        }
                    }
                    //Если был старт отпуска 
                    else if(CD.Run_at.length())
                    {
                        //Ищем конец отпуска или ошибку
                        if(!CD.End_at.length() && !CD.Error_at.length())
                        {
                            //Поиск конца процесса
                            SearthEnd_at(conn, CD, id);
                        }

                        int ID = 0;
                        int Peth = Stoi(CD.Peth);
                        if(Peth == 1) ID = AppFurn1.ProcFault->ID;
                        else if(Peth == 2) ID = AppFurn2.ProcFault->ID;

                        //Если нет времени ошибки
                        if(ID && !CD.Error_at.length() && CD.Run_at.length() && CD.End_at.length())
                        {
                            std::stringstream sd;
                            sd << "SELECT DISTINCT ON (id) create_at FROM todos WHERE";
                            sd << " create_at >= '" << CD.Run_at << "'";
                            sd << " AND create_at <= '" << CD.End_at << "'";
                            sd << " AND content = 'true' AND id_name = " << ID;
                            sd << " ORDER BY id ASC LIMIT 1";
                            std::string comand = sd.str();
                            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                            PGresult* res = conn.PGexec(comand);
                            if(PQresultStatus(res) == PGRES_TUPLES_OK)
                            {
                                int line = PQntuples(res);
                                if(line)
                                    CD.Error_at = conn_spis.PGgetvalue(res, line - 1, 0);
                            }
                            else
                                LOG_ERR_SQL(SQLLogger, res, comand);
                            PQclear(res);

                            if(CD.Error_at.length())
                            {
                                std::stringstream sf;
                                sf << "UPDATE cassette SET event = 4, error_at = '" << CD.Error_at << "' WHERE id = " << id;
                                SETUPDATESQL(SQLLogger, conn, sf);
                            }
                        }

                    }
                    else
                    {
                        if(CD.Delete_at.length())
                        {
                            std::stringstream sf;
                            sf << "UPDATE cassette SET event = 7 WHERE id = " << id;
                            SETUPDATESQL(SQLLogger, conn, sf);
                        }
                        else if(CD.Event != "2" && !CD.Delete_at.length())
                        {
                            std::stringstream sf;
                            sf << "UPDATE cassette SET event = 2 WHERE id = " << id;
                            SETUPDATESQL(SQLLogger, conn, sf);
                        }
                    }
                }
            }
        }
    };

#pragma region Функции с кассетами в базе

    bool IsCassette(TCassette& CD)
    {
        int32_t Hour = Stoi(CD.Hour);
        int32_t Day = Stoi(CD.Day);
        int32_t Month = Stoi(CD.Month);
        int32_t Year = Stoi(CD.Year);
        int32_t CassetteNo = Stoi(CD.CassetteNo);
        return Day && Month && Year && CassetteNo;
    }

    bool IsCassete1(T_cassette& CD)
    {
        int32_t Hour = CD.Hour->GetInt();//CD.Hour->GetInt();
        int32_t Day = CD.Day->GetInt();//->GetInt();
        int32_t Month = CD.Month->GetInt();//->GetInt();
        int32_t Year = CD.Year->GetInt();//->GetInt();
        int32_t CassetteNo = CD.CassetteNo->GetInt();// GetInt();
        return Day && Month && Year && CassetteNo;
    }
    bool IsCassete2(T_cassette& CD)
    {
        int32_t Hour = Stoi(CD.Hour->GetString());//CD.Hour->GetInt();
        int32_t Day = Stoi(CD.Day->GetString());//->GetInt();
        int32_t Month = Stoi(CD.Month->GetString());//->GetInt();
        int32_t Year = Stoi(CD.Year->GetString());//->GetInt();
        int32_t CassetteNo = CD.CassetteNo->GetInt();
        return (Day && Month && Year && CassetteNo) || (!Day && !Month && !Year && !CassetteNo);
    }

    std::string AddCD(T_cassette& CD)
    {
        std::stringstream sd;
        sd << " hour = " << CD.Hour->GetInt();//Furn.Cassette.Hour->GetInt(); //GetString();
        sd << " AND day = " << CD.Day->GetInt();//GetInt(); //GetString();
        sd << " AND month = " << CD.Month->GetInt();//GetInt(); //GetString();
        sd << " AND year = " << CD.Year->GetInt();//GetInt(); //GetString();
        sd << " AND cassetteno = " << CD.CassetteNo->GetInt();//GetInt(); //GetString();
        return sd.str();
    }

    void UpdateCassetteProcRun(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        T_cassette& CD = Furn.Cassette;
        if(IsCassete1(CD))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "peth = " << Peth;
            sd << ", run_at = now()";
            
            sd << ", event = 3";
            sd << ", tempref = " << Furn.TempRef->GetFloat();//GetFloat(); //GetString();
            sd << ", pointtime_1 = " << Furn.PointTime_1->GetFloat();//GetFloat(); //GetString();
            sd << ", pointref_1 = " << Furn.PointRef_1->GetFloat();//GetFloat(); //GetString();
            sd << ", timeprocset = " << Furn.TimeProcSet->GetFloat();//GetFloat(); //GetString();
            sd << ", pointdtime_2 = " << Furn.PointDTime_2->GetFloat();//GetFloat(); //GetString();

            sd << " WHERE";
            sd << AddCD(CD);
            //sd << " hour = " << Stoi(Furn.Cassette.Hour->GetString());//Furn.Cassette.Hour->GetInt(); //GetString();
            //sd << " AND day = " << Stoi(Furn.Cassette.Day->GetString());//GetInt(); //GetString();
            //sd << " AND month = " << Stoi(Furn.Cassette.Month->GetString());//GetInt(); //GetString();
            //sd << " AND year = " << Stoi(Furn.Cassette.Year->GetString());//GetInt(); //GetString();
            //sd << " AND cassetteno = " << Stoi(Furn.Cassette.CassetteNo->GetString());//GetInt(); //GetString();
            SETUPDATESQL(SQLLogger, conn, sd);
            
            sd = std::stringstream("");
            sd << "UPDATE cassette SET ";
            sd << "error_at = DEFAULT, ";
            sd << "end_at = DEFAULT";
            sd << " WHERE" << AddCD(CD);
            //sd << " hour = " << Stoi(CD.Hour->GetString());//Furn.Cassette.Hour->GetInt(); //GetString();
            //sd << " AND day = " << Stoi(CD.Day->GetString());//GetInt(); //GetString();
            //sd << " AND month = " << Stoi(CD.Month->GetString());//GetInt(); //GetString();
            //sd << " AND year = " << Stoi(CD.Year->GetString());//GetInt(); //GetString();
            //sd << " AND cassetteno = " << Stoi(CD.CassetteNo->GetString());//GetInt(); //GetString();
            SETUPDATESQL(SQLLogger, conn, sd);

            LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={}",
                     FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.Hour->GetString(), Furn.Cassette.CassetteNo->GetString());
        }
        else
            LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={}",
                     FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.Hour->GetString(), Furn.Cassette.CassetteNo->GetString());

    }
    void UpdateCassetteProcEnd(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        T_cassette& CD = Furn.Cassette;
        if(IsCassete1(CD))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "end_at = now()";
            sd << ", event = 5";
            sd << " WHERE";
            //sd << " end_at IS NULL";
            sd << " peth = " << Peth;
            sd << " AND" << AddCD(CD);
            //sd << " hour = " << Stoi(CD.Hour->GetString());//Furn.Cassette.Hour->GetInt(); //GetString();
            //sd << " AND day = " << Stoi(CD.Day->GetString());//GetInt(); //GetString();
            //sd << " AND month = " << Stoi(CD.Month->GetString());//GetInt(); //GetString();
            //sd << " AND year = " << Stoi(CD.Year->GetString());//GetInt(); //GetString();
            //sd << " AND cassetteno = " << Stoi(CD.CassetteNo->GetString());//GetInt(); //GetString();
            SETUPDATESQL(SQLLogger, conn, sd);
            LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={}",
                     FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.Hour->GetString(), Furn.Cassette.CassetteNo->GetString());
        }
        else
            LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={}",
                     FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.Hour->GetString(), Furn.Cassette.CassetteNo->GetString());
    }
    void UpdateCassetteProcError(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        T_cassette& CD = Furn.Cassette;
        if(IsCassete1(CD))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "run_at = DEFAULT";
            sd << ", error_at = now()";
            sd << ", event = 4";
            sd << " WHERE";
            sd << " peth = " << Peth << " AND" << AddCD(CD);
            //sd << " hour = " << Stoi(CD.Hour->GetString());//Furn.Cassette.Hour->GetInt(); //GetString();
            //sd << " AND day = " << Stoi(CD.Day->GetString());//GetInt(); //GetString();
            //sd << " AND month = " << Stoi(CD.Month->GetString());//GetInt(); //GetString();
            //sd << " AND year = " << Stoi(CD.Year->GetString());//GetInt(); //GetString();
            //sd << " AND cassetteno = " << Stoi(CD.CassetteNo->GetString());//GetInt(); //GetString();
            SETUPDATESQL(SQLLogger, conn, sd);
            LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={}",
                     FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.Hour->GetString(), Furn.Cassette.CassetteNo->GetString());
        }
        else
            LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={}",
                     FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.Hour->GetString(), Furn.Cassette.CassetteNo->GetString());
    }

    void SetTemperCassette(PGConnection& conn, T_cassette& CD, std::string teper)
    {
        if(teper != CD.facttemper)
            if(IsCassete1(CD))
            {
                CD.facttemper = teper;
                std::stringstream sd;
                sd << "UPDATE cassette SET";
                sd << " facttemper = " << teper;
                sd << " WHERE " << AddCD(CD);
                //sd << " hour = " << Stoi(CD.Hour->GetString());//Furn.Cassette.Hour->GetInt(); //GetString();
                //sd << " AND day = " << Stoi(CD.Day->GetString());//GetInt(); //GetString();
                //sd << " AND month = " << Stoi(CD.Month->GetString());//GetInt(); //GetString();
                //sd << " AND year = " << Stoi(CD.Year->GetString());//GetInt(); //GetString();
                //sd << " AND cassetteno = " << Stoi(CD.CassetteNo->GetString());//GetInt(); //GetString();
                SETUPDATESQL(SQLLogger, conn, sd);
            }
    }

    void SetUpdateCassete(PGConnection& conn, T_ForBase_RelFurn& Furn, std::string update)
    {
        T_cassette& CD = Furn.Cassette;
        if(IsCassete1(CD))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << update;
            sd << " WHERE error_at IS NULL AND " << AddCD(CD);
            //sd << " hour = " << Stoi(CD.Hour->GetString());//Furn.Cassette.Hour->GetInt(); //GetString();
            //sd << " AND day = " << Stoi(CD.Day->GetString());//GetInt(); //GetString();
            //sd << " AND month = " << Stoi(CD.Month->GetString());//GetInt(); //GetString();
            //sd << " AND year = " << Stoi(CD.Year->GetString());//GetInt(); //GetString();
            //sd << " AND cassetteno = " << Stoi(CD.CassetteNo->GetString());//GetInt(); //GetString();
            SETUPDATESQL(SQLLogger, conn, sd);
        }
    }

    bool isCassete(PGConnection& conn, T_cass& tc)
    {
        //if(tc.id) return true;
        if(tc.Day && tc.Month && tc.Year && tc.CassetteNo)
        {
            std::stringstream sd;
            sd << "SELECT id FROM cassette2 WHERE";
            sd << " cassetteno = " << tc.CassetteNo;
            sd << " AND hour = " << tc.Hour;
            sd << " AND day = " << tc.Day;
            sd << " AND month = " << tc.Month;
            sd << " AND year = " << tc.Year;
            sd << " ORDER BY id;";
    
            std::string comand = sd.str();
            PGresult* res = conn.PGexec(comand);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                int len = PQntuples(res);
                if(len)
                {
                    tc.id = Stoi(conn.PGgetvalue(res, len - 1, 0));
                }
            }
            else
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);
    
            return true;
        }
        return false;
    }

    void write(T_cass& P1, int p)
    {
        std::ofstream s("cass1.csv", std::fstream::binary | std::ios::out | std::ios::app | std::ios::ate);
        if(s.is_open())
        {
            s   << p << ";"
                << " " << P1.Run_at << ";"
                << " " << P1.End_at << ";"
                << P1.Year << ";"
                << P1.Month << ";"
                << P1.Day << ";"
                << P1.Hour << ";"
                << P1.CassetteNo << ";"
                << " " << P1.Err_at << ";";
            s   << std::endl;
            s.close();
        }
    }

    //Добавление в таблицу cassette2
    void UpdateCassette(PGConnection& conn, T_cass& tc, const int nPetch)
    {
        if(tc.id)
        {
            std::stringstream sd;
            sd << "UPDATE cassette2 SET";
            if(tc.End_at.length())sd << " end_at = '" << tc.End_at << "',";
            if(tc.Err_at.length())sd << " error_at = '" << tc.Err_at << "',";
            sd << " peth = " << nPetch;
            sd << " WHERE id = " << tc.id;
            SETUPDATESQL(SQLLogger, conn, sd);
        }
        else
        {
            std::stringstream sd;
            sd << "INSERT INTO cassette2 (";
            sd << "hour, day, month, year, cassetteno, peth" ;
            if(tc.Run_at.length()) sd << ", run_at";
            if(tc.Err_at.length()) sd << ", error_at";
            if(tc.End_at.length()) sd << ", end_at";
            sd << ") VALUES (";
            sd << tc.Hour << ", " << tc.Day << ", " << tc.Month << ", " << tc.Year << ", " << tc.CassetteNo << ", " << nPetch;
            sd << ", '" << tc.Run_at << "'";
            sd << "', '" << tc.Err_at << "'";
            sd << "', '" << tc.End_at << "'";
            sd << ");";
            SETUPDATESQL(SQLLogger, conn, sd);
        }
    }

#pragma endregion

    //Печ отпуска #1
    namespace Furn1{
        T_cass Petch;

        const int nPetch = 1;
        DWORD Data_WDG_toBase(Value* value)
        {
            if(value->GetBool())
            {
                AppFurn1.Furn_old_dt = time(NULL);
                try
                {
                    AppFurn1.WDG_fromBase->Set_Value(true);
                }
                CATCH(PethLogger, "Ошибка передачи данных ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value");
                //catch(std::exception& exc)
                //{
                //    LOG_ERROR(PethLogger, "{:90}| Ошибка передачи данных ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value", FUNCTION_LINE_NAME, exc.what());
                //}
                //catch(...)
                //{
                //    LOG_ERROR(PethLogger, "{:90}| Ошибка передачи данных ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value", FUNCTION_LINE_NAME);
                //};

                struct tm TM;
                localtime_s(&TM, &AppFurn1.Furn_old_dt);
                SetWindowText(winmap(value->winId), string_time(&TM).c_str());
            }
            return 0;
        }

        //BOOL Работа
        DWORD ProcRun(Value* value)
        {
            std::string out = "Простой";
            if(value->GetBool()/*= GetBool()*/)
            {
                out = GetShortTimes();
                UpdateCassetteProcRun(conn_temp, AppFurn1, nPetch);

#pragma region Добавление в таблицу cassette2
                if(isCassete(conn_temp, Petch) && !Petch.Run_at.size())
                {
                    Petch.Run_at = GetDataTimeString();
                    //UpdateCassette(conn_temp, Petch, nPetch);
                    std::stringstream sd;
                    sd << "INSERT INTO cassette2 (";
                    sd << "hour, day, month, year, cassetteno, peth, run_at";
                    sd << ") VALUES (";
                    sd << Petch.Hour << ", " 
                        << Petch.Day << ", " 
                        << Petch.Month << ", " 
                        << Petch.Year << ", " 
                        << Petch.CassetteNo << ", " 
                        << nPetch << ", '" 
                        << Petch.Run_at << "');";
                    SETUPDATESQL(SQLLogger, conn_temp, sd);
                }
#pragma endregion

            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value)
        {
            std::string out = "";
            if(value->GetBool())
            {
                out = GetShortTimes();
                UpdateCassetteProcEnd(conn_temp, AppFurn1, nPetch);
                AppFurn1.Cassette.facttemper = "0";

#pragma region Добавление в таблицу cassette2
                if(isCassete(conn_temp, Petch) && Petch.Run_at.size())
                {
                    Petch.End_at = GetDataTimeString();
                    UpdateCassette(conn_temp, Petch, nPetch);
                    write(Petch, nPetch);
                }
#pragma endregion

            }
            Petch = T_cass();

            MySetWindowText(winmap(value->winId), out.c_str());


            return 0;
        }

        //BOOL Авария процесса
        DWORD ProcError(Value* value)
        {
            std::string out = "";
            if(value->GetBool())
            {
                out = GetShortTimes();
                UpdateCassetteProcError(conn_temp, AppFurn1, nPetch);

#pragma region Добавление в таблицу cassette2
                if(isCassete(conn_temp, Petch) && Petch.Run_at.size())
                {
                    Petch.Err_at = GetDataTimeString();
                    UpdateCassette(conn_temp, Petch, nPetch);
                }
#pragma endregion

            }
            MySetWindowText(winmap(value->winId), out.c_str());

            return 0;
        }

        DWORD Hour(Value* value)
        {
            MySetWindowText(value);
            Petch.Hour = value->GetInt();//value->GetInt();
            return 0;
        }
        DWORD Day(Value* value)
        {
            MySetWindowText(value);
            Petch.Day = value->GetInt();//value->GetInt();
            return 0;
        }
        DWORD Month(Value* value)
        {
            MySetWindowText(value);
            Petch.Month = value->GetInt();//value->GetInt();
            return 0;
        }
        DWORD Year(Value* value)
        {
            MySetWindowText(value);
            Petch.Year = value->GetInt();//value->GetInt();
            return 0;
        }
        DWORD No(Value* value)
        {
            MySetWindowText(value);
            Petch.CassetteNo = value->GetInt();//value->GetInt();
            return 0;
        }

        DWORD TimeHeatAcc(Value* value)
        {
            MySetWindowText(value);
            if(value->GetFloat())//value->GetFloat())
                SetUpdateCassete(conn_temp, AppFurn1, "heatacc = " + value->GetString() );
            return 0;
        }
        DWORD TimeHeatWait(Value* value)
        {
            MySetWindowText(value);
            if(value->GetFloat())//GetFloat())
                SetUpdateCassete(conn_temp, AppFurn1, "heatwait = " + value->GetString());
            return 0;
        }
        DWORD TimeTotal(Value* value)
        {
            MySetWindowText(value);
            if(value->GetFloat())//GetFloat())
                SetUpdateCassete(conn_temp, AppFurn1, "total = " + value->GetString());
            return 0;
        }

        //REAL Время до окончания процесса, мин
        DWORD TimeToProcEnd(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            float f = value->GetFloat();//GetFloat();
            if(f <= 5.0 && f >= 4.9)
            {
                MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.TempAct->GetString().c_str());
                SetTemperCassette(conn_temp, AppFurn1.Cassette, AppFurn1.TempAct->GetString());
            }
            else if(f >= AppFurn1.TimeProcSet->GetFloat())//GetFloat())
            {
                AppFurn1.Cassette.facttemper = "0";
                MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.Cassette.facttemper.c_str());

            }
            return 0;
        }


    }

    //Печ отпуска #2
    namespace Furn2{
        T_cass Petch;

        const int nPetch = 2;

        DWORD Data_WDG_toBase(Value* value)
        {
            if(value->GetBool())
            {
                AppFurn2.Furn_old_dt = time(NULL);
                try
                {
                    AppFurn2.WDG_fromBase->Set_Value(true);
                }
                CATCH(PethLogger, "Ошибка передачи данных ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value");
                //catch(std::exception& exc)
                //{
                //    LOG_ERROR(PethLogger, "{:90}| Ошибка передачи данных ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value {}", FUNCTION_LINE_NAME, exc.what());
                //}
                //catch(...)
                //{
                //    LOG_ERROR(PethLogger, "Unknown Ошибка передачи данных ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value ", FUNCTION_LINE_NAME);
                //};

                struct tm TM;
                localtime_s(&TM, &AppFurn2.Furn_old_dt);
                SetWindowText(winmap(value->winId), string_time(&TM).c_str());
            }
            return 0;
        }

        //BOOL Работа
        DWORD ProcRun(Value* value)
        {
            std::string out = "Простой";
            if(value->GetBool())
            {
                out = GetShortTimes();
                UpdateCassetteProcRun(conn_temp, AppFurn2, nPetch);

#pragma region Добавление в таблицу cassette2
                if(isCassete(conn_temp, Petch) && !Petch.Run_at.size())
                {
                    Petch.Run_at = GetDataTimeString();
                    std::stringstream sd;
                    sd << "INSERT INTO cassette2 (";
                    sd << "hour, day, month, year, cassetteno, peth, run_at";
                    sd << ") VALUES (";
                    sd << Petch.Hour << ", " << Petch.Day << ", " << Petch.Month << ", " << Petch.Year << ", " << Petch.CassetteNo << ", " << nPetch << ", '" << Petch.Run_at << "');";
                    SETUPDATESQL(SQLLogger, conn_temp, sd);
#pragma endregion
                }

            }
            MySetWindowText(winmap(value->winId), out.c_str());

            return 0;
        }

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value)
        {
            std::string out = "";
            if(value->GetBool())
            {
                out = GetShortTimes();
                UpdateCassetteProcEnd(conn_temp, AppFurn2, nPetch);
                AppFurn1.Cassette.facttemper = "0";

#pragma region Добавление в таблицу cassette2
                if(isCassete(conn_temp, Petch) && Petch.Run_at.size())
                {
                    Petch.End_at = GetDataTimeString();
                    UpdateCassette(conn_temp, Petch, nPetch);
                }
#pragma endregion
            }
            Petch = T_cass();

            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL Авария процесса
        DWORD ProcError(Value* value)
        {
            std::string out = "";
            if(value->GetBool())
            {
                out = GetShortTimes();
                UpdateCassetteProcError(conn_temp, AppFurn2, nPetch);

#pragma region Добавление в таблицу cassette2
                if(isCassete(conn_temp, Petch))
                {
                    Petch.Err_at = GetDataTimeString();
                    UpdateCassette(conn_temp, Petch, nPetch);
                }
#pragma endregion
            }
            MySetWindowText(winmap(value->winId), out.c_str());


            return 0;
        }

        DWORD Hour(Value* value)
        {
            MySetWindowText(value);
            Petch.Hour = value->GetInt();//value->GetInt();
            return 0;
        }
        DWORD Day(Value* value)
        {
            MySetWindowText(value);
            Petch.Day = value->GetInt();//value->GetInt();
            return 0;
        }
        DWORD Month(Value* value)
        {
            MySetWindowText(value);
            Petch.Month = value->GetInt();//value->GetInt();
            return 0;
        }
        DWORD Year(Value* value)
        {
            MySetWindowText(value);
            Petch.Year = value->GetInt();//value->GetInt();
            return 0;
        }
        DWORD No(Value* value)
        {
            MySetWindowText(value);
            Petch.CassetteNo =value->GetInt();// value->GetInt();
            return 0;
        }

        DWORD TimeHeatAcc(Value* value)
        {
            MySetWindowText(value);
            if(value->GetFloat())//GetFloat())
                SetUpdateCassete(conn_temp, AppFurn2, "heatacc = " + value->GetString());
            return 0;
        }
        DWORD TimeHeatWait(Value* value)
        {
            MySetWindowText(value);
            if(value->GetFloat())//GetFloat())
                SetUpdateCassete(conn_temp, AppFurn2, "heatwait = " + value->GetString());
            return 0;
        }
        DWORD TimeTotal(Value* value)
        {
            MySetWindowText(value);
            if(value->GetFloat())//GetFloat())
                SetUpdateCassete(conn_temp, AppFurn2, "total = " + value->GetString());
            return 0;
        }

        //REAL Время до окончания процесса, мин
        DWORD TimeToProcEnd(Value* value)
        {
            MySetWindowText(winmap(value->winId), value->GetString().c_str());
            float f = value->GetFloat();// GetFloat();
            if(f <= 5.0 && f >= 4.9)
            {
                MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.TempAct->GetString().c_str());
                SetTemperCassette(conn_temp, AppFurn2.Cassette, AppFurn2.TempAct->GetString());
            }
            else if(f >= AppFurn2.TimeProcSet->GetFloat())//GetFloat())
            {
                AppFurn2.Cassette.facttemper = "0";
                MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.Cassette.facttemper.c_str());

            }
            return 0;
        }

    };

}


void SetUpdateCassete(PGConnection& conn, TCassette& cassette, std::string update, std::string where)
{
    if(S107::IsCassette(cassette))
    {
        std::stringstream sd;
        sd << "UPDATE cassette SET ";
        sd << update;
        sd << " WHERE " + where;
        sd << " id = " << cassette.Id;
        sd << ";";
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
        SETUPDATESQL(SQLLogger, conn, sd);
    }
}

