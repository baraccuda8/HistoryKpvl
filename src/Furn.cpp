#include "pch.h"

#include "win.h"
#include "main.h"
#include "StringData.h"
#include "file.h"
#include "SQL.h"
#include "ValueTag.h"
#include "term.h"
//#include "hard.h"
//#include "Graff.h"
//#include "KPVL.h"
#include "Furn.h"
#include "Pdf.h"

//extern std::shared_ptr<spdlog::logger> PethLogger;
//std::shared_ptr<spdlog::logger> FurnLogger_1;
//std::shared_ptr<spdlog::logger> FurnLogger_2;


namespace S107
{
    std::string URI = "opc.tcp://192.168.9.40:4840";
    std::string ServerDataTime = "";


    //������ ������� � �������
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
        int TempRef = 0;           //�������� �������� �����������
        int PointTime_1 = 0;       //����� �������
        int PointRef_1 = 0;        //������� �����������
        int TimeProcSet = 0;       //������ ����� �������� (�������), ���
        int PointTime_2 = 0;      //����� ��������
        int facttemper = 0;          //���� ����������� �� 5 ����� �� ����� �������
        int Finish_at = 0;
        int HeatAcc = 0;           //���� ����� �������
        int HeatWait = 0;          //���� ����� ��������
        int Total = 0;             //���� ����� �����
        int Correct = 0;           //
        int Pdf = 0;               //
        int Return_at = 0;

    };

    //��������� ������ �������
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

#pragma region ������� � ��������� � ����
    //������ �������
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
            cassette.TempRef = conn.PGgetvalue(res, line, Coll::TempRef);           //�������� �������� �����������
            cassette.PointTime_1 = conn.PGgetvalue(res, line, Coll::PointTime_1);       //����� �������
            cassette.PointRef_1 = conn.PGgetvalue(res, line, Coll::PointRef_1);        //������� �����������
            cassette.TimeProcSet = conn.PGgetvalue(res, line, Coll::TimeProcSet);       //������ ����� �������� (�������), ���
            cassette.PointTime_2 = conn.PGgetvalue(res, line, Coll::PointTime_2);      //����� ��������
            cassette.facttemper = conn.PGgetvalue(res, line, Coll::facttemper);          //���� ����������� �� 5 ����� �� ����� �������
            cassette.Finish_at = GetStringData(conn.PGgetvalue(res, line, Coll::Finish_at)); //���������� �������� + 15 �����
            cassette.HeatAcc = conn.PGgetvalue(res, line, Coll::HeatAcc); //���������� �������� + 15 �����
            cassette.HeatWait = conn.PGgetvalue(res, line, Coll::HeatWait); //���������� �������� + 15 �����
            cassette.Total = conn.PGgetvalue(res, line, Coll::Total); //���������� �������� + 15 �����
            cassette.Correct = conn.PGgetvalue(res, line, Coll::Correct); //���������� �������� + 15 �����
            cassette.Pdf = conn.PGgetvalue(res, line, Coll::Pdf); //���������� �������� + 15 �����
            cassette.Return_at = conn.PGgetvalue(res, line, Coll::Return_at); //���������� �������� + 15 �����
        }

    }
    int GetId(PGConnection& conn, T_Fcassette& CD, LOGGER Logger)
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
                LOG_ERR_SQL(Logger, res, command);
            PQclear(res);
        }
        CATCH(Logger, "");
        return id;
    }
	int GetId(PGConnection& conn, int Year, int Month, int Day, int CassetteNo, int Hour, LOGGER Logger)
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
                LOG_ERR_SQL(Logger, res, command);
            PQclear(res);
        }
        CATCH(Logger, "");
        return id;
    }
	//int GetId(PGConnection& conn, Tcass& P)
 //   {
 //       int id = 0;
 //       try
 //       {
 //           std::stringstream ss;
 //           ss << "SELECT id FROM cassette WHERE";
 //           ss << " year = '" << P.Year << "'";
 //           ss << " AND month = '" << P.Month << "'";
 //           ss << " AND day = '" << P.Day << "'";
 //           ss << " AND cassetteno = " << P.CassetteNo;
 //           ss << " AND hour = " << P.Hour;
 //           std::string command = ss.str();
 //           PGresult* res = conn.PGexec(command);
 //           if(PQresultStatus(res) == PGRES_TUPLES_OK)
 //           {
 //                  
 //               if(PQntuples(res))
 //               {
 //                   std::string s = conn.PGgetvalue(res, 0, 0);
 //                   if(s.length())
 //                       id = Stoi(s);
 //                   else
 //                       id = 0;
 //               }
 //           }
 //           else
 //               LOG_ERR_SQL(PethLogger, res, command);
 //           PQclear(res);
 //       }
 //       CATCH(PethLogger, "");
 //       return id;
 //   }

	template <class T>
    int GetId(PGConnection& conn, T& CD, LOGGER Logger)
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
                LOG_ERR_SQL(Logger, res, command);
            PQclear(res);
        }
        CATCH(Logger, "");
        return id;
    }

    int GetEvent(PGConnection& conn, T_Fcassette& CD, LOGGER Logger)
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

            //LOG_INFO(Logger, "{:90}| {}", FUNCTION_LINE_NAME, command);
            PGresult* res = conn.PGexec(command);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                if(PQntuples(res))
                    events = Stoi(conn.PGgetvalue(res, 0, 0));
            }
            else
                LOG_ERR_SQL(Logger, res, command);
            PQclear(res);
        }
        CATCH(Logger, "");
        return events;
    }

	template <class T>
	bool IsCassette(T& CD)
	{
			//int32_t Hour = Stoi(CD.Hour);
		int32_t Day = Stoi(CD.Day);
		int32_t Month = Stoi(CD.Month);
		int32_t Year = Stoi(CD.Year);
		int32_t CassetteNo = Stoi(CD.CassetteNo);
		return Day && Month && Year && CassetteNo;
		return false;
	}

    bool IsFCassete(T_Fcassette& CD, LOGGER Logger)
    {
        try
        {
            int32_t Day = CD.Day->Val.As<int32_t>();
            int32_t Month = CD.Month->Val.As<int32_t>();
            int32_t Year = CD.Year->Val.As<int32_t>();
            int32_t CassetteNo = CD.CassetteNo->Val.As<int32_t>();
            return Day && Month && Year && CassetteNo;
        }
        CATCH(Logger, "");
        return false;
    }
#pragma endregion

    namespace SQL{
        bool GetCountSheet(PGConnection& conn, TCassette& CD, LOGGER Logger)
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
                            SETUPDATESQL(Logger, conn, ss);
                            return false;
                        }
                    }
                    else
                    {
                        LOG_ERR_SQL(Logger, res, command);
                        PQclear(res);
                    }
                }
            }
            CATCH(Logger, "");
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

        void FURN_SQL(PGConnection& conn, std::deque<TCassette>& allCassette, LOGGER Logger)
        {
            ReadTag();
            try
            {

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
                ss << " AND finish_at IS NULL AND delete_at IS NULL ORDER BY event ASC, create_at DESC;"; //DESC
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
                    LOG_ERR_SQL(Logger, res, command);
                PQclear(res);
                for(auto& a : allCassette)
                    GetCountSheet(conn, a, Logger);
            }
            CATCH(Logger, "");
        }

    };


#pragma region ������� � ��������� � ����
	
	Tcass& GetIgCassetteFurn(int Peth)
	{
		static Tcass T;
		if(Peth == 1)
		{
			return Furn1::Petch;
		} else if(Peth == 2)
		{
			return Furn2::Petch;
		} else
			throw std::exception(__FUN("������ ������ ����: " + std::to_string(Peth)));
		return T;
	}

	T_ForBase_RelFurn& GetBaseRelFurn(int Peth)
	{
		if(Peth == 1)
		{
			return Furn1::Furn;
		} else if(Peth == 2)
		{
			return Furn2::Furn;
		} else
			throw std::exception(__FUN("������ ������ ����: " + std::to_string(Peth)));
		return Furn1::Furn;
	}

	std::string GetErrorIdCassete(int Peth)
	{
		Tcass& P = GetIgCassetteFurn(Peth);
		std::stringstream ss;
		ss << "������ ������ �������: " << Peth;
		ss << " Year = " << P.Year;
		ss << " Month = " << P.Month;
		ss << " Day = " << P.Day;
		ss << " Hour =  " << P.Hour;
		ss << " CassetteNo =  " << P.CassetteNo;
		return ss.str();
	}



    bool GetFinishCassete(PGConnection& conn, LOGGER Logger, TCassette& it)
    {
        try
        {
            if(!it.End_at.length()) return false;
            if(!it.Run_at.length()) return false;
            if(it.Finish_at.length()) return true;
            time_t tmp_at = DataTimeOfString(it.End_at);
            time_t tFinish = tmp_at + (60 * 15); //+15 �����

            std::string finish = GetStringOfDataTime(&tFinish);;
            std::string Currt = GetStringDataTime();

            //������������ ���� ������ 15 ����� ����� ����� �������
            if(Currt >= finish)
                it.Finish_at = finish;

            return (bool)it.Finish_at.length();
        }
        CATCH(Logger, "");
        return false;
    }

	int UpdateCassetteProcRun(PGConnection& conn, int Peth, LOGGER Logger)
	{
		int id = 0;
		T_ForBase_RelFurn& F = GetBaseRelFurn(Peth);
		Tcass& P = GetIgCassetteFurn(Peth);
		T_Fcassette& CD = F.Cassette;

		std::string Year = F.Cassette.Year->GetString();
		std::string Month = F.Cassette.Month->GetString();
		std::string Day = F.Cassette.Day->GetString();
		std::string Hour = F.Cassette.Hour->GetString();
		std::string CassetteNo = F.Cassette.CassetteNo->GetString();

		std::string PointRef_1 = F.PointRef_1->GetString();
		std::string PointTime_1 = F.PointTime_1->GetString();
		std::string PointTime_2 = F.PointTime_2->GetString();
		std::string TimeProcSet = F.TimeProcSet->GetString();

		try
		{
			if(!P.Run)
			{
				P.Run = true;
				P.Run_at = GetStringDataTime();
				P.Year = Year;
				P.Month = Month;
				P.Day = Day;
				P.Hour = Hour;
				P.CassetteNo = CassetteNo;
			}

			LOG_INFO(Logger, "{:90}| ProcRun Peth = {}, Run_at = {}, Year = {}, Month = {}, Day = {}, Hour = {}, CassetteNo = {}",
					 FUNCTION_LINE_NAME, P.Peth, P.Run_at, P.Year, P.Month, P.Day, P.Hour, P.CassetteNo);
		}
		CATCH(Logger, "");

		try
		{
			if(IsFCassete(CD, Logger))
			{
				id = GetId(conn, CD, Logger);
				if(id)
				{
					std::stringstream sd;
					sd << "UPDATE cassette SET ";

					if(GetEvent(conn, CD, Logger) != 3)
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

					sd << "pointref_1 = " << PointRef_1 << ", ";
					sd << "pointtime_1 = " << PointTime_1 << ", ";
					sd << "pointdtime_2 = " << PointTime_2 << ", ";
					sd << "timeprocset = " << TimeProcSet << " ";
					sd << "WHERE id = " << id;

					LOG_INFO(Logger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
					SETUPDATESQL(Logger, conn, sd);
					LOG_INFO(Logger, "{:90}| Peth={}, Year={}, Month={}, Day={}, Hour={}, CassetteNo={} Event = 3",
							 FUNCTION_LINE_NAME, Peth, Year, Month, Day, Hour, CassetteNo);
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
					sd << PointRef_1 << ", ";
					sd << PointTime_1 << ", ";
					sd << PointTime_2 << ", ";
					sd << TimeProcSet << ", ";
					sd << Year << ", ";
					sd << Month << ", ";
					sd << Day << ", ";
					sd << Hour << ", ";
					sd << CassetteNo << ", ";
					sd << "-1)";
					#pragma endregion

					LOG_INFO(Logger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
					SETUPDATESQL(Logger, conn, sd);

					id = GetId(conn, CD, Logger);
				}
			}
			else
			{ 
				P = Tcass(Peth);
				throw std::exception(__FUN(GetErrorIdCassete(Peth)));
			}
		}
		CATCH(Logger, "");
		return id;
	}

	//TCassette itTCassette;
    int UpdateCassetteProcEnd(PGConnection& conn, int Peth, LOGGER Logger)
    {
        int id = 0;
		Tcass& P = GetIgCassetteFurn(Peth);
		T_Fcassette& CD = GetBaseRelFurn(Peth).Cassette;

        try
        {
			#pragma region ���� stringstream sd
			std::stringstream sd;
			sd << "UPDATE cassette SET end_at = now() WHERE id = ";
			#pragma endregion

			if(IsFCassete(CD, Logger)) id = GetId(conn, CD, Logger);
            if(id)
            {
				LOG_INFO(Logger, "{:90}| ProcEnd Peth = {}, End_at = {}, Year = {}, Month = {}, Day = {}, Hour = {}, CassetteNo = {}, Id = {}",
						 FUNCTION_LINE_NAME, Peth, 
						 GetStringDataTime(), CD.Year->GetString(),  CD.Month->GetString(),  CD.Day->GetString(),  CD.Hour->GetString(),  CD.CassetteNo->GetString(),  id);

				sd << id;
                LOG_INFO(Logger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                SETUPDATESQL(Logger, conn, sd);
            }
			else
			{ 
				if(IsCassette(P)) id = GetId(conn, P, Logger);
				if(id)
				{
					LOG_INFO(Logger, "{:90}| ProcEnd Peth = {}, Run_at = {}, Year = {}, Month = {}, Day = {}, Hour = {}, CassetteNo = {}, Id = {}",
							 FUNCTION_LINE_NAME, P.Peth, P.Run_at, P.Year, P.Month, P.Day, P.Hour, P.CassetteNo, id);

					sd << id;
					LOG_INFO(Logger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
					SETUPDATESQL(Logger, conn, sd);
				}
				else
				{
					throw std::exception(__FUN(GetErrorIdCassete(Peth)));
				}

			}
        }
        CATCH(Logger, "");
        return 0;
    }

    int UpdateCassetteProcError(PGConnection& conn, int Peth, LOGGER Logger)
    {
        int id = 0;
		Tcass& P = GetIgCassetteFurn(Peth);
		T_Fcassette& CD = GetBaseRelFurn(Peth).Cassette;

        try
        {

			#pragma region ���� stringstream sd
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "run_at = DEFAULT";
            sd << ", error_at = now()";
            sd << ", event = 4";
            sd << " WHERE";
            sd << " id = ";
			#pragma endregion

			if(IsFCassete(CD, Logger)) id = GetId(conn, CD, Logger);

            if(id)
            {
                sd << id;
                LOG_INFO(Logger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
                SETUPDATESQL(Logger, conn, sd);
            }
			else
			{
				if(IsCassette(P)) id = GetId(conn, P, Logger);
				if(id)
				{
					sd << id;
					LOG_INFO(Logger, "{:89}| {}", FUNCTION_LINE_NAME, sd.str());
					SETUPDATESQL(Logger, conn, sd);
				}
				else
				{
					throw std::exception(__FUN(GetErrorIdCassete(Peth)));
				}
			}
        }
        CATCH(Logger, "");
        return id;
    }

    int ReturnCassette(PGConnection& conn, int Peth, LOGGER Logger)
    {
        int id = 0;
		Tcass& P = GetIgCassetteFurn(Peth);
		T_Fcassette& CD = GetBaseRelFurn(Peth).Cassette;

		try
        {
		
			#pragma region ���� stringstream sd
			std::stringstream sd;
			sd << "UPDATE cassette SET";
			sd << " event = 2, ";
			sd << " run_at = DEFAULT, ";
			sd << " error_at = DEFAULT, ";
			sd << " end_at = DEFAULT, ";
			sd << " delete_at = DEFAULT, ";
			sd << " finish_at = DEFAULT, ";
			sd << " return_at = now(), ";
			sd << " peth = " << Peth;
			sd << " WHERE id = ";
			#pragma endregion

			if(IsFCassete(CD, Logger)) id = GetId(conn, CD, Logger);
            if(id)
            {
                sd << id;
                LOG_INFO(Logger, "{:89}| {}", FUNCTION_LINE_NAME, sd.str());
                SETUPDATESQL(Logger, conn, sd);
            }
			else
			{ 
				if(IsCassette(P)) id = GetId(conn, P, Logger);
				if(id)
				{
					sd << id;
					LOG_INFO(Logger, "{:89}| {}", FUNCTION_LINE_NAME, sd.str());
					SETUPDATESQL(Logger, conn, sd);
				}
				else
				{
					throw std::exception(__FUN(GetErrorIdCassete(Peth)));
				}
			}
        }
        CATCH(Logger, "");
        return 0;
    }

    void SetTemperCassette(PGConnection& conn, T_Fcassette& CD, std::string teper, LOGGER Logger)
    {
        try
        {
            if(teper != CD.facttemper)
            {
                if(IsFCassete(CD, Logger))
                {
                    int id = GetId(conn, CD, Logger);
                    CD.facttemper = teper;
                    std::stringstream sd;
                    sd << "UPDATE cassette SET";
                    sd << " facttemper = " << teper;
                    sd << " WHERE id = " << id;
                    SETUPDATESQL(Logger, conn, sd);
                }
            }
        }
        CATCH(Logger, "");
    }

    void SetUpdateCassete(PGConnection& conn, T_ForBase_RelFurn& Furn, std::string update, LOGGER Logger)
    {
        try
        {
            T_Fcassette& CD = Furn.Cassette;
            if(IsFCassete(CD, Logger))
            {
                int id = GetId(conn, CD, Logger); 
                std::stringstream sd;
                sd << "UPDATE cassette SET ";
                sd << update;
                sd << " WHERE id = " << id;
                SETUPDATESQL(Logger, conn, sd);
            }
        }
        CATCH(Logger, "");
    }
#pragma endregion

    //��� ������� #1
    namespace Furn1{
        const int nPetch = 1;
		Tcass Petch(nPetch);
        int CassetteId = 0;
		LOGGER Logger = NULL;

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
                    CATCH(Logger, "������ �������� ������ ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value");
                    //catch(std::exception& exc)
                    //{
                    //    LOG_ERROR(PethLogger, "{:90}| ������ �������� ������ ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value", FUNCTION_LINE_NAME, exc.what());
                    //}
                    //catch(...)
                    //{
                    //    LOG_ERROR(PethLogger, "{:90}| ������ �������� ������ ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value", FUNCTION_LINE_NAME);
                    //};

                    struct tm TM;
                    localtime_s(&TM, &Furn.Furn_old_dt);
                    SetWindowText(winmap(value->winId), string_time(&TM).c_str());
                }
            }
            CATCH(Logger, "");
            return 0;
        }

        //BOOL ������
        DWORD ProcRun(Value* value)
        {
            try
            {
                std::string out = "�������";
				
				if(value->GetBool())
				{
                    out = GetShortTimes();
                    CassetteId = UpdateCassetteProcRun(*value->Conn, nPetch, Logger);
                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(Logger, "");
            return 0;
        }

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value)
        {
            try
            {
                std::string out = "";
                if(value->GetBool())
                {
                    out = GetShortTimes();
                    CassetteId = UpdateCassetteProcEnd(*value->Conn, nPetch, Logger);
                    Furn.Cassette.facttemper = "0";
					Petch = Tcass (nPetch);
                }

                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(Logger, "");

            return 0;
        }

        //BOOL ������ ��������
        DWORD ProcError(Value* value)
        {
            try
            {
                std::string out = "";
                if(value->GetBool())
                {
                    out = GetShortTimes();
                    CassetteId = UpdateCassetteProcError(*value->Conn, nPetch, Logger);

                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(Logger, "");

            return 0;
        }

        //BOOL ������� ������ � ������
        DWORD ReturnCassetteCmd(Value* value)
        {
            try
            {
                bool b = value->GetBool();
                if(b)
                {
                    CassetteId = ReturnCassette(*value->Conn, nPetch, Logger);
                    value->Set_Value(false);
                    //LOG_INFO(FurnLogger, "{:89}| {} {}", FUNCTION_LINE_NAME, "peth 1: ReturnCassetteCmd", b);
                }
            }
            CATCH(Logger, "");

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
            CATCH(Logger, "");

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
            CATCH(Logger, "");

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
            CATCH(Logger, "");

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
            CATCH(Logger, "");

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
            CATCH(Logger, "");

            return 0;
        }

        DWORD TimeHeatAcc(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//value->GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "heatacc = " + value->GetString(), Logger);
            }
            CATCH(Logger, "");

            return 0;
        }
        DWORD TimeHeatWait(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "heatwait = " + value->GetString(), Logger);
            }
            CATCH(Logger, "");
            return 0;
        }
        DWORD TimeTotal(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "total = " + value->GetString(), Logger);
            }
            CATCH(Logger, "");
            return 0;
        }

        //REAL ����� �� ��������� ��������, ���
        DWORD TimeToProcEnd(Value* value)
        {
            try
            {
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                float f = value->GetFloat();//GetFloat();
                if(f <= 5.0 && f >= 4.9)
                {
                    MySetWindowText(winmap(RelF1_Edit_Proc1), Furn.TempAct->GetString().c_str());
                    SetTemperCassette(*value->Conn, Furn.Cassette, Furn.TempAct->GetString(), Logger);
                }
                else if(f >= Furn.TimeProcSet->GetFloat())//GetFloat())
                {
                    Furn.Cassette.facttemper = "0";
                    MySetWindowText(winmap(RelF1_Edit_Proc1), Furn.Cassette.facttemper.c_str());

                }
            }
            CATCH(Logger, "");

            return 0;
        }


    }

    //��� ������� #2
    namespace Furn2{
        const int nPetch = 2;
		Tcass Petch(nPetch);
        int CassetteId = 0;
		LOGGER Logger = NULL;

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
                    CATCH(Logger, "������ �������� ������ ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value");
                    //catch(std::exception& exc)
                    //{
                    //    LOG_ERROR(FurnLogger, "{:90}| ������ �������� ������ ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value {}", FUNCTION_LINE_NAME, exc.what());
                    //}
                    //catch(...)
                    //{
                    //    LOG_ERROR(FurnLogger, "Unknown ������ �������� ������ ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value ", FUNCTION_LINE_NAME);
                    //};

                    struct tm TM;
                    localtime_s(&TM, &Furn.Furn_old_dt);
                    SetWindowText(winmap(value->winId), string_time(&TM).c_str());
                }
            }
            CATCH(Logger, "");
            return 0;
        }

        //BOOL ������
        DWORD ProcRun(Value* value)
        {
            try
            {
                std::string out = "�������";
				
				if(value->GetBool())
				{
					out = GetShortTimes();
					CassetteId = UpdateCassetteProcRun(*value->Conn, nPetch, Logger);
                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(Logger, "");
            return 0;
        }

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value)
        {
            try
            {
                std::string out = "";
                if(value->GetBool())
                {
                    out = GetShortTimes();
                    UpdateCassetteProcEnd(*value->Conn, nPetch, Logger);
                    CassetteId = 0;
                    Furn.Cassette.facttemper = "0";
					Petch = Tcass (nPetch);
                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(Logger, "");
            return 0;
        }

        //BOOL ������ ��������
        DWORD ProcError(Value* value)
        {
            try
            {
                std::string out = "";
                if(value->GetBool())
                {
                    out = GetShortTimes();
                    CassetteId = UpdateCassetteProcError(*value->Conn, nPetch, Logger);
                }
                MySetWindowText(winmap(value->winId), out.c_str());
            }
            CATCH(Logger, "");
            return 0;
        }

        //BOOL ������� ������ � ������
        DWORD ReturnCassetteCmd(Value* value)
        {
            try
            {
                bool b = value->GetBool();
                if(b)
                {
                    CassetteId = ReturnCassette(*value->Conn, nPetch, Logger);
                    value->Set_Value(false);
                    //LOG_INFO(FurnLogger, "{:89}| {} {}", FUNCTION_LINE_NAME, "peth 2: ReturnCassetteCmd", b);
                }
            }
            CATCH(Logger, "");
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
            CATCH(Logger, "");
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
            CATCH(Logger, "");
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
            CATCH(Logger, "");
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
            CATCH(Logger, "");
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
            CATCH(Logger, "");
            return 0;
        }

        DWORD TimeHeatAcc(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "heatacc = " + value->GetString(), Logger);
            }
            CATCH(Logger, "");
            return 0;
        }
        DWORD TimeHeatWait(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "heatwait = " + value->GetString(), Logger);
            }
            CATCH(Logger, "");
            return 0;
        }
        DWORD TimeTotal(Value* value)
        {
            try
            {
                MySetWindowText(value);
                if(value->GetFloat())//GetFloat())
                    SetUpdateCassete(*value->Conn, Furn, "total = " + value->GetString(), Logger);
            }
            CATCH(Logger, "");
            return 0;
        }

        //REAL ����� �� ��������� ��������, ���
        DWORD TimeToProcEnd(Value* value)
        {
            try
            {
                MySetWindowText(winmap(value->winId), value->GetString().c_str());
                float f = value->GetFloat();// GetFloat();
                if(f <= 5.0 && f >= 4.9)
                {
                    MySetWindowText(winmap(RelF2_Edit_Proc1), Furn.TempAct->GetString().c_str());
                    SetTemperCassette(*value->Conn, Furn.Cassette, Furn.TempAct->GetString(), Logger);
                }
                else if(f >= Furn.TimeProcSet->GetFloat())//GetFloat())
                {
                    Furn.Cassette.facttemper = "0";
                    MySetWindowText(winmap(RelF2_Edit_Proc1), Furn.Cassette.facttemper.c_str());

                }
            }
            CATCH(Logger, "");
            return 0;
        }

    };

}


//void SetUpdateCassete(PGConnection& conn, TCassette& cassette, std::string update, std::string where)
//{
//    try
//    {
//        if(S107::IsCassette(cassette))
//        {
//            std::stringstream sd;
//            sd << "UPDATE cassette SET ";
//            sd << update;
//            sd << " WHERE " + where;
//            sd << " id = " << cassette.Id;
//            sd << ";";
//            LOG_INFO(Logger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
//            SETUPDATESQL(Logger, conn, sd);
//        }
//    }
//    CATCH(Logger, "");
//}

