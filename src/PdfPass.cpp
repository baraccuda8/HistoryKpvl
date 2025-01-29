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

std::shared_ptr<spdlog::logger> PdfLog = NULL;

extern GUID guidBmp;
extern GUID guidJpeg;
extern GUID guidGif;
extern GUID guidTiff;
extern GUID guidPng;

extern Gdiplus::Font font1;
extern Gdiplus::Font font2;

extern std::string lpLogPdf;

namespace PDF
{
	//Перечисление дирректорий. Возвращает только файлы формата
	std::vector<boost::filesystem::path> getDir (const std::string lpDir)
	{
		std::vector<boost::filesystem::path> paths = getDirContents (lpDir);
		paths.erase(std::remove_if(paths.begin(), paths.end(),
					[&, lpDir](boost::filesystem::path path)
		{
			return !boost::regex_match(path.generic_string(), boost::regex(lpDir + "/*.*"));
		}), paths.end());

		return paths;
	}


	void TodosColumn(PGresult* res)
	{
		if(TODOS::type == 0)
		{
			int nFields = PQnfields(res);
			for(int j = 0; j < nFields; j++)
			{
				std::string l =  utf8_to_cp1251(PQfname(res, j));
				if(l == "create_at") TODOS::create_at = j;
				else if(l == "id") TODOS::id = j;
				else if(l == "id_name") TODOS::id_name = j;
				else if(l == "content") TODOS::content = j;
				else if(l == "type") TODOS::type = j;
				else if(l == "name") TODOS::name = j;
			}
		}
	}

	OpcUa::Variant GetVarVariant(OpcUa::VariantType Type, std::string value)
	{
		if(Type == OpcUa::VariantType::BOOLEAN)        return (bool)(value == "true");
		else if(Type == OpcUa::VariantType::SBYTE)     return int8_t(atoi_t(int8_t, atoi, value));
		else if(Type == OpcUa::VariantType::BYTE)      return uint8_t(atoi_t(uint8_t, atoi, value));
		else if(Type == OpcUa::VariantType::INT16)     return int16_t(atoi_t(int16_t, atoi, value));
		else if(Type == OpcUa::VariantType::UINT16)    return uint16_t(atoi_t(uint16_t, atoi, value));
		else if(Type == OpcUa::VariantType::INT32)     return int32_t(atoi_t(int32_t, atol, value));
		else if(Type == OpcUa::VariantType::UINT32)    return uint32_t(atoi_t(uint32_t, atol, value));
		else if(Type == OpcUa::VariantType::INT64)     return int64_t(atoi_t(int64_t, atoll, value));
		else if(Type == OpcUa::VariantType::UINT64)    return int64_t(atoi_t(uint64_t, atoll, value));
		else if(Type == OpcUa::VariantType::FLOAT)     return float(atoi_t(float, atof, value));
		else if(Type == OpcUa::VariantType::DOUBLE)    return double(atoi_t(double, atof, value));
		else if(Type == OpcUa::VariantType::STRING)    return /*utf8_to_cp1251*/(value);
		return OpcUa::Variant();
	}

	bool cmpData(T_Todos& first, T_Todos& second)
	{
		return first.create_at < second.create_at;
	}

	void GetTodosSQL(PGConnection& conn, MapTodos& mt, std::string& command, int p)
	{

		//if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		PGresult* res = conn.PGexec(command);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int nFields = PQnfields(res);
			int line = PQntuples(res);
			if(line)
			{
				//float f = static_cast<float>(atof(conn.PGgetvalue(res, 0, 1).c_str()));
				for(int l = 0; l < line; l++)
				{
					T_Todos td;
					td.create_at = conn.PGgetvalue(res, l, 0);
					td.id = Stoi(conn.PGgetvalue(res, l, 1));
					td.id_name = Stoi(conn.PGgetvalue(res, l, 2));
					td.value = conn.PGgetvalue(res, l, 3);
					int type = Stoi(conn.PGgetvalue(res, l, 4));
					td.content = PDF::GetVarVariant((OpcUa::VariantType)type, td.value);
					td.Petch = p;
					//if(nFields >= 6)
					//	td.id_name_at = conn.PGgetvalue(res, l, 5);

					mt.push_back(td);
				}
			}
		}
		PQclear(res);
	}
	void OutDebugInfo(TCassette& Cassette, TSheet& Sheet)
	{
		try
		{
			std::stringstream sss;
			sss << "Печь №" << Cassette.Peth << "   "
				<< "Дата: " << Cassette.Run_at << "   "
				<< "Кассета: "
				<< std::setw(4) << std::setfill('0') << Cassette.Year << "-"
				<< std::setw(2) << std::setfill('0') << Cassette.Month << "-"
				<< std::setw(2) << std::setfill('0') << Cassette.Day << " "
				<< std::setw(2) << std::setfill('0') << Cassette.Hour << " № "
				<< std::setw(2) << std::setfill('0') << Cassette.CassetteNo << "   "
				<< "Лист: "
				<< "Дата старт: " << Sheet.Start_at << "   "
				<< std::setw(6) << std::setfill('0') << Sheet.Melt << "-"
				<< std::setw(2) << std::setfill('0') << Sheet.Slab << "-"
				<< std::setw(3) << std::setfill('0') << Sheet.PartNo << "-"
				<< std::setw(3) << std::setfill('0') << Sheet.Pack << "-"
				<< std::setw(3) << std::setfill('0') << Sheet.Sheet << "/"
				<< std::setw(2) << std::setfill('0') << Sheet.SubSheet

				;
			SetWindowText(hWndDebug, sss.str().c_str());
		}CATCH(PdfLog, "");
	}

	namespace PASSPORT
	{
		#pragma region draw_pdf
		jmp_buf env;


		void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data)
		{
			LOG_ERROR(PdfLog, "{:90}| ERROR: error_no={}, detail_no={}", FUNCTION_LINE_NAME, (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
			//longjmp(env, 1);
		}
		void draw_text(HPDF_Page page, double x, double y, std::string label)
		{
			HPDF_Page_BeginText (page);
			HPDF_Page_MoveTextPos (page, HPDF_REAL(x + 5), HPDF_REAL(y + 6));
			HPDF_Page_ShowText (page, cp1251_to_utf8(label).c_str());
			HPDF_Page_EndText (page);
		}
		void draw_text_rect(HPDF_Page page, double x, double y, double w, double h, std::string label)
		{
			draw_text(page, x, y, label);
			HPDF_Page_Rectangle(page, HPDF_REAL(x), HPDF_REAL(y), HPDF_REAL(w), HPDF_REAL(h));
			HPDF_Page_Stroke (page);
		}
		void draw_text_rect(HPDF_Page page, double x, double y, double w, double h, float label)
		{
			//std::stringstream ss;
			//ss << boost::format("%.1f") % label;
			draw_text(page, x, y, (boost::format("%.1f") % label).str());
			HPDF_Page_Rectangle(page, HPDF_REAL(x), HPDF_REAL(y), HPDF_REAL(w), HPDF_REAL(h));
			HPDF_Page_Stroke (page);
		}
		void draw_text_rect(HPDF_Page page, double x, double y, double w, double h, int label)
		{
			draw_text(page, x, y, (boost::format("%d") % label).str());
			HPDF_Page_Rectangle(page, HPDF_REAL(x), HPDF_REAL(y), HPDF_REAL(w), HPDF_REAL(h));
			HPDF_Page_Stroke (page);
		}
		void draw_rect(HPDF_Page page, double x, double y, double w, double h)
		{
			HPDF_Page_Rectangle(page, HPDF_REAL(x), HPDF_REAL(y), HPDF_REAL(w), HPDF_REAL(h));
			HPDF_Page_Stroke (page);
		}
		#pragma endregion

		std::map <int, std::string> MonthName{
			{1, "January"},
			{2, "February"},
			{3, "March"},
			{4, "April"},
			{5, "May"},
			{6, "June"},
			{7, "July"},
			{8, "August"},
			{9, "September"},
			{10, "October"},
			{11, "November"},
			{12, "December"},
		};

		void PdfClass::GetCassette(TCassette& cassette, TSheet& Sheet)
		{
			try
			{
				cassette = TCassette();

				std::stringstream sd;
				sd << "SELECT * FROM cassette WHERE";
				sd << " year = " << Stoi(Sheet.Year);
				sd << " AND month = " << Stoi(Sheet.Month);
				sd << " AND day = " << Stoi(Sheet.Day);
				//if(Stoi(Sheet.Year) >= 2024 && Stoi(Sheet.Month) >= 8)
				sd << " AND hour = " << Sheet.Hour;

				sd << " AND cassetteno = " << Sheet.CassetteNo;
				sd << " AND delete_at IS NULL";
				sd << " ORDER BY run_at DESC;";

				std::string command = sd.str();
				if(DEB)LOG_INFO(PdfLog, "{:90}| {}", FUNCTION_LINE_NAME, command);
				PGresult* res = conn.PGexec(command);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					if(conn.PQntuples(res))
						S107::GetCassette(conn, res, cassette, 0);
				} else
					LOG_ERR_SQL(PdfLog, res, command);
				PQclear(res);
			}CATCH(PdfLog, "");
		}

		void PdfClass::GetSheet(TCassette& Cassette)
		{
			try
			{
				AllPfdSheet.erase(AllPfdSheet.begin(), AllPfdSheet.end());
				std::stringstream sd;
				sd << "SELECT * FROM sheet WHERE "; //delete_at IS NULL AND 

				//if(Stoi(Cassette.Year) >= 2024 && Stoi(Cassette.Month) >= 8)
				sd << "hour = " << Stoi(Cassette.Hour) << " AND ";

				sd << "day = '" << Stoi(Cassette.Day) << "' AND ";
				sd << "month = '" << Stoi(Cassette.Month) << "' AND ";
				sd << "year = '" << Stoi(Cassette.Year) << "' AND ";
				sd << "cassetteno = " << Stoi(Cassette.CassetteNo) << " AND ";
				sd << "delete_at IS NULL ";
				sd << "ORDER BY start_at DESC;";
				std::string command = sd.str();
				LOG_INFO(PdfLog, "{:90}| {}", FUNCTION_LINE_NAME, command);
				PGresult* res = conn.PGexec(command);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					for(int l = 0; l < line; l++)
					{
						TSheet sheet;
						KPVL::SQL::GetSheet(conn, res, l, sheet);
						AllPfdSheet.push_back(sheet);
						if(!isRun)
						{
							PQclear(res);
							return;
						}
					}
				} else
					LOG_ERR_SQL(PdfLog, res, command);
				PQclear(res);
			}CATCH(PdfLog, "");
		}

		float PdfClass::GetStartTime(std::string start, int ID)
		{
			float fTemp = 0;
			std::tm TM_Temp = {0};
			//std::string sBegTime2 = Start;
			std::stringstream sde;
			sde << "SELECT content FROM todos WHERE id_name = " << ID;
			sde << " AND create_at < '" << start << "'";
			sde << " ORDER BY create_at DESC LIMIT 1;";

			std::string command = sde.str();
			PGresult* res = conn.PGexec(command);
			if(PQresultStatus(res) == PGRES_TUPLES_OK)
			{
				if(PQntuples(res))
				{
					fTemp = Stof(conn.PGgetvalue(res, 0, 0));
				}
			} else LOG_ERR_SQL(PdfLog, res, command);
			PQclear(res);
			return fTemp;
		}

		void PdfClass::GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID)
		{
			try
			{
				if(!Start.length()) throw std::exception(__FUN("Ошибка даты Start = 0"));
				if(!Stop.length())  throw std::exception(__FUN("Ошибка даты Stop = 0"));

				//std::string start = Start;
				float fTemp = GetStartTime(Start, ID);
				time_t t1 = DataTimeOfString(Start);

				tr[Start] = std::pair(0, fTemp);

				#pragma region MyRegion

				std::stringstream sdt;
				sdt << "SELECT create_at, content FROM todos WHERE id_name = " << ID;
				sdt << " AND create_at >= '" << Start << "'";
				sdt << " AND create_at <= '" << Stop << "'";
				sdt << " ORDER BY create_at ASC;";
				#pragma endregion

				#pragma region SQL
				std::string command = sdt.str();
				PGresult* res = conn.PGexec(command);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					if(line)
					{
						for(int l = 0; l < line; l++)
						{
							std::string sData = conn.PGgetvalue(res, l, 0);
							fTemp = Stof(conn.PGgetvalue(res, l, 1));
							if(fTemp > 0.f && fTemp <= 999.f)
								tr[sData] = std::pair(int(difftime(DataTimeOfString(sData), t1)), fTemp);
						}
					}
				} else LOG_ERR_SQL(PdfLog, res, command);
				PQclear(res);
				#pragma endregion

				tr[Stop] = std::pair(int(difftime(DataTimeOfString(Stop), t1)), fTemp);

			}CATCH(PdfLog, "");
		}

		void PdfClass::SqlTempActKPVL1(std::string Stop, T_fTemp& fT)
		{
			try
			{
				std::stringstream sdt;
				sdt << "SELECT DISTINCT ON(id_name) id_name, create_at, content FROM todos WHERE (";

				sdt << "id_name = " << Hmi210_1.Htr2_1->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_2->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_3->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_4->ID;

				sdt << ") AND create_at <= '" << Stop;
				sdt << "' ORDER BY id_name DESC, create_at ASC;";
				std::string command = sdt.str();
				if(DEB)LOG_INFO(PdfLog, "{:90}| {}", FUNCTION_LINE_NAME, command);
				PGresult* res = conn.PGexec(command);

				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					for(int l = 0; l < line; l++)
					{
						PFS pfs;
						int id_name = Stoi(conn.PGgetvalue(res, l, 0));
						pfs.data = conn.PGgetvalue(res, l, 1);
						pfs.temper = Stof(conn.PGgetvalue(res, l, 2));

						if(id_name == Hmi210_1.Htr2_1->ID) fT.t1 = pfs.temper;
						if(id_name == Hmi210_1.Htr2_2->ID) fT.t2 = pfs.temper;
						if(id_name == Hmi210_1.Htr2_3->ID) fT.t3 = pfs.temper;
						if(id_name == Hmi210_1.Htr2_4->ID) fT.t4 = pfs.temper;
					}
				} else
					LOG_ERR_SQL(PdfLog, res, command);
				PQclear(res);
				fT.t0 = (fT.t1 + fT.t2 + fT.t3 + fT.t4) / 4.0f;
			}CATCH(PdfLog, "");
		}

		void PdfClass::SqlTempActKPVL2(std::string Stop, T_fTemp& fT)
		{
			try
			{
				std::stringstream sdt;
				sdt << "SELECT DISTINCT ON(id_name) id_name, create_at, content FROM todos WHERE (";

				sdt << "id_name = " << Hmi210_1.Htr1_1->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_2->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_3->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_4->ID;

				sdt << ") AND create_at <= '" << Stop;
				sdt << "' ORDER BY id_name DESC, create_at ASC;";
				std::string command = sdt.str();
				if(DEB)LOG_INFO(PdfLog, "{:90}| {}", FUNCTION_LINE_NAME, command);
				PGresult* res = conn.PGexec(command);

				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					for(int l = 0; l < line; l++)
					{
						PFS pfs;
						int id_name = Stoi(conn.PGgetvalue(res, l, 0));
						pfs.data = conn.PGgetvalue(res, l, 1);
						pfs.temper = Stof(conn.PGgetvalue(res, l, 2));

						if(id_name == Hmi210_1.Htr1_1->ID) fT.t1 = pfs.temper;
						if(id_name == Hmi210_1.Htr1_2->ID) fT.t2 = pfs.temper;
						if(id_name == Hmi210_1.Htr1_3->ID) fT.t3 = pfs.temper;
						if(id_name == Hmi210_1.Htr1_4->ID) fT.t4 = pfs.temper;
					}
				} else
					LOG_ERR_SQL(PdfLog, res, command);
				PQclear(res);
				fT.t0 = (fT.t1 + fT.t2 + fT.t3 + fT.t4) / 4.0f;
			}CATCH(PdfLog, "");
		}


		void PdfClass::SqlTempActKPVL1(std::string Start, std::string Stop, VPFS& pF1, int Temperature)
		{
			try
			{
				T_fTemp fT;
				SqlTempActKPVL1(Start, fT);
				PFS pfs;
				pfs.data = Start;

				pfs.temper = fT.t1;
				pF1.push_back(pfs);
				pfs.temper = fT.t2;
				pF1.push_back(pfs);
				pfs.temper = fT.t3;
				pF1.push_back(pfs);
				pfs.temper = fT.t4;
				pF1.push_back(pfs);

				std::stringstream sdt;
				sdt << "SELECT id_name, create_at, content FROM todos WHERE (";
				sdt << "id_name = " << Hmi210_1.Htr2_1->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_2->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_3->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_4->ID;
				sdt << ") AND create_at >= '" << Start;
				sdt << "' AND create_at < '" << Stop;
				sdt << "' ORDER BY create_at ASC;";

				std::string command = sdt.str();
				if(DEB)LOG_INFO(PdfLog, "{:90}| {}", FUNCTION_LINE_NAME, command);
				PGresult* res = conn.PGexec(command);

				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					if(line)
					{
						if(!Temperature) SrTemp = 0.0f;

						for(int l = 0; l < line; l++)
						{
							PFS pfs;
							int id_name = Stoi(conn.PGgetvalue(res, l, 0));
							pfs.data = conn.PGgetvalue(res, l, 1);
							pfs.temper = Stof(conn.PGgetvalue(res, l, 2));

							if(id_name == Hmi210_1.Htr2_1->ID) pF1.push_back(pfs);
							else if(id_name == Hmi210_1.Htr2_2->ID) pF1.push_back(pfs);
							else if(id_name == Hmi210_1.Htr2_3->ID) pF1.push_back(pfs);
							else if(id_name == Hmi210_1.Htr2_4->ID) pF1.push_back(pfs);
						}
					}
				} else
					LOG_ERR_SQL(PdfLog, res, command);
				PQclear(res);
			}CATCH(PdfLog, "");
		}

		void PdfClass::SqlTempActKPVL2(std::string Start, std::string Stop, VPFS& pF2, int Temperature)
		{
			try
			{
				T_fTemp fT;
				SqlTempActKPVL2(Start, fT);
				PFS pfs;
				pfs.data = Start;

				pfs.temper = fT.t1;
				pF2.push_back(pfs);
				pfs.temper = fT.t2;
				pF2.push_back(pfs);
				pfs.temper = fT.t3;
				pF2.push_back(pfs);
				pfs.temper = fT.t4;
				pF2.push_back(pfs);

				std::stringstream sdt;
				sdt << "SELECT id_name, create_at, content FROM todos WHERE (";
				sdt << "id_name = " << Hmi210_1.Htr1_1->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_2->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_3->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_4->ID;
				sdt << ") AND create_at >= '" << Start;
				sdt << "' AND create_at < '" << Stop;
				sdt << "' ORDER BY create_at ASC;";


				std::string command = sdt.str();
				if(DEB)LOG_INFO(PdfLog, "{:90}| {}", FUNCTION_LINE_NAME, command);
				PGresult* res = conn.PGexec(command);

				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					if(line)
					{
						if(!Temperature) SrTemp = 0.0f;


						PFS vpfs[4];
						for(int l = 0; l < line; l++)
						{
							PFS pfs;
							int id_name = Stoi(conn.PGgetvalue(res, l, 0));
							pfs.data = conn.PGgetvalue(res, l, 1);
							pfs.temper = Stof(conn.PGgetvalue(res, l, 2));

							if(id_name == Hmi210_1.Htr1_1->ID) { vpfs[0] = pfs; pF2.push_back(pfs); } else if(id_name == Hmi210_1.Htr1_2->ID) { vpfs[1] = pfs; pF2.push_back(pfs); } else if(id_name == Hmi210_1.Htr1_3->ID) { vpfs[2] = pfs; pF2.push_back(pfs); } else if(id_name == Hmi210_1.Htr1_4->ID) { vpfs[3] = pfs; pF2.push_back(pfs); }

						}
						vpfs[0].data = Stop;
						vpfs[1].data = Stop;
						vpfs[2].data = Stop;
						vpfs[3].data = Stop;
						pF2.push_back(vpfs[0]);
						pF2.push_back(vpfs[1]);
						pF2.push_back(vpfs[2]);
						pF2.push_back(vpfs[3]);
					}
				} else
					LOG_ERR_SQL(PdfLog, res, command);
				PQclear(res);
			}CATCH(PdfLog, "");
		}

		void PdfClass::GetSrTemper(std::vector<PFS>& pF, std::map<int, PFS>& mF)
		{
			std::tm TM;
			if(pF.size())
			{
				auto data = pF.begin()->data;
				time_t tS1 = DataTimeOfString(data, TM);

				int oldStep = 0;
				for(auto& a : pF)
				{
					a.sec = int(difftime(DataTimeOfString(a.data), tS1));

					int st = a.sec / StepSec;
					if(oldStep == st)
					{
						if(a.temper > 0)
						{
							mF[st].temper += a.temper;
							mF[st].data = a.data;
							mF[st].count++;
							mF[st].sec = a.sec;
						}
					} else
					{
						if(a.temper > 0)
						{
							oldStep = st;
							mF[st].temper += a.temper;
							mF[st].data = a.data;
							mF[st].count = 1;
							mF[st].sec = a.sec;
						}
					}
				}
				for(auto& a : mF)
					if(a.second.count)
						a.second.temper /= a.second.count;

			}
		}

		void PdfClass::UpdateTemperature(TSheet& Sheet)
		{
			try
			{
				float Htr1_1 = 0;
				float Htr1_2 = 0;
				float Htr1_3 = 0;
				float Htr1_4 = 0;

				MapTodos DataSheetTodos;
				std::stringstream ssd;
				ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
				ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name)";
				//ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name)";
				ssd << " FROM todos WHERE";
				ssd << " create_at > '" << Sheet.Start_at << "' AND";
				ssd << " create_at < '" << Sheet.DataTime_End << "' AND ";
				ssd << "(id_name = " << Hmi210_1.Htr1_1->ID;
				ssd << " OR id_name = " << Hmi210_1.Htr1_2->ID;
				ssd << " OR id_name = " << Hmi210_1.Htr1_3->ID;
				ssd << " OR id_name = " << Hmi210_1.Htr1_4->ID;
				ssd << ") ORDER BY id_name, create_at DESC;";

				std::string command = ssd.str();
				GetTodosSQL(conn, DataSheetTodos, command);

				for(auto a : DataSheetTodos)
				{
					if(a.id_name == Hmi210_1.Htr1_1->ID) Htr1_1 = a.content.As<float>();
					if(a.id_name == Hmi210_1.Htr1_2->ID) Htr1_2 = a.content.As<float>();
					if(a.id_name == Hmi210_1.Htr1_3->ID) Htr1_3 = a.content.As<float>();
					if(a.id_name == Hmi210_1.Htr1_4->ID) Htr1_4 = a.content.As<float>();
				}

				float Temperature = (Htr1_1 + Htr1_2 + Htr1_3 + Htr1_4) / 4.0f;
				if(Temperature != 0)
				{
					Sheet.Temperature = std::to_string(Temperature);
					std::ostringstream oss;
					oss << std::setprecision(0) << std::fixed << Temperature;
					KPVL::Sheet::SetUpdateSheet(conn, Sheet, " temperature = " + oss.str(), "");

					Sheet.Temperature = oss.str();
				}
			}CATCH(PdfLog, "");

		}

		void PdfClass::SqlTempActKPVL(T_SqlTemp& tr, TSheet& Sheet)
		{
			try
			{
				tr.erase(tr.begin(), tr.end());
				std::string Pos1 = Sheet.Start_at;
				std::string Pos2 = Sheet.SecondPos_at;
				std::string Pos3 = Sheet.DataTime_End;

				if(Pos1.length() < 1) return;
				if(Pos3.length() < 1)return;


				std::vector<PFS>pF1;
				std::vector<PFS>pF2;

				std::map<int, PFS>mF1;
				std::map<int, PFS>mF2;

				SqlTempActKPVL1(Pos1, Pos2, pF1, Stoi(Sheet.Temperature));
				SqlTempActKPVL2(Pos2, Pos3, pF2, Stoi(Sheet.Temperature));

				GetSrTemper(pF1, mF1);
				GetSrTemper(pF2, mF2);

				time_t t1 = DataTimeOfString(Pos1); //mF1.begin()->second.data);
				for(auto a : mF1)
				{
					if(a.second.temper > 0 && a.second.temper < 999.0f)
					{
						int t = int(difftime(DataTimeOfString(a.second.data), t1));
						tr[a.second.data] = std::pair(t, a.second.temper);
					}
				}

				//time_t t2 = DataTimeOfString(mF2.begin()->second.data);
				for(auto a : mF2)
				{
					if(a.second.temper > 0 && a.second.temper < 999.0f)
					{
						int t = int(difftime(DataTimeOfString(a.second.data), t1));
						tr[a.second.data] = std::pair(t, a.second.temper);
					}
				}
				if(Stof(Sheet.Temperature) == 0)
					UpdateTemperature(Sheet);
			}CATCH(PdfLog, "");
		}

		void PdfClass::DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat)
		{
			Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));

			std::wstring::const_iterator start = str.begin();
			std::wstring::const_iterator end = str.end();
			boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}):\\d{1,2}.*");
			boost::match_results<std::wstring::const_iterator> what;

			if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 1)
				temp.DrawString(what[1].str().c_str(), -1, &font1, Rect, &stringFormat, &Gdi_brush);
		}


		void PdfClass::DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, float sd, std::wstring sDataBeg)
		{
			Gdiplus::RectF RectText(Rect);

			Gdiplus::RectF boundRect;
			temp.MeasureString(sDataBeg.c_str(), 5, &font1, RectText, &stringFormat, &boundRect);

			boundRect.X = Rect.X + float(sd);
			boundRect.X -= boundRect.Width / 2;
			boundRect.Height += 3;

			//temp.DrawRectangle(&pen, boundRect);

			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
			temp.DrawString(sDataBeg.c_str(), 5, &font1, boundRect, &stringFormat, &Gdi_brush);
		}


		void PdfClass::DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st)
		{
			Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 1); //Черный
			Gdiplus::Pen Gdi_L2(clor, 1);
			auto b = st.begin();

			double coeffW = (double)(Rect.Width) / double(maxd - mind);
			double coeffH = (double)(Rect.Height - Rect.Y) / (double)(f_maxt - f_mint);

			Gdiplus::PointF p1;
			Gdiplus::PointF p2;
			p1.X = Rect.X;
			p1.Y = Rect.Y + float((f_maxt - b->second.second) * coeffH);

			Gdiplus::GraphicsPath path;
			path.StartFigure();
			for(auto& a : st)
			{
				//if(a.second.second >= 0.0f && a.second.second < 999.0f)
				{
					p2.X = Rect.X + float((a.second.first - mind) * coeffW);
					p2.Y = Rect.Y + float((f_maxt - a.second.second) * coeffH);
					path.AddLine(p1, p2);
					p1 = p2;
				}
			}
			temp.DrawPath(&Gdi_L2, &path);
			path.Reset();
		}

		void PdfClass::DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG, std::wstring s1, std::wstring s2)
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

			Gdiplus::RectF Rect1 = RectG;

			temp.TranslateTransform(0, RectG.Height);
			temp.RotateTransform(-90);

			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

			//std::wstring theString = L"Температура С°";

			Gdiplus::RectF boundRect = {0, 0, RectG.Height + 5, 20};
			temp.DrawString(s1.c_str(), -1, &font1, boundRect, &stringFormat, &Gdi_brush);
			temp.ResetTransform();

			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
			//theString = L"Время час:мин";
			temp.DrawString(s2.c_str(), -1, &font1, Rect1, &stringFormat, &Gdi_brush);

			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}

		void PdfClass::DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act, T_SqlTemp& Ref)
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);

			double coeffH = (double)(Rect.Height - Rect.Y) / (double)(f_maxt - f_mint);

			for(double d = f_mint; d <= f_maxt; d += f_step)
			{
				float mY = Rect.Y + float((f_maxt - d) * coeffH);
				Gdiplus::PointF pt1 = {Rect.X - 5,				mY};
				Gdiplus::PointF pt2 = {Rect.X + Rect.Width,	mY};
				temp.DrawLine(&Gdi_L1, pt1, pt2);

				Gdiplus::RectF Rect2 = {Rect.X - 45, mY - 11, 40, 20};

				std::wstringstream sdw;
				sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << d;

				temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);

			}
			int tt = 0;
		}

		void PdfClass::DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, int msec)
		{
			float W = float(msec) / Rect.Width;

			float h1 = 60.0f;
			float h2 = 60.0f * 5.0f;
			float h3 = 60.0f;

			if((msec / 60) > 60)
			{
				h1 = 60.0f * 15.0f;
				h2 = 60.0f * 60.0f;
				h3 = 60.0f * 60.0f;
			}

			for(float e = 0; e <= Rect.Width; e++)
			{
				float sd = W * e;
				bool b1 = std::fmod(sd, h1) < W;
				bool b2 = std::fmod(sd, h2) < W;

				if(b1 /*&& (e + 15) < Rect.Width*/)
				{
					Gdiplus::PointF pt1 = {Rect.X + float(e), Rect.Y};
					Gdiplus::PointF pt2 = {Rect.X + float(e), Rect.Height - (b2 ? 10 : 18)};
					if(b2)
						temp.DrawLine(&Gdi_D1, pt1, pt2);
					else
						temp.DrawLine(&Gdi_L1, pt1, pt2);
				}
				if(b2)
				{
					{
						std::wstringstream sdw;
						sdw << std::fixed << std::setprecision(0) << (sd / h3);
						std::wstring sDataBeg = sdw.str();
						DrawT(temp, Rect, e, sDataBeg);
					}
				}
			}
		}

		void PdfClass::GetMinMax(T_SqlTemp& Act, T_SqlTemp& Ref)
		{
			auto a = Act.begin()->second;
			auto r = Ref.begin()->second;

			auto ar = Act.rbegin()->second;
			auto rr = Ref.rbegin()->second;
			mind = std::min<int64_t>(a.first, r.first);
			maxd = std::max<int64_t>(ar.first - a.first, rr.first - r.first);
			//maxd = std::max<int64_t>(maxd, );

			f_maxt = 0.0f;
			f_mint = 999.0f;

			for(auto a : Ref)
			{
				if(a.second.second >= 0.0f && a.second.second < 999.0f)
				{
					f_maxt = std::fmaxf(f_maxt, a.second.second);
					f_mint = std::fminf(f_mint, a.second.second);
				}
			}
			for(auto a : Act)
			{
				if(a.second.second >= 0.0f && a.second.second < 999.0f)
				{
					f_maxt = std::fmaxf(f_maxt, a.second.second);
					f_mint = std::fminf(f_mint, a.second.second);
				}
			}

			f_step = 0.0;
			float fmaxt = 0.0f;
			float fmint = 999.0f;
			float cstep = 0.0f;
			do
			{
				f_step += 50.0;
				fmaxt = f_maxt / f_step;
				fmint = f_mint / f_step;
				fmaxt = ceil(fmaxt);
				fmint = floor(fmint);
				cstep = (fmaxt - fmint);
			} while(cstep >= 6.0);

			f_maxt = fmaxt * f_step;
			f_mint = fmint * f_step;
		}

		void PdfClass::PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage, int msec, std::wstring s1, std::wstring s2)
		{
			try
			{
				Gdiplus::REAL Width = 525;
				Gdiplus::REAL Height = 205;
				HDC hdc = GetDC(Global0);
				Gdiplus::Graphics g(hdc);
				Gdiplus::Bitmap backBuffer (INT(Width), INT(Height), &g);
				Gdiplus::Graphics temp(&backBuffer);
				Gdiplus::RectF RectG(0, 0, Width, Height);

				//Отчищаем
				temp.Clear(Gdiplus::Color(255, 255, 255));

				if(!Act.size()) return;
				if(!Ref.size()) return;

				Gdiplus::Color Blue(0, 0, 255);
				Gdiplus::Color Red(255, 0, 0);

				Gdiplus::RectF RectG2(RectG);

				RectG2.Y += 5;
				RectG2.Height -= 40;

				RectG2.X += 55;
				RectG2.Width -= 65;

				Gdiplus::RectF RectG3(RectG2);
				RectG3.Height += 17;

				GetMinMax(Act, Ref);
				DrawGridTemp(temp, RectG2, Act, Ref);
				DrawGridTime(temp, RectG3, msec);
				DrawGridOssi(temp, RectG, s1, s2);

				DrawBottom(temp, RectG2, Red, Ref);	//Красный; Заданное значение температуры
				DrawBottom(temp, RectG2, Blue, Act);	//Синий; Фактическое значение температуры

				std::wstring SaveFile(fImage.begin(), fImage.end());
				backBuffer.Save(SaveFile.c_str(), &guidJpeg, NULL);

				DeleteDC(hdc);
			}CATCH(PdfLog, "");
		}

		void PdfClass::DrawHeap(HPDF_REAL left, HPDF_REAL Y)
		{
			try
			{
				draw_text (page, left + 10, Y, "Параметры");
				draw_text (page, left + 265, Y, "Задание");
				draw_text (page, left + 371, Y, "Факт");

				HPDF_Page_SetFontAndSize (page, font, 8);
				draw_text (page, left + 316, Y + 10, "Доступное");
				draw_text (page, left + 313, Y - 0, "отклонение/");
				draw_text (page, left + 317, Y - 10, "диапазон");
				HPDF_Page_SetFontAndSize (page, font, 10);
			}CATCH(PdfLog, "");
		}

		HPDF_REAL PdfClass::DrawKpvl(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
		{
			HPDF_REAL Y = top - 0;
			try
			{
				draw_text_rect (page, left + 0, Y, w, YP, "Дата и время загрузки");
				std::string outDate = "";
				std::string outTime = "";
				GetDataTimeStr(Sheet.Start_at, outDate, outTime);
				draw_text_rect (page, left + 270, Y, DXP, YP, outDate);  //Дата XP = 70
				draw_text_rect (page, left + 340, Y, DXP, YP, outTime);  //Время

				Y -= 25;
				DrawHeap(left, Y);

				Y -= 25;

				draw_text_rect (page, left + 0, Y, w, YP, "Время нахождения листа в закалочной печи. мин");
				draw_text_rect (page, left + 270, Y, XP, YP, Stof(Sheet.TimeForPlateHeat));	//Задание Время нахождения листа в закалочной печи. мин
				draw_text_rect (page, left + 370, Y, XP, YP, Stof(Sheet.DataTime_All));		//Факт Время нахождения листа в закалочной печи. мин

				draw_rect(page, left + 310, Y, DXP - 10, YP);
				draw_text (page, left + 330, Y, "±2");		// Доступное отклонение/диапазон


				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Температура закалки, °С");
				draw_text_rect (page, left + 270, Y, XP, YP, Stof(Sheet.Temper));				//Задание Температуры закалки
				draw_text_rect (page, left + 370, Y, XP, YP, Stof(Sheet.Temperature));				//Факт Температуры закалки

				draw_rect(page, left + 310, Y, DXP - 10, YP);
				draw_text (page, left + 327, Y, "±10");		// Доступное отклонение/диапазон

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Давление воды в коллекторе закал. машины, бар");
				draw_text_rect (page, left + 270, Y, XP, YP, Stof(Sheet.PresToStartComp));	//Задание Давления воды
				draw_text_rect (page, left + 370, Y, XP, YP, Stof(Sheet.Za_PT3));				//Факт Давления воды

				draw_rect(page, left + 310, Y, DXP - 10, YP);
				draw_text (page, left + 315, Y, "3.6 - 4.6");		// Доступное отклонение/диапазон

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Температура закалочной жидкости, °С");
				draw_text_rect (page, left + 370, Y, XP, YP, Stof(Sheet.Za_TE3));				//Факт Температура воды, °С

				Y -= 10;
				HPDF_Page_MoveTo (page, 15, Y);
				HPDF_Page_LineTo (page, Width - 20, Y);
				HPDF_Page_Stroke (page);
				Y -= 20;
			}CATCH(PdfLog, "");

			return Y;
		}

		HPDF_REAL PdfClass::DrawFurn(TCassette& Cassette, HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
		{
			HPDF_REAL Y = top - 0;
			try
			{
				std::stringstream ssd;
				ssd << "Печь № " << Cassette.Peth << " Кассета: ";
				ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Hour) << " ";
				ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Day) << "-";
				ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Month) << "-";
				ssd << std::setw(4) << std::setfill('0') << Stoi(Cassette.Year) << " №";
				ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.CassetteNo);
				draw_text_rect (page, left, Y, w, YP, ssd.str().c_str());
				Y -= 25;

				draw_text_rect (page, left + 0, Y, w, YP, "Дата и время загрузки");
				std::string outDate = "";
				std::string outTime = "";
				GetDataTimeStr(Cassette.Run_at, outDate, outTime);
				draw_text_rect (page, left + 270, Y, DXP, YP, outDate);  //Дата
				draw_text_rect (page, left + 340, Y, DXP, YP, outTime);  //Время

				Y -= 25;
				DrawHeap(left, Y);

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Время нагрева до температуры отпуска, мин");

				draw_text_rect (page, left + 270, Y, XP, YP, Stof(Cassette.PointTime_1));						//Задание
				draw_text_rect (page, left + 370, Y, XP, YP, Stof(Cassette.HeatAcc));							//Факт
				draw_rect(page, left + 310, Y, DXP - 10, YP);
				draw_text (page, left + 315, Y, "120-180");		// Доступное отклонение/диапазон

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Время выдержки при заданной температуре, мин");
				draw_text_rect (page, left + 270, Y, XP, YP, Stof(Cassette.PointTime_2));						//Задание
				draw_text_rect (page, left + 370, Y, XP, YP, Stof(Cassette.HeatWait));							//Факт
				draw_rect(page, left + 310, Y, DXP - 10, YP);
				draw_text (page, left + 330, Y, "±5");		// Доступное отклонение/диапазон

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Температура отпуска, °С");
				draw_text_rect (page, left + 270, Y, XP, YP, Stof(Cassette.PointRef_1));						//Задание
				draw_text_rect (page, left + 370, Y, XP, YP, Stof(Cassette.facttemper));							//Факт
				draw_rect(page, left + 310, Y, DXP - 10, YP);
				draw_text (page, left + 327, Y, "±10");		// Доступное отклонение/диапазон

				Y -= 10;
				HPDF_Page_MoveTo (page, 15, Y);
				HPDF_Page_LineTo (page, Width - 20, Y);
				HPDF_Page_Stroke (page);
				Y -= 20;
			}CATCH(PdfLog, "");
			return Y;
		}
		
		//Создание нового листа PDF
		bool PdfClass::NewPdf()
		{
			try
			{
				pdf = HPDF_New (error_handler, NULL);
				if(!pdf)
					throw std::runtime_error(__FUN("Error new HPDF_New"));

				if(setjmp(env))
				{
					HPDF_Free (pdf);
					throw std::runtime_error(__FUN("Error setjmp(env)"));
				}

				// create default-font

				HPDF_UseUTFEncodings(pdf);
				HPDF_SetCurrentEncoder(pdf, "UTF-8");
				char* detail_font_name = (char*)HPDF_LoadTTFontFromFile (pdf, "arial.ttf", HPDF_TRUE);

				if(!strlen(detail_font_name))
					throw std::runtime_error(__FUN("HPDF_LoadTTFontFromFile(\"arial.ttf\") "));

				font = HPDF_GetFont(pdf, detail_font_name, "UTF-8");


				page = HPDF_AddPage(pdf);

				HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);

				Height = HPDF_Page_GetHeight (page);
				Width = HPDF_Page_GetWidth(page);
				coeff = Width / Height;

				HPDF_Page_SetLineWidth(page, 2); //Ширина линии 0.0 - 1.0
				HPDF_Page_Rectangle(page, 10, 10, Width - 20, Height - 20);
				HPDF_Page_Stroke(page);


				HPDF_Page_SetLineWidth(page, 0.5); //Ширина линии 0.0 - 1.0
				return true;

			}CATCH(PdfLog, "");
			return false;
		}
		
		
		//Сохранение листа PDF
		void PdfClass::SavePDF(TSheet& Sheet)
		{
			std::stringstream urls;
			std::stringstream temp;
			try
			{
				temp << lpLogPdf;
				urls << lpLogPdf;
				CheckDir(temp.str());
			}CATCH(PdfLog, " File: " + temp.str() + " ");

			std::tm TM;
			DataTimeOfString(Sheet.Start_at, TM);

			try
			{
				temp << "/" << TM.tm_year;
				CheckDir(temp.str());
			}CATCH(PdfLog, " File: " + temp.str() + " ");

			try
			{
				temp << "/" << MonthName[TM.tm_mon];
				CheckDir(temp.str());
			}CATCH(PdfLog, " File: " + temp.str() + " " + std::to_string(TM.tm_mon) + " ");

			try
			{
				temp << "/" << std::setw(2) << std::setfill('0') << std::right << TM.tm_mday;
				CheckDir(temp.str());
			}CATCH(PdfLog, " File: " + temp.str() + " ");

			std::stringstream tfile;
			std::stringstream ifile;
			try
			{
				tfile << std::setw(6) << std::setfill('0') << Sheet.Melt << "-";
				tfile << std::setw(3) << std::setfill('0') << Sheet.PartNo << "-";
				tfile << std::setw(3) << std::setfill('0') << Sheet.Pack << "-";
				tfile << std::setw(3) << std::setfill('0') << Sheet.Sheet << "-";
				tfile << std::setw(2) << std::setfill('0') << Sheet.SubSheet;

				ifile << std::setw(4) << std::setfill('0') << Sheet.Year << "-";
				ifile << std::setw(2) << std::setfill('0') << Sheet.Month << "-";
				ifile << std::setw(2) << std::setfill('0') << Sheet.Day << "-";
				ifile << std::setw(2) << std::setfill('0') << Sheet.Hour << "-";
				ifile << std::setw(2) << std::setfill('0') << Sheet.CassetteNo;
			}CATCH(PdfLog, " File1: " + temp.str() + "/" + tfile.str());

			std::string outDate = "";
			std::string outTime = "";
			GetDataTimeStr(Sheet.Start_at, outDate, outTime);
			boost::replace_all(outTime, ":", ".");

			FileName = temp.str() + "/" + tfile.str() + " " + outTime + ".pdf";
			std::string ImgeName1 = temp.str() + "/" + ifile.str() + ".jpg";
			std::string ImgeName2 = temp.str() + "/" + tfile.str() + " " + outTime + ".jpg";

			int t = 0;

			try
			{

				try
				{
					if(std::filesystem::exists(furnImage))
						std::filesystem::copy_file(furnImage, ImgeName1, /*std::filesystem::copy_options::skip_existing |*/ std::filesystem::copy_options::overwrite_existing);

					if(std::filesystem::exists(tempImage))
						std::filesystem::copy_file(tempImage, ImgeName2, /*std::filesystem::copy_options::skip_existing |*/ std::filesystem::copy_options::overwrite_existing);

				}
				CATCH(PdfLog, " File1: " + FileName + " ");

				HPDF_SaveToFile (pdf, FileName.c_str());
				HPDF_Free (pdf);


				std::stringstream ssd;
				ssd << "UPDATE sheet SET pdf = '" << FileName;
				ssd << "' WHERE id = " << Sheet.id; //delete_at IS NULL AND 
				std::string command = ssd.str();
				PGresult* res = conn.PGexec(command);
				if(PQresultStatus(res) == PGRES_FATAL_ERROR)
					LOG_ERROR(PdfLog, "{:89}| {}", (std::string(__FUNCTION__) + std::string(":") + std::to_string(1037)), command);;
				PQclear(res);
			}
			CATCH(PdfLog, " File1: " + FileName + " ");


		}

		HPDF_REAL PdfClass::DrawHeder(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top)
		{
			HPDF_REAL Y = top - 40;
			try
			{
				HPDF_Page_SetFontAndSize (page, font, 14);

				//842 / 2 = 
				draw_text (page, left + 365, Y, "ПАСПОРТ");


				HPDF_Page_SetFontAndSize (page, font, 10);
				Y -= 30;
				draw_text (page, left + 20, Y, "Марка стали");
				draw_text_rect (page, left + 100, Y, DXP2, YP, Sheet.Alloy);

				draw_text (page, left + 220, Y, "Толщина, мм");
				draw_text_rect (page, left + 300, Y, DXP2, YP, Sheet.Thikness);


				Y -= 30;
				draw_text (page, left + 20, Y, "Плавка");
				draw_text_rect (page, left + 70, Y, DXP2, YP, Sheet.Melt);

				draw_text (page, left + 180, Y, "Партия");
				draw_text_rect (page, left + 230, Y, DXP2, YP, Sheet.PartNo);

				draw_text (page, left + 350, Y, "Пачка");
				draw_text_rect (page, left + 390, Y, DXP2, YP, Sheet.Pack);

				draw_text (page, left + 510, Y, "Лист");
				draw_text_rect (page, left + 550, Y, DXP2, YP, Sheet.Sheet + " / " + Sheet.SubSheet);

				draw_text (page, left + 680, Y, "Сляб");
				draw_text_rect (page, left + 720, Y, DXP2, YP, Sheet.Slab);

				Y -= 10;
				HPDF_Page_MoveTo (page, left + 15, Y);
				HPDF_Page_LineTo (page, left + Width - 20, Y);
				HPDF_Page_Stroke (page);
				Y -= 20;
			}CATCH(PdfLog, "");
			return Y;
		}

		//Рисуем Закалка
		HPDF_REAL PdfClass::DrawKpvlPDF(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top)
		{
			HPDF_REAL Y = top - 5;
			try
			{
				//График температуры закалки
				HPDF_Page_SetFontAndSize (page, font, 12);
				draw_text(page, 380, Y, "Закалка");
				Y -= 25;

				HPDF_Page_SetFontAndSize (page, font, 10);
				HPDF_REAL Y1 = DrawKpvl(Sheet, 410, Y, Width - 432);

				//HPDF_REAL r0 = 310;
				HPDF_REAL r1 = Y - Y1;
				HPDF_REAL r0 = top - r1; //top = 275
				HPDF_REAL r3 = top - Y1 + (Y - Y1);

				HPDF_Page_Rectangle(page, 20, r0, 374, r1 - 10);
				HPDF_Page_Stroke(page);

				//std::ifstream ifs(tempImage);
				//if(!ifs.bad())
				std::fstream ifs(tempImage, std::fstream::binary | std::fstream::in);
				if(ifs.is_open())
				{
					ifs.close();
					HPDF_Image image1 = HPDF_LoadJpegImageFromFile(pdf, tempImage.c_str());
					HPDF_Page_DrawImage (page, image1, 22, r0 + 1, 370, r1 - 13);
				}
				Y = r3 - 20;
			}CATCH(PdfLog, "");
			return Y;
		}

		//Рисуем Отпуск
		HPDF_REAL PdfClass::DrawFurnPDF(TCassette& Cassette, HPDF_REAL left, HPDF_REAL top)
		{
			HPDF_REAL Y = top - 5;
			try
			{
				//График температуры отпуска
				HPDF_Page_SetFontAndSize (page, font, 12);
				draw_text(page, 380, Y, "Отпуск");
				Y -= 25;

				HPDF_Page_SetFontAndSize (page, font, 10);
				HPDF_REAL Y1 = DrawFurn(Cassette, 410, Y, Width - 432);

				//HPDF_REAL r0 = top - 140; //top = 275
				HPDF_REAL r1 = Y - Y1;

				HPDF_REAL r0 = top - r1; //top = 275
				HPDF_REAL r3 = top - Y1 + (Y - Y1);

				HPDF_Page_Rectangle(page, 20, r0, 374, r1 - 10);
				//HPDF_Page_Rectangle(page, 20, r3 - 15, 374, r1 - 10);

				HPDF_Page_Stroke(page);
				std::fstream ifs(furnImage, std::fstream::binary | std::fstream::in);
				//std::ifstream ifs(furnImage);
				//if(!ifs.bad())
				if(ifs.is_open())
				{
					ifs.close();
					HPDF_Image image2 = HPDF_LoadJpegImageFromFile(pdf, furnImage.c_str());
					HPDF_Page_DrawImage (page, image2, 22, r0 + 1, 370, r1 - 13);
					//HPDF_Page_DrawImage (page, image2, 22, r3 - 14, 370, r1 - 13);
				}
			}CATCH(PdfLog, "");
			return Y;
		}

		PdfClass::PdfClass(TCassette& Cassette, bool end)
		{
			InitLogger(PdfLog);
			try
			{
				CONNECTION1(conn, PdfLog);

				#pragma region Готовим графики

				#pragma region общий stringFormat для графиков
				{
					stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
					stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
				}
				#pragma endregion

				#pragma region furnImage.jpg
				{
					std::stringstream ssd;
					ssd << std::setw(4) << std::setfill('0') << Stoi(Cassette.Year) << "-";
					ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Month) << "-";
					ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Day) << "-";
					ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Hour) << "-";
					ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.CassetteNo) << ".jpg";
					furnImage = ssd.str();
				}
				#pragma endregion


				try
				{
					GetSheet(Cassette);
					if(!AllPfdSheet.size())
						throw std::exception("Ошибка AllPfdSheet = 0");


					#pragma region Номер печи для FurnRef и FurnAct

					FurnRef.erase(FurnRef.begin(), FurnRef.end());
					FurnAct.erase(FurnAct.begin(), FurnAct.end());

					if(!Cassette.Run_at.length() || !Cassette.Finish_at.length()) return;

					int Ref_ID = 0;
					int Act_ID = 0;

					int P = atoi(Cassette.Peth.c_str());
					if(P == 1)//Первая отпускная печь
					{
						Ref_ID = ForBase_RelFurn_1.TempRef->ID;
						Act_ID = ForBase_RelFurn_1.TempAct->ID;
					} else if(P == 2)//Вторая отпускная печь
					{
						Ref_ID = ForBase_RelFurn_2.TempRef->ID;
						Act_ID = ForBase_RelFurn_2.TempAct->ID;
					} else return;

					if(Ref_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, Ref_ID);
					if(Act_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, Act_ID);



					//Рисуем график FURN
					time_t tF1 = DataTimeOfString(Cassette.Run_at);
					time_t tF2 = DataTimeOfString(Cassette.Finish_at);
					int tF = int(difftime(tF2, tF1));

					PaintGraff(FurnAct, FurnRef, furnImage, tF, L"Температура С°", L"Время час");

					#pragma endregion

					#pragma endregion

					for(auto& Sheet : AllPfdSheet)
					{
						if(!isRun) break;
						//Sheet = a;

						#pragma region tempImage.jpg
						std::stringstream ssh;
						ssh << std::setw(6) << std::setfill('0') << Stoi(Sheet.Melt) << "-";
						ssh << std::setw(3) << std::setfill('0') << Stoi(Sheet.PartNo) << "-";
						ssh << std::setw(3) << std::setfill('0') << Stoi(Sheet.Pack) << "-";
						ssh << std::setw(3) << std::setfill('0') << Stoi(Sheet.Sheet) << "-";
						ssh << std::setw(2) << std::setfill('0') << Stoi(Sheet.SubSheet);
						LOG_INFO(PdfLog, "{:90}| Паспорт для листа: {}", FUNCTION_LINE_NAME, ssh.str());
						ssh << ".jpg";
						tempImage = ssh.str();
						#pragma endregion				

						OutDebugInfo(Cassette, Sheet);

						#pragma region Графики закалки
						TempRef.erase(TempRef.begin(), TempRef.end());
						TempAct.erase(TempAct.begin(), TempAct.end());

						//Закалка
						SqlTempActKPVL(TempAct, Sheet);
						GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1->ID);

						//Рисуем график KPVL
						time_t tK1 = DataTimeOfString(Sheet.Start_at);
						time_t tK2 = DataTimeOfString(Sheet.DataTime_End);
						int tK = int(difftime(tK2, tK1));

						PaintGraff(TempAct, TempRef, tempImage, tK, L"Температура С°", L"Время мин");
						#pragma endregion

						#pragma region Создание PFD файла

						if(NewPdf())
						{
							//Рисуем PDF заголовок
							HPDF_REAL Y1 = DrawHeder(Sheet, 0, Height);

							//Рисуем PDF Закалка
							HPDF_REAL Y2 = DrawKpvlPDF(Sheet, 0, Y1) - 40;

							//Рисуем PDF Отпуск
							HPDF_REAL Y3 = DrawFurnPDF(Cassette, 0, Y2);

							//auto index = a.index();
							//Сохраняем PDF
							SavePDF(Sheet);

						}

						#pragma endregion

						remove(tempImage.c_str());

						#if _DEBUG
						if(end)
						{
							std::string url = FileName;
							boost::replace_all(url, "/", "\\");
							ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
							return;
						}
						#endif
					}
					remove(furnImage.c_str());
				}
				CATCH(PdfLog, "");

				std::stringstream ssq;
				ssq << "UPDATE cassette SET pdf = now() WHERE id = " << Cassette.Id;
				SETUPDATESQL(PdfLog, conn, ssq);
			}
			CATCH(PdfLog, "");
		}

		PdfClass::PdfClass(TSheet& Sheet, bool end)
		{
			InitLogger(PdfLog);
			TCassette Cassette;
			try
			{
				CONNECTION1(conn, PdfLog);

				#pragma region Готовим графики

				#pragma region общий stringFormat для графиков
				{
					stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
					stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
				}
				#pragma endregion

				if(!Sheet.Correct.length())
					throw std::exception((std::string("Ошибка Sheet.Correct = 0, Sheet.Id") + Sheet.id).c_str());

				try
				{
					GetCassette(Cassette, Sheet);
					if(Cassette.Event != "5" || !Cassette.Correct.length())
						throw std::exception(("Касета не готова: Event = " + Cassette.Event + "Correct = " + Cassette.Correct + ", Sheet.Id" + Sheet.id).c_str());
					#pragma region Номер печи для FurnRef и FurnAct


					#pragma region furnImage.jpg
					std::stringstream ssd;
					ssd << std::setw(4) << std::setfill('0') << Stoi(Cassette.Year) << "-";
					ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Month) << "-";
					ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Day) << "-";
					ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.Hour) << "-";
					ssd << std::setw(2) << std::setfill('0') << Stoi(Cassette.CassetteNo) << ".jpg";
					furnImage = ssd.str();
					#pragma endregion

					FurnRef.erase(FurnRef.begin(), FurnRef.end());
					FurnAct.erase(FurnAct.begin(), FurnAct.end());

					if(!Cassette.Run_at.length() || !Cassette.Finish_at.length()) return;

					int Ref_ID = 0;
					int Act_ID = 0;
					float PointRef = 999;

					int P = atoi(Cassette.Peth.c_str());
					if(P == 1)//Первая отпускная печь
					{
						Ref_ID = ForBase_RelFurn_1.TempRef->ID;
						Act_ID = ForBase_RelFurn_1.TempAct->ID;
					} else if(P == 2)//Вторая отпускная печь
					{
						Ref_ID = ForBase_RelFurn_2.TempRef->ID;
						Act_ID = ForBase_RelFurn_2.TempAct->ID;
					} else return;

					if(Ref_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, Ref_ID);
					if(Act_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, Act_ID);



					//Рисуем график FURN
					time_t tF1 = DataTimeOfString(Cassette.Run_at);
					time_t tF2 = DataTimeOfString(Cassette.Finish_at);
					int tF = int(difftime(tF2, tF1));

					PaintGraff(FurnAct, FurnRef, furnImage, tF, L"Температура С°", L"Время час");

					#pragma endregion

					#pragma endregion

					{
						if(!isRun)
							throw std::exception("Завершение работы");

						#pragma region tempImage.jpg
						std::stringstream ssh;
						ssh << std::setw(6) << std::setfill('0') << Stoi(Sheet.Melt) << "-";
						ssh << std::setw(3) << std::setfill('0') << Stoi(Sheet.PartNo) << "-";
						ssh << std::setw(3) << std::setfill('0') << Stoi(Sheet.Pack) << "-";
						ssh << std::setw(3) << std::setfill('0') << Stoi(Sheet.Sheet) << "-";
						ssh << std::setw(2) << std::setfill('0') << Stoi(Sheet.SubSheet);
						LOG_INFO(PdfLog, "{:90}| Паспорт для листа: {}", FUNCTION_LINE_NAME, ssh.str());
						ssh << ".jpg";
						tempImage = ssh.str();
						#pragma endregion				

						OutDebugInfo(Cassette, Sheet);

						#pragma region Графики закалки
						TempRef.erase(TempRef.begin(), TempRef.end());
						TempAct.erase(TempAct.begin(), TempAct.end());

						//Закалка
						SqlTempActKPVL(TempAct, Sheet);
						//float PointRef = GenSeqFromHmi.TempSet1->Val.As<float>() + 50.f;
						GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1->ID);

						//Рисуем график KPVL
						time_t tK1 = DataTimeOfString(Sheet.Start_at);
						time_t tK2 = DataTimeOfString(Sheet.DataTime_End);
						int tK = int(difftime(tK2, tK1));

						PaintGraff(TempAct, TempRef, tempImage, tK, L"Температура С°", L"Время мин");
						#pragma endregion

						#pragma region Создание PFD файла

						if(NewPdf())
						{
							//Рисуем PDF заголовок
							HPDF_REAL Y1 = DrawHeder(Sheet, 0, Height);

							//Рисуем PDF Закалка
							HPDF_REAL Y2 = DrawKpvlPDF(Sheet, 0, Y1) - 40;

							//Рисуем PDF Отпуск
							HPDF_REAL Y3 = DrawFurnPDF(Cassette, 0, Y2);

							//auto index = a.index();
							//Сохраняем PDF
							SavePDF(Sheet);

						}

						#pragma endregion

						remove(tempImage.c_str());

						#ifdef _DEBUG
						if(end)
						{
							std::string url = FileName;
							boost::replace_all(url, "/", "\\");
							ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
							return;
						}
						#endif
					}
					remove(furnImage.c_str());
				}
				CATCH(PdfLog, "");

				std::stringstream ssq;
				ssq << "UPDATE cassette SET pdf = now() WHERE id = " << Cassette.Id;
				SETUPDATESQL(PdfLog, conn, ssq);
			}
			CATCH(PdfLog, "");
		}

	};

	//Автоматическое создание по кассете
	void PrintCassettePdfAuto(TCassette& TC)
	{
		InitLogger(PdfLog);
		try
		{
			if(TC.Run_at.length() && TC.Finish_at.length())
			{
				std::stringstream sss;
				sss << boost::format("%|04|-") % Stoi(TC.Year);
				sss << boost::format("%|02|-") % Stoi(TC.Month);
				sss << boost::format("%|02|-") % Stoi(TC.Day);
				sss << boost::format("%|02| ") % Stoi(TC.Hour);
				sss << "№ " << Stoi(TC.CassetteNo);
				SetWindowText(hWndDebug, sss.str().c_str());
				LOG_INFO(PdfLog, "{:90}| Паспорта для кассеты: {}", FUNCTION_LINE_NAME, sss.str());

				PASSPORT::PdfClass pdf(TC, false);
				SetWindowText(hWndDebug, "");
			}
		}
		CATCH(PdfLog, "");
	}
		
}
	
