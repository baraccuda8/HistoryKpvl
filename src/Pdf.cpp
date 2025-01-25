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


#include "hpdf.h"


#include <filesystem>

std::shared_ptr<spdlog::logger> PdfLog = NULL;
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

//Для выполнения дебага, начинапь прзод с самогго начала ишнарируя "correct IS NULL AND"
#define TESTPDF

////Для выполнения дебага, не сохранять в бпзе]
#define TESTPDF2


extern std::string lpLogPdf;
#if _DEBUG
extern std::string lpLogPdf2;
#endif
//extern const std::string FORMATTIME;
extern Gdiplus::Font font1;
extern Gdiplus::Font font2;

extern GUID guidBmp;
extern GUID guidJpeg;
extern GUID guidGif;
extern GUID guidTiff;
extern GUID guidPng;

extern std::map<int, std::string> GenSeq1;
extern std::map<int, std::string> GenSeq2;
extern std::map<int, std::string> GenSeq3;

namespace PDF
{
#if _DEBUG
	void CopyAllFile()
	{
		//namespace fs = std::filesystem;
		const char* dir1 = lpLogPdf.c_str(); ; // lpLogPdf2.c_str();
		const char* dir2 = lpLogPdf.c_str();
		//try
		//{
		//	if(!stat(dir1, &buff) && !stat(dir2, &buff))
		//	std::filesystem::copy(dir1, dir2,
		//						  std::filesystem::copy_options::update_existing
		//					   //|  std::filesystem::copy_options::overwrite_existing
		//						  | std::filesystem::copy_options::recursive);
		//}
		//CATCH(PdfLog, "");;

		dir2 = "\\\\192.168.9.63\\Prog\\KPVL\\Pdf";
		try
		{
			struct stat buff;
			if(!stat(dir1, &buff) && !stat(dir2, &buff))
			std::filesystem::copy(dir1, dir2,
								  std::filesystem::copy_options::update_existing
							   //|  std::filesystem::copy_options::overwrite_existing
								  | std::filesystem::copy_options::recursive);
		}
		CATCH(PdfLog, "");;
	}

	void DelAllPdf(std::string dir)
	{
		struct stat buff;
		boost::system::error_code ec;
		if(!stat(dir.c_str(), &buff))
			std::filesystem::remove_all(dir, ec);
		//std::string err = "";
		//if(ec)
		//{
		//	std::vector<boost::filesystem::path> patch = getDir(dir.string());
		//	for(auto& p : patch)
		//		remove(p.string().c_str());
		//	if(ec)
		//		LOG_ERROR(PdfLogger, "{:90}| Eor Delete dir: \"{}\"", "", ec.message());
		//	patch.erase(patch.begin(), patch.end());
		//}
	}
#endif

	bool Correct = FALSE;


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

	void GetTodosSQL(PGConnection& conn, MapTodos& mt, std::string& command, int p = 0)
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

		class PdfClass{
		public:
			std::string tempImage = ".jpg";
			std::string furnImage = ".jpg";

			HPDF_Doc  pdf = NULL;
			HPDF_Font font = NULL;
			HPDF_Page page = NULL;
			HPDF_REAL Height = 0;
			HPDF_REAL Width = 0;
			HPDF_REAL coeff = 0;
			float SrTemp = 0.0f;
			//std::string strSrTemp = "";
			std::string FileName = "";
			int StepSec = 5;


			PGConnection conn;
			//TSheet Sheet;
			//TCassette Cassette;

			std::deque<TSheet>AllPfdSheet;

			int64_t MaxSecCount = 0LL;
			const int XP = 40;
			const int DXP = 70;
			const int DXP2 = 100;
			const int YP = 18;

			T_SqlTemp FurnRef = {};	//Задание
			T_SqlTemp FurnAct = {};	//Актуальное

			T_SqlTemp TempRef = {};	//Задание
			T_SqlTemp TempAct = {};	//Актуальное

			float f_mint = 0.0f;
			float f_maxt = 0.0f;
			float f_step = 0.0f;

			int64_t mind = 0LL;
			int64_t maxd = 0LL;

			//PdfClass(TSheet& Sheet, bool end);
			//PdfClass(TCassette& TC, bool end);
			~PdfClass()
			{
				//conn.PGDisConnection();
			};

			Gdiplus::StringFormat stringFormat;

			Gdiplus::Pen pen = Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0));
			Gdiplus::SolidBrush Gdi_brush = Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
			Gdiplus::Pen Gdi_L1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 1); //Черный
			Gdiplus::Pen Gdi_D1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 2); //Черный
			Gdiplus::Pen Gdi_R1 = Gdiplus::Pen(Gdiplus::Color(192, 0, 0), 0.5); //Черный

			//void GetSrTemper(std::vector<PFS>& pF, std::map<int, PFS>& mF);
			//void GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID, float PointRef);
			//void SqlTempActKPVL1(std::string Stop, T_fTemp& fT);
			//void SqlTempActKPVL1(std::string Start, std::string Stop, VPFS& pfs1, int Temperature);
			//void SqlTempActKPVL2(std::string Stop, T_fTemp& fT);
			//void SqlTempActKPVL2(std::string Start, std::string Stop, VPFS& pfs2, int Temperature);
			//void SqlTempActKPVL(T_SqlTemp& tr, TSheet& Sheet);
			//
			//void DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);
			//void DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, float sd, std::wstring sDataBeg);
			//void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st);
			//void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);
			//void DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act, T_SqlTemp& Ref);
			//void DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG, std::wstring s1, std::wstring s2);
			//void DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, int msec);
			//void GetMinMax(T_SqlTemp& Act, T_SqlTemp& Ref);
			//
			////void DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);
			//void PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage, int msec, std::wstring s1, std::wstring s2);
			//
			//bool NewPdf();
			//void SavePDF(TSheet& Sheet);
			//HPDF_REAL DrawHeder(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top);
			//
			//HPDF_REAL DrawFurn(TCassette& Cassette, HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
			//HPDF_REAL DrawKpvl(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
			//HPDF_REAL DrawKpvlPDF(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top);
			//
			//HPDF_REAL DrawFurnPDF(TCassette& Cassette, HPDF_REAL left, HPDF_REAL top);
			//void UpdateTemperature(TSheet& d);
			////void UpdateTemperature(T_SqlTemp& tr, TSheet& Sheet);
			//void GetSheet(TCassette& Cassette);
			//void GetCassette(TCassette& cassette, TSheet& Sheet);
			//void DrawHeap(HPDF_REAL left, HPDF_REAL Y);
		//};

			void GetCassette(TCassette& cassette, TSheet& Sheet)
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

			void GetSheet(TCassette& Cassette)
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

			std::string GetStartTime(std::string Start, int ID)
			{
					std::tm TM_Temp = {0};
					//std::string sBegTime2 = Start;
					std::stringstream sde;
					sde << "SELECT create_at FROM todos WHERE id_name = " << ID;
					sde << " AND create_at <= '";
					sde << Start;
					sde << "' ORDER BY create_at DESC LIMIT 1;";
					std::string command = sde.str();
					if(DEB)LOG_INFO(PdfLog, "{:90}| {}", FUNCTION_LINE_NAME, command);
					PGresult* res = conn.PGexec(command);
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						if(PQntuples(res))
							Start = conn.PGgetvalue(res, 0, 0);
					} else
						LOG_ERR_SQL(PdfLog, res, command);
					PQclear(res);
					return Start;
			}

			void GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID)
			{
				try
				{
					time_t t1 = DataTimeOfString(Start);
					std::string start = GetStartTime(Start, ID);

					std::stringstream sdt;
					sdt << "SELECT create_at, content FROM todos WHERE id_name = " << ID;
					if(Start.length()) sdt << " AND create_at >= '" << start << "'";
					if(Stop.length()) sdt << " AND create_at <= '" << Stop << "'";
					sdt << " ORDER BY create_at ASC;";

					std::string command = sdt.str();
					PGresult* res = conn.PGexec(command);
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						int line = PQntuples(res);
						if(line)
						{
							int i = 0;

							float fTemp = Stof(conn.PGgetvalue(res, 0, 1).c_str());
							tr[Start] = std::pair(0, fTemp);


							for(int l = 0; l < line; l++)
							{
								std::string sData = conn.PGgetvalue(res, l, 0);

								if(Start <= sData)
								{
									std::string sTemp = conn.PGgetvalue(res, l, 1);
									int t = int(difftime(DataTimeOfString(sData), t1));

									fTemp = Stof(sTemp.c_str());
									if(fTemp > 0.f && fTemp <= 999.f)
										tr[sData] = std::pair(t, fTemp);
								}
							}


							int t = int(difftime(DataTimeOfString(Stop), t1));
							tr[Stop] = std::pair(t, fTemp);
						}
					} else
						LOG_ERR_SQL(PdfLog, res, command);

					PQclear(res);
				}CATCH(PdfLog, "");
			}

			void SqlTempActKPVL1(std::string Stop, T_fTemp& fT)
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
			void SqlTempActKPVL2(std::string Stop, T_fTemp& fT)
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


			void SqlTempActKPVL1(std::string Start, std::string Stop, VPFS& pF1, int Temperature)
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

			void SqlTempActKPVL2(std::string Start, std::string Stop, VPFS& pF2, int Temperature)
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

			void GetSrTemper(std::vector<PFS>& pF, std::map<int, PFS>& mF)
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
						} 
						else
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

			void UpdateTemperature(TSheet& Sheet)
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
				}CATCH(SheetLogger, "");

			}

			void SqlTempActKPVL(T_SqlTemp& tr, TSheet& Sheet)
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

			void DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat)
			{
				Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));

				std::wstring::const_iterator start = str.begin();
				std::wstring::const_iterator end = str.end();
				boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}):\\d{1,2}.*");
				boost::match_results<std::wstring::const_iterator> what;

				if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 1)
					temp.DrawString(what[1].str().c_str(), -1, &font1, Rect, &stringFormat, &Gdi_brush);
			}


			void DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, float sd, std::wstring sDataBeg)
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


			void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st)
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

			void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
			{
				//try
				//{
				//	Gdiplus::Pen Gdi_L1(Gdiplus::Color(255, 0, 0), 2); //Красный
				//	Gdiplus::Pen Gdi_L2(Gdiplus::Color(0, 0, 255), 2); //Синий
				//
				//	Gdiplus::PointF pt1 ={Rect.X + 0, Rect.Y + 5};
				//	Gdiplus::PointF pt2 ={Rect.X + 20, Rect.Y + 5};
				//
				//	temp.DrawLine(&Gdi_L1, pt1, pt2);
				//
				//	pt1 ={Rect.X + 100, Rect.Y + 5};
				//	pt2 ={Rect.X + 120, Rect.Y + 5};
				//
				//	temp.DrawLine(&Gdi_L2, pt1, pt2);
				//
				//	stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
				//
				//	Gdiplus::RectF Rect2 = Rect;
				//	Rect2.X += 25;
				//	Rect2.Y -= 5;
				//	Rect2.Height = 20;
				//	temp.DrawString(L"Задание", -1, &font2, Rect2, &stringFormat, &Gdi_brush);
				//
				//	Rect2 = Rect;
				//	Rect2.X += 125;
				//	Rect2.Y -= 5;
				//	Rect2.Height = 20;
				//	temp.DrawString(L"Факт", -1, &font2, Rect2, &stringFormat, &Gdi_brush);
				//}CATCH(PdfLogger, "");
			}

			void DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG, std::wstring s1, std::wstring s2)
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

			void DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act, T_SqlTemp& Ref)
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

			void DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, int msec)
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

				//int sd = int(W * Rect.Width);
				//Gdiplus::PointF pt1 ={Rect.X + Rect.Width, Rect.Y};
				//Gdiplus::PointF pt2 ={Rect.X + Rect.Width, Rect.Height - 10};
				//
				//temp.DrawLine(&Gdi_D1, pt1, pt2);
				//
				//std::wstringstream sdw;
				//sdw << std::fixed << std::setprecision(1) << (sd / h3);
				//std::wstring sDataBeg = sdw.str();
				//DrawT(temp, Rect, Rect.Width, sDataBeg);

			}

			void GetMinMax(T_SqlTemp& Act, T_SqlTemp& Ref)
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

			void PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage, int msec, std::wstring s1, std::wstring s2)
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

			void DrawHeap(HPDF_REAL left, HPDF_REAL Y)
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

			HPDF_REAL DrawKpvl(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
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

			HPDF_REAL DrawFurn(TCassette& Cassette, HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
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
			bool NewPdf()
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
			void SavePDF(TSheet& Sheet)
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

			HPDF_REAL DrawHeder(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top)
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
			HPDF_REAL DrawKpvlPDF(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top)
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
			HPDF_REAL DrawFurnPDF(TCassette& Cassette, HPDF_REAL left, HPDF_REAL top)
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

			PdfClass(TCassette& Cassette, bool end)
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
				CATCH(CorrectLog, "");
			};

			PdfClass(TSheet& Sheet, bool end)
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

											//for(auto& Sheet : AllPfdSheet)
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
				CATCH(CorrectLog, "");
			};
	};
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

			//void GetStartTime(PGConnection& conn, int peth)
			//{
			//	//std::string start = "";
			//	try
			//	{
			//		//std::string command = "SELECT run_at FROM cassette WHERE run_at IS NOT NULL AND correct IS NULL AND pdf IS NULL AND delete_at IS NULL AND CAST(event AS integer) = 5 ";
			//
			//		std::stringstream ss1;
			//		ss1 << "(SELECT run_at FROM cassette WHERE run_at IS NOT NULL AND (correct IS NOT NULL AND pdf IS NOT NULL) AND delete_at IS NULL AND CAST(event AS integer) = 5 "; //delete_at IS NULL AND 
			//		ss1 << "AND peth = " << peth;
			//		ss1 << " ORDER BY run_at ASC LIMIT 1) ";
			//	
			//		std::stringstream ssd;
			//		ssd << "SELECT run_at, ";
			//		ssd << "run_at - TIME '02:00:00'";	//Минус 2 часа
			//		ssd << "  FROM cassette WHERE (correct IS NULL OR pdf IS NULL) AND delete_at IS NULL AND run_at >= "; //delete_at IS NULL AND 
			//		ssd << ss1.str();
			//		ssd << "AND peth = " << peth;
			//		ssd << " ORDER BY run_at ASC LIMIT 1;";
			//
			//		std::string command = ssd.str();
			//		PGresult* res = conn.PGexec(command);
			//		if(PQresultStatus(res) == PGRES_TUPLES_OK)
			//		{
			//			if(conn.PQntuples(res))
			//			{
			//				DateCorrect = conn.PGgetvalue(res, 0, 0);
			//				DateStart = conn.PGgetvalue(res, 0, 1);
			//				DateStop = "";
			//			}
			//		}
			//		else
			//		{
			//			LOG_ERR_SQL(CassetteLogger, res, command);
			//		}
			//		PQclear(res);
			//	}CATCH(CassetteLogger, "");
			//}
			
			void GetStopTime(PGConnection& conn, std::string Start, int ID)
			{
				if(!DateStart.length()) return;
				std::stringstream sss;
				sss << "SELECT create_at"
					//", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)"
					" FROM todos WHERE id_name = " << ID	    //Конец отпуска
					<< " AND create_at >= '" << Start << "'"
					<< " AND content = 'true'"
					<< " ORDER BY create_at DESC LIMIT 1"; // LIMIT 3
				std::string command = sss.str();
				PGresult* res = conn.PGexec(command);
				//Заполняем данные
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
					sss << " id_name = " << ID; 			//Фактическое значение температуры
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

			void GetVal(PGConnection& conn, TCassette& it, T_ForBase_RelFurn* Furn)
			{
				try
				{
	#pragma region Готовим запрос в базу

					std::stringstream sss;
					sss << "SELECT DISTINCT ON (id_name) id_name, content";
					sss << ", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)";
					sss << " FROM todos WHERE (";
					sss << " id_name = " << Furn->ActTimeHeatAcc->ID << " OR";	//Факт время нагрева
					sss << " id_name = " << Furn->ActTimeHeatWait->ID << " OR";	//Факт время выдержки
					sss << " id_name = " << Furn->ActTimeTotal->ID;				//Факт общее время
					sss << ")";
					sss << " AND create_at >= '" << it.Run_at << "'";
					sss << " AND create_at < '" << it.End_at << "'";
					sss << " AND cast (content as numeric) <> 0";
					sss << " ORDER BY id_name, create_at DESC"; // LIMIT 3

	#pragma endregion

	#pragma region Запрос в базу

					std::string command = sss.str();
					PGresult* res = conn.PGexec(command);
					//Заполняем данные
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						int line = PQntuples(res);
						//Заполняем данные
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

				try
				{
	#pragma region Гогтвим запрос в базу

					std::stringstream sss;
					sss << "SELECT DISTINCT ON (id_name) id_name, content";
					sss << ", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)";
					sss << " FROM todos WHERE (";
					sss << " id_name = " << Furn->PointRef_1->ID << " OR";		//Уставка температуры
					sss << " id_name = " << Furn->PointTime_1->ID << " OR";		//Задание Время нагрева
					sss << " id_name = " << Furn->PointTime_2->ID << " OR";	//Задание Время выдержки
					sss << " id_name = " << Furn->TimeProcSet->ID;				//Полное время процесса (уставка), мин
					sss << ")";	//Факт время выдержки
					sss << " AND create_at < '" << it.End_at << "'";
					sss << " ORDER BY id_name, create_at DESC"; // LIMIT 4

	#pragma endregion

	#pragma region Запрос в базу

					std::string command = sss.str();
					PGresult* res = conn.PGexec(command);
					//Заполняем данные
					if(PQresultStatus(res) == PGRES_TUPLES_OK)
					{
						int line = PQntuples(res);
						//Заполняем данные
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

				try
				{
					time_t temp  = (time_t)difftime(DataTimeOfString(it.End_at), 5 * 60); //Вычисть 5 минут до конца отпуска
					std::string End_at  = GetStringOfDataTime(&temp);
					it.facttemper = GetVal(conn, Furn->TempAct->ID, it.Run_at, End_at);

				}
				CATCH(CassetteLogger, "");
			}
		
			void EndCassette(PGConnection& conn, TCassette& it, std::fstream& s1)
			{
				try
				{
					T_ForBase_RelFurn* Furn = GetFurn(Petch);

					it.Peth = std::to_string(Petch);

					if(/*S107::IsCassette(it) &&*/ it.Run_at.length())
					{
						GetVal(conn, it, Furn);

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
						//tm_Fin = tm_End1 + (60 * 15); //+15 минут
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

							s1 << Stof(it.PointRef_1) << ";";		//Уставка температуры
							s1 << Stof(it.facttemper) << ";";		//Фактическое значение температуры

							s1 << Stof(it.PointTime_1) << ";";		//Задание Время нагрева
							s1 << Stof(it.HeatAcc) << ";";			//Факт время нагрева
							s1 << Stof(it.PointTime_2) << ";";	//Задание Время выдержки
							s1 << Stof(it.HeatWait) << ";";		//Факт время выдержки

							s1 << Stof(it.TimeProcSet) << ";";		//Полное время процесса (уставка), мин
							s1 << Stof(it.Total) << ";";			//Факт общее время

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
	#pragma region Запись в файл заголовка Sdg.csv
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

	#pragma region Запись в файл тела Sdg.csv
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
					if(it.PointTime_1.length())ct.PointTime_1 = it.PointTime_1;    //Время разгона
					if(it.PointRef_1.length())ct.PointRef_1 = it.PointRef_1;      //Уставка температуры
					if(it.TimeProcSet.length())ct.TimeProcSet = it.TimeProcSet;    //Полное время процесса (уставка), мин
					if(it.PointTime_2.length())ct.PointTime_2 =it.PointTime_2;   //Время выдержки
					if(it.facttemper.length())ct.facttemper = it.facttemper;			//Факт температуры за 5 минут до конца отпуска
					if(it.Finish_at.length())ct.Finish_at = it.Finish_at;        //Завершение процесса + 15 минут
					if(it.HeatAcc.length())ct.HeatAcc = it.HeatAcc;			//Факт время нагрева
					if(it.HeatWait.length())ct.HeatWait = it.HeatWait;          //Факт время выдержки
					if(it.Total.length())ct.Total = it.Total;				//Факт общее время

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
						ssd << ", facttemper = " << ct.facttemper; //Факт температуры за 5 минут до конца отпуска


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

					if(it.PointTime_1.length())ct.PointTime_1 = it.PointTime_1;    //Время разгона
					if(it.PointRef_1.length())ct.PointRef_1 = it.PointRef_1;      //Уставка температуры
					if(it.TimeProcSet.length())ct.TimeProcSet = it.TimeProcSet;    //Полное время процесса (уставка), мин
					if(it.PointTime_2.length())ct.PointTime_2 =it.PointTime_2;   //Время выдержки
					if(it.facttemper.length())ct.facttemper = it.facttemper;			//Факт температуры за 5 минут до конца отпуска
					if(it.Finish_at.length())ct.Finish_at = it.Finish_at;        //Завершение процесса + 15 минут
					if(it.HeatAcc.length())ct.HeatAcc = it.HeatAcc;			//Факт время нагрева
					if(it.HeatWait.length())ct.HeatWait = it.HeatWait;          //Факт время выдержки
					if(it.Total.length())ct.Total = it.Total;				//Факт общее время

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
					//Запись в базу
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
	#pragma region Запись в файл Cass.csv заголовка
					std::fstream s1("Cass.csv", std::fstream::binary | std::fstream::out);
					s1 << "Индекс;"
						<< "Старт;"
						<< "Стоп;"
						<< "Финиш;"
						<< "Год;"
						<< "Месяц;"
						<< "День;"
						<< "Кассета;"
						<< "Авария;"
						<< "Печь;"
						<< "Уставка T;"			//Уставка температуры
						<< "Факт Т;"			//Фактическое значение температуры
						<< "Задание нагрева;"	//Задание Время нагрева
						<< "Факт нагрева;"		//Факт время нагрева
						<< "Задание выдержки;"	//Задание Время выдержки
						<< "Факт выдержки;"		//Факт время выдержки
						<< "Задание процесса;"	//Полное время процесса (уставка), мин
						<< "Факт процесса;"		//Факт общее время
						<< std::endl;
	#pragma endregion

	#pragma region Запись в файл Cass.csv тела
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
								<< it.second.PointRef_1 << ";"		//Уставка температуры
								<< it.second.facttemper << ";"		//Фактическое значение температуры
								<< it.second.PointTime_1 << ";"		//Задание Время нагрева
								<< it.second.HeatAcc << ";"			//Факт время нагрева
								<< it.second.PointTime_2 << ";"	//Задание Время выдержки
								<< it.second.HeatWait << ";"		//Факт время выдержки
								<< it.second.TimeProcSet << ";"		//Полное время процесса (уставка), мин
								<< it.second.Total << ";"			//Факт общее время

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
						//std::string sg = std::string("Получение кассеты из базы ") + std::to_string(P0.size()) + std::string(" :") + std::to_string(lin++);
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


					SetWindowText(hWndDebug, "Закончил копирование паспортов");
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
				//Заполняем данные
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					//Заполняем данные
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
				//Заполняем данные
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					//Заполняем данные
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
			const int st1_3 = 3;	//Открыть входную дверь
			const int st1_4 = 4;	//Загрузка в печь
			const int st1_5 = 5;	//Закрыть входную дверь
			const int st1_6 = 6;	//Нагрев листа
			const int st1_7 = 7;	//Передача на 2 рольганг
			const int st1_8 = 8;	//Передача на 2-й рольганг печи

			//GenSeqToHmi.Seq_2_StateNo
			const int st2_3 = 3;	//Прием заготовки с 1-го рольганга печи
			const int st2_4 = 4;	//Осциляция.Нагрев Листа
			const int st2_5 = 5;	//Открыть выходную дверь
			const int st2_6 = 6;	//Выдача в линию закалки
			const int st2_7 = 7;	//Закрыть выходную дверь

			//GenSeqToHmi.Seq_3_StateNo
			const int st3_1 = 1;	//Ожидание заготовки
			//const int st3_3 = 3;	//Выдача заготовки
			const int st3_4 = 4;	//Окончание цикла обработки

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
			////Фиксируем на начало входа в печь State_1 = 3;
			//void GetDataSheet1(PGConnection& conn, T_IdSheet& ids);
			//
			////Фиксируем на начало входа в печь State_2 = 5;
			//void GetDataSheet2(PGConnection& conn, T_IdSheet& ids);
			//
			////Фиксируем на начало входа в печь State_2 = 5 плюс 5 секунд;
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

					//Дата, время загрузки листа в закалочную печь
					//Марка стали
					//Толщина листа, мм
					//Плавка
					//партия
					//пачка
					//номер листа
					//Заданная температура
					//скорость выдачи, мм/с
					//
					//Скоростная секция. Давление воды в баке
					//Скоростная секция. Температура воды в баке
					//Скоростная секция. Давление в верхнем коллекторе
					//Скоростная секция. Давление нижнем коллекторе
					//Скоростная секция. Положение клапана верх
					//Скоростная секция. Положение клапана низ
					// 
					//Ламинарная секция1  Положение клапана верх
					//Ламинарная секция1  Положение клапана низ
					//Ламинарная секция2  Положение клапана верх
					//Ламинарная секция2  Положение клапана низ
					//Температура воды в поддоне

					ss1
						<< "Id;"
						<< "В зону 1;"
						<< "В зону 2;"
						<< "В охлаждение;"
						<< "В Кантовку;"
						<< "DataTime_End;"
						<< "В кантовку;"
						<< "Кантовка;"

						<< "DataTime_All;"
						<< "TimeForPlateHeat;"
						<< "TimeStart;"

						<< "Марка;"
						<< "Толщина;"
						<< "Плавка;"
						<< "Партия;"
						<< "Пачка;"
						<< "Сляб;"
						<< "Лист;"
						<< "Сублист;"
						<< "TempSet1;"
						<< "UnloadSpeed;"
						<< "Za_PT3;"
						<< "Za_TE3;"
						<< "LaminPressTop;"
						<< "LaminPressBot;"
						<< "SpeedTopSet;"
						<< "SpeedBotSet;"
						<< "Маскирование;"
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

						<< "Ошибка;"
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

			//Фиксируем на начало входа в печь State_1 = 3;
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
					ssd << "(id_name = " << GenSeqFromHmi.TempSet1->ID;			//Уставка температуры
					ssd << " OR id_name = " << Par_Gen.UnloadSpeed->ID;			//Уставка скорости
					ssd << " OR id_name = " << Par_Gen.TimeForPlateHeat->ID;	//Уставка времени
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
						if(a.id_name == GenSeqFromHmi.TempSet1->ID)				ids.Temper = a.content.As<float>();				//Уставка температуры
						if(a.id_name == Par_Gen.UnloadSpeed->ID)				ids.Speed = a.content.As<float>();				//Уставка скорости
						if(a.id_name == Par_Gen.TimeForPlateHeat->ID)			ids.TimeForPlateHeat = a.content.As<float>();	//Уставка времени

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

			//Фиксируем на начало выхода из печи State_2 = 5;
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
					ssd << " OR id_name = " << GenSeqFromHmi.TempSet1->ID;		//Уставка температуры
					ssd << " OR id_name = " << Par_Gen.UnloadSpeed->ID;			//Уставка скорости
					ssd << " OR id_name = " << Par_Gen.TimeForPlateHeat->ID;	//Уставка времени

					ssd << ") ORDER BY id_name, create_at DESC";

					std::string command = ssd.str();
					GetTodosSQL(conn, DataSheetTodos, command);

					for(auto a : DataSheetTodos)
					{
						if(a.id_name == AI_Hmi_210.Za_TE3->ID)			ids.Za_TE3 = a.content.As<float>();
						if(a.id_name == AI_Hmi_210.Za_PT3->ID)			ids.Za_PT3 = a.content.As<float>();
						if(a.id_name == AI_Hmi_210.LAM_TE1->ID)			ids.LAM_TE1 = a.content.As<float>();

						if(a.id_name == GenSeqFromHmi.TempSet1->ID)		ids.Temper = a.content.As<float>();				//Уставка температуры
						if(a.id_name == Par_Gen.UnloadSpeed->ID)		ids.Speed = a.content.As<float>();				//Уставка скорости
						if(a.id_name == Par_Gen.TimeForPlateHeat->ID)	ids.TimeForPlateHeat = a.content.As<float>();	//Уставка времени
					}
				}CATCH(SheetLogger, "");
			}

			//Фиксируем на начало выхода из печи State_2 = 5 плюс 5 секунд;
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

					ssd << " start_at = '" << td.Start1 << "'";				//Дата, время загрузки листа в закалочную печь
					ssd << ", datatime_end = '" << td.DataTime_End << "'";		//Дата, время выдачи листа из закалочной печи

					if(td.Start2.length())			ssd << ", secondpos_at = '" << td.Start2 << "'";		//Преход во вторую зону
					if(td.InCant.length())			ssd << ", incant_at = '" << td.InCant << "'";		//Преход на кантовку
					if(td.Cant.length())			ssd << ", cant_at = '" << td.Cant << "'";			//Канитовка

					ssd << ", alloy = '" << td.Alloy << "'";				//Марка стали
					ssd << ", thikness = '" << td.Thikness << "'";			//Толщина листа, мм

					if(td.TimeForPlateHeat)
						ssd << ", timeforplateheat = " << std::setprecision(1) << std::fixed << td.TimeForPlateHeat;	//Задание Время нахождения листа в закалочной печи. мин
					if(td.DataTime_All)
						ssd << ", datatime_all = " << std::setprecision(1) << std::fixed << td.DataTime_All;			//Факт Время нахождения листа в закалочной печи. мин


					if(td.Speed)
						ssd << ", speed = " << std::setprecision(1) << std::fixed << td.Speed;						//Скорость выгрузки
					if(td.Temper)
						ssd << ", temper = " << std::setprecision(1) << std::fixed << td.Temper;						//Уставка температуры

					if(td.PresToStartComp)
						ssd << ", prestostartcomp = " << std::setprecision(1) << std::fixed << td.PresToStartComp;				//
					if(td.SpeedTopSet)
						ssd << ", posclapantop = " << std::setprecision(1) << std::fixed << td.SpeedTopSet;			//Клапан. Скоростная секция. Верх
					if(td.SpeedBotSet)
						ssd << ", posclapanbot = " << std::setprecision(1) << std::fixed << td.SpeedBotSet;			//Клапан. Скоростная секция. Низ
					if(td.Lam1PosClapanTop)
						ssd << ", lam1posclapantop = " << std::setprecision(1) << std::fixed << td.Lam1PosClapanTop;		//Клапан. Ламинарная секция 1. Верх
					if(td.Lam1PosClapanBot)
						ssd << ", lam1posclapanbot = " << std::setprecision(1) << std::fixed << td.Lam1PosClapanBot;		//Клапан. Ламинарная секция 1. Низ
					if(td.Lam2PosClapanTop)
						ssd << ", lam2posclapantop = " << std::setprecision(1) << std::fixed << td.Lam2PosClapanTop;		//Клапан. Ламинарная секция 2. Верх
					if(td.Lam2PosClapanBot)
						ssd << ", lam2posclapanbot = " << std::setprecision(1) << std::fixed << td.Lam2PosClapanBot;		//Клапан. Ламинарная секция 2. Низ
					ssd << ", mask = '" << td.Mask << "'";					//Режим работы клапана
					//Фиксируем на выходе из печи State_2 = 5;
					if(td.LAM_TE1)
						ssd << ", lam_te1 = " << std::setprecision(1) << std::fixed << td.LAM_TE1;					//Температура воды в поддоне
					if(td.Za_TE3)
						ssd << ", za_te3 = " << std::setprecision(1) << std::fixed << td.Za_TE3;						//Температура воды в баке
					if(td.Za_PT3)
						ssd << ", za_pt3 = " << std::setprecision(1) << std::fixed << td.Za_PT3;						//Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)

					//Фиксируем на выходе из печи State_2 = 5 плюс 5 секунд;
					if(td.LaminPressTop)
						ssd << ", lampresstop = " << std::setprecision(1) << std::fixed << td.LaminPressTop;			//Давление в верхнем коллекторе
					if(td.LaminPressBot)
						ssd << ", lampressbot = " << std::setprecision(1) << std::fixed << td.LaminPressBot;			//Давление в нижнем коллекторе

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
					ssd << ", pos = " << td.Pos;
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
					ssd << "create_at, ";				//Дата, время загрузки листа в закалочную печь
					ssd << "start_at, ";				//Дата, время загрузки листа в закалочную печь
					ssd << "datatime_end, ";			//Дата, время выдачи листа из закалочной печи

					if(td.Start2.length())			ssd << "secondpos_at, ";	//Преход во вторую зону
					if(td.InCant.length())			ssd << "incant_at, ";		//Преход на кантовку
					if(td.Cant.length())			ssd << "cant_at, ";			//Канитовка

					ssd << "datatime_all,";			//Факт Время нахождения листа в закалочной печи. мин
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

					ssd << "lampresstop, ";				//Давление в верхнем коллекторе
					ssd << "lampressbot, ";				//Давление в нижнем коллекторе
					ssd << "lam_te1, ";					//Температура воды в поддоне
					ssd << "za_te3, ";					//Температура воды в баке
					ssd << "za_pt3, ";					//Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)
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

					if(td.Start2.length())			ssd << "'" << td.Start2 << "', ";	//Преход во вторую зону
					if(td.InCant.length())			ssd << "'" << td.InCant << "', ";	//Преход на кантовку
					if(td.Cant.length())			ssd << "'" << td.Cant << "', ";		//Канитовка
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
					ssd << td.Pos << ", ";
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

				#pragma region Вывод инфы
					std::stringstream sss;
					sss << "InZone1 №" << i;
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

					#pragma region Вывод инфы
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
					#pragma region Вывод инфы
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
						#pragma region Вывод инфы
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
						#pragma region Вывод инфы
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

					#pragma region Вывод инфы
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

			//Загрузка в печь
			void GetSeq_1_StateNo(PGConnection& conn, T_Todos& td, size_t count, size_t i, std::fstream& ff2)
			{
				int16_t st = td.content.As<int16_t>();
//Открыть входную дверь
				if(st == st1_3)
				{
					InZone11(conn, td.create_at, count, i);
					SaveT_IdSheetBodyCsv(ff2, Ids1);
				}
				//"Загрузка в печь"
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
				//"Закрыть входную дверь"
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
				//"Нагрев листа"
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
				//"Передача на 2 рольганг" || "Передача на 2-й рольганг печи"
				else if(st == st1_7 || st == st1_8)
				{
					InZone2(conn, td.create_at);
				}
			}

			//Вторая зона печи
			void GetSeq_2_StateNo(PGConnection& conn, T_Todos& td)
			{
				int16_t st = td.content.As<int16_t>();
				if(st == st2_3)	//3 = Прием заготовки с 1-го рольганга печи
				{
					InZone2(conn, td.create_at);
				} else if(st == st2_5 || st == st2_6 || st == st2_7) //Выгрузка из печи - "Открыть выходную дверь" || Выдача в линию закалки" || "Закрыть выходную дверь"
				{
					InZone3(conn, td.create_at);
				}
			}

			//Зона Охлаждения
			void GetSeq_3_StateNo(PGConnection& conn, T_Todos& td)
			{
				int16_t st = td.content.As<int16_t>();
				if(st == st3_1 || st == st3_4)	//1 = Ожидание заготовки; 4 - Окончание цикла обработки
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

					////Сортируем по времени
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

						//Загрузка в печь
						if(td.id_name == GenSeqToHmi.Seq_1_StateNo->ID)
						{
							GetSeq_1_StateNo(conn, td, count, i, ff2);
						}
						//Вторая зона печи
						else if(td.id_name == GenSeqToHmi.Seq_2_StateNo->ID)
						{
							GetSeq_2_StateNo(conn, td);
						}
						//Зона Охлаждения
						else if(td.id_name == GenSeqToHmi.Seq_3_StateNo->ID)
						{
							GetSeq_3_StateNo(conn, td);
						}
						//Кантовка
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
					" start_at - INTERVAL '2 MINUTES', "						//Минус 2 минуты
					" start_at + INTERVAL '60 MINUTES' "						//Плюс 60 минут
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

					//Время кантовки
					if(sheet.Cant_at.length())
					{
						time_t td = DataTimeOfString(sheet.Cant_at);
						return td + 5;	//Плюс 5 секунда
					}
					//Время листа на кантовку
					else if(sheet.InCant_at.length())
					{
						time_t td = DataTimeOfString(sheet.InCant_at);
						return td + 30 * 60;	//Плюс 30 минут
					}
					//Время листа выхода из печи
					else if(sheet.DataTime_End.length())
					{
						time_t td = DataTimeOfString(sheet.DataTime_End);
						return td + 30 * 60;	//Плюс 30 минут
					}
					//Время передачи листа во вторую зону
					else if(sheet.SecondPos_at.length())
					{
						time_t td = DataTimeOfString(sheet.SecondPos_at);
						float TimeHeat = Stof(sheet.TimeForPlateHeat);
						return td + (time_t)TimeHeat * 60 + 30 * 60;	//Плюс 30 минут, Плюс Время сигнализации окончания нагрева, мин
					}
					//Время входа листа в печь
					else if(sheet.Start_at.length())
					{
						time_t td = DataTimeOfString(sheet.Start_at);
						float TimeHeat = Stof(sheet.TimeForPlateHeat);
						return td + (time_t)TimeHeat * 60 + 40 * 60;	//Плюс 40 минут, Плюс Время сигнализации окончания нагрева, мин
					}
					//Время создания листа
					//else if(sheet.Create_at.length())
					//{ 
						//time_t td = DataTimeOfString(sheet.Create_at);
						//float TimeHeat = Stof(sheet.TimeForPlateHeat);
						//return td + (time_t)TimeHeat * 60 + (60 * 60);	//Плюс 60 минут, Плюс Время сигнализации окончания нагрева, мин
					//}
				}
			} else
				LOG_ERR_SQL(PdfLog, res, command);
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

					//Время кантовки
					if(sheet.Cant_at.length())
					{
						time_t td = DataTimeOfString(sheet.Cant_at) + 5;
						return GetStringOfDataTime(&td);
						//return td ;	//Плюс 5 секунда
					}
					//Время листа на кантовку
					else if(sheet.InCant_at.length())
					{
						time_t td = DataTimeOfString(sheet.InCant_at) + 30 * 60;
						return GetStringOfDataTime(&td);
						//return td + 30 * 60;	//Плюс 30 минут
					}
					//Время листа выхода из печи
					else if(sheet.DataTime_End.length())
					{
						time_t td = DataTimeOfString(sheet.DataTime_End) + 30 * 60;
						return GetStringOfDataTime(&td);
						//return td + 30 * 60;	//Плюс 30 минут
					}
					//Время передачи листа во вторую зону
					else if(sheet.SecondPos_at.length())
					{
						float TimeHeat = Stof(sheet.TimeForPlateHeat);
						time_t td = DataTimeOfString(sheet.SecondPos_at) + (time_t)TimeHeat * 60 + 30 * 60;
						return GetStringOfDataTime(&td);
						//return td + (time_t)TimeHeat * 60 + 30 * 60;	//Плюс 30 минут, Плюс Время сигнализации окончания нагрева, мин
					}
					//Время входа листа в печь
					else if(sheet.Start_at.length())
					{
						float TimeHeat = Stof(sheet.TimeForPlateHeat);
						time_t td = DataTimeOfString(sheet.Start_at) + (time_t)TimeHeat * 60 + 40 * 60;
						return GetStringOfDataTime(&td);
						//return td + (time_t)TimeHeat * 60 + 40 * 60;	//Плюс 40 минут, Плюс Время сигнализации окончания нагрева, мин
					}
				}
			} else
				LOG_ERR_SQL(PdfLog, res, command);
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
	//	else   td += (time_t)((60 * 60 * 1) + (10 * 60));		// + 60 минут + 10 минут
	//	stop = GetStringOfDataTime(&td);
	//
	//	if(id.length())
	//	{
	//		SetWindowText(hWndDebug, ("Корректировка листа id = " + id).c_str());
	//		LOG_INFO(SheetLogger, "Корректировка листа id = {}", id);
	//
	//		if(start.length() && stop.length())
	//			SHEET::GetSheets sheet(conn, start, stop);
	//		else
	//			LOG_INFO(SheetLogger, "Ошибка корректировки листа id = {}, start = {}, stop = {}", id, start, stop);
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
//			LOG_INFO(SheetLogger, "Принудительная корректировка листов");
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
//			LOG_INFO(SheetLogger, "Закончили принудительную корректировку листов");
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
			"AND (SELECT correct FROM cassette WHERE cassette.id = sheet.cassette AND pdf IS NOT NULL) IS NOT NULL ) OR id = 29271 "
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
			std::string out = "Лист: " + Sheet.id;
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
							SetWindowText(hWndDebug, ("Коррекции листов: " + GetStringDataTime()).c_str());
							GetSheets sheets(conn, id, start, stop);

							SetWindowText(hWndDebug, ("Отметка о корекции: " + GetStringDataTime()).c_str());
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

			//ПДФ без коррекции
			HendCassettePDF(conn);
		}
		CATCH(CassetteLogger, "");
	
		isCorrectCassette = false;
		SetWindowText(hWndDebug, ("Закончили коррекчию кассет: " + GetStringDataTime()).c_str());
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
					
					//ПДФ без коррекции
					HendCassettePDF(conn);
				}
			}
		}
		CATCH(CassetteLogger, "");

		isCorrectCassette = false;
		SetWindowText(hWndDebug, ("Закончили коррекчию кассет: " + GetStringDataTime()).c_str());
		return 0;
	}

	//Поток автоматической корректировки
	DWORD WINAPI RunCassettelPdf(LPVOID)
	{
		try
		{
			InitLogger(CorrectLog);
			//PGConnection conn;
			//CONNECTION1(conn, CorrectLog);
#if HENDINSERT
			//Для ручного тестирования
			#if !_DEBUG
			#error HENDINSERT в Релисе!!!.
			//("В Релисе!!!");
			#endif
			//SetWindowText(hWndDebug, "Стартанул");

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
			//SetWindowText(hWndDebug, "Закончил");
			isRun = false;
			return 0;

#else
				std::string out1 = "Вход в создание паспортов: " + GetStringDataTime();
				LOG_INFO(CorrectLog, "{:90}| {}", FUNCTION_LINE_NAME, out1);
				bool OldNotCorrect = NotCorrect;
				while(isRun)
				{
					if(!NotCorrect)
					{

						std::string out = "Запуск коррекции листов: " + GetStringDataTime();
						SetWindowText(hWndDebug, out.c_str());

						CorrectSheet(0);

						out = "Запуск создание паспортов: " + GetStringDataTime();
						SetWindowText(hWndDebug, out.c_str());

						CorrectCassette(0);
						out = "Закончили создание паспортов: " + GetStringDataTime();
						SetWindowText(hWndDebug, out.c_str());

#ifdef _ReleaseD
						isRun = false;
#endif

#ifdef _DEBUG
						//В дебаге один проход и выход из программы
						isRun = false;
					}
#else
					}
					int f = (NotCorrect ? 10 : 60 * 1); //10 секунд или 1 минута
					while(isRun && --f > 0 /*&& OldNotCorrect == NotCorrect*/)
					{
						if(!isCorrectCassette && !isCorrectSheet)
							SetWindowText(hWndDebug, ("Закончили создание паспортов: Ожидаю " + std::to_string(f) + " сек.").c_str());

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

		std::string out2 = "Выход из создания паспортов: " + GetStringDataTime();
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
