#include "pch.h"

#include "win.h"
#include "main.h"
#include "StringData.h"
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
std::shared_ptr<spdlog::logger> FurnLogger_1;
std::shared_ptr<spdlog::logger> FurnLogger_2;


namespace S107
{
    std::string URI = "opc.tcp://192.168.9.40:4840";
    std::string ServerDataTime = "";


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
        int PointTime_2 = 0;      //Время выдержки
        int facttemper = 0;          //Факт температуры за 5 минут до конца отпуска
        int Finish_at = 0;
        int HeatAcc = 0;           //Факт время нагрева
        int HeatWait = 0;          //Факт время выдержки
        int Total = 0;             //Факт общее время
        int Correct = 0;           //
        int Pdf = 0;               //
        int Return_at = 0;

    };

    //Получение номера колонки
    void GetCollumn(PGresult* res)
    {
        if(!Coll::Return_at)
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
                else if(l == "pointdtime_2") Coll::PointTime_2 = j;
                else if(l == "facttemper") Coll::facttemper = j;
                else if(l == "finish_at") Coll::Finish_at = j;
                else if(l == "heatacc")Coll::HeatAcc = j;
                else if(l == "heatwait")Coll::HeatWait = j;
                else if(l == "total")Coll::Total = j;
                else if(l == "correct")Coll::Correct = j;
                else if(l == "pdf")Coll::Pdf = j;
                else if(l == "hour")Coll::Hour = j;
                else if(l == "return_at")Coll::Return_at = j;
                
            }
        }
    }

#pragma region Функции с кассетами в базе
    //Чтение кассеты
    void GetCassette(PGConnection& conn, PGresult* res, TCassette& cassette, int line)
    {
        GetCollumn(res);
        if(Coll::Return_at)
        {
            cassette.Create_at = GetStringData(conn.PGgetvalue(res, line, Coll::Create_at));
            cassette.Id = conn.PGgetvalue(res, line, Coll::Id);
            cassette.Event = conn.PGgetvalue(res, line, Coll::Event);
            cassette.Hour = conn.PGgetvalue(res, line, Coll::Hour);
            cassette.Day = conn.PGgetvalue(res, line, Coll::Day);
            cassette.Month = conn.PGgetvalue(res, line, Coll::Month);
            cassette.Year = conn.PGgetvalue(res, line, Coll::Year);
            cassette.CassetteNo = conn.PGgetvalue(res, line, Coll::CassetteNo);
            cassette.SheetInCassette = conn.PGgetvalue(res, line, Coll::SheetInCassette);
            cassette.Close_at = GetStringData(conn.PGgetvalue(res, line, Coll::Close_at));
            cassette.Peth = conn.PGgetvalue(res, line, Coll::Peth);
            cassette.Run_at = GetStringData(conn.PGgetvalue(res, line, Coll::Run_at));
            cassette.Error_at = GetStringData(conn.PGgetvalue(res, line, Coll::Error_at));
            cassette.End_at = GetStringData(conn.PGgetvalue(res, line, Coll::End_at));
            cassette.Delete_at = GetStringData(conn.PGgetvalue(res, line, Coll::Delete_at));
            cassette.TempRef = conn.PGgetvalue(res, line, Coll::TempRef);           //Заданное значение температуры
            cassette.PointTime_1 = conn.PGgetvalue(res, line, Coll::PointTime_1);       //Время разгона
            cassette.PointRef_1 = conn.PGgetvalue(res, line, Coll::PointRef_1);        //Уставка температуры
            cassette.TimeProcSet = conn.PGgetvalue(res, line, Coll::TimeProcSet);       //Полное время процесса (уставка), мин
            cassette.PointTime_2 = conn.PGgetvalue(res, line, Coll::PointTime_2);      //Время выдержки
            cassette.facttemper = conn.PGgetvalue(res, line, Coll::facttemper);          //Факт температуры за 5 минут до конца отпуска
            cassette.Finish_at = GetStringData(conn.PGgetvalue(res, line, Coll::Finish_at)); //Завершение процесса + 15 минут
            cassette.HeatAcc = conn.PGgetvalue(res, line, Coll::HeatAcc); //Завершение процесса + 15 минут
            cassette.HeatWait = conn.PGgetvalue(res, line, Coll::HeatWait); //Завершение процесса + 15 минут
            cassette.Total = conn.PGgetvalue(res, line, Coll::Total); //Завершение процесса + 15 минут
            cassette.Correct = conn.PGgetvalue(res, line, Coll::Correct); //Завершение процесса + 15 минут
            cassette.Pdf = conn.PGgetvalue(res, line, Coll::Pdf); //Завершение процесса + 15 минут
            cassette.Return_at = conn.PGgetvalue(res, line, Coll::Return_at); //Завершение процесса + 15 минут
        }

    }
    int GetId(PGConnection& conn, TCassette& CD)
    {
        int id = 0;
        try
        {
            std::stringstream ss;
            ss << "SELECT id FROM cassette WHERE";
            ss << " year = '" << CD.Year << "'";
            ss << " AND day = '" << CD.Day << "'";
            ss << " AND month = '" << CD.Month << "'";
            ss << " AND hour = " << CD.Hour;
            ss << " AND cassetteno = " << CD.CassetteNo;
            std::string command = ss.str();
            PGresult* res = conn.PGexec(command);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                if(PQntuples(res))
                    id = Stoi(conn.PGgetvalue(res, 0, 0));
            }
            else
                LOG_ERR_SQL(PethLogger, res, command);
            PQclear(res);
        }
        CATCH(PethLogger, "");
        return id;
    }
    int GetId(PGConnection& conn, T_Fcassette& CD)
    {
        int id = 0;
        try
        {
            std::stringstream ss;
            ss << "SELECT id FROM cassette WHERE";
            ss << " year = '" << CD.Year->Val.As<int32_t>() << "'";
            ss << " AND month = '" << CD.Month->Val.As<int32_t>() << "'";
            ss << " AND day = '" << CD.Day->Val.As<int32_t>() << "'";
            ss << " AND cassetteno = " << CD.CassetteNo->Val.As<int32_t>();
            ss << " AND hour = " << CD.Hour->Val.As<uint16_t>();
            std::string command = ss.str();
            PGresult* res = conn.PGexec(command);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                   
                if(PQntuples(res))
                {
                    std::string s = conn.PGgetvalue(res, 0, 0);
                    if(s.length())
                        id = Stoi(s);
                    else
                        id = 0;
                }
            }
            else
                LOG_ERR_SQL(PethLogger, res, command);
            PQclear(res);
        }
        CATCH(PethLogger, "");
        return id;
    }
	int GetId(PGConnection& conn, int Year, int Month, int Day, int CassetteNo, int Hour )
    {
        int id = 0;
        try
        {
            std::stringstream ss;
            ss << "SELECT id FROM cassette WHERE";
            ss << " year = '" << Year << "'";
            ss << " AND month = '" << Month << "'";
            ss << " AND day = '" << Day << "'";
            ss << " AND cassetteno = " << CassetteNo;
            ss << " AND hour = " << Hour;
            std::string command = ss.str();
            PGresult* res = conn.PGexec(command);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                   
                if(PQntuples(res))
                {
                    std::string s = conn.PGgetvalue(res, 0, 0);
                    if(s.length())
                        id = Stoi(s);
                    else
                        id = 0;
                }
            }
            else
                LOG_ERR_SQL(PethLogger, res, command);
            PQclear(res);
        }
        CATCH(PethLogger, "");
        return id;
    }

    int GetEvent(PGConnection& conn, T_Fcassette& CD)
    {
        int events = 0;
        try
        {
            std::stringstream ss;
            ss << "SELECT event FROM cassette WHERE";
            ss << " year = '" << CD.Year->Val.As<int32_t>() << "'";
            ss << " AND day = '" << CD.Day->Val.As<int32_t>() << "'";
            ss << " AND month = '" << CD.Month->Val.As<int32_t>() << "'";
            ss << " AND hour = " << CD.Hour->Val.As<uint16_t>();
            ss << " AND cassetteno = " << CD.CassetteNo->Val.As<int32_t>();
            std::string command = ss.str();

            //LOG_INFO(PethLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
            PGresult* res = conn.PGexec(command);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                if(PQntuples(res))
                    events = Stoi(conn.PGgetvalue(res, 0, 0));
            }
            else
                LOG_ERR_SQL(PethLogger, res, command);
            PQclear(res);
        }
        CATCH(PethLogger, "");
        return events;
    }
    bool IsCassette(TCassette& CD)
    {
        try
        {
     //int32_t Hour = Stoi(CD.Hour);
            int32_t Day = Stoi(CD.Day);
            int32_t Month = Stoi(CD.Month);
            int32_t Year = Stoi(CD.Year);
            int32_t CassetteNo = Stoi(CD.CassetteNo);
            return Day && Month && Year && CassetteNo;
        }
        CATCH(PethLogger, "");
        return false;
    }
    bool IsFCassete(T_Fcassette& CD)
    {
        try
        {
            int32_t Day = CD.Day->Val.As<int32_t>();
            int32_t Month = CD.Month->Val.As<int32_t>();
            int32_t Year = CD.Year->Val.As<int32_t>();
            int32_t CassetteNo = CD.CassetteNo->Val.As<int32_t>();
            return Day && Month && Year && CassetteNo;
        }
        CATCH(PethLogger, "");
        return false;
    }
#pragma endregion

    namespace SQL{
        bool GetCountSheet(PGConnection& conn, TCassette& CD)
        {
            try
            {
                if(IsCassette(CD))
                {
                    std::stringstream ss;
                    ss << "SELECT count(*) FROM sheet WHERE delete_at IS NULL ";
                    ss << " AND year = '" << CD.Year << "'";
                    ss << " AND month = '" << CD.Month << "'";
                    ss << " AND day = '" << CD.Day << "'";
                    ss << " AND hour = '" << CD.Hour << "'";
                    ss << " AND cassetteno = " << CD.CassetteNo;
                    std::string command = ss.str();

                    PGresult* res = conn.PGexec(command);
                    if(PQresultStatus(res) == PGRES_TUPLES_OK)
                    {
                        std::string countList = "-1";
                        if(PQntuples(res))
                            countList = conn.PGgetvalue(res, 0, 0);
                        PQclear(res);

                        if(Stoi(countList) == 0) countList = "-1";

                        if(countList != CD.SheetInCassette)
                        {
                            CD.SheetInCassette = countList;
                            std::stringstream ss;
                            ss << "UPDATE cassette SET sheetincassette = " << countList;
                            ss << " WHERE id = " << CD.Id;
                            SETUPDATESQL(PethLogger, conn, ss);
                            return false;
                        }
                    }
                    else
                    {
                        LOG_ERR_SQL(PethLogger, res, command);
                        PQclear(res);
                    }
                }
            }
            CATCH(PethLogger, "");
            return false;
        }

        void ReadTag()
        {
            while(!AppFurn1.Cassette.Hour->Codesys)
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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
        }

        void FURN_SQL(PGConnection& conn, std::deque<TCassette>& allCassette)
        {
            ReadTag();
            try
            {

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
                ss << " AND delete_at IS NULL ORDER BY event ASC, create_at DESC;"; //DESC
                std::string command = ss.str();

                PGresult* res = conn.PGexec(command);
                if(PQresultStatus(res) == PGRES_TUPLES_OK)
                {

                    allCassette.erase(allCassette.begin(), allCassette.end());
                    int line =  PQntuples(res);
                    for(int l = 0; l < line; l++)
                    {
                        TCassette cassette;
                        GetCassette(conn, res, cassette, l);
                        allCassette.push_back(cassette);
                        if(!isRun) return;
                    }
                }
                else
                    LOG_ERR_SQL(PethLogger, res, command);
                PQclear(res);
                for(auto& a : allCassette)
                    GetCountSheet(conn, a);
            }
            CATCH(PethLogger, "");
        }

    };


#pragma region Функции с кассетами в базе
	void GetFurn(Tcass& Petch, int Peth)
	{
			if(Peth == 1)
			{
				Petch = Furn1::Petch;
			}
			if(Peth == 2)
			{
				Petch = Furn2::Petch;
			}
	}


    bool GetFinishCassete(PGConnection& conn, std::shared_ptr<spdlog::logger> Logger, TCassette& it)
    {
        try
        {
            if(!it.End_at.length()) return false;
            if(!it.Run_at.length()) return false;
            if(it.Finish_at.length()) return true;
            time_t tmp_at = DataTimeOfString(it.End_at);
            time_t tFinish = tmp_at + (60 * 15); //+15 минут

            std::string finish = GetStringOfDataTime(&tFinish);;
            std::string Currt = GetStringDataTime();

            //Финализируем если прошло 15 минут после конца отпуска
            if(Currt >= finish)
                it.Finish_at = finish;

            return (bool)it.Finish_at.length();
        }
        CATCH(Logger, "");
        return false;
    }

    int UpdateCassetteProcRun(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        int id = 0;
        try
        {
			Tcass& P = Furn1::Petch;
			GetFurn(P, Peth);
			if(!P.Run)
			{

				P.Run = true;
				P.Run_at = GetStringDataTime();
				P.Year = Furn.Cassette.Year->GetString();
				P.Month = Furn.Cassette.Month->GetString();
				P.Day = Furn.Cassette.Day->GetString();
				P.Hour = Furn.Cassette.Hour->GetString();
				P.CassetteNo = Furn.Cassette.CassetteNo->GetString();
			}

			LOG_INFO(PethLogger, "{:90}| ProcRun Peth = {}, Run_at = {}, Year = {}, Month = {}, Day = {}, Hour = {}, CassetteNo = {}",
					 FUNCTION_LINE_NAME, P.Peth, P.Run_at, P.Year, P.Month, P.Day, P.Hour, P.CassetteNo);

            T_Fcassette& CD = Furn.Cassette;
            if(IsFCassete(CD))
            {
                id = GetId(conn, CD);
                if(id)
                {
                    std::stringstream sd ;
                    sd << "UPDATE cassette SET ";

                    if(GetEvent(conn, CD) != 3)
                    {
                        sd << "peth = " << Peth << ", ";
                        sd << "event = 3, ";
                        sd << "run_at = now(), ";
                        sd << "end_at = DEFAULT, ";
                        sd << "finish_at = DEFAULT, ";
                        sd << "error_at = DEFAULT, ";
                        sd << "correct = DEFAULT, ";
                        sd << "pdf = DEFAULT, ";
                    }

                    sd << "pointref_1 = " << Furn.PointRef_1->Val.As<float>() << ", ";
                    sd << "pointtime_1 = " << Furn.PointTime_1->Val.As<float>() << ", ";
                    sd << "pointdtime_2 = " << Furn.PointTime_2->Val.As<float>() << ", ";
                    sd << "timeprocset = " << Furn.TimeProcSet->Val.As<float>() << " ";
                    sd << "WHERE id = " << id;

                    LOG_INFO(PethLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                    SETUPDATESQL(PethLogger, conn, sd);
                    LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={} Event = 3",
                             FUNCTION_LINE_NAME, Peth, 
                             Furn.Cassette.Year->GetString(), 
                             Furn.Cassette.Month->GetString(), 
                             Furn.Cassette.Day->GetString(), 
                             Furn.Cassette.Hour->GetString(), 
                             Furn.Cassette.CassetteNo->GetString());
                }
                else
                {
					#pragma region MyRegion

                    std::stringstream sd;
                    sd << "INSERT INTO cassette (";
                    sd << "event, ";
                    sd << "peth, ";
                    sd << "run_at, ";
                    sd << "end_at, ";
                    sd << "finish_at, ";
                    sd << "error_at, ";
                    sd << "correct, ";
                    sd << "pdf, ";
                    sd << "pointref_1, ";
                    sd << "pointtime_1, ";
                    sd << "pointdtime_2, ";
                    sd << "timeprocset, ";
                    sd << "year, ";
                    sd << "month, ";
                    sd << "day, ";
                    sd << "hour, ";
                    sd << "cassetteno, ";
                    sd << "sheetincassette";
                    sd << ") VALUES (";

                    sd << "3, ";
                    sd << Peth << ", ";
                    sd << "now(), ";
                    sd << "DEFAULT, ";
                    sd << "DEFAULT, ";
                    sd << "DEFAULT, ";
                    sd << "DEFAULT, ";
                    sd << "DEFAULT, ";
                    sd << Furn.PointRef_1->Val.As<float>() << ", ";
                    sd << Furn.PointTime_1->Val.As<float>() << ", ";
                    sd << Furn.PointTime_2->Val.As<float>() << ", ";
                    sd << Furn.TimeProcSet->Val.As<float>() << ", ";
                    sd << CD.Year->Val.As<int32_t>() << ", ";
                    sd << CD.Month->Val.As<int32_t>() << ", ";
                    sd << CD.Day->Val.As<int32_t>() << ", ";
                    sd << CD.Hour->Val.As<uint16_t>() << ", ";
                    sd << CD.CassetteNo->Val.As<int32_t>() << ", ";
                    sd << "-1)";
					#pragma endregion

                    LOG_INFO(PethLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                    SETUPDATESQL(PethLogger, conn, sd);
					
                    id = GetId(conn, CD);
                }
            }
        }
        CATCH(PethLogger, "");
        return id;
    }

	//TCassette itTCassette;
    int UpdateCassetteProcEnd(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        int id = 0;
        try
        {
			Tcass& P = Furn1::Petch;
			GetFurn(P, Peth);

			LOG_INFO(TestLogger, "{:90}| ProcEnd Peth = {}, Run_at = {}, Year = {}, Month = {}, Day = {}, Hour = {}, CassetteNo = {}",
					 FUNCTION_LINE_NAME,P.Peth, P.Run_at, P.Year, P.Month, P.Day, P.Hour, P.CassetteNo);

			P = Tcass(Peth);

            T_Fcassette& CD = Furn.Cassette;

            if(IsFCassete(CD))
            {
                id = GetId(conn, CD);
                std::stringstream sd;
                sd << "UPDATE cassette SET end_at = now() WHERE id = " << id;
                LOG_INFO(PethLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                SETUPDATESQL(PethLogger, conn, sd);
                //LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={}",
                //         FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.Hour->GetString(), Furn.Cassette.CassetteNo->GetString());
            }
			//else
			//{ 
			//	std::time_t st = time(NULL);
			//	tm curr_tm;
			//	localtime_s(&curr_tm, &st);
			//	int32_t Year = (curr_tm.tm_year + 1900);
			//	int32_t Month = (curr_tm.tm_mon + 1);
			//	int32_t Day = (curr_tm.tm_mday);
			//	uint16_t Hour = (curr_tm.tm_hour);
			//	int32_t CassetteNo = 255;
			//
			//	std::string str = GetStringOfDataTime(st);
			//
			//	std::stringstream sd;
			//	sd << "INSERT INTO cassette (";
			//	sd << "event, ";
			//	sd << "run_at, ";
			//	sd << "end_at, ";
			//	sd << "finish_at, ";
			//	sd << "peth, ";
			//
			//	sd << "year, ";
			//	sd << "month, ";
			//	sd << "day, ";
			//	sd << "hour, ";
			//	sd << "cassetteno, ";
			//	sd << "sheetincassette";
			//
			//	sd << ") VALUES (5";
			//	
			//	sd << ", " << str;
			//	sd << ", " << str;
			//	sd << ", " << str;
			//
			//	sd << ", " << Peth;
			//	sd << ", " << Year;
			//	sd << ", " << Month;
			//	sd << ", " << Day;
			//	sd << ", " << Hour;
			//	sd << ", " << CassetteNo;
			//	sd << ", 0)";
			//	
			//	LOG_INFO(TestLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
			//    SETUPDATESQL(PethLogger, conn, sd);
			//	id = GetId(conn, Year, Month, Day, CassetteNo, Hour);
			//      LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={} Id={}", FUNCTION_LINE_NAME, Peth, Year, Month, Day, Hour, CassetteNo, id);
			//
			//	itTCassette = TCassette();
			//	itTCassette.Id = std::to_string(id);
			//	itTCassette.Peth = std::to_string(Peth);
			//	itTCassette.Run_at = str;
			//	itTCassette.End_at = str;
			//	itTCassette.Finish_at = str;
			//
			//	CreateThread(0, 0, PDF::CorrectCassetteFinal, (LPVOID)&itTCassette, 0, 0);
			//
			//}
        }
        CATCH(PethLogger, "");
        return 0;
    }
    int UpdateCassetteProcError(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth)
    {
        int id = 0;
        try
        {
            T_Fcassette& CD = Furn.Cassette;
            if(IsFCassete(CD))
            {
                id = GetId(conn, CD);
                std::stringstream sd;
                sd << "UPDATE cassette SET ";
                sd << "run_at = DEFAULT";
                sd << ", error_at = now()";
                sd << ", event = 4";
                sd << " WHERE";
                sd << " id = " << id;
                LOG_INFO(TestLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                //SETUPDATESQL(PethLogger, conn, sd);
                LOG_INFO(PethLogger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={} Event = 4",
                         FUNCTION_LINE_NAME, Peth, Furn.Cassette.Year->GetString(), Furn.Cassette.Month->GetString(), Furn.Cassette.Day->GetString(), Furn.Cassette.Hour->GetString(), Furn.Cassette.CassetteNo->GetString());
            }
        }
        CATCH(PethLogger, "");
        return id;
    }

    int ReturnCassette(PGConnection& conn, T_ForBase_RelFurn& Furn, const int nPetch)
    {
        int id = 0;
        try
        {
            T_Fcassette& CD = Furn.Cassette;
            if(IsFCassete(Furn.Cassette))
            {
                id = GetId(conn, CD);
                std::stringstream sd;
                sd << "UPDATE cassette SET";
                sd << " event = 2, ";
                sd << " run_at = DEFAULT, ";
                sd << " error_at = DEFAULT, ";
                sd << " end_at = DEFAULT, ";
                sd << " delete_at = DEFAULT, ";
                sd << " finish_at = DEFAULT, ";
                sd << " return_at = now(), ";
                sd << " peth = " << nPetch;
                sd << " WHERE id = " << id;

                LOG_INFO(TestLogger, "{:89}| {}", FUNCTION_LINE_NAME, sd.str());
                SETUPDATESQL(PethLogger, conn, sd);
            }
        }
        CATCH(PethLogger, "");
        return 0;
    }

    void SetTemperCassette(PGConnection& conn, T_Fcassette& CD, std::string teper)
    {
        try
        {
            if(teper != CD.facttemper)
            {
                if(IsFCassete(CD))
                {
                    int id = GetId(conn, CD);
                    CD.facttemper = teper;
                    std::stringstream sd;
                    sd << "UPDATE cassette SET";
                    sd << " facttemper = " << teper;
                    sd << " WHERE id = " << id;
                    SETUPDATESQL(PethLogger, conn, sd);
                }
            }
        }
        CATCH(PethLogger, "");
    }

    void SetUpdateCassete(PGConnection& conn, T_ForBase_RelFurn& Furn, std::string update)
    {
        try
        {
            T_Fcassette& CD = Furn.Cassette;
            if(IsFCassete(CD))
            {
                int id = GetId(conn, CD); 
                std::stringstream sd;
                sd << "UPDATE cassette SET ";
                sd << update;
                sd << " WHERE id = " << id;
                SETUPDATESQL(PethLogger, conn, sd);
            }
        }
        CATCH(PethLogger, "");
    }
#pragma endregion

    //Печ отпуска #1
    namespace Furn1{
        const int nPetch = 1;
		Tcass Petch(nPetch);
        int CassetteId = 0;
		T_ForBase_RelFurn& Furn = ForBase_RelFurn_1;


        DWORD Data_WDG_toBase(Value* value)
        {
            try
            {
                if(value->GetBool())
                {
                    Furn.Furn_old_dt = time(NULL);
                    try
                    {
                        //Furn.WDG_toBase->Set_Value(false);
                        Furn.WDG_fromBase->Set_Value(true);
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
                    localtime_s(&TM, &Furn.Furn_old_dt);
                    SetWindowText(winmap(value->winId), string_time(&TM).c_str());
                }
            }
            CATCH(PethLogger, "");
            return 0;
        }

        //BOOL Работа
        DWORD ProcRun(Value* value)
        {
            try
            {
                std::string out = "Простой";
				
				if(value->GetBool())
				{
                    out = GetShortTimes();
                    CassetteId = UpdateCassetteProcRun(*value->Conn, Furn, nPetch);
                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(PethLogger, "");
            return 0;
        }

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value)
        {
            try
            {
                std::string out = "";
                if(value->GetBool())
                {
                    out = GetShortTimes();
                    CassetteId = UpdateCassetteProcEnd(*value->Conn, Furn, nPetch);
                    Furn.Cassette.facttemper = "0";

                }

                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(PethLogger, "");

            return 0;
        }

        //BOOL Авария процесса
        DWORD ProcError(Value* value)
        {
            try
            {
                std::string out = "";
                if(value->GetBool())
                {
                    out = GetShortTimes();
                    CassetteId = UpdateCassetteProcError(*value->Conn, Furn, nPetch);

                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(PethLogger, "");

            return 0;
        }

        //BOOL Возврат касеты в список
        DWORD ReturnCassetteCmd(Value* value)
        {
            try
            {
                bool b = value->GetBool();
                if(b)
                {
                    CassetteId = ReturnCassette(*value->Conn, Furn, nPetch);
                    value->Set_Value(false);
                    LOG_INFO(PethLogger, "{:89}| {} {}", FUNCTION_LINE_NAME, "peth 1: ReturnCassetteCmd", b);
                }
                if(DEB)LOG_INFO(PethLogger, "{:89}| {} {}", FUNCTION_LINE_NAME, "peth 1: ReturnCassetteCmd", b);
            }
            CATCH(PethLogger, "");

            return 0;
        }

        DWORD Hour(Value* value)
        {
            try
            {
				if(!Petch.Run)
					Petch.Hour = ForBase_RelFurn_1.Cassette.Hour->GetString();
				MySetWindowText(winmap(value->winId), Petch.Hour);
                //MySetWindowText(value);
            }
            CATCH(PethLogger, "");

            return 0;
        }
        DWORD Day(Value* value)
        {
            try
            {
                //MySetWindowText(value);
				if(!Petch.Run)
					Petch.Day = Furn.Cassette.Day->GetString();
				MySetWindowText(winmap(value->winId),  Petch.Day);
            }
            CATCH(PethLogger, "");

            return 0;
        }
        DWORD Month(Value* value)
        {
            try
            {
                //MySetWindowText(value);
				if(!Petch.Run)
					Petch.Month = Furn.Cassette.Month->GetString();
				MySetWindowText(winmap(value->winId), Petch.Month);
            }
            CATCH(PethLogger, "");

            return 0;
        }
        DWORD Year(Value* value)
        {
            try
            {
                //MySetWindowText(value);
				if(!Petch.Run)
					Petch.Year = Furn.Cassette.Year->GetString();
				MySetWindowText(winmap(value->winId), Petch.Year);
            }
            CATCH(PethLogger, "");

            return 0;
        }
        DWORD No(Value* value)
        {
            try
            {
                //MySetWindowText(value);
				if(!Petch.Run)
					Petch.CassetteNo = Furn.Cassette.CassetteNo->GetString();
				MySetWindowText(winmap(value->winId), Petch.CassetteNo);
            }
            CATCH(PethLogger, "");

            return 0;
        }

        DWORD TimeHeatAcc(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//value->GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "heatacc = " + value->GetString());
            }
            CATCH(PethLogger, "");

            return 0;
        }
        DWORD TimeHeatWait(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "heatwait = " + value->GetString());
            }
            CATCH(PethLogger, "");
            return 0;
        }
        DWORD TimeTotal(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "total = " + value->GetString());
            }
            CATCH(PethLogger, "");
            return 0;
        }

        //REAL Время до окончания процесса, мин
        DWORD TimeToProcEnd(Value* value)
        {
            try
            {
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                float f = value->GetFloat();//GetFloat();
                if(f <= 5.0 && f >= 4.9)
                {
                    MySetWindowText(winmap(RelF1_Edit_Proc1), Furn.TempAct->GetString().c_str());
                    SetTemperCassette(*value->Conn, Furn.Cassette, Furn.TempAct->GetString());
                }
                else if(f >= Furn.TimeProcSet->GetFloat())//GetFloat())
                {
                    Furn.Cassette.facttemper = "0";
                    MySetWindowText(winmap(RelF1_Edit_Proc1), Furn.Cassette.facttemper.c_str());

                }
            }
            CATCH(PethLogger, "");

            return 0;
        }


    }

    //Печ отпуска #2
    namespace Furn2{
        const int nPetch = 2;
		Tcass Petch(nPetch);
        int CassetteId = 0;
		T_ForBase_RelFurn& Furn = ForBase_RelFurn_2;

        DWORD Data_WDG_toBase(Value* value)
        {
            try{
                if(value->GetBool())
                {
                    Furn.Furn_old_dt = time(NULL);
                    try
                    {
                        //Furn.WDG_toBase->Set_Value(false);
                        Furn.WDG_fromBase->Set_Value(true);
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
                    localtime_s(&TM, &Furn.Furn_old_dt);
                    SetWindowText(winmap(value->winId), string_time(&TM).c_str());
                }
            }
            CATCH(PethLogger, "");
            return 0;
        }

        //BOOL Работа
        DWORD ProcRun(Value* value)
        {
            try
            {
                std::string out = "Простой";
				
				if(value->GetBool())
				{
					out = GetShortTimes();
					CassetteId = UpdateCassetteProcRun(*value->Conn, Furn, nPetch);
				
#pragma region Добавление в таблицу cassette2
                    //if(isCassete(conn_temp, Petch) && !Petch.Run_at.size())
                    //{
                    //    Petch.Run_at = GetDataTimeString();
                    //    std::stringstream sd;
                    //    sd << "INSERT INTO cassette2 (";
                    //    sd << "hour, day, month, year, cassetteno, peth, run_at";
                    //    sd << ") VALUES (";
                    //    sd << Petch.Hour << ", " << Petch.Day << ", " << Petch.Month << ", " << Petch.Year << ", " << Petch.CassetteNo << ", " << nPetch << ", '" << Petch.Run_at << "');";
                    //    SETUPDATESQL(PethLogger, conn_temp, sd);
                    //}
#pragma endregion

                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(PethLogger, "");
            return 0;
        }

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value)
        {
            try
            {
                std::string out = "";
                if(value->GetBool())
                {
                    out = GetShortTimes();
                    UpdateCassetteProcEnd(*value->Conn, Furn, nPetch);
                    CassetteId = 0;
                    Furn.Cassette.facttemper = "0";

#pragma region Добавление в таблицу cassette2
                    //if(isCassete(conn_temp, Petch) && Petch.Run_at.size())
                    //{
                    //    Petch.End_at = GetDataTimeString();
                    //    UpdateCassette(conn_temp, Petch, nPetch);
                    //}
#pragma endregion
                }
                //Petch = T_cass();

                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(PethLogger, "");
            return 0;
        }

        //BOOL Авария процесса
        DWORD ProcError(Value* value)
        {
            try
            {
                std::string out = "";
                if(value->GetBool())
                {
                    out = GetShortTimes();
                    CassetteId = UpdateCassetteProcError(*value->Conn, Furn, nPetch);

#pragma region Добавление в таблицу cassette2
                    //if(isCassete(conn_temp, Petch))
                    //{
                    //    Petch.Err_at = GetDataTimeString();
                    //    UpdateCassette(conn_temp, Petch, nPetch);
                    //}
#pragma endregion
                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(PethLogger, "");
            return 0;
        }

        //BOOL Возврат касеты в список
        DWORD ReturnCassetteCmd(Value* value)
        {
            try
            {
                bool b = value->GetBool();
                if(b)
                {
                    CassetteId = ReturnCassette(*value->Conn, Furn, nPetch);
                    value->Set_Value(false);
                    LOG_INFO(PethLogger, "{:89}| {} {}", FUNCTION_LINE_NAME, "peth 2: ReturnCassetteCmd", b);
                }
                if(DEB)LOG_INFO(PethLogger, "{:89}| {} {}", FUNCTION_LINE_NAME, "peth 2: ReturnCassetteCmd", b);
            }
            CATCH(PethLogger, "");
            return 0;
        }


        DWORD Hour(Value* value)
        {
            try
            {
				//MySetWindowText(value);
                if(!Petch.Run)
					Petch.Hour = Furn.Cassette.Hour->GetString();
				MySetWindowText(winmap(value->winId), Petch.Hour);
            }
            CATCH(PethLogger, "");
            return 0;
        }
        DWORD Day(Value* value)
        {
            try
            {
                //MySetWindowText(value);
				if(!Petch.Run)
					Petch.Day = Furn.Cassette.Day->GetString();
				MySetWindowText(winmap(value->winId),  Petch.Day);
            }
            CATCH(PethLogger, "");
            return 0;
        }
        DWORD Month(Value* value)
        {
            try
            {
                //MySetWindowText(value);
                if(!Petch.Run)
					Petch.Month = Furn.Cassette.Month->GetString();
				MySetWindowText(winmap(value->winId), Petch.Month);
            }
            CATCH(PethLogger, "");
            return 0;
        }
        DWORD Year(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(!Petch.Run)
					Petch.Year = Furn.Cassette.Year->GetString();
				MySetWindowText(winmap(value->winId), Petch.Year);
            }
            CATCH(PethLogger, "");
            return 0;
        }
        DWORD No(Value* value)
        {
            try
            {
                //MySetWindowText(value);
                if(!Petch.Run)
					Petch.CassetteNo = Furn.Cassette.CassetteNo->GetString();
				MySetWindowText(winmap(value->winId), Petch.CassetteNo);
            }
            CATCH(PethLogger, "");
            return 0;
        }

        DWORD TimeHeatAcc(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "heatacc = " + value->GetString());
            }
            CATCH(PethLogger, "");
            return 0;
        }
        DWORD TimeHeatWait(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "heatwait = " + value->GetString());
            }
            CATCH(PethLogger, "");
            return 0;
        }
        DWORD TimeTotal(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "total = " + value->GetString());
            }
            CATCH(PethLogger, "");
            return 0;
        }

        //REAL Время до окончания процесса, мин
        DWORD TimeToProcEnd(Value* value)
        {
            try
            {
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                float f = value->GetFloat();// GetFloat();
                if(f <= 5.0 && f >= 4.9)
                {
                    MySetWindowText(winmap(RelF2_Edit_Proc1), Furn.TempAct->GetString().c_str());
                    SetTemperCassette(*value->Conn, Furn.Cassette, Furn.TempAct->GetString());
                }
                else if(f >= Furn.TimeProcSet->GetFloat())//GetFloat())
                {
                    Furn.Cassette.facttemper = "0";
                    MySetWindowText(winmap(RelF2_Edit_Proc1), Furn.Cassette.facttemper.c_str());

                }
            }
            CATCH(PethLogger, "");
            return 0;
        }

    };

}


void SetUpdateCassete(PGConnection& conn, TCassette& cassette, std::string update, std::string where)
{
    try
    {
        if(S107::IsCassette(cassette))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << update;
            sd << " WHERE " + where;
            sd << " id = " << cassette.Id;
            sd << ";";
            LOG_INFO(PethLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
            SETUPDATESQL(PethLogger, conn, sd);
        }
    }
    CATCH(PethLogger, "");
}

