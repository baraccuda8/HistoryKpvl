#include "pch.h"
#include <optional>
#include <setjmp.h>
//#include <codecvt>

#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include "win.h"
#include "main.h"
#include "StringData.h"
#include "file.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "Pdf.h"
#include "Graff.h"


#include "PdfPass.h"


#include <filesystem>


std::shared_ptr<spdlog::logger> SheetLogger = NULL;
std::shared_ptr<spdlog::logger> CassetteLogger = NULL;
std::shared_ptr<spdlog::logger> CorrectLog = NULL;

#if _DEBUG
#define HENDINSERT 1
#define NO_UPDATE 1
#else
#define HENDINSERT 0
#define NO_UPDATE 0
#endif


extern TSheet PalletSheet[7];

//��� ���������� ������, �������� ����� � ������� ������ ��������� "correct IS NULL AND"
#define TESTPDF

////��� ���������� ������, �� ��������� � ����]
#define TESTPDF2


extern std::map<int, std::string> GenSeq1;
extern std::map<int, std::string> GenSeq2;
extern std::map<int, std::string> GenSeq3;

namespace PDF
{
	bool Correct = false;

	namespace CASSETTE
	{
		typedef std::map <std::string, T_Todos> MapRunn;
		T_ForBase_RelFurn* GetFurn(int Peth)
		{
			if(Peth == 1)
				return &ForBase_RelFurn_1;
			if(Peth == 2)
				return &ForBase_RelFurn_2;

			throw std::runtime_error(__FUN(("Error patametr Furn = ") + std::to_string(Peth)));
		}

		std::string getHour(std::string Hour)
		{
			std::string shour = "-1";
			boost::regex xRegEx("^\\d{4}-\\d{2}-\\d{2} (\\d{2}):\\d{2}:\\d{2}.*");
			boost::match_results<std::string::const_iterator>what;
			boost::regex_search(Hour, what, xRegEx, boost::match_default);
			if(what.size())
			{
				int hour = Stoi(what[1]);
				shour = std::to_string(hour);
			}
			return shour;
		}
		void GetHendCassettePDF(PGConnection& conn, std::vector <std::string>& IDS)
		{
			try
			{
				//std::string command = "SELECT id, run_at  FROM cassette WHERE pdf IS NULL AND run_at > (now() - interval '7 day') AND CAST(event AS integer) = 5 ORDER BY run_at;";
				std::string command = "SELECT id FROM cassette WHERE "
					"delete_at IS NULL AND "
					      "pdf IS NULL AND "
					  "correct IS NOT NULL AND "
					"finish_at IS NOT NULL AND "
					   "end_at IS NOT NULL AND "
					   "run_at IS NOT NULL "
					"ORDER BY run_at DESC;";
				//std::string command = "SELECT id, run_at FROM cassette WHERE CAST(event AS integer) >= 5 ORDER BY run_at;";
				PGresult* res = conn.PGexec(command);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line =  PQntuples(res);
					for(int l = 0; l < line; l++)
					{
						std::string ids = conn.PGgetvalue(res, l, 0);
						if(Stoi(ids))
							IDS.push_back(ids);
					}
				}
				else
					LOG_ERR_SQL(CassetteLogger, res, command);
				PQclear(res);
			}CATCH(CassetteLogger, "");
		}

		void HendCassettePDF(PGConnection& conn)
		{
			InitLogger(CassetteLogger);
			try
			{
				std::vector <std::string> IDS;
				GetHendCassettePDF(conn, IDS);
		
				for(auto ids : IDS)
				{
					std::string command = "SELECT * FROM cassette WHERE id = " + ids;
					PGresult* res = conn.PGexec(command);
					TCassette Cassette;
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						if(conn.PQntuples(res))
							S107::GetCassette(conn, res, Cassette, 0);
						PQclear(res);
						PASSPORT::PdfClass sdc(Cassette, false);
					}
					else
					{
						LOG_ERR_SQL(CassetteLogger, res, command);
						PQclear(res);
					}
					
				}
			}CATCH(CassetteLogger, "");
		}

		class GetCassettes
		{
		public:
			//std::string DateCorrect = "";
			std::string DateStart = "";
			std::string DateStop = "";
			std::fstream fUpdateCassette;
			int Petch = 0;
			int Id = 0;
			
			int lin = 1;

			void GetStopTime(PGConnection& conn, std::string Start, int ID)
			{
				if(!DateStart.length()) return;
				std::stringstream sss;
				sss << "SELECT create_at"
					//", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)"
					" FROM todos WHERE id_name = " << ID	    //����� �������
					<< " AND create_at >= '" << Start << "'"
					<< " AND content = 'true'"
					<< " ORDER BY create_at DESC LIMIT 1"; // LIMIT 3
				std::string command = sss.str();
				PGresult* res = conn.PGexec(command);
				//��������� ������
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					if(PQntuples(res))
						DateStop = conn.PGgetvalue(res, 0, 0);
				}
				else
				{
					LOG_ERR_SQL(CassetteLogger, res, command);
				}
				PQclear(res);
			}

			std::string GetVal(PGConnection& conn, int ID, std::string Run_at, std::string End_at)
			{
				std::string f = "0";
				try
				{
					std::stringstream sss;
					sss << "SELECT content FROM todos WHERE ";
					sss << " id_name = " << ID; 			//����������� �������� �����������
					sss << " AND create_at >= '" << Run_at << "'";
					sss << " AND create_at < '" << End_at << "'";
					sss << " ORDER BY create_at DESC LIMIT 1";
					std::string command = sss.str();
					PGresult* res = conn.PGexec(command);
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						int line = PQntuples(res);
						if(line)
							f = conn.PGgetvalue(res, line - 1, 0);
					}
					else
					{
						LOG_ERR_SQL(CassetteLogger, res, command);
					}
					PQclear(res);
				}
				CATCH(CassetteLogger, "");;
				return f;
			}

			void GetActTime(PGConnection& conn, TCassette& it, T_ForBase_RelFurn* Furn)
			{
				try
				{
				#pragma region ������� ������ � ����

					std::stringstream sss;
					sss << "SELECT DISTINCT ON (id_name) id_name, content";
					sss << ", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)";
					sss << " FROM todos WHERE (";
					sss << " id_name = " << Furn->ActTimeHeatAcc->ID << " OR";	//���� ����� �������
					sss << " id_name = " << Furn->ActTimeHeatWait->ID << " OR";	//���� ����� ��������
					sss << " id_name = " << Furn->ActTimeTotal->ID;				//���� ����� �����
					sss << ")";
					sss << " AND create_at >= '" << it.Run_at << "'";
					sss << " AND create_at < '" << it.End_at << "'";
					sss << " AND cast (content as numeric) <> 0";
					sss << " ORDER BY id_name, create_at DESC"; // LIMIT 3

				#pragma endregion

				#pragma region ������ � ����

					std::string command = sss.str();
					PGresult* res = conn.PGexec(command);
					//��������� ������
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						int line = PQntuples(res);
						//��������� ������
						for(int l = 0; l < line && isRun; l++)
						{
							int id = Stoi(conn.PGgetvalue(res, l, 0));
							std::string f = conn.PGgetvalue(res, l, 1);

							if(f.length())
							{
								if(id == Furn->ActTimeHeatAcc->ID && !it.HeatAcc.length()) it.HeatAcc = f;
								if(id == Furn->ActTimeHeatWait->ID && !it.HeatWait.length()) it.HeatWait = f;
								if(id == Furn->ActTimeTotal->ID && !it.Total.length()) it.Total = f;
							}
						}
					}
					else
					{
						LOG_ERR_SQL(CassetteLogger, res, command);
					}
					PQclear(res);

				#pragma endregion
				}
				CATCH(CassetteLogger, "");
			}

			void GetPointTime(PGConnection& conn, TCassette& it, T_ForBase_RelFurn* Furn)
			{
				try
				{
				#pragma region ������� ������ � ����

					std::stringstream sss;
					sss << "SELECT DISTINCT ON (id_name) id_name, content";
					sss << ", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)";
					sss << " FROM todos WHERE (";
					sss << " id_name = " << Furn->PointRef_1->ID << " OR";		//������� �����������
					sss << " id_name = " << Furn->PointTime_1->ID << " OR";		//������� ����� �������
					sss << " id_name = " << Furn->PointTime_2->ID << " OR";	//������� ����� ��������
					sss << " id_name = " << Furn->TimeProcSet->ID;				//������ ����� �������� (�������), ���
					sss << ")";	//���� ����� ��������
					sss << " AND create_at < '" << it.End_at << "'";
					sss << " ORDER BY id_name, create_at DESC"; // LIMIT 4

				#pragma endregion

				#pragma region ������ � ����

					std::string command = sss.str();
					PGresult* res = conn.PGexec(command);
					//��������� ������
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						int line = PQntuples(res);
						//��������� ������
						for(int l = 0; l < line && isRun; l++)
						{
							int id = Stoi(conn.PGgetvalue(res, l, 0));
							std::string f = conn.PGgetvalue(res, l, 1);

							if(f.length())
							{
								if(id == Furn->PointRef_1->ID && !it.PointRef_1.length())  it.PointRef_1 = f;
								if(id == Furn->PointTime_1->ID && !it.PointTime_1.length()) it.PointTime_1 = f;
								if(id == Furn->PointTime_2->ID && !it.PointTime_2.length()) it.PointTime_2 = f;
								if(id == Furn->TimeProcSet->ID && !it.TimeProcSet.length()) it.TimeProcSet = f;
							}
						}
					}
					else
					{
						LOG_ERR_SQL(CassetteLogger, res, command);
					}
					PQclear(res);

				#pragma endregion
				}
				CATCH(CassetteLogger, "");
			}

			void GetFactTemper(PGConnection& conn, TCassette& it, T_ForBase_RelFurn* Furn)
			{
				try
				{
					time_t temp  = (time_t)difftime(DataTimeOfString(it.End_at), 5 * 60); //������� 5 ����� �� ����� �������
					std::string End_at  = GetStringOfDataTime(&temp);
					it.facttemper = GetVal(conn, Furn->TempAct->ID, it.Run_at, End_at);

				}
				CATCH(CassetteLogger, "");
			}

			void GetValues(PGConnection& conn, TCassette& it, T_ForBase_RelFurn* Furn)
			{
				GetActTime(conn, it, Furn);
				GetPointTime(conn, it, Furn);
				GetFactTemper(conn, it, Furn);
			}
		
			void EndCassette(PGConnection& conn, TCassette& it, std::fstream& s1)
			{
				try
				{
					T_ForBase_RelFurn* Furn = GetFurn(Petch);

					it.Peth = std::to_string(Petch);

					if(/*S107::IsCassette(it) &&*/ it.Run_at.length())
					{
						GetValues(conn, it, Furn);

						//std::tm TM_Run;
						//std::tm TM_End;
						//std::tm TM_All;
						//std::tm TM_Fin;
						////struct tm TM_All;
						//std::time_t tm_Fin;
						//
						//std::time_t tm_Run = DataTimeOfString(P.Run_at, TM_Run);
						//std::time_t tm_End1;
						////std::time_t tm_End = DataTimeOfString(P.End_at, TM_End);
						////std::time_t tm_All = (time_t)difftime(tm_End1, tm_Run);
						//
						////gmtime_s(&TM_All, &tm_All);
						////TM_All.tm_year -= 1900;
						////TM_All.tm_mday -= 1;
						//
						//tm_Fin = DataTimeOfString(P.End_at, TM_Fin);
						//tm_Fin = tm_End1 + (60 * 15); //+15 �����
						//localtime_s(&TM_Fin, &tm_Fin);
						//
						////P1.End_at
						//P.Finish_at = GetDataTimeString(TM_Fin);
						//
						//std::stringstream sg2;
						S107::GetFinishCassete(conn, CassetteLogger, it);
						{
							s1 << Stoi(it.Id) << ";";
							s1 << " " << it.Run_at << ";";
							s1 << " " << it.End_at << ";";
							s1 << " " << it.Finish_at << ";";
							s1 << Stoi(it.Year) << ";";
							s1 << Stoi(it.Month) << ";";
							s1 << Stoi(it.Day) << ";";
							s1 << Stoi(it.Hour) << ";";
							s1 << Stoi(it.CassetteNo) << ";";
							s1 << Stoi(it.Peth) << ";";
								//<< " " << P.Error_at << ";";

							s1 << Stof(it.PointRef_1) << ";";		//������� �����������
							s1 << Stof(it.facttemper) << ";";		//����������� �������� �����������

							s1 << Stof(it.PointTime_1) << ";";		//������� ����� �������
							s1 << Stof(it.HeatAcc) << ";";			//���� ����� �������
							s1 << Stof(it.PointTime_2) << ";";	//������� ����� ��������
							s1 << Stof(it.HeatWait) << ";";		//���� ����� ��������

							s1 << Stof(it.TimeProcSet) << ";";		//������ ����� �������� (�������), ���
							s1 << Stof(it.Total) << ";";			//���� ����� �����

							s1 << std::endl;
						}


					}
				}
				CATCH(CassetteLogger, "");
			}


			void GetCassetteData(PGConnection& conn, std::string id, TCassette& ct, TCassette& it)
			{
				try
				{
					//02-01-2025 12:48:42
					std::stringstream com;
					com << "SELECT * FROM cassette WHERE";

					if(Stoi(it.Year) > 2024 || (Stoi(it.Year) <= 2024 && Stoi(it.Month) >= 8) )
						com << " hour = " << Stoi(it.Hour);
					else
						com << " hour < 1";;
					com << " AND day = " << Stoi(it.Day);
					com << " AND month = " << Stoi(it.Month);
					com << " AND year = " << Stoi(it.Year);
					com << " AND cassetteno = " << Stoi(it.CassetteNo);
					com << " AND correct IS NULL";
					com << " ORDER BY run_at DESC LIMIT 1";

					std::string command = com.str();
					if(DEB)LOG_INFO(CassetteLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
					PGresult* res = conn.PGexec(command);
					//LOG_INFO(CassetteLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						if(PQntuples(res))
							S107::GetCassette(conn, res, ct, 0);
					} else
						LOG_ERR_SQL(CassetteLogger, res, command);
					PQclear(res);
				}
				CATCH(CassetteLogger, "");
			}


			void SaveFileSdg(MapRunn& CassetteTodos)
			{
	#ifdef _DEBUG
				try
				{
	#pragma region ������ � ���� ��������� Sdg.csv
					std::fstream sdg("Sdg_" + std::to_string(Petch) + ".csv", std::fstream::binary | std::fstream::out);

					sdg << "first;";
					sdg << "id;";
					sdg << "create_at;";
					sdg << "id_name;";
					sdg << "value;";
					sdg << "Petch;";
					sdg << "id_name;";
					sdg << "id_name_at;";
					sdg << std::endl;
	#pragma endregion

	#pragma region ������ � ���� ���� Sdg.csv
					int index = 0;
					for(auto& a : CassetteTodos)
					{
						sdg << a.first << ";";
						sdg << a.second.id << ";";
						sdg << " " << a.second.create_at << ";";
						sdg << a.second.id_name << ";";
						sdg << a.second.value << ";";
						sdg << a.second.Petch << ";";
						sdg << a.second.id_name << ";";
						sdg << a.second.id_name_at << ";";
						sdg << std::endl;
					}
					sdg.close();
	#pragma endregion
				}
				CATCH(CassetteLogger, "");
	#endif
			}


			int GetCountSheet(PGConnection& conn, TCassette& ct)
			{
				int countSheet = 0;
				std::stringstream set;
				set << "SELECT COUNT(*) FROM sheet WHERE"; //delete_at IS NULL AND 
				set << " year = '" << Stoi(ct.Year) << "'";
				set << " AND month = '" << Stoi(ct.Month) << "'";
				set << " AND day = '" << Stoi(ct.Day) << "'";
				set << " AND cassetteno = " << Stoi(ct.CassetteNo);;
				if(Stoi(ct.Year) > 2024 || (Stoi(ct.Year) == 2024 && Stoi(ct.Month) >= 8) )
					set << " AND hour = " << Stoi(ct.Hour);

				std::string command = set.str();
				PGresult* res = conn.PGexec(command);
				if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
					countSheet = Stoi(conn.PGgetvalue(res, 0, 0));
				PQclear(res);
				return countSheet;
			}


			void SaveDataBaseForId(PGConnection& conn, TCassette& ct, TCassette& it)
			{
				std::stringstream sg2;
				try
				{
					int countSheet = GetCountSheet(conn, it);
					if(!countSheet) countSheet = -1;

					if(it.Create_at.length())ct.Create_at = it.Create_at;
					else
						if(it.Run_at.length())ct.Create_at = it.Run_at;

					if(it.Id.length())ct.Id = it.Id;
					if(it.Year.length())ct.Year = it.Year;
					if(it.Month.length())ct.Month =it.Month;
					if(it.Day.length())ct.Day = it.Day;

					if(Stoi(it.Year) > 2024 || (Stoi(it.Year) == 2024 && Stoi(it.Month) >= 8) )
						ct.Hour = it.Hour;
					else
						ct.Hour = "-1";

					if(it.CassetteNo.length())ct.CassetteNo = it.CassetteNo;
					if(it.Peth.length())ct.Peth = it.Peth;
					if(it.Run_at.length())ct.Run_at = it.Run_at;
					if(it.Error_at.length())ct.Error_at = it.Error_at;
					if(it.End_at.length())ct.End_at = it.End_at;
					if(it.PointTime_1.length())ct.PointTime_1 = it.PointTime_1;    //����� �������
					if(it.PointRef_1.length())ct.PointRef_1 = it.PointRef_1;      //������� �����������
					if(it.TimeProcSet.length())ct.TimeProcSet = it.TimeProcSet;    //������ ����� �������� (�������), ���
					if(it.PointTime_2.length())ct.PointTime_2 =it.PointTime_2;   //����� ��������
					if(it.facttemper.length())ct.facttemper = it.facttemper;			//���� ����������� �� 5 ����� �� ����� �������
					if(it.Finish_at.length())ct.Finish_at = it.Finish_at;        //���������� �������� + 15 �����
					if(it.HeatAcc.length())ct.HeatAcc = it.HeatAcc;			//���� ����� �������
					if(it.HeatWait.length())ct.HeatWait = it.HeatWait;          //���� ����� ��������
					if(it.Total.length())ct.Total = it.Total;				//���� ����� �����

					std::stringstream ssd;
					ssd << "UPDATE cassette SET ";
					ssd << "";

					if(ct.Run_at.length())
						ssd << "run_at = '" << ct.Run_at << "', ";
					else
						ssd << "run_at = DEFAULT, ";

					if(ct.End_at.length())
						ssd << "end_at = '" << ct.End_at << "', ";
					else
						ssd << "end_at = DEFAULT, ";

					if(ct.Finish_at.length())
						ssd << "finish_at = '" << ct.Finish_at << "', correct = now()";
					else
						ssd << "finish_at = DEFAULT, correct = now()";


					if(Stof(ct.PointRef_1))
						ssd << ", pointref_1 = " << ct.PointRef_1;

					if(Stof(ct.facttemper))
						ssd << ", facttemper = " << ct.facttemper; //���� ����������� �� 5 ����� �� ����� �������


					if(Stof(ct.PointTime_1))
						ssd << ", pointtime_1 = " << ct.PointTime_1;
					if(Stof(ct.HeatAcc))
						ssd << ", heatacc = " << ct.HeatAcc;


					if(Stof(ct.PointTime_2))
						ssd << ", pointdtime_2 = " << ct.PointTime_2;
					if(Stof(ct.HeatWait))
						ssd << ", heatwait = " << ct.HeatWait;

					if(Stof(ct.TimeProcSet))
						ssd << ", timeprocset = " << ct.TimeProcSet;
					if(Stof(ct.Total))
						ssd << ", total = " << ct.Total;
				
					if(countSheet)
						ssd << ", sheetincassette = " << countSheet;
					else
						ssd << ", sheetincassette = -1";

					if(ct.Peth.length())
						ssd << ", peth = " << ct.Peth;

					ssd << " WHERE id = " << ct.Id;
					std::string command = ssd.str();

					LOG_INFO(CassetteLogger, "{:89}| {}", FUNCTION_LINE_NAME, ssd.str());
					SetWindowText(hWndDebug, command.c_str());
					SETUPDATESQL(CassetteLogger, conn, ssd);
					

					fUpdateCassette << ssd.str() << std::endl;
					fUpdateCassette.flush();

					//PrintCassettePdfAuto(ct);
				}
				CATCH(CassetteLogger, "")
			}


			void SaveDataBaseNotId(PGConnection& conn, TCassette& ct, TCassette& it)
			{
				try
				{
					int countSheet = GetCountSheet(conn, it);
					if(!countSheet) countSheet = -1;

					if(it.Create_at.length())ct.Create_at = it.Create_at;
					else
						if(it.Run_at.length())ct.Create_at = it.Run_at;
																													
					if(it.Id.length())ct.Id = it.Id;
					if(it.Year.length())ct.Year = it.Year;
					if(it.Month.length())ct.Month =it.Month;
					if(it.Day.length())ct.Day = it.Day;

					if(Stoi(it.Year) > 2024 || (Stoi(it.Year) == 2024 && Stoi(it.Month) >= 8) )
						ct.Hour = it.Hour;
					else
						ct.Hour = "-1";

					if(it.CassetteNo.length())ct.CassetteNo = it.CassetteNo;

					if(it.Peth.length())ct.Peth = it.Peth;
					if(it.Run_at.length())ct.Run_at = it.Run_at;
					if(it.Error_at.length())ct.Error_at = it.Error_at;
					if(it.End_at.length())ct.End_at = it.End_at;

					if(it.PointTime_1.length())ct.PointTime_1 = it.PointTime_1;    //����� �������
					if(it.PointRef_1.length())ct.PointRef_1 = it.PointRef_1;      //������� �����������
					if(it.TimeProcSet.length())ct.TimeProcSet = it.TimeProcSet;    //������ ����� �������� (�������), ���
					if(it.PointTime_2.length())ct.PointTime_2 =it.PointTime_2;   //����� ��������
					if(it.facttemper.length())ct.facttemper = it.facttemper;			//���� ����������� �� 5 ����� �� ����� �������
					if(it.Finish_at.length())ct.Finish_at = it.Finish_at;        //���������� �������� + 15 �����
					if(it.HeatAcc.length())ct.HeatAcc = it.HeatAcc;			//���� ����� �������
					if(it.HeatWait.length())ct.HeatWait = it.HeatWait;          //���� ����� ��������
					if(it.Total.length())ct.Total = it.Total;				//���� ����� �����

					{
						std::stringstream ssd;
						ssd << "INSERT INTO cassette ";
						ssd << "(";
						ssd << "create_at, ";
						ssd << "event, ";
						ssd << "year, ";
						ssd << "month, ";
						ssd << "day, ";
						ssd << "hour, ";
						ssd << "cassetteno, ";
						ssd << "sheetincassette, ";
						ssd << "peth, ";
						ssd << "run_at, ";
						ssd << "error_at, ";
						ssd << "end_at, ";
						ssd << "finish_at, ";
						ssd << "pointtime_1, ";
						ssd << "pointref_1, ";
						ssd << "timeprocset, ";
						ssd << "pointdtime_2, ";
						ssd << "facttemper, ";
						ssd << "heatacc, ";
						ssd << "heatwait, ";
						ssd << "total, ";
						ssd << "correct";
						ssd << ") VALUES (";

						if(ct.Create_at.length())
							ssd << "'" << ct.Create_at << "', ";
						else
							ssd << "now(), ";

						ssd << "5, ";
						ssd << Stoi(ct.Year) << ", ";
						ssd << Stoi(ct.Month) << ", ";
						ssd << Stoi(ct.Day) << ", ";
						ssd << Stoi(ct.Hour) << ", ";
						ssd << Stoi(ct.CassetteNo) << ", ";

						ssd << countSheet << ", ";

						ssd << Stoi(ct.Peth) << ", ";

						if(ct.Run_at.length())
							ssd << "'" << ct.Run_at << "', ";
						else
							ssd << "DEFAULT, ";

						if(ct.Error_at.length())
							ssd << "'" << ct.Error_at << "', ";
						else
							ssd << "DEFAULT, ";

						if(ct.End_at.length())
							ssd << "'" << ct.End_at << "', ";
						else
							ssd << "DEFAULT, ";
						if(ct.Finish_at.length())
							ssd << "'" << ct.Finish_at << "', ";
						else
							ssd << "DEFAULT, ";

						ssd << Stof(ct.PointTime_1) << ", ";
						ssd << Stof(ct.PointRef_1) << ", ";
						ssd << Stof(ct.TimeProcSet) << ", ";
						ssd << Stof(ct.PointTime_2) << ", ";
						ssd << Stof(ct.facttemper) << ", ";
						ssd << Stof(ct.HeatAcc) << ", ";
						ssd << Stof(ct.HeatWait) << ", ";
						ssd << Stof(ct.Total) << ", ";
						ssd << "now());";
						std::string command = ssd.str();
						LOG_INFO(CassetteLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
						SetWindowText(hWndDebug, command.c_str());
						SETUPDATESQL(CassetteLogger, conn, ssd);

						fUpdateCassette << ssd.str();
						fUpdateCassette << "\t" << ct.Id << std::endl;
						fUpdateCassette.flush();
					}

					{
						std::stringstream ssd;
						ssd << "SELECT id FROM cassette ";
						ssd << "WHERE hour = " << ct.Hour;
						ssd << " AND day = " << ct.Day;
						ssd << " AND month = " << ct.Month;
						ssd << " AND year = " << ct.Year;
						ssd << " AND cassetteno = " << ct.CassetteNo;
						ssd << " ORDER BY id LIMIT 1";
						//command = ssg.str();

						PGresult* res = conn.PGexec(ssd.str());
						if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
						{
							it.Id = conn.PGgetvalue(res, 0, 0);
						}
						else
						{
							if(PQresultStatus(res) == PGRES_FATAL_ERROR)
								LOG_ERR_SQL(CassetteLogger, res, ssd.str());
						}

						PQclear(res);
						ct.Id = it.Id;
					}
					//PrintCassettePdfAuto(ct);
				}
				CATCH(CassetteLogger, "");
			}

			void SaveDataBase(PGConnection& conn, TCassette& ct, TCassette& it)
			{
				try
				{
					//������ � ����
					std::stringstream ssg;
					ssg << "SELECT id FROM cassette ";
					ssg << "WHERE";
					if(Stoi(it.Year) > 2024 || (Stoi(it.Year) == 2024 && Stoi(it.Month) >= 8) )
						ssg << " hour = " << Stoi(it.Hour) << " AND";

					ssg << " day = " << Stoi(it.Day);
					ssg << " AND month = " << Stoi(it.Month);
					ssg << " AND year = " << Stoi(it.Year);
					ssg << " AND cassetteno = " << Stoi(it.CassetteNo);
					ssg << " ORDER BY id LIMIT 1";
					std::string command = ssg.str();

					if(DEB)LOG_INFO(CassetteLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
					PGresult* res = conn.PGexec(command);
					if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
						it.Id = conn.PGgetvalue(res, 0, 0);
					PQclear(res);

					if(Stoi(it.Id))
					{
						SaveDataBaseForId(conn, ct, it);
					}
					else
					{
						SaveDataBaseNotId(conn, ct, it);
					}
				}
				CATCH(CassetteLogger, "");
			}

			void SaveFileCass1(TCASS& P0)
			{

		//#ifdef _DEBUG
				try
				{
	#pragma region ������ � ���� Cass.csv ���������
					std::fstream s1("Cass.csv", std::fstream::binary | std::fstream::out);
					s1 << "������;"
						<< "�����;"
						<< "����;"
						<< "�����;"
						<< "���;"
						<< "�����;"
						<< "����;"
						<< "�������;"
						<< "������;"
						<< "����;"
						<< "������� T;"			//������� �����������
						<< "���� �;"			//����������� �������� �����������
						<< "������� �������;"	//������� ����� �������
						<< "���� �������;"		//���� ����� �������
						<< "������� ��������;"	//������� ����� ��������
						<< "���� ��������;"		//���� ����� ��������
						<< "������� ��������;"	//������ ����� �������� (�������), ���
						<< "���� ��������;"		//���� ����� �����
						<< std::endl;
	#pragma endregion

	#pragma region ������ � ���� Cass.csv ����
					for(auto& it : P0)
					{
						if(!isRun)return;
						if(it.second.Run_at.length() && it.second.End_at.length() && S107::IsCassette(it.second))
						{
							s1 << it.first << ";"
								<< " " << it.second.Run_at << ";"
								<< " " << it.second.End_at << ";"
								<< " " << it.second.Finish_at << ";"
								<< it.second.Year << ";"
								<< it.second.Month << ";"
								<< it.second.Day << ";"
								<< it.second.Hour << ";"
								<< it.second.CassetteNo << ";"
								<< " " << it.second.Error_at << ";"
								<< it.second.Peth << ";"
								<< it.second.PointRef_1 << ";"		//������� �����������
								<< it.second.facttemper << ";"		//����������� �������� �����������
								<< it.second.PointTime_1 << ";"		//������� ����� �������
								<< it.second.HeatAcc << ";"			//���� ����� �������
								<< it.second.PointTime_2 << ";"	//������� ����� ��������
								<< it.second.HeatWait << ";"		//���� ����� ��������
								<< it.second.TimeProcSet << ";"		//������ ����� �������� (�������), ���
								<< it.second.Total << ";"			//���� ����� �����

								<< std::endl;
						}
					}
					s1.close();
	#pragma endregion
				}
				CATCH(CassetteLogger, "");
		//#endif
			}


			void SaveBaseCassete(PGConnection& conn, TCassette& it)
			{
				try
				{
					TCassette ct;
					if(it.Run_at.length() && it.End_at.length() && S107::IsCassette(it))
					{
						//std::string sg = std::string("��������� ������� �� ���� ") + std::to_string(P0.size()) + std::string(" :") + std::to_string(lin++);
						//SetWindowText(hWndDebug, sg.c_str());

						GetCassetteData(conn, it.Id, ct, it);

						if(ct.facttemper.length())	it.facttemper = ct.facttemper;
						if(ct.Id.length())			it.Id = ct.Id;

						TCassette ct1;
						if(S107::GetFinishCassete(conn, CassetteLogger, it))
							SaveDataBase(conn, ct1, it);
					}
				}CATCH(CassetteLogger, "");
			}

			template <class T>
			bool IsCassette(PGConnection& conn, T& p)
			{
				int Year = 0;
				int Day = 0;
				int Month = 0;
				int CassetteNo = 0;

				try{
					Year = Stoi(p.Year);
					Day = Stoi(p.Day);
					Month = Stoi(p.Month);
					CassetteNo = Stoi(p.CassetteNo);

					if(!Year || !Month || !Day || !CassetteNo)
					{
						std::string ssa = "SELECT year, month, day, hour, cassetteno FROM cassette WHERE id = " + std::to_string(Id);

						PGresult* res = conn.PGexec(ssa);
						if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
						{
							p.Year = conn.PGgetvalue(res, 0, 0);
							p.Month = conn.PGgetvalue(res, 0, 1);
							p.Day = conn.PGgetvalue(res, 0, 2);
							p.Hour = conn.PGgetvalue(res, 0, 3);
							p.CassetteNo = conn.PGgetvalue(res, 0, 4);

							Year = Stoi(p.Year);
							Month = Stoi(p.Month);
							Day = Stoi(p.Day);
							CassetteNo = Stoi(p.CassetteNo);
						} 
						else
						{
							if(PQresultStatus(res) == PGRES_FATAL_ERROR)
								LOG_ERR_SQL(CassetteLogger, res, ssa);
						}

						PQclear(res);
					}
				}
				CATCH(CassetteLogger, "");

				return (Day && Month && Year && CassetteNo);
			}

			void PrepareDataBase(PGConnection& conn, Tcass& cass, TCassette& P, T_Todos& a, int Petch, std::fstream& s1)
			{
				try
				{
					T_ForBase_RelFurn* Furn = GetFurn(Petch);

					//Furn->Cassette.Hour->ID
					if(a.id_name == Furn->Cassette.Hour->ID)
					{
						if(!cass.Run)
							cass.Hour = a.value;
					}

					//Furn->Cassette.Day->ID
					if(a.id_name == Furn->Cassette.Day->ID && a.content.As<int32_t>())
					{
						if(!cass.Run)
							cass.Day = a.value;
					}

					//Furn->Cassette.Month->ID
					if(a.id_name == Furn->Cassette.Month->ID && a.content.As<int32_t>())
					{
						if(!cass.Run)
							cass.Month = a.value;
					}

					//Furn->Cassette.Year->ID
					if(a.id_name == Furn->Cassette.Year->ID && a.content.As<int32_t>())
					{
						if(!cass.Run)
							cass.Year = a.value;
					}

					//Furn->Cassette.CassetteNo->ID
					if(a.id_name == Furn->Cassette.CassetteNo->ID && a.content.As<int32_t>())
					{
						if(!cass.Run)
							cass.CassetteNo = a.value;
					}

					//Furn->ProcRun->ID
					if(a.id_name == Furn->ProcRun->ID && a.content.As<bool>())
					{
						cass.Run = a.content.As<bool>();

						if(IsCassette(conn, cass))
						{
							if(!cass.Run_at.length())
							{
								cass.Run_at = a.create_at;
								P = cass;
							} 
							else
							{
								if(
									IsCassette(conn, P) &&
									(
										//Stoi(P.Hour) != Stoi(cass.Hour) ||
										Stoi(P.Day) != Stoi(cass.Day) ||
										Stoi(P.Month) != Stoi(cass.Month) ||
										Stoi(P.Year) != Stoi(cass.Year) ||
										Stoi(P.CassetteNo) != Stoi(cass.CassetteNo)
										)
									)
								{
									//P = cass;
									P.End_at = a.create_at;
									EndCassette(conn, P, s1);
									SaveBaseCassete(conn, P);
							
									P = TCassette();
									P = cass;
								}
							}
						}
					}

					//Furn->ProcEnd->ID
					if(a.id_name == Furn->ProcEnd->ID && a.content.As<bool>())
					{
						if(IsCassette(conn, cass) && cass.Run_at.size())
						{
							P = cass;
							P.End_at = a.create_at;
							EndCassette(conn, P, s1);
							SaveBaseCassete(conn, P);
						}
						P = TCassette();
						cass = Tcass(Petch);
					}

					//Furn->ProcFault->ID
					if(a.id_name == Furn->ProcFault->ID && a.content.As<bool>())
					{
						if(!P.Error_at.length() && S107::IsCassette(P))
						{
							P.Error_at = a.create_at;
						}
					}


				}
				CATCH(CassetteLogger, "");
			}

			void GetCassetDataBase(PGConnection& conn, MapRunn& CassetteTodos, int Petch)
			{
				try
				{
					T_ForBase_RelFurn* Furn = GetFurn(Petch);

					if(!DateStart.length())
					{
						return;
					}

					{
						std::stringstream ssd;
						ssd << "SELECT create_at, id, id_name, content";
						ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
						ssd << ", (SELECT comment AS name FROM tag WHERE tag.id = todos.id_name) ";
						ssd << "FROM todos WHERE ";
						ssd << "create_at >= '" << DateStart << "' AND ";

						if(DateStop.length())
							ssd << "create_at < '" << DateStop << "' AND ";

						ssd << "(id_name = " << Furn->ProcEnd->ID;
						ssd << " OR id_name = " << Furn->ProcRun->ID;
						ssd << " OR id_name = " << Furn->ProcFault->ID;

						ssd << " OR id_name = " << Furn->Cassette.Hour->ID ;
						ssd << " OR (id_name = " << Furn->Cassette.Day->ID  << " AND content <> '0')";
						ssd << " OR (id_name = " << Furn->Cassette.Month->ID  << " AND content <> '0')";
						ssd << " OR (id_name = " << Furn->Cassette.Year->ID  << " AND content <> '0')";
						ssd << " OR (id_name = " << Furn->Cassette.CassetteNo->ID  << " AND content <> '0')";
						ssd << ") ORDER BY create_at ASC"; //AND content <> 'false' AND content <> '0' 

						std::string command = ssd.str();
						//GetTodosSQL(conn, CassetteTodos, command, Petch);


						PGresult* res = conn.PGexec(command);
						if(PQresultStatus(res) == PGRES_TUPLES_OK)
						{
							TodosColumn(res);
						
							int line = PQntuples(res);
							std::string old_value = "";
							for(int l = 0; l < line && isRun; l++)
							{
								T_Todos ct;
								ct.value = conn.PGgetvalue(res, l, TODOS::content);
								ct.create_at = conn.PGgetvalue(res, l, TODOS::create_at);
								ct.id = Stoi(conn.PGgetvalue(res, l, TODOS::id));
								ct.id_name = Stoi(conn.PGgetvalue(res, l, TODOS::id_name));
								ct.type =  Stoi(conn.PGgetvalue(res, l, TODOS::type));
								ct.content = PDF::GetVarVariant((OpcUa::VariantType)ct.type, ct.value);
								ct.id_name_at = conn.PGgetvalue(res, l, TODOS::name);
								ct.Petch = Petch;

								CassetteTodos[ct.create_at] = ct;
								//CassetteTodos.push_back(ct);
							}
						}
						else
						{
							LOG_ERR_SQL(CassetteLogger, res, command);
						}
						PQclear(res);

					}
				}
				CATCH(CassetteLogger, "");

				SaveFileSdg(CassetteTodos);
			}

			void SetAllCorrect(PGConnection& conn, std::string Date, int Petch)
			{
				std::stringstream as;
				as << "UPDATE cassette SET correct = now() WHERE ";
				as << " peth = " << Petch;
				if(Date.length())
				as << " AND run_at >= '" << Date << "'";
				as << " AND delete_at IS NULL";
				as << " AND correct IS NULL";
				as << " AND finish_at IS NOT NULL";
				as << " AND end_at IS NOT NULL";
				as << " AND run_at IS NOT NULL";
				//as << " AND sheetincassette > 0";
				SETUPDATESQL(CassetteLogger, conn, as);
			}
			void GetCassette(PGConnection& conn)
			{
				try
				{
					std::string name = "cass 1_2.csv";
					std::fstream s1(name, std::fstream::binary | std::fstream::out | std::fstream::app);
					MapRunn CassetteTodos;
					Tcass TP = Tcass(Petch);
					TCassette P;
					GetCassetDataBase(conn, CassetteTodos, Petch);

					for(auto& a1 : CassetteTodos)
					{
						if(!isRun)break;
						if(a1.second.Petch == Petch)
							PrepareDataBase(conn, TP, P, a1.second, Petch, s1);
					}
					SetAllCorrect(conn, DateStart, Petch);

					s1.close();
				}
				CATCH(CassetteLogger, "");
			}


			GetCassettes(PGConnection& conn, int id, int petch, std::string start, std::string stop)
			{
				InitLogger(CassetteLogger);
				try
				{

					Id = id;
					Petch = petch;
					DateStart = start;
					DateStop = stop;

					remove("Cass.csv");
					remove("cass 1_2.csv");
					remove("Sdg.csv");
					remove("all_tag.csv");

					remove("UpdateCassette.txt");
					fUpdateCassette = std::fstream("UpdateCassette.txt", std::fstream::binary | std::fstream::out | std::ios::app);

					GetCassette(conn);
					fUpdateCassette.close();


					SetWindowText(hWndDebug, "�������� ����������� ���������");
				}
				CATCH(CassetteLogger, "");

			}

		};

		void CorreatStart(PGConnection& conn, int Peth, std::string& Start)
		{
				T_ForBase_RelFurn* Furn = GetFurn(Peth);

				std::stringstream sss;
				sss << "SELECT DISTINCT ON (id_name) id_name, create_at";
				//sss << ", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)";
				sss << " FROM todos WHERE";
				sss << " id_name = " << Furn->ProcEnd->ID;
				sss << " AND create_at <= '" << Start << "'";
				sss << " AND content <> 'false'";
				sss << " ORDER BY id_name, create_at DESC"; // LIMIT 3

				std::string command = sss.str();
				PGresult* res = conn.PGexec(command);
				//��������� ������
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					//��������� ������
					for(int l = 0; l < line && isRun; l++)
					{
						int id = Stoi(conn.PGgetvalue(res, l, 0));
						std::string f = conn.PGgetvalue(res, l, 1);

						if(f.length())
						{
							if(id == Furn->ProcEnd->ID) Start = f;
						}
					}
				} else
				{
					LOG_ERR_SQL(CassetteLogger, res, command);
				}
				PQclear(res);
		}

		void CorreatStop(PGConnection& conn, int Peth, std::string& Stop)
		{
				T_ForBase_RelFurn* Furn = GetFurn(Peth);

				std::stringstream sss;
				sss << "SELECT DISTINCT ON (id_name) id_name, create_at + INTERVAL '15 min'";
				//sss << ", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)";
				sss << " FROM todos WHERE";
				sss << " id_name = " << Furn->ProcEnd->ID;
				sss << " AND create_at >= '" << Stop << "'";
				sss << " AND content = 'true'";
				sss << " ORDER BY id_name, create_at ASC"; // LIMIT 3

				std::string command = sss.str();
				PGresult* res = conn.PGexec(command);
				//��������� ������
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					//��������� ������
					for(int l = 0; l < line && isRun; l++)
					{
						int id = Stoi(conn.PGgetvalue(res, l, 0));
						std::string f = conn.PGgetvalue(res, l, 1);

						if(f.length())
						{
							if(id == Furn->ProcEnd->ID) Stop = f;
						}
					}
				} else
				{
					LOG_ERR_SQL(CassetteLogger, res, command);
				}
				PQclear(res);
		}
		int GetStartTime(PGConnection& conn, int& Peth, std::string& Start, std::string& Stop)
		{
			Start = "";
			Stop = "";
			Peth = 0;

			std::string id = "";
			//std::string start = "";
			//std::string stop = "";
			std::string peth = "";
			std::stringstream ass;
			//ass << "SELECT create_at WHERE create_at";
			std::stringstream ss1;
			ss1 << "SELECT id, peth, run_at FROM cassette WHERE "
				"correct IS NULL AND "
				"run_at IS NOT NULL AND "
				"finish_at IS NOT NULL AND "
				"delete_at IS NULL AND "
				"CAST(event AS integer) = 5 "; //delete_at IS NULL AND 
			ss1 << "AND peth <> 0";
			ss1 << " ORDER BY run_at ASC LIMIT 1";

			std::string command = ss1.str();
			PGresult* res = conn.PGexec(command);
			if(PQresultStatus(res) == PGRES_TUPLES_OK)
			{
				if(conn.PQntuples(res))
				{
					id =  conn.PGgetvalue(res, 0, 0);
					peth = conn.PGgetvalue(res, 0, 1);
					Start = conn.PGgetvalue(res, 0, 2);
					Stop = Start;
				}
			} else
			{
				LOG_ERR_SQL(CassetteLogger, res, command);
			}
			PQclear(res);

			Peth = Stoi(peth);
			if(Start.length() && Stoi(id) && Peth)
			{

				CorreatStart(conn, Peth, Start);
				CorreatStop(conn, Peth, Stop);
			}
			return Stoi(id);
		}
	};

	namespace SHEET
	{

		class GetSheets
		{
		public:
			//GenSeqToHmi.Seq_1_StateNo
			const int st1_3 = 3;	//������� ������� �����
			const int st1_4 = 4;	//�������� � ����
			const int st1_5 = 5;	//������� ������� �����
			const int st1_6 = 6;	//������ �����
			const int st1_7 = 7;	//�������� �� 2 ��������
			const int st1_8 = 8;	//�������� �� 2-� �������� ����

			//GenSeqToHmi.Seq_2_StateNo
			const int st2_3 = 3;	//����� ��������� � 1-�� ��������� ����
			const int st2_4 = 4;	//���������.������ �����
			const int st2_5 = 5;	//������� �������� �����
			const int st2_6 = 6;	//������ � ����� �������
			const int st2_7 = 7;	//������� �������� �����

			//GenSeqToHmi.Seq_3_StateNo
			const int st3_1 = 1;	//�������� ���������
			//const int st3_3 = 3;	//������ ���������
			const int st3_4 = 4;	//��������� ����� ���������

			int AllId = 0;
			int iAllId = 0;
			int ID = 0;

			std::string StartSheet = "";
			std::string StopSheet = "";


			V_Todos allTime;

			T_IdSheet Ids1;
			T_IdSheet Ids2;
			T_IdSheet Ids3;

			T_IdSheet Ids5;

			V_IdSheet Ids6;

			std::fstream ss1;
			std::fstream fUpdateSheet;


			//void SaveBodyCsv(std::fstream& s1, T_IdSheet& ids, std::string Error);
			//std::fstream SaveHeadCsv(std::string name);
			//bool isSheet(T_IdSheet& t);
			//
			//T_IdSheet GetIdSheet(PGConnection& conn, std::string Start, size_t count, size_t i);
			//
			//float GetTime(std::string Start, std::vector<T_Todos>& allTime);
			//
			//void GetAllTime(PGConnection& conn, std::vector<T_Todos>& allTime);
			//
			//void GetMask(uint16_t v, std::string& MaskV);
			//
			////��������� �� ������ ����� � ���� State_1 = 3;
			//void GetDataSheet1(PGConnection& conn, T_IdSheet& ids);
			//
			////��������� �� ������ ����� � ���� State_2 = 5;
			//void GetDataSheet2(PGConnection& conn, T_IdSheet& ids);
			//
			////��������� �� ������ ����� � ���� State_2 = 5 ���� 5 ������;
			//void GetDataSheet3(PGConnection& conn, T_IdSheet& ids);
			//
			//void GetSeq_1(PGConnection& conn, MapTodos& allSheetTodos);
			//void GetSeq_2(PGConnection& conn, MapTodos& allSheetTodos);
			//void GetSeq_3(PGConnection& conn, MapTodos& allSheetTodos);
			//void GetSeq_6(PGConnection& conn, MapTodos& allSheetTodos);
			//
			//float GetDataTime_All(std::string TimeStart, std::string Start3);
			//
			//void Get_ID_S(PGConnection& conn, T_IdSheet& td);
			//int Get_ID_C(PGConnection& conn, T_IdSheet& td);
			//void GetID(PGConnection& conn, T_IdSheet& td);
			//
			//void SetCountSheetInCassette(PGConnection& conn, T_IdSheet& td);
			//	
			//void GetTemperatute(PGConnection& conn, T_IdSheet& td);
			//void UpdateSheet(PGConnection& conn, T_IdSheet& td);
			//
			//bool InZone1(PGConnection& conn, std::string create_at, size_t count, size_t i);
			//bool InZone2(PGConnection& conn, std::string create_at);
			//bool InZone3(PGConnection& conn, std::string create_at);
			//bool InZone5(PGConnection& conn, std::string create_at);
			//
			//void InZone62(PGConnection& conn, T_IdSheet& ids, std::string create_at);
			//void InZone6(PGConnection& conn, std::string create_at);
			//
			//std::fstream SaveStepHeadCsv(std::string name);
			//void SaveStepBodyCsv(std::fstream& ss1, T_Todos& td);
			//
			//std::fstream SaveT_IdSheetHeadCsv(std::string name);
			//void SaveT_IdSheetBodyCsv(std::fstream& ss1, T_IdSheet& ids);
			//void GetSheetCassette(PGConnection& conn, T_IdSheet& ids);
			//
			//GetSheets(PGConnection& conn, std::string datestart, std::string datestop = "");


			void SaveBodyCsv(std::fstream& s1, T_IdSheet& ids, std::string Error)
			{
				try
				{
					std::stringstream ssd;

					ssd << ids.id << ";";

					ssd << " " << GetStringData(ids.Start1) << ";";
					ssd << " " << GetStringData(ids.Start2) << ";";
					ssd << " " << GetStringData(ids.Start3) << ";";
					ssd << " " << GetStringData(ids.Start5) << ";";


					ssd << " " << GetStringData(ids.DataTime_End) << ";";
					ssd << " " << GetStringData(ids.InCant) << ";";
					ssd << " " << GetStringData(ids.Cant) << ";";
					//ssd << " " << GetStringData(ids.Start4) << ";";

					ssd << std::fixed << std::setprecision(1) << ids.DataTime_All << ";";
					ssd << std::fixed << std::setprecision(1) << ids.TimeForPlateHeat << ";";
					ssd << " " << GetStringData(ids.DataTime) << ";";

					ssd << ids.Alloy << ";";
					ssd << ids.Thikness << ";";

					ssd << ids.Melt << ";";
					ssd << ids.PartNo << ";";
					ssd << ids.Pack << ";";
					ssd << ids.Slab << ";";
					ssd << ids.Sheet << ";";
					ssd << ids.SubSheet << ";";

					ssd << std::fixed << std::setprecision(1) << ids.Temper << ";";
					ssd << std::fixed << std::setprecision(1) << ids.Speed << ";";
					ssd << std::fixed << std::setprecision(1) << ids.Za_PT3 << ";";
					ssd << std::fixed << std::setprecision(1) << ids.Za_TE3 << ";";
					ssd << std::fixed << std::setprecision(1) << ids.LaminPressTop << ";";
					ssd << std::fixed << std::setprecision(1) << ids.LaminPressBot << ";";
					ssd << std::fixed << std::setprecision(1) << ids.SpeedTopSet << ";";
					ssd << std::fixed << std::setprecision(1) << ids.SpeedBotSet << ";";
					ssd << "\'" << ids.Mask << "\';";
					ssd << std::fixed << std::setprecision(1) << ids.Lam1PosClapanTop << ";";
					ssd << std::fixed << std::setprecision(1) << ids.Lam1PosClapanBot << ";";
					ssd << std::fixed << std::setprecision(1) << ids.Lam2PosClapanTop << ";";
					ssd << std::fixed << std::setprecision(1) << ids.Lam2PosClapanBot << ";";
					ssd << std::fixed << std::setprecision(1) << ids.LAM_TE1 << ";";
					ssd << std::fixed << std::setprecision(1) << ids.Temperature << ";";

					ssd << ids.year << ";";
					ssd << ids.month << ";";
					ssd << ids.day << ";";
					ssd << ids.cassetteno << ";";
					ssd << ids.sheetincassette << ";";
					ssd << ids.Pos << ";";

					ssd << Error << ";";
					ssd << " " << GetStringData(ids.TimeTest) << ";";


					//ssd << std::fixed << std::setprecision(1) << ids.TempSet1 << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.UnloadSpeed << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.LaminPressTop << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.LaminPressBot << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.SpeedTopSet << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.SpeedBotSet << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.LAM1_TopSet << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.LAM1_BotSet << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.LAM2_TopSet << ";";
					//ssd << std::fixed << std::setprecision(1) << ids.LAM2_BotSet << ";";


					std::string Time = ssd.str();

					boost::replace_all(Time, ".", ",");
					s1 << Time;
					s1 << std::endl;
				}CATCH(SheetLogger, "");
			}

			std::fstream SaveHeadCsv(std::string name)
			{
				std::fstream ss1(name, std::fstream::binary | std::fstream::out);
				try
				{

					//����, ����� �������� ����� � ���������� ����
					//����� �����
					//������� �����, ��
					//������
					//������
					//�����
					//����� �����
					//�������� �����������
					//�������� ������, ��/�
					//
					//���������� ������. �������� ���� � ����
					//���������� ������. ����������� ���� � ����
					//���������� ������. �������� � ������� ����������
					//���������� ������. �������� ������ ����������
					//���������� ������. ��������� ������� ����
					//���������� ������. ��������� ������� ���
					// 
					//���������� ������1  ��������� ������� ����
					//���������� ������1  ��������� ������� ���
					//���������� ������2  ��������� ������� ����
					//���������� ������2  ��������� ������� ���
					//����������� ���� � �������

					ss1
						<< "Id;"
						<< "� ���� 1;"
						<< "� ���� 2;"
						<< "� ����������;"
						<< "� ��������;"
						<< "DataTime_End;"
						<< "� ��������;"
						<< "��������;"

						<< "DataTime_All;"
						<< "TimeForPlateHeat;"
						<< "TimeStart;"

						<< "�����;"
						<< "�������;"
						<< "������;"
						<< "������;"
						<< "�����;"
						<< "����;"
						<< "����;"
						<< "�������;"
						<< "TempSet1;"
						<< "UnloadSpeed;"
						<< "Za_PT3;"
						<< "Za_TE3;"
						<< "LaminPressTop;"
						<< "LaminPressBot;"
						<< "SpeedTopSet;"
						<< "SpeedBotSet;"
						<< "������������;"
						<< "LAM1_BotSet;"
						<< "LAM1_TopSet;"
						<< "LAM2_TopSet;"
						<< "LAM2_BotSet;"
						<< "LAM_TE1;"
						<< "Temperature;"

						<< "Year;"
						<< "Month;"
						<< "Day;"
						<< "CassetteNo;"
						<< "SheetInCassette;"
						<< "Pos;"

						<< "������;"
						<< "TimeTest;"

						<< std::endl;
				}CATCH(SheetLogger, "");
				return ss1;
			}

			bool isSheet(T_IdSheet& t)
			{
				return t.Melt /*&& t.Pack*/ && t.PartNo && t.Sheet;
			}

			T_IdSheet GetIdSheet(PGConnection& conn, std::string Start, size_t count, size_t)
			{
				T_IdSheet ids;
				try
				{
					{
						std::stringstream ssd;
						ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
						ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
						//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
						ssd << " FROM todos WHERE";
						//ssd << " create_at <= '" << Start << "' + INTERVAL '2 min' AND ";
						ssd << " create_at <= '" << Start << "' AND ";
						ssd << "(id_name = " << PlateData[0].AlloyCodeText->ID;
						ssd << " OR id_name = " << PlateData[0].ThiknessText->ID;
						ssd << " OR (id_name = " << PlateData[0].Melt->ID << " AND content <> '0')";
						ssd << " OR (id_name = " << PlateData[0].PartNo->ID << " AND content <> '0')";
						ssd << " OR (id_name = " << PlateData[0].Sheet->ID << " AND content <> '0')";

						ssd << " OR id_name = " << PlateData[0].Slab->ID;
						ssd << " OR id_name = " << PlateData[0].Pack->ID;
						ssd << " OR id_name = " << PlateData[0].SubSheet->ID;
						ssd << ") ORDER BY id_name, create_at DESC;";

						std::string comand1 = ssd.str();

						MapTodos idSheet1;
						GetTodosSQL(conn, idSheet1, comand1);
						for(auto t : idSheet1)
						{
							if(t.id_name == PlateData[0].AlloyCodeText->ID) ids.Alloy = t.value;
							if(t.id_name == PlateData[0].ThiknessText->ID) ids.Thikness = t.value;
							if(t.id_name == PlateData[0].Melt->ID)		ids.Melt = t.content.As<int32_t>();
							if(t.id_name == PlateData[0].Slab->ID)		ids.Slab = t.content.As<int32_t>();
							if(t.id_name == PlateData[0].PartNo->ID)	ids.PartNo = t.content.As<int32_t>();
							if(t.id_name == PlateData[0].Pack->ID)		ids.Pack = t.content.As<int32_t>();
							if(t.id_name == PlateData[0].Sheet->ID)		ids.Sheet = t.content.As<int32_t>();
							if(t.id_name == PlateData[0].SubSheet->ID)	ids.SubSheet = t.content.As<int32_t>();
						}
					}
				}CATCH(SheetLogger, "");
				return ids;
			}

			float GetTime(std::string Start, std::vector<T_Todos>& allTime)
			{
				try
				{
					for(auto& a : allTime)
					{
						if(a.create_at <= Start)
						{
							return a.content.As<float>();
						}
					}
				}CATCH(SheetLogger, "");
				return 0.0f;
			}

			void GetAllTime(PGConnection& conn, std::vector<T_Todos>& allTime)
			{
				try
				{
					std::stringstream ssd;
					ssd << "SELECT create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " id_name = " << Par_Gen.TimeForPlateHeat->ID;
					ssd << " AND CAST(content AS DOUBLE PRECISION) <> 0.0";
					ssd << " ORDER BY create_at DESC;";
					std::string command = ssd.str();

					if(DEB)LOG_INFO(SheetLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
					PGresult* res = conn.PGexec(command);
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						int line = PQntuples(res);
						if(line)
						{
							int nFields = PQnfields(res);
							for(int l = 0; l < line; l++)
							{
								T_Todos td;
								td.create_at = conn.PGgetvalue(res, l, 0);
								td.id = Stoi(conn.PGgetvalue(res, l, 1));
								td.id_name = Stoi(conn.PGgetvalue(res, l, 2));
								td.value = conn.PGgetvalue(res, l, 3);
								int type = Stoi(conn.PGgetvalue(res, l, 4));
								td.content = PDF::GetVarVariant((OpcUa::VariantType)type, td.value);
								if(nFields >= 6)
									td.id_name_at = conn.PGgetvalue(res, l, 5);
								allTime.push_back(td);
							}
						}
					}
				}
				CATCH(SheetLogger, "");
			}

			void GetMask(uint16_t v, std::string& MaskV)
			{
				try
				{
					for(int i = 0; i < 9; i++)
					{
						if((1 << i) & v)
						{
							MaskV[i] = '1';
						}
						else
						{
							MaskV[i] = '0';
						}
					}
				}CATCH(SheetLogger, "");
			}

			//��������� �� ������ ����� � ���� State_1 = 3;
			void GetDataSheet1(PGConnection& conn, T_IdSheet& ids)
			{
				try
				{
					MapTodos DataSheetTodos;
					std::stringstream ssd;
					ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " create_at <= '" << ids.Start1 << "' AND ";
					ssd << "(id_name = " << GenSeqFromHmi.TempSet1->ID;			//������� �����������
					ssd << " OR id_name = " << Par_Gen.UnloadSpeed->ID;			//������� ��������
					ssd << " OR id_name = " << Par_Gen.TimeForPlateHeat->ID;	//������� �������
					ssd << " OR id_name = " << Par_Gen.PresToStartComp->ID;
					ssd << " OR id_name = " << HMISheetData.SpeedSection.Top->ID;
					ssd << " OR id_name = " << HMISheetData.SpeedSection.Bot->ID;
					ssd << " OR id_name = " << HMISheetData.LaminarSection1.Top->ID;
					ssd << " OR id_name = " << HMISheetData.LaminarSection1.Bot->ID;
					ssd << " OR id_name = " << HMISheetData.LaminarSection2.Top->ID;
					ssd << " OR id_name = " << HMISheetData.LaminarSection2.Bot->ID;
					ssd << " OR id_name = " << HMISheetData.Valve_1x->ID;
					ssd << " OR id_name = " << HMISheetData.Valve_2x->ID;
					ssd << ") ORDER BY id_name, create_at DESC;";

					std::string command = ssd.str();
					GetTodosSQL(conn, DataSheetTodos, command);

					for(auto a : DataSheetTodos)
					{
						if(a.id_name == GenSeqFromHmi.TempSet1->ID)				ids.Temper = a.content.As<float>();				//������� �����������
						if(a.id_name == Par_Gen.UnloadSpeed->ID)				ids.Speed = a.content.As<float>();				//������� ��������
						if(a.id_name == Par_Gen.TimeForPlateHeat->ID)			ids.TimeForPlateHeat = a.content.As<float>();	//������� �������

						if(a.id_name == Par_Gen.PresToStartComp->ID)			ids.PresToStartComp = a.content.As<float>();
						if(a.id_name == HMISheetData.SpeedSection.Top->ID)		ids.SpeedTopSet = a.content.As<float>();
						if(a.id_name == HMISheetData.SpeedSection.Bot->ID)		ids.SpeedBotSet = a.content.As<float>();
						if(a.id_name == HMISheetData.LaminarSection1.Top->ID)	ids.Lam1PosClapanTop = a.content.As<float>();
						if(a.id_name == HMISheetData.LaminarSection1.Bot->ID)	ids.Lam1PosClapanBot = a.content.As<float>();
						if(a.id_name == HMISheetData.LaminarSection2.Top->ID)	ids.Lam2PosClapanTop = a.content.As<float>();
						if(a.id_name == HMISheetData.LaminarSection2.Bot->ID)	ids.Lam2PosClapanBot = a.content.As<float>();
						if(a.id_name == HMISheetData.Valve_1x->ID)
						{
							ids.Valve_1x = a.content.As<uint16_t>();
							GetMask(ids.Valve_1x, ids.Mask1);
							ids.Mask = ids.Mask1 + " " + ids.Mask2;

						}
						if(a.id_name == HMISheetData.Valve_2x->ID)
						{
							ids.Valve_2x = a.content.As<uint16_t>();
							GetMask(ids.Valve_2x, ids.Mask2);
							ids.Mask = ids.Mask1 + " " + ids.Mask2;
						}
					}
				}CATCH(SheetLogger, "");
			}

			//��������� �� ������ ������ �� ���� State_2 = 5;
			void GetDataSheet2(PGConnection& conn, T_IdSheet& ids)
			{
				try
				{
					MapTodos DataSheetTodos;

					std::stringstream ssd;
					ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " create_at < '" << ids.DataTime_End << "' AND ";
					ssd << "(id_name = "    << AI_Hmi_210.Za_TE3->ID;
					ssd << " OR id_name = " << AI_Hmi_210.Za_PT3->ID;
					ssd << " OR id_name = " << AI_Hmi_210.LAM_TE1->ID;
					ssd << " OR id_name = " << GenSeqFromHmi.TempSet1->ID;		//������� �����������
					ssd << " OR id_name = " << Par_Gen.UnloadSpeed->ID;			//������� ��������
					ssd << " OR id_name = " << Par_Gen.TimeForPlateHeat->ID;	//������� �������

					ssd << ") ORDER BY id_name, create_at DESC";

					std::string command = ssd.str();
					GetTodosSQL(conn, DataSheetTodos, command);

					for(auto a : DataSheetTodos)
					{
						if(a.id_name == AI_Hmi_210.Za_TE3->ID)			ids.Za_TE3 = a.content.As<float>();
						if(a.id_name == AI_Hmi_210.Za_PT3->ID)			ids.Za_PT3 = a.content.As<float>();
						if(a.id_name == AI_Hmi_210.LAM_TE1->ID)			ids.LAM_TE1 = a.content.As<float>();

						if(a.id_name == GenSeqFromHmi.TempSet1->ID)		ids.Temper = a.content.As<float>();				//������� �����������
						if(a.id_name == Par_Gen.UnloadSpeed->ID)		ids.Speed = a.content.As<float>();				//������� ��������
						if(a.id_name == Par_Gen.TimeForPlateHeat->ID)	ids.TimeForPlateHeat = a.content.As<float>();	//������� �������
					}
				}CATCH(SheetLogger, "");
			}

			//��������� �� ������ ������ �� ���� State_2 = 5 ���� 5 ������;
			void GetDataSheet3(PGConnection& conn, T_IdSheet& ids)
			{
				try
				{
					MapTodos DataSheetTodos;
					std::tm tmp;
					time_t t1 = DataTimeOfString(ids.DataTime_End, tmp) + 5;
					std::string start = GetStringOfDataTime(&t1);

					std::stringstream ssd;
					ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " create_at < '" << start << "' AND ";
					ssd << "(id_name = " << AI_Hmi_210.LaminPressTop->ID;
					ssd << " OR id_name = " << AI_Hmi_210.LaminPressBot->ID;
					ssd << ") ORDER BY id_name, create_at DESC LIMIT 2;";

					std::string command = ssd.str();
					GetTodosSQL(conn, DataSheetTodos, command);

					for(auto a : DataSheetTodos)
					{
						if(a.id_name == AI_Hmi_210.LaminPressTop->ID) ids.LaminPressTop = a.content.As<float>();
						if(a.id_name == AI_Hmi_210.LaminPressBot->ID) ids.LaminPressBot = a.content.As<float>();
					}
				}CATCH(SheetLogger, "");
			}

			void GetSeq_1(PGConnection& conn, MapTodos& allSheetTodos)
			{
				try
				{
					std::stringstream ssd;

					ssd << "SELECT create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " create_at >= '" << StartSheet << "'";
					if(StopSheet.length()) ssd << " AND create_at < '" << StopSheet << "'";
					ssd << " AND id_name = " << GenSeqToHmi.Seq_1_StateNo->ID;
					ssd << " AND CAST(content AS INTEGER) >= " << st1_3;
					//ssd << " AND CAST(content AS INTEGER) <> " << st1_6;
					ssd << " ORDER BY create_at;";
					std::string command = ssd.str();
					GetTodosSQL(conn, allSheetTodos, command);
				}CATCH(SheetLogger, "");
			}
			void GetSeq_2(PGConnection& conn, MapTodos& allSheetTodos)
			{
				try
				{
					std::stringstream ssd;
					ssd << "SELECT create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " create_at >= '" << StartSheet << "'";
					if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
					ssd << " AND id_name = " << GenSeqToHmi.Seq_2_StateNo->ID;
					ssd << " AND CAST(content AS integer) >= " << st2_3;
					ssd << " AND CAST(content AS integer) <> " << st2_4;
					ssd << " ORDER BY create_at;";

					std::string command = ssd.str();
					GetTodosSQL(conn, allSheetTodos, command);
				}CATCH(SheetLogger, "");
			}

			void GetSeq_3(PGConnection& conn, MapTodos& allSheetTodos)
			{
				try
				{
					std::stringstream ssd;
					ssd << "SELECT create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " create_at >= '" << StartSheet << "'";
					if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
					ssd << " AND id_name = " << GenSeqToHmi.Seq_3_StateNo->ID;
					ssd << " AND (CAST(content AS integer) = " << st3_1;
					ssd << " OR CAST(content AS integer) = " << st3_4;
					ssd << ") ORDER BY create_at;";

					std::string command = ssd.str();
					GetTodosSQL(conn, allSheetTodos, command);
				}CATCH(SheetLogger, "");
			}
			void GetSeq_6(PGConnection& conn, MapTodos& allSheetTodos)
			{
				try
				{
					std::stringstream ssd;
					ssd << "SELECT create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " create_at >= '" << StartSheet << "'";
					if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
					ssd << " AND id_name = " << HMISheetData.NewData->ID;
					ssd << " AND content <> 'false'";
					ssd << " ORDER BY create_at;";

					std::string command = ssd.str();
					GetTodosSQL(conn, allSheetTodos, command);
				}CATCH(SheetLogger, "");
			}


			float GetDataTime_All(std::string TimeStart, std::string Start3)
			{
				return float(DataTimeDiff(Start3, TimeStart)) / 60.0f;
			}

			void Get_ID_S(PGConnection& conn, T_IdSheet& td)
			{
				try
				{
					std::stringstream ssd;
					ssd << "SELECT id, day, month, year, hour, cassetteno, sheetincassette, pos, news, correct FROM sheet ";
					ssd << "WHERE melt = " << td.Melt;
					ssd << " AND partno = " << td.PartNo;
					ssd << " AND pack = " << td.Pack;
					ssd << " AND sheet = " << td.Sheet;
					ssd << " AND slab = " << td.Slab;
					ssd << " AND subsheet = " << td.SubSheet;
					ssd << " ORDER BY start_at LIMIT 1";
					std::string command = ssd.str();
					if(DEB)LOG_INFO(SheetLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
					PGresult* res = conn.PGexec(command);
					if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
					{
						td.id = Stoi(conn.PGgetvalue(res, 0, 0));
						td.day = Stoi(conn.PGgetvalue(res, 0, 1));
						td.month = Stoi(conn.PGgetvalue(res, 0, 2));
						td.year = Stoi(conn.PGgetvalue(res, 0, 3));
						td.hour = Stoi(conn.PGgetvalue(res, 0, 4));
						td.cassetteno = Stoi(conn.PGgetvalue(res, 0, 5));
						td.sheetincassette = Stoi(conn.PGgetvalue(res, 0, 6));
						td.Pos = Stoi(conn.PGgetvalue(res, 0, 7));
						td.news = Stoi(conn.PGgetvalue(res, 0, 8));
						td.Correct = GetStringData(conn.PGgetvalue(res, 0, 9));

					}
					PQclear(res);
				}CATCH(SheetLogger, "");
			}

			void GetID(PGConnection& conn, T_IdSheet& td)
			{
				try
				{
					if(isSheet(td))
					{
						std::stringstream ssd;
						ssd << "SELECT id FROM sheet ";
						ssd << "WHERE melt = " << td.Melt;
						ssd << " AND slab = " << td.Slab;
						ssd << " AND partno = " << td.PartNo;
						ssd << " AND pack = " << td.Pack;
						ssd << " AND sheet = " << td.Sheet;
						ssd << " AND subsheet = " << td.SubSheet;
						ssd << " ORDER BY id LIMIT 1";
						PGresult* res = conn.PGexec(ssd.str());
						if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
							td.id = Stoi(conn.PGgetvalue(res, 0, 0));
						PQclear(res);
					}
				}CATCH(SheetLogger, "");
			}
			int Get_ID_C(PGConnection& conn, T_IdSheet& td)
			{
				int id = 0;
				try
				{
					std::stringstream ssd;
					ssd << "SELECT id FROM cassette ";
					ssd << "WHERE day = " << td.day;
					ssd << " AND month = " << td.month;
					ssd << " AND year = " << td.year;
					//if(td.year >= 2024 && td.month >= 8)
						ssd << " AND hour = " << td.hour;
					//else
					//	ssd << " AND hour < 1";

					ssd << " AND cassetteno = " << td.cassetteno;
					ssd << " ORDER BY id LIMIT 1";
					std::string command = ssd.str();

					if(DEB)LOG_INFO(SheetLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
					PGresult* res = conn.PGexec(command);
					if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
						id = Stoi(conn.PGgetvalue(res, 0, 0));
					PQclear(res);
				}CATCH(SheetLogger, "");
				return id;
			}

			void SetCountSheetInCassette(PGConnection& conn, T_IdSheet& td)
			{
				try
				{
					//KPVL::UpdateCountSheet(conn, td.cassetteno);

					std::stringstream ssd;
					ssd << "SELECT id FROM cassette WHERE";
					ssd << " year = " << td.year << " AND";
					ssd << " month = " << td.month << " AND";
					ssd << " day = " << td.day << " AND";
					//if(td.year >= 2024 && td.month >= 8)
						ssd << " hour = " << td.hour << " AND";
					ssd << " cassetteno = " << td.cassetteno;
					ssd << " ORDER BY run_at DESC LIMIT 1;";
					std::string command = ssd.str();
					PGresult* res = conn.PGexec(command);
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						if(PQntuples(res))
							td.cassette = Stoi(conn.PGgetvalue(res, 0, 0));
					}
					else
						LOG_ERR_SQL(SheetLogger, res, "");
					PQclear(res);
				}CATCH(SheetLogger, "");
			}

			void GetTemperatute(PGConnection& conn, T_IdSheet& td)
			{
				try
				{
					MapTodos DataSheetTodos;
					std::tm tmp;
					time_t t1 = DataTimeOfString(td.DataTime_End, tmp);
					std::string Start = GetStringOfDataTime(&t1); 

					std::stringstream ssd;
					ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE";
					ssd << " create_at < '" << Start << "' AND ";
					ssd << "(id_name = " << Hmi210_1.Htr1_1->ID;
					ssd << " OR id_name = " << Hmi210_1.Htr1_2->ID;
					ssd << " OR id_name = " << Hmi210_1.Htr1_3->ID;
					ssd << " OR id_name = " << Hmi210_1.Htr1_4->ID;
					ssd << ") ORDER BY id_name, create_at DESC LIMIT 4;";

					std::string command = ssd.str();
					GetTodosSQL(conn, DataSheetTodos, command);

					float Htr1_1 = 0;
					float Htr1_2 = 0;
					float Htr1_3 = 0;
					float Htr1_4 = 0;

					for(auto a : DataSheetTodos)
					{
						if(a.id_name == Hmi210_1.Htr1_1->ID) Htr1_1 = a.content.As<float>();
						if(a.id_name == Hmi210_1.Htr1_2->ID) Htr1_2 = a.content.As<float>();
						if(a.id_name == Hmi210_1.Htr1_3->ID) Htr1_3 = a.content.As<float>();
						if(a.id_name == Hmi210_1.Htr1_4->ID) Htr1_4 = a.content.As<float>();
					}
					td.Temperature = (Htr1_1 + Htr1_2 + Htr1_3 + Htr1_4) / 4.0f;

					int tt = 0;
				}CATCH(SheetLogger, "");

			}

			bool Update(PGConnection& conn, T_IdSheet& td)
			{
				try
				{
					#pragma region MyRegion
					std::stringstream ssd;
					ssd << "UPDATE sheet SET";

					ssd << " start_at = '" << td.Start1 << "'";				//����, ����� �������� ����� � ���������� ����
					ssd << ", datatime_end = '" << td.DataTime_End << "'";		//����, ����� ������ ����� �� ���������� ����

					if(td.Start2.length())			ssd << ", secondpos_at = '" << td.Start2 << "'";		//������ �� ������ ����
					if(td.InCant.length())			ssd << ", incant_at = '" << td.InCant << "'";		//������ �� ��������
					if(td.Cant.length())			ssd << ", cant_at = '" << td.Cant << "'";			//���������

					ssd << ", alloy = '" << td.Alloy << "'";				//����� �����
					ssd << ", thikness = '" << td.Thikness << "'";			//������� �����, ��

					if(td.TimeForPlateHeat)
						ssd << ", timeforplateheat = " << std::setprecision(1) << std::fixed << td.TimeForPlateHeat;	//������� ����� ���������� ����� � ���������� ����. ���
					if(td.DataTime_All)
						ssd << ", datatime_all = " << std::setprecision(1) << std::fixed << td.DataTime_All;			//���� ����� ���������� ����� � ���������� ����. ���


					if(td.Speed)
						ssd << ", speed = " << std::setprecision(1) << std::fixed << td.Speed;						//�������� ��������
					if(td.Temper)
						ssd << ", temper = " << std::setprecision(1) << std::fixed << td.Temper;						//������� �����������

					if(td.PresToStartComp)
						ssd << ", prestostartcomp = " << std::setprecision(1) << std::fixed << td.PresToStartComp;				//
					if(td.SpeedTopSet)
						ssd << ", posclapantop = " << std::setprecision(1) << std::fixed << td.SpeedTopSet;			//������. ���������� ������. ����
					if(td.SpeedBotSet)
						ssd << ", posclapanbot = " << std::setprecision(1) << std::fixed << td.SpeedBotSet;			//������. ���������� ������. ���
					if(td.Lam1PosClapanTop)
						ssd << ", lam1posclapantop = " << std::setprecision(1) << std::fixed << td.Lam1PosClapanTop;		//������. ���������� ������ 1. ����
					if(td.Lam1PosClapanBot)
						ssd << ", lam1posclapanbot = " << std::setprecision(1) << std::fixed << td.Lam1PosClapanBot;		//������. ���������� ������ 1. ���
					if(td.Lam2PosClapanTop)
						ssd << ", lam2posclapantop = " << std::setprecision(1) << std::fixed << td.Lam2PosClapanTop;		//������. ���������� ������ 2. ����
					if(td.Lam2PosClapanBot)
						ssd << ", lam2posclapanbot = " << std::setprecision(1) << std::fixed << td.Lam2PosClapanBot;		//������. ���������� ������ 2. ���
					ssd << ", mask = '" << td.Mask << "'";					//����� ������ �������
					//��������� �� ������ �� ���� State_2 = 5;
					if(td.LAM_TE1)
						ssd << ", lam_te1 = " << std::setprecision(1) << std::fixed << td.LAM_TE1;					//����������� ���� � �������
					if(td.Za_TE3)
						ssd << ", za_te3 = " << std::setprecision(1) << std::fixed << td.Za_TE3;						//����������� ���� � ����
					if(td.Za_PT3)
						ssd << ", za_pt3 = " << std::setprecision(1) << std::fixed << td.Za_PT3;						//�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)

					//��������� �� ������ �� ���� State_2 = 5 ���� 5 ������;
					if(td.LaminPressTop)
						ssd << ", lampresstop = " << std::setprecision(1) << std::fixed << td.LaminPressTop;			//�������� � ������� ����������
					if(td.LaminPressBot)
						ssd << ", lampressbot = " << std::setprecision(1) << std::fixed << td.LaminPressBot;			//�������� � ������ ����������

					if(td.Temperature)
						ssd << ", temperature = " << std::setprecision(1) << std::fixed << td.Temperature;

					#pragma endregion

					ssd << ", day = " << td.day;
					ssd << ", month = " << td.month;
					ssd << ", year = " << td.year;
					ssd << ", hour = " << td.hour;
					ssd << ", cassetteno = " << td.cassetteno;
					ssd << ", sheetincassette = " << td.sheetincassette;
					ssd << ", cassette = " << td.cassette;
					ssd << ", pos = " << 7;
					ssd << ", delete_at = DEFAULT, correct = now()";
					ssd << " WHERE id = " << td.id;
					#pragma region MyRegion

					SETUPDATESQL(SheetLogger, conn, ssd);

					SetWindowText(hWndDebug, ssd.str().c_str());
					fUpdateSheet << ssd.str() << std::endl;
					fUpdateSheet.flush();
					LOG_INFO(SheetLogger, "{:90}| {}", FUNCTION_LINE_NAME, ssd.str());
					#pragma endregion
					return true;
				}
				CATCH(SheetLogger, "");
				return false;
			}

			void Insert(PGConnection & conn, T_IdSheet & td)
			{
				try
				{
				//if(iAllId) td.id = iAllId++;
					std::stringstream ssd;
					ssd << "INSERT INTO sheet (";
					ssd << "create_at, ";				//����, ����� �������� ����� � ���������� ����
					ssd << "start_at, ";				//����, ����� �������� ����� � ���������� ����
					ssd << "datatime_end, ";			//����, ����� ������ ����� �� ���������� ����

					if(td.Start2.length())			ssd << "secondpos_at, ";	//������ �� ������ ����
					if(td.InCant.length())			ssd << "incant_at, ";		//������ �� ��������
					if(td.Cant.length())			ssd << "cant_at, ";			//���������

					ssd << "datatime_all,";			//���� ����� ���������� ����� � ���������� ����. ���
					ssd << "alloy, ";
					ssd << "thikness, ";
					ssd << "melt, ";
					ssd << "slab, ";
					ssd << "partno, ";
					ssd << "pack, ";
					ssd << "sheet, ";
					ssd << "subsheet, ";
					ssd << "temper, ";
					ssd << "speed, ";
					ssd << "timeforplateheat, ";
					ssd << "prestostartcomp, ";
					ssd << "posclapantop, ";
					ssd << "posclapanbot, ";
					ssd << "lam1posclapantop, ";
					ssd << "lam1posclapanbot, ";
					ssd << "lam2posclapantop, ";
					ssd << "lam2posclapanbot, ";
					ssd << "mask, ";

					ssd << "lampresstop, ";				//�������� � ������� ����������
					ssd << "lampressbot, ";				//�������� � ������ ����������
					ssd << "lam_te1, ";					//����������� ���� � �������
					ssd << "za_te3, ";					//����������� ���� � ����
					ssd << "za_pt3, ";					//�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)
					ssd << "temperature, ";

					ssd << " day, ";
					ssd << " month, ";
					ssd << " year, ";
					ssd << " hour, ";
					ssd << " cassetteno, ";
					ssd << " sheetincassette, ";
					ssd << " cassette, ";


					ssd << "correct, ";
					//ssd << "pdf, ";
					ssd << "pos, ";
					ssd << "news";


					ssd << ") VALUES (";
					ssd << "'" << td.DataTime << "', ";
					ssd << "'" << td.Start1 << "', ";
					ssd << "'" << td.DataTime_End << "', ";

					if(td.Start2.length())			ssd << "'" << td.Start2 << "', ";	//������ �� ������ ����
					if(td.InCant.length())			ssd << "'" << td.InCant << "', ";	//������ �� ��������
					if(td.Cant.length())			ssd << "'" << td.Cant << "', ";		//���������
					ssd << std::setprecision(1) << std::fixed << td.DataTime_All;
					//ssd << std::round(td.DataTime_All*10.0f) / 10.0f << ", ";
					ssd << "'" << td.Alloy << "', ";
					ssd << "'" << td.Thikness << "', ";
					ssd << td.Melt << ", ";
					ssd << td.Slab << ", "; //Slab
					ssd << td.PartNo << ", ";
					ssd << td.Pack << ", ";
					ssd << td.Sheet << ", ";
					ssd << td.SubSheet << ", ";
					ssd << td.Temper << ", ";
					ssd << td.Speed << ", ";
					ssd << td.TimeForPlateHeat << ", ";
					ssd << td.PresToStartComp << ", ";
					ssd << td.SpeedTopSet << ", ";
					ssd << td.SpeedBotSet << ", ";
					ssd << td.Lam1PosClapanTop << ", ";
					ssd << td.Lam1PosClapanBot << ", ";
					ssd << td.Lam2PosClapanTop << ", ";
					ssd << td.Lam2PosClapanBot << ", ";
					ssd << "'" + td.Mask + "', ";

					ssd << td.LaminPressTop << ", ";
					ssd << td.LaminPressBot << ", ";

					ssd << td.LAM_TE1 << ", ";
					ssd << td.Za_TE3 << ", ";
					ssd << td.Za_PT3 << ", ";
					ssd << td.Temperature << ", ";

					ssd << "'" << td.day << "', ";
					ssd << "'" << td.month << "', ";
					ssd << "'" << td.year << "', ";
					ssd << td.hour << ", ";
					ssd << td.cassetteno << ", ";
					ssd << td.sheetincassette << ", ";
					ssd << td.cassette << ", ";

					ssd << "now(), ";
					ssd << 7 << ", ";
					ssd << td.news << ");";
					SETUPDATESQL(SQLLogger, conn, ssd);

					SetWindowText(hWndDebug, ssd.str().c_str());
					fUpdateSheet << ssd.str() << std::endl;
					fUpdateSheet.flush();
					LOG_INFO(SheetLogger, "{:90}| {}", FUNCTION_LINE_NAME, ssd.str());

					GetID(conn, td);
					LOG_INFO(SheetLogger, "{:90}| Sheet ID = {}", FUNCTION_LINE_NAME, td.id);
				}
				CATCH(SheetLogger, "");
			}

			bool UpdateSheet(PGConnection& conn, T_IdSheet& td)
			{
				try
				{
					GetTemperatute(conn, td);
					SetCountSheetInCassette(conn, td);
					if(!td.Start1.length() || !td.DataTime_End.length()) return false;
					if(td.id)
					{
						if(ID && ID != td.id) return false;
						return Update(conn, td);
					}
					else if(td.Melt /*&& td.Pack*/ && td.PartNo && td.Sheet)
					{
						Insert(conn, td);
					}
				}CATCH(SheetLogger, "");
				return false;
			}

			bool InZone11(PGConnection& conn, std::string create_at, size_t count, size_t i)
			{
				try
				{
					T_IdSheet ids = GetIdSheet(conn, create_at, count, i);
					if(!ids.id)
						GetID(conn, ids);

				#pragma region ����� ����
					std::stringstream sss;
					sss << "InZone1 �" << i;
					sss << " Start1: " << ids.Start1;
					sss << " Id: " << ids.id;
					sss << " Melt: " << ids.Melt;
					sss << " PartNo: " << ids.PartNo;
					sss << " Pack: " << ids.Pack;
					sss << " Slab: " << ids.Slab;
					sss << " Sheet: " << ids.Sheet;
					sss << " SubSheet: " << ids.SubSheet;
					SetWindowText(hWndDebug, sss.str().c_str());
				#pragma endregion

					if(isSheet(ids))
					{
						if(isSheet(Ids1))
						{
							InZone2(conn, create_at);
						}

						Ids1 = ids;
						Ids1.DataTime = create_at;
						Ids1.Start1 = create_at;

					}
				}CATCH(SheetLogger, "");
				return true;
			}

			bool InZone12(PGConnection& conn, std::string create_at, size_t count, size_t i)
			{
				T_IdSheet ids = GetIdSheet(conn, create_at, count, i);
				Ids1.Alloy = ids.Alloy;
				Ids1.Thikness = ids.Thikness;
				Ids1.Melt = ids.Melt;
				Ids1.Slab = ids.Slab;
				Ids1.PartNo = ids.PartNo;
				Ids1.Pack = ids.Pack;
				Ids1.Sheet = ids.Sheet;
				Ids1.SubSheet = ids.SubSheet;

				if(!Ids1.DataTime.length())Ids1.DataTime = create_at;
				if(!Ids1.Start1.length())Ids1.Start1 = create_at;
				return true;
			}

			bool InZone1(PGConnection& conn, std::string create_at, size_t count, size_t i)
			{
				if(!isSheet(Ids1))
					return InZone11(conn, create_at, count, i);
				else
					return InZone12(conn, create_at, count, i);
			}


			bool InZone2(PGConnection& conn, std::string create_at)
			{
				try
				{
					if(isSheet(Ids1))
					{
						if(isSheet(Ids2))
						{
							InZone3(conn, create_at);
						}

						Ids2 = Ids1;
						Ids2.Start2 = create_at;
						Ids1 = T_IdSheet();
						if(!Ids2.id)
							GetID(conn, Ids2);

					#pragma region ����� ����
						std::stringstream sss;
						sss << "InZone2";
						sss << " Start1: " << Ids2.Start1;
						sss << " Id: " << Ids2.id;
						sss << " Melt: " << Ids2.Melt;
						sss << " PartNo: " << Ids2.PartNo;
						sss << " Pack: " << Ids2.Pack;
						sss << " Slab: " << Ids2.Slab;
						sss << " Sheet: " << Ids2.Sheet;
						sss << " SubSheet: " << Ids2.SubSheet;
						SetWindowText(hWndDebug, sss.str().c_str());
					#pragma endregion
					}
				}CATCH(SheetLogger, "");
				return true;
			}
			bool InZone3(PGConnection& conn, std::string create_at)
			{
				try
				{
					if(isSheet(Ids2))
					{
						if(isSheet(Ids3))
						{
							InZone5(conn, create_at);
						}
						Ids3 = Ids2;
						Ids3.DataTime_End = create_at;
						Ids3.Start3 = create_at;
						Ids2 = T_IdSheet();

						if(!Ids3.id)
							GetID(conn, Ids3);
					#pragma region ����� ����
						std::stringstream sss;
						sss << "InZone3";
						sss << " Start1: " << Ids3.Start1;
						sss << " Id: " << Ids3.id;
						sss << " Melt: " << Ids3.Melt;
						sss << " PartNo: " << Ids3.PartNo;
						sss << " Pack: " << Ids3.Pack;
						sss << " Slab: " << Ids3.Slab;
						sss << " Sheet: " << Ids3.Sheet;
						sss << " SubSheet: " << Ids3.SubSheet;
						SetWindowText(hWndDebug, sss.str().c_str());
					#pragma endregion
					}
				}CATCH(SheetLogger, "");
				return true;
			}
			bool InZone5(PGConnection& conn, std::string create_at)
			{
				try
				{
					if(isSheet(Ids3))
					{
						if(isSheet(Ids5))
						{
							InZone6(conn, create_at);
						}

						Ids5 = Ids3;
						//Ids5.Start5 = create_at;
						Ids5.Start5 = create_at;
						Ids5.InCant = create_at;
						Ids3 = T_IdSheet();

						if(!Ids5.id)
							GetID(conn, Ids5);
						#pragma region ����� ����
						std::stringstream sss;
						sss << "InZone5";
						sss << " Start1: " << Ids5.Start1;
						sss << " Id: " << Ids5.id;
						sss << " Melt: " << Ids5.Melt;
						sss << " PartNo: " << Ids5.PartNo;
						sss << " Pack: " << Ids5.Pack;
						sss << " Slab: " << Ids5.Slab;
						sss << " Sheet: " << Ids5.Sheet;
						sss << " SubSheet: " << Ids5.SubSheet;
						SetWindowText(hWndDebug, sss.str().c_str());
						#pragma endregion
					}
					if(isSheet(Ids5))
					{
						Ids5.Start5 = create_at;
						Ids5.InCant = create_at;
						Ids3 = T_IdSheet();

						if(!Ids5.id)
							GetID(conn, Ids5);
						#pragma region ����� ����
						std::stringstream sss;
						sss << "InZone5";
						sss << " Start1: " << Ids5.Start1;
						sss << " Id: " << Ids5.id;
						sss << " Melt: " << Ids5.Melt;
						sss << " PartNo: " << Ids5.PartNo;
						sss << " Pack: " << Ids5.Pack;
						sss << " Slab: " << Ids5.Slab;
						sss << " Sheet: " << Ids5.Sheet;
						sss << " SubSheet: " << Ids5.SubSheet;
						SetWindowText(hWndDebug, sss.str().c_str());
						#pragma endregion
					}
				}CATCH(SheetLogger, "");
				return true;
			}
			void GetSheetCassette(PGConnection& conn, T_IdSheet& ids)
			{
				try
				{
					int Hour = 0;
					int Day = 0;
					int Month = 0;
					int Year = 0;
					int CassetteNo = 0;
					int SheetInCassette = 0;

					MapTodos DataSheetTodos;
					std::stringstream ssd;
					ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
					ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
					//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
					ssd << " FROM todos WHERE ";
					//ssd << "create_at >= '" << ids.Start3 << "' AND ";
					if(ids.Cant.length())
						ssd << "create_at < '" << ids.Cant << "' AND ";
					else if(ids.InCant.length())
						ssd << "create_at < '" << ids.InCant << "' AND ";
					else
						throw std::exception(__FUN("ids.Cant = NULL AND ids.InCant = NULL"));

					ssd << "(id_name = " << HMISheetData.Cassette.Hour->ID << " OR";
					ssd << " id_name = " << HMISheetData.Cassette.Day->ID << " OR";
					ssd << " id_name = " << HMISheetData.Cassette.Month->ID << " OR";
					ssd << " id_name = " << HMISheetData.Cassette.Year->ID << " OR";
					ssd << " id_name = " << HMISheetData.Cassette.CassetteNo->ID << " OR";
					ssd << " id_name = " << HMISheetData.Cassette.SheetInCassette->ID;
					ssd << ") ORDER BY id_name, create_at DESC;";

					std::string command = ssd.str();
					GetTodosSQL(conn, DataSheetTodos, command);

					for(auto a : DataSheetTodos)
					{
						if(a.id_name == HMISheetData.Cassette.Hour->ID) Hour = Stoi(a.value);
						if(a.id_name == HMISheetData.Cassette.Day->ID) Day = Stoi(a.value);
						if(a.id_name == HMISheetData.Cassette.Month->ID) Month = Stoi(a.value);
						if(a.id_name == HMISheetData.Cassette.Year->ID) Year = Stoi(a.value);
						if(a.id_name == HMISheetData.Cassette.CassetteNo->ID) CassetteNo = Stoi(a.value);
						if(a.id_name == HMISheetData.Cassette.SheetInCassette->ID) SheetInCassette = Stoi(a.value);
					}

					if(Day && Month && Year && CassetteNo)
					{
						ids.hour = Hour;
						ids.day = Day;
						ids.month = Month;
						ids.year = Year;
						ids.cassetteno = CassetteNo;
						ids.sheetincassette = SheetInCassette + 1;
					}

				}CATCH(SheetLogger, "");

				if(ids.day && ids.month && ids.year && ids.cassetteno)
					ids.Pos = 7;
				else
					ids.Pos = 16;
			}


			bool InZone62(PGConnection& conn, T_IdSheet& Ids, std::string create_at)
			{
				try
				{
					T_IdSheet ids = Ids;

					Ids = T_IdSheet();

					ids.Cant = create_at;
					if(ID && ID != ids.id) return false;
					GetDataSheet1(conn, ids);
					GetDataSheet2(conn, ids);
					GetDataSheet3(conn, ids);
					Get_ID_S(conn, ids);

					#pragma region ����� ����
						std::stringstream sss;
						sss << "InZone6 ";
						sss << " Start1: " << ids.Start1;
						sss << " Id: " << ids.id;
						sss << " Melt: " << ids.Melt;
						sss << " PartNo: " << ids.PartNo;
						sss << " Pack: " << ids.Pack;
						sss << " Slab: " << ids.Slab;
						sss << " Sheet: " << ids.Sheet;
						sss << " SubSheet: " << ids.SubSheet;
						SetWindowText(hWndDebug, sss.str().c_str());
					#pragma endregion

					if(!ids.Start2.length())
					{
						std::tm TM;
						time_t tm1 = DataTimeOfString(ids.DataTime_End, TM);
						time_t tm2 = DataTimeOfString(ids.Start1, TM);

						time_t tm = time_t(tm1 + (difftime(tm1, tm2) / 2.0));
						ids.Start2 = GetStringOfDataTime(&tm);
					}

					float t = GetTime(ids.DataTime_End, allTime);
					if(t) ids.TimeForPlateHeat = t;

					ids.DataTime_All = GetDataTime_All(ids.Start1, ids.DataTime_End);
					float f = fabsf(ids.DataTime_All - ids.TimeForPlateHeat);
					if(f > 2.0f)
						ids.DataTime_All = GetDataTime_All(ids.Start1, ids.DataTime_End);


					GetSheetCassette(conn, ids);

					SaveBodyCsv(ss1, ids, "");
					bool ret = UpdateSheet(conn, ids);
					Ids6.push_back(ids);
					return ret;
				}CATCH(SheetLogger, "");
				return false;
			}

			bool InZone6(PGConnection& conn, std::string create_at)
			{
				try
				{
					if(isSheet(Ids5))
					{
						return InZone62(conn, Ids5, create_at);
					}
					//else if(isSheet(Ids3))
					//{
					//	InZone62(conn, Ids3, create_at);
					//}
				}CATCH(SheetLogger, "");
				return false;
			}

			std::fstream SaveStepHeadCsv(std::string name)
			{
				std::fstream ss1(name, std::fstream::binary | std::fstream::out);
				try
				{
					ss1 << "create_at;";
					ss1 << "id;";
					ss1 << "value;";
					ss1 << "id_name;";
					ss1 << "comment;";
					ss1 << std::endl;
				}CATCH(SheetLogger, "");
				return ss1;
			}
			void SaveStepBodyCsv(std::fstream& ss1, T_Todos& td)
			{
				try
				{
					ss1 << " " << td.create_at << ";";
					ss1 << td.id << ";";
					if(td.id_name == GenSeqToHmi.Seq_1_StateNo->ID)
						ss1 << GenSeq1[Stoi(td.value)] << ";";
					else if(td.id_name == GenSeqToHmi.Seq_2_StateNo->ID)
						ss1 << GenSeq2[Stoi(td.value)] << ";";
					else if(td.id_name == GenSeqToHmi.Seq_3_StateNo->ID)
						ss1 << GenSeq3[Stoi(td.value)] << ";";
					else
						ss1 << td.value << ";";
					ss1 << td.id_name << ";";
					ss1 << td.id_name_at << ";";
					ss1 << std::endl;
				}CATCH(SheetLogger, "");
			}

			std::fstream SaveT_IdSheetHeadCsv(std::string name)
			{
				std::fstream ss1(name, std::fstream::binary | std::fstream::out);
				try
				{
					ss1 << " " << "Start1" << ";";
					ss1 << " " << "TimeStart" << ";";
					ss1 << "Melt" << ";";
					ss1 << "PartNo" << ";";
					ss1 << "Pack" << ";";
					ss1 << "Sheet" << ";";
					ss1 << "SubSheet" << ";";
					ss1 << std::endl;
				}CATCH(SheetLogger, "");
				return ss1;
			}
			void SaveT_IdSheetBodyCsv(std::fstream& ss1, T_IdSheet& ids)
			{
				try
				{
					ss1 << " " << ids.Start1 << ";";
					ss1 << " " << ids.DataTime << ";";
					ss1 << ids.Melt << ";";
					ss1 << ids.PartNo << ";";
					ss1 << ids.Pack << ";";
					ss1 << ids.Sheet << ";";
					ss1 << ids.SubSheet << ";";
					ss1 << std::endl;
				}CATCH(SheetLogger, "");
			}

			//�������� � ����
			void GetSeq_1_StateNo(PGConnection& conn, T_Todos& td, size_t count, size_t i, std::fstream& ff2)
			{
				int16_t st = td.content.As<int16_t>();
//������� ������� �����
				if(st == st1_3)
				{
					InZone11(conn, td.create_at, count, i);
					SaveT_IdSheetBodyCsv(ff2, Ids1);
				}
				//"�������� � ����"
				else if(st == st1_4)
				{
					InZone1(conn, td.create_at, count, i);
					SaveT_IdSheetBodyCsv(ff2, Ids1);

					if(!Ids1.CloseInDor.length())
					{
						if(isSheet(Ids1))
						{
							Ids1.CloseInDor = td.create_at;
							Ids1.Start1 = td.create_at;
						}
					}
				}
				//"������� ������� �����"
				else if(st == st1_5)
				{
					InZone1(conn, td.create_at, count, i);
					SaveT_IdSheetBodyCsv(ff2, Ids1);

					if(!Ids1.CloseInDor.length())
					{
						if(isSheet(Ids1))
						{
							Ids1.CloseInDor = td.create_at;
							Ids1.Start1 = td.create_at;
						}
					}

					if(!Ids1.Nagrev.length())
					{
						InZone1(conn, td.create_at, count, i);
						SaveT_IdSheetBodyCsv(ff2, Ids1);

						if(isSheet(Ids1))
						{
							Ids1.Nagrev = td.create_at;
							Ids1.Start1 = td.create_at;
						}
					}

				}
				//"������ �����"
				else if(st == st1_6)
				{
					if(!Ids1.Nagrev.length())
					{
						InZone1(conn, td.create_at, count, i);
						SaveT_IdSheetBodyCsv(ff2, Ids1);

						if(isSheet(Ids1))
						{
							Ids1.Nagrev = td.create_at;
							Ids1.Start1 = td.create_at;
						}
					}
				}
				//"�������� �� 2 ��������" || "�������� �� 2-� �������� ����"
				else if(st == st1_7 || st == st1_8)
				{
					InZone2(conn, td.create_at);
				}
			}

			//������ ���� ����
			void GetSeq_2_StateNo(PGConnection& conn, T_Todos& td)
			{
				int16_t st = td.content.As<int16_t>();
				if(st == st2_3)	//3 = ����� ��������� � 1-�� ��������� ����
				{
					InZone2(conn, td.create_at);
				} else if(st == st2_5 || st == st2_6 || st == st2_7) //�������� �� ���� - "������� �������� �����" || ������ � ����� �������" || "������� �������� �����"
				{
					InZone3(conn, td.create_at);
				}
			}

			//���� ����������
			void GetSeq_3_StateNo(PGConnection& conn, T_Todos& td)
			{
				int16_t st = td.content.As<int16_t>();
				if(st == st3_1 || st == st3_4)	//1 = �������� ���������; 4 - ��������� ����� ���������
					InZone5(conn, td.create_at);

			}

			GetSheets(PGConnection& conn, int id, std::string datestart, std::string datestop)
			{
				InitLogger(SheetLogger);
				try
				{
					ID = id;
					StartSheet = datestart;
					StopSheet = datestop;

					MapTodos allSheetTodos;


					remove("UpdateSheet.txt");
					fUpdateSheet = std::fstream("UpdateSheet.txt", std::fstream::binary | std::fstream::out | std::ios::app);


					ss1 = SaveHeadCsv("Sheet.csv");


					GetAllTime(conn, allTime);

					//GenSeqToHmi.Seq_1_StateNo
					GetSeq_1(conn, allSheetTodos);

					//GenSeqToHmi.Seq_2_StateNo
					GetSeq_2(conn, allSheetTodos);

					//GenSeqToHmi.Seq_3_StateNo
					GetSeq_3(conn, allSheetTodos);

					//HMISheetData.NewData
					GetSeq_6(conn, allSheetTodos);

					////��������� �� �������
					std::sort(allSheetTodos.begin(), allSheetTodos.end(), cmpData);

					////GenSeqToHmi.Seq_3_StateNo
					//GetSeq_3(conn);


					size_t count = allSheetTodos.size();
					size_t i = count;
					std::fstream ff1 = SaveStepHeadCsv("Step.csv");
					std::fstream ff2 = SaveT_IdSheetHeadCsv("T_IdSheet.csv");


					for(MapTodos::iterator it = allSheetTodos.begin(); isRun && it != allSheetTodos.end(); it++)
					{
						T_Todos& td = *it;

						SaveStepBodyCsv(ff1, td);

						//�������� � ����
						if(td.id_name == GenSeqToHmi.Seq_1_StateNo->ID)
						{
							GetSeq_1_StateNo(conn, td, count, i, ff2);
						}
						//������ ���� ����
						else if(td.id_name == GenSeqToHmi.Seq_2_StateNo->ID)
						{
							GetSeq_2_StateNo(conn, td);
						}
						//���� ����������
						else if(td.id_name == GenSeqToHmi.Seq_3_StateNo->ID)
						{
							GetSeq_3_StateNo(conn, td);
						}
						//��������
						else if(td.id_name == HMISheetData.NewData->ID && td.content.As<bool>())
						{
							if(InZone6(conn, td.create_at))
								break;
						}

						i--;
					}
					ff2.close();
					ff1.close();
					ss1.close();
					fUpdateSheet.close();
					INT II = 0;
				}CATCH(SheetLogger, "");
			}
		};


		std::string GetStartTime(PGConnection& conn, int& id, std::string& start, std::string& stop)
		{
			//std::string start = "";
			try
			{
				id = 0;
				start = "";
				stop = "";
				//std::string command = "SELECT now() - INTERVAL '30 MINUTES'";
				std::string command =
					"SELECT id, "
					" start_at - INTERVAL '2 MINUTES', "						//����� 2 ������
					" start_at + INTERVAL '60 MINUTES' "						//���� 60 �����
					"FROM sheet WHERE "
					"delete_at IS NULL AND correct IS NULL AND CAST(pos AS integer) > 6 ORDER BY start_at LIMIT 1";
					//" FROM sheet WHERE correct IS NULL AND start_at > ("
					//"SELECT start_at FROM sheet WHERE correct IS NOT NULL ORDER BY start_at DESC LIMIT 1"
					//") AND CAST(pos AS integer) > 6 "
					//"ORDER BY start_at ASC LIMIT 1;";

				PGresult* res = conn.PGexec(command);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					if(conn.PQntuples(res))
					{
						std::string ids = conn.PGgetvalue(res, 0, 0);
						id = Stoi(ids);
						start = conn.PGgetvalue(res, 0, 1);
						stop = conn.PGgetvalue(res, 0, 2);
					}
				}
				else
					LOG_ERR_SQL(SheetLogger, res, command);
				PQclear(res);
			}CATCH(SheetLogger, "");
			return start;
		}

		std::string GetStartTime2(PGConnection& conn)
		{
			std::string start = "";
			try
			{
				std::string command = "SELECT now() - INTERVAL '30 MINUTES'";

				PGresult* res = conn.PGexec(command);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					if(conn.PQntuples(res))
					{
						start = conn.PGgetvalue(res, 0, 0);
					}
				}
				else
				{
					LOG_ERR_SQL(SheetLogger, res, command);
				}
				PQclear(res);
			}CATCH(SheetLogger, "");
			return start;
		}

	};

	bool isCorrectSheet = false;
	bool isCorrectCassette = false;

	time_t GetSheetOfBase(PGConnection& conn, std::string id)
	{
		try
		{
			std::stringstream sd;
			sd << "SELECT * FROM sheet WHERE id = " << id; //delete_at IS NULL AND 
			std::string command = sd.str();
			PGresult* res = conn.PGexec(command);
			if(PQresultStatus(res) == PGRES_TUPLES_OK)
			{
				if(PQntuples(res))
				{
					TSheet sheet;
					KPVL::SQL::GetSheet(conn, res, 0, sheet);
					PQclear(res);

					//����� ��������
					if(sheet.Cant_at.length())
					{
						time_t td = DataTimeOfString(sheet.Cant_at);
						return td + 5;	//���� 5 �������
					}
					//����� ����� �� ��������
					else if(sheet.InCant_at.length())
					{
						time_t td = DataTimeOfString(sheet.InCant_at);
						return td + 30 * 60;	//���� 30 �����
					}
					//����� ����� ������ �� ����
					else if(sheet.DataTime_End.length())
					{
						time_t td = DataTimeOfString(sheet.DataTime_End);
						return td + 30 * 60;	//���� 30 �����
					}
					//����� �������� ����� �� ������ ����
					else if(sheet.SecondPos_at.length())
					{
						time_t td = DataTimeOfString(sheet.SecondPos_at);
						float TimeHeat = Stof(sheet.TimeForPlateHeat);
						return td + (time_t)TimeHeat * 60 + 30 * 60;	//���� 30 �����, ���� ����� ������������ ��������� �������, ���
					}
					//����� ����� ����� � ����
					else if(sheet.Start_at.length())
					{
						time_t td = DataTimeOfString(sheet.Start_at);
						float TimeHeat = Stof(sheet.TimeForPlateHeat);
						return td + (time_t)TimeHeat * 60 + 40 * 60;	//���� 40 �����, ���� ����� ������������ ��������� �������, ���
					}
					//����� �������� �����
					//else if(sheet.Create_at.length())
					//{ 
						//time_t td = DataTimeOfString(sheet.Create_at);
						//float TimeHeat = Stof(sheet.TimeForPlateHeat);
						//return td + (time_t)TimeHeat * 60 + (60 * 60);	//���� 60 �����, ���� ����� ������������ ��������� �������, ���
					//}
				}
			} else
				LOG_ERR_SQL(SheetLogger, res, command);
			PQclear(res);
		}
		CATCH(SheetLogger, "");
		return 0;
	}

	std::string GetSheetOfBaseString(PGConnection& conn, std::string id)
	{
		try
		{
			std::stringstream sd;
			sd << "SELECT * FROM sheet WHERE id = " << id; //delete_at IS NULL AND 
			std::string command = sd.str();
			PGresult* res = conn.PGexec(command);
			if(PQresultStatus(res) == PGRES_TUPLES_OK)
			{
				if(PQntuples(res))
				{
					TSheet sheet;
					KPVL::SQL::GetSheet(conn, res, 0, sheet);
					PQclear(res);

					//����� ��������
					if(sheet.Cant_at.length())
					{
						time_t td = DataTimeOfString(sheet.Cant_at) + 5;
						return GetStringOfDataTime(&td);
						//return td ;	//���� 5 �������
					}
					//����� ����� �� ��������
					else if(sheet.InCant_at.length())
					{
						time_t td = DataTimeOfString(sheet.InCant_at) + 30 * 60;
						return GetStringOfDataTime(&td);
						//return td + 30 * 60;	//���� 30 �����
					}
					//����� ����� ������ �� ����
					else if(sheet.DataTime_End.length())
					{
						time_t td = DataTimeOfString(sheet.DataTime_End) + 30 * 60;
						return GetStringOfDataTime(&td);
						//return td + 30 * 60;	//���� 30 �����
					}
					//����� �������� ����� �� ������ ����
					else if(sheet.SecondPos_at.length())
					{
						float TimeHeat = Stof(sheet.TimeForPlateHeat);
						time_t td = DataTimeOfString(sheet.SecondPos_at) + (time_t)TimeHeat * 60 + 30 * 60;
						return GetStringOfDataTime(&td);
						//return td + (time_t)TimeHeat * 60 + 30 * 60;	//���� 30 �����, ���� ����� ������������ ��������� �������, ���
					}
					//����� ����� ����� � ����
					else if(sheet.Start_at.length())
					{
						float TimeHeat = Stof(sheet.TimeForPlateHeat);
						time_t td = DataTimeOfString(sheet.Start_at) + (time_t)TimeHeat * 60 + 40 * 60;
						return GetStringOfDataTime(&td);
						//return td + (time_t)TimeHeat * 60 + 40 * 60;	//���� 40 �����, ���� ����� ������������ ��������� �������, ���
					}
				}
			} else
				LOG_ERR_SQL(SheetLogger, res, command);
			PQclear(res);
		}
		CATCH(SheetLogger, "");
		return "";
	}

	//void CorrectSheetDebug(PGConnection& conn, std::string start, std::string id)
	//{
	//	std::string stop = "";
	//	time_t td = DataTimeOfString(start);
	//	time_t ed = GetSheetOfBase(conn, id);
	//	if(ed) td = ed;
	//	else   td += (time_t)((60 * 60 * 1) + (10 * 60));		// + 60 ����� + 10 �����
	//	stop = GetStringOfDataTime(&td);
	//
	//	if(id.length())
	//	{
	//		SetWindowText(hWndDebug, ("������������� ����� id = " + id).c_str());
	//		LOG_INFO(SheetLogger, "������������� ����� id = {}", id);
	//
	//		if(start.length() && stop.length())
	//			SHEET::GetSheets sheet(conn, start, stop);
	//		else
	//			LOG_INFO(SheetLogger, "������ ������������� ����� id = {}, start = {}, stop = {}", id, start, stop);
	//
	//		std::stringstream ssd;
	//		ssd << "UPDATE sheet SET correct = now() WHERE correct IS NULL AND id = " << id;
	//		SETUPDATESQL(SheetLogger, conn, ssd);
	//	}
	//}
	//
//	void CorrectSheetDebug(PGConnection& conn)
//	{
//#ifndef NOSQL
//
//		InitLogger(SheetLogger);
//		try
//		{
//			LOG_INFO(SheetLogger, "�������������� ������������� ������");
//			std::stringstream ssd;
//			ssd << "SELECT id, (start_at - INTERVAL '5 MINUTES') AS start FROM sheet WHERE correct IS NULL AND pos > 6 ORDER BY start_at LIMIT 1;";
//			//ssd << "SELECT id, start_at - INTERVAL '5 MINUTES' FROM sheet WHERE correct >= '14-10-2024 19:53:30' AND correct <= '14-10-2024 20:00:23' AND pos >= 6 ORDER BY id DESC, start_at;";
//			std::string command = ssd.str();
//			if(DEB)LOG_INFO(CassetteLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
//
//			while(isRun)
//			{
//				PGresult* res = conn.PGexec(command);
//				if(PQresultStatus(res) == PGRES_TUPLES_OK)
//				{
//					if(PQntuples(res))
//					{
//						std::string id = conn.PGgetvalue(res, 0, 0);
//						std::string sheet_at = conn.PGgetvalue(res, 0, 1);
//						PQclear(res);
//						//if(Stoi(id) > 20000)
//						CorrectSheetDebug(conn, sheet_at, id);
//						continue;
//					}
//				}
//				else
//					LOG_ERR_SQL(CassetteLogger, res, command);
//				PQclear(res);
//				break;
//			}
//
//			LOG_INFO(SheetLogger, "��������� �������������� ������������� ������");
//
//		}
//		CATCH(SheetLogger, "");
//
//#endif
//
//	}

	void DbugPdf(PGConnection& conn)
	{
		std::string deb = "SELECT * FROM sheet WHERE (pdf = '' AND cassette <> '0' "
			"AND (SELECT correct FROM cassette WHERE cassette.id = sheet.cassette AND pdf IS NOT NULL) IS NOT NULL ) " 
			#ifdef _ReleaseD
			"OR id = 29271 "
			#endif
			"ORDER BY start_at DESC;";
		std::deque<TSheet>MasSheet;
		PGresult* res = conn.PGexec(deb);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			for(int l = 0; l < line; l++)
			{
				TSheet sheet;
				KPVL::SQL::GetSheet(conn, res, l, sheet);
				MasSheet.push_back(sheet);
				if(!isRun) return;
			}
		}
		else
			LOG_ERR_SQL(CorrectLog, res, deb);
		PQclear(res);

		for(auto& Sheet : MasSheet)
		{
			std::string out = "����: " + Sheet.id;
			SetWindowText(hWndDebug, out.c_str());

			PASSPORT::PdfClass t = PASSPORT::PdfClass(Sheet, false);
		}

	}

	void SetDefCorrect(PGConnection& conn, int id)
	{
		try{

			std::time_t st = time(NULL);
			tm curr_tm;
			localtime_s(&curr_tm, &st);

			std::stringstream as;
			as << "UPDATE sheet SET correct = '2000-";//01-01 00:00:00' ";
			as << std::setw(2) << std::setfill('0') << (curr_tm.tm_mon + 1) << "-";
			as << std::setw(2) << std::setfill('0') << curr_tm.tm_mday << " ";
			as << std::setw(2) << std::setfill('0') << curr_tm.tm_hour << ":";
			as << std::setw(2) << std::setfill('0') << curr_tm.tm_min << ":";
			as << std::setw(2) << std::setfill('0') << curr_tm.tm_sec;
			as << "' WHERE correct IS NULL AND pos > 6 AND id = " << id;
			LOG_INFO(SheetLogger, "{:90}| {}", FUNCTION_LINE_NAME, as.str());

			SETUPDATESQL(SheetLogger, conn, as);
		}
		CATCH(SheetLogger, "");
	}

	DWORD CorrectSheet(LPVOID)
	{
		InitLogger(SheetLogger);
	
		if(isCorrectSheet) return 0;
		isCorrectSheet = true;
	
		try
		{
			PGConnection conn;
			CONNECTION1(conn, SheetLogger);
			std::string start = "";
			std::string stop = "";
			int id = 0;
			do{
				//std::string start = "2024-11-21 06:35:47";
				using namespace SHEET;
				GetStartTime(conn, id, start, stop);

				if(start.length())
				{
					try
					{
						if(start.length())
						{
							SetWindowText(hWndDebug, ("��������� ������: " + GetStringDataTime()).c_str());
							GetSheets sheets(conn, id, start, stop);

							SetWindowText(hWndDebug, ("������� � ��������: " + GetStringDataTime()).c_str());
							if(id)
								SetDefCorrect(conn, id);
						}
					}
					CATCH(SheetLogger, "");
				}
			} while(start.length());

			DbugPdf(conn);
		}
		CATCH(SheetLogger, "");
	
		isCorrectSheet = false;
	
		return 0;
	}


	DWORD CorrectCassette(LPVOID)
	{
		InitLogger(CassetteLogger);
	
		if(isCorrectCassette) return 0;
	
		isCorrectCassette = true;
		try
		{
			PGConnection conn;
			CONNECTION1(conn, CassetteLogger);
			std::string start = "";
			std::string stop = "";
			int Peth = 0;
			int Id = 0;
			using namespace CASSETTE;

			do{
				Id = GetStartTime(conn, Peth, start, stop);

				if(Peth && start.length() && stop.length())
					GetCassettes cass(conn, Id, Peth, start, stop);
			} while(Peth && start.length() && stop.length());

			//��� ��� ���������
			HendCassettePDF(conn);
		}
		CATCH(CassetteLogger, "");
	
		isCorrectCassette = false;
		SetWindowText(hWndDebug, ("��������� ��������� ������: " + GetStringDataTime()).c_str());
		return 0;
	}

	DWORD CorrectCassetteFinal(LPVOID lp)
	{
		InitLogger(CassetteLogger);

		if(isCorrectCassette) return 0;

		isCorrectCassette = true;
		try
		{

			TCassette* it = (TCassette*)lp;
			if(it)
			{
				int Peth = Stoi(it->Peth);
				int Id = Stoi(it->Id);
				std::string start = it->Return_at;
				std::string stop = start;

				if(Peth && start.length())
				{
					PGConnection conn;
					CONNECTION1(conn, CassetteLogger);

					using namespace CASSETTE;
					CorreatStart(conn, Peth, start);
					CorreatStop(conn, Peth, stop);
					GetCassettes cass(conn, Id, Peth, start, stop);
					
					//��� ��� ���������
					HendCassettePDF(conn);
				}
			}
		}
		CATCH(CassetteLogger, "");

		isCorrectCassette = false;
		SetWindowText(hWndDebug, ("��������� ��������� ������: " + GetStringDataTime()).c_str());
		return 0;
	}

	//����� �������������� �������������
	DWORD WINAPI RunCassettelPdf(LPVOID)
	{
		try
		{
			InitLogger(CorrectLog);
			//PGConnection conn;
			//CONNECTION1(conn, CorrectLog);
#if HENDINSERT
			//��� ������� ������������
			#if !_DEBUG
			#error HENDINSERT � ������!!!.
			//("� ������!!!");
			#endif
			//SetWindowText(hWndDebug, "���������");

			//PGConnection conn;
			//CONNECTION1(conn, CorrectLog);
			
			//std::string start = "2025-01-04 07:20:00"; //29-12-2024 00:00:00
			//std::string stop =  "2025-01-04 13:00:00";
			//SHEET::GetSheets sheets(conn, start, stop);
			//26-12-2024 03:47:43
			// 
			//CASSETTE::GetCassettes cass("", "");
			//CorrectSheet(0);
			//std::stringstream ssd;
			//ssd << "UPDATE cassette SET pdf = DEFAULT WHERE run_at >= '" << start << "' AND ;";
			//SETUPDATESQL(CorrectLog, conn, ssd);
			//CASSETTE::HendCassettePDF(conn);


			//CASSETTE::GetCassettes cass(start, stop);
			//CASSETTE::GetCassettes cass("", "");
			
			//CorrectCassette(0);

			//CorrectSheetDebug(conn);
			//SetWindowText(hWndDebug, "��������");
			isRun = false;
			return 0;

#else
				std::string out1 = "���� � �������� ���������: " + GetStringDataTime();
				LOG_INFO(CorrectLog, "{:90}| {}", FUNCTION_LINE_NAME, out1);
				bool OldNotCorrect = NotCorrect;
				while(isRun)
				{
					if(!NotCorrect)
					{

						std::string out = "������ ��������� ������: " + GetStringDataTime();
						SetWindowText(hWndDebug, out.c_str());

						CorrectSheet(0);

						out = "������ �������� ���������: " + GetStringDataTime();
						SetWindowText(hWndDebug, out.c_str());

						CorrectCassette(0);
						out = "��������� �������� ���������: " + GetStringDataTime();
						SetWindowText(hWndDebug, out.c_str());

#ifdef _ReleaseD
						isRun = false;
#endif

#ifdef _DEBUG
						//� ������ ���� ������ � ����� �� ���������
						isRun = false;
					}
#else
					}
					int f = (NotCorrect ? 10 : 60 * 1); //10 ������ ��� 1 ������
					while(isRun && --f > 0 /*&& OldNotCorrect == NotCorrect*/)
					{
						if(!isCorrectCassette && !isCorrectSheet)
							SetWindowText(hWndDebug, ("��������� �������� ���������: ������ " + std::to_string(f) + " ���.").c_str());

						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					}
#endif // _DEBUG
					OldNotCorrect = NotCorrect;
				}

				PDF::Correct = false;
				LOG_INFO(CorrectLog, "{:90}| Stop Pdf Debug", FUNCTION_LINE_NAME);
#endif
		}
		CATCH(CorrectLog, "");

		std::string out2 = "����� �� �������� ���������: " + GetStringDataTime();
		LOG_INFO(CorrectLog, "{:90}| {}", FUNCTION_LINE_NAME, out2);
		SetWindowText(hWndDebug, out2.c_str());
		return 0;
	}
}

HANDLE hRunAllPdf = NULL;
void Open_PDF()
{
	hRunAllPdf = CreateThread(0, 0, PDF::RunCassettelPdf, (LPVOID)0, 0, 0);
}

void Close_PDF()
{
    WaitCloseTheread(hRunAllPdf, "hRunAllPdf");
}
