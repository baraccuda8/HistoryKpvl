#include "pch.h"
#include <optional>
#include <setjmp.h>

#include "win.h"
#include "main.h"
#include "file.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "Pdf.h"
#include "Graff.h"


#include "hpdf.h"


#include <filesystem>

std::shared_ptr<spdlog::logger> PdfLogger;

//Для выполнения дебага, начинапь прзод с самогго начала ишнарируя "correct IS NULL AND"
#define TESTPDF

////Для выполнения дебага, не сохранять в бпзе]
#define TESTPDF2


//typedef struct T_cass{
//	int nom = 0;
//	int id = 0;
//	std::string Run_at = "";
//	std::string End_at = "";
//	std::string Err_at = "";
//	std::string Fin_at = "";
//
//	std::tm TM_Run = std::tm();
//	std::tm TM_End = std::tm();
//	std::tm TM_Fin = std::tm();
//	std::tm TM_All = std::tm();
//	time_t tm_Run = 0;
//	time_t tm_End = 0;
//	time_t tm_Fin = 0;
//	time_t tm_All = 0;
//
//	float facttemper = 0.0f;
//
//	int Repeat_at = 0;
//
//	int Day = 0;
//	int Month = 0;
//	int Year = 0;
//	int CassetteNo = 0;
//	int Petch = 0;
//
//	float PointRef_1 = 0.0f;		//Уставка Tемпературы
//	float TempAct = 0.0f;			//Фактическое значение температуры
//
//	float PointTime_1 = 0.0f;		//Заданное нагрева
//	float ActTimeHeatAcc = 0.0f;	//Факт время нагрева
//
//	float PointDTime_2 = 0.0f;		//Заданное Выдержка
//	float ActTimeHeatWait = 0.0f;	//Факт время выдержки
//
//	float TimeProcSet = 0.0f;		//Полное время процесса (уставка), мин
//	float ActTimeTotal = 0.0f;		//Факт общее время
//};


extern std::string lpLogPdf;
//extern const std::string FORMATTIME;
extern Gdiplus::Font font1;
extern Gdiplus::Font font2;

extern GUID guidBmp;
extern GUID guidJpeg;
extern GUID guidGif;
extern GUID guidTiff;
extern GUID guidPng;


namespace PDF
{
	typedef struct T_IdSheet{
		int id = 0;
		std::string Start1 = "";		//Дата, время загрузки листа в закалочную печь
		std::string Start2 = "";		//Дата, время загрузки листа во вторую зону
		std::string DataTime_End = "";		//Дата, время выгрузки листа из печи 

		std::string TimeTest = "";		//Для тестов
		std::string DataTime = "";		//Старт нашрева

		int Melt = 0;					//Плавка
		int Pack = 0;					//Пачка
		int PartNo = 0;					//Партия
		int Sheet = 0;					//Номер листа
		int SubSheet = 0;				//Номер подлиста
		int Slab = 0;					//Сляб

		std::string Alloy = "нет";		//Марка стали
		std::string Thikness = "нет";	//Толщина листа, мм

		//float HeatTime_Z2 = 0.0f;		//время нагрева в зоне 2
		float TimeForPlateHeat = 0.0f;	//Задание Время нахождения листа в закалочной печи. мин
		float DataTime_All = 0.0f;		//Факт Время нахождения листа в закалочной печи. мин
		float PresToStartComp = 0.0f;	//Задание Давления воды

		//Фиксируем на начало входа в печь State_1 = 3;
		float Temper = 0.0f;			//Уставка температуры
		float Speed = 0.0f;		//Скорость выгрузки
		float SpeedTopSet = 0.0f;		//Клапан. Скоростная секция. Верх
		float SpeedBotSet = 0.0f;		//Клапан. Скоростная секция. Низ
		float Lam1PosClapanTop = 0.0f;		//Клапан. Ламинарная секция 1. Верх
		float Lam1PosClapanBot = 0.0f;		//Клапан. Ламинарная секция 1. Низ
		float Lam2PosClapanTop = 0.0f;		//Клапан. Ламинарная секция 2. Верх
		float Lam2PosClapanBot = 0.0f;		//Клапан. Ламинарная секция 2. Низ
		uint16_t Valve_1x = 0;			//Режим работы клапана 1
		uint16_t Valve_2x = 0;			//Режим работы клапана 2
		std::string Mask1 = "000000000";//Режим работы клапана 1
		std::string Mask2 = "000000000";//Режим работы клапана 2
		std::string Mask = "";			//Режим работы клапана
		//Фиксируем на выходе из печи State_2 = 5;
		float LAM_TE1 = 0.0f;			//Температура воды в поддоне
		float Za_TE3 = 0.0f;			//Температура воды в баке
		float Za_PT3 = 0.0f;			//Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)

		//Фиксируем на выходе из печи State_2 = 5 плюс 5 секунд;
		float LaminPressTop = 0.0f;		//Давление в верхнем коллекторе 
		float LaminPressBot = 0.0f;		//Давление в нижнем коллекторе

		int Pos = 0;

		int day = 0;
		int month = 0; 
		int year = 0; 
		int cassetteno = 0;
	}T_IdSheet;

	bool Correct = FALSE;
	typedef std::vector<T_Todos> V_Todos;
	void TodosColumn(PGresult* res)
	{
		try
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
		CATCH(PdfLogger, "");;
	}

	OpcUa::Variant GetVarVariant(OpcUa::VariantType Type, std::string value)
	{
		try
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
		}
		CATCH(PdfLogger, "");;
		return OpcUa::Variant();
	}

	namespace Cassette
	{
		std::string tempImage = ".jpg";
		std::string furnImage = ".jpg";

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


		jmp_buf env;

		void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data)
		{
			LOG_ERROR(PdfLogger, "{:90}| ERROR: error_no={}, detail_no={}", FUNCTION_LINE_NAME, (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
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

		class CassettePdfClass{
		public:

			HPDF_Doc  pdf;
			HPDF_Font font;
			HPDF_Page page;
			HPDF_REAL Height = 0;
			HPDF_REAL Width = 0;
			HPDF_REAL coeff = 0;
			float SrTemp = 0;
			std::string strSrTemp = "";
			std::string FileName;


			PGConnection conn;
			TSheet Sheet;
			std::deque<TSheet>AllPfdSheet;
			TCassette Cassette;

			int MaxSecCount = 0;
			const int XP = 70;
			const int YP = 18;

			T_SqlTemp FurnRef ={};	//Задание
			T_SqlTemp FurnAct ={};	//Актуальное

			T_SqlTemp TempRef ={};	//Задание
			T_SqlTemp TempAct ={};	//Актуальное

			double f_mint = 0.0;
			double f_maxt = 0.0;

			double mint = 0.0;
			double maxt = 0.0;
			double cstep = 0.0;
			double fstep = 0.0;
			int64_t mind = 0;
			int64_t maxd = 0;


			CassettePdfClass(TCassette& TC);
			CassettePdfClass(TSheet& sheet, bool view = true);
			//CassettePdfClass(T_IdSheet& sheet, bool view = false);
			~CassettePdfClass()
			{
				//conn.PGDisConnection();
			};

			Gdiplus::StringFormat stringFormat;

			Gdiplus::Pen pen = Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0));
			Gdiplus::SolidBrush Gdi_brush = Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
			Gdiplus::Pen Gdi_L1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 0.5); //Черный

			void GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID);
			void SqlTempActKPVL(T_SqlTemp& tr);

			void DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);
			void DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, double sd, std::wstring sDataBeg);
			//void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st, int64_t mind, int64_t maxd, double mint, double maxt);;
			void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st);
			void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);
			void DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act, T_SqlTemp& Ref);
			void DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG);
			void DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act);

			//void DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);
			void PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage);

			bool NewPdf();
			void SavePDF();
			HPDF_REAL DrawHeder(HPDF_REAL left, HPDF_REAL top);

			HPDF_REAL DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
			HPDF_REAL DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
			HPDF_REAL DrawKpvlPDF(HPDF_REAL left, HPDF_REAL top);

			HPDF_REAL DrawFurnPDF(HPDF_REAL left, HPDF_REAL top);

			void GetSheet();
			void GetCassette(TCassette& cassette);
		};

		void CassettePdfClass::GetCassette(TCassette& cassette)
		{
			try
			{
				cassette = TCassette();
				if(Sheet.Year.empty() || !Sheet.Year.length()) return;
				if(Sheet.Month.empty() || !Sheet.Month.length()) return;
				if(Sheet.Day.empty() || !Sheet.Day.length()) return;
				if(Sheet.CassetteNo.empty() || !Sheet.CassetteNo.length()) return;

				std::string comand = "SELECT * FROM cassette ";
				comand += "WHERE ";
				comand += "year = " + Sheet.Year + " AND ";
				comand += "month = " + Sheet.Month + " AND ";
				comand += "day = " + Sheet.Day + " AND ";
				comand += "cassetteno = " + Sheet.CassetteNo + " ";
				comand += "ORDER BY create_at DESC";
				comand += ";";

				if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
				PGresult* res = conn.PGexec(comand);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					S107::GetColl(res);
					if(conn.PQntuples(res))
						S107::GetCassette(res, cassette, 0);
				}
				else
					LOG_ERR_SQL(PdfLogger, res, comand);
				PQclear(res);
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		}

		void CassettePdfClass::GetSheet()
		{
			try
			{
				AllPfdSheet.erase(AllPfdSheet.begin(), AllPfdSheet.end());
				std::stringstream sd;
				sd << "SELECT * FROM sheet WHERE ";
				sd << "day = '" << Cassette.Day << "' AND ";
				sd << "month = '" << Cassette.Month << "' AND ";
				sd << "year = '" << Cassette.Year << "' AND ";
				sd << "cassetteno = " << Cassette.CassetteNo << " ";
				sd << "ORDER BY  create_at DESC, pos DESC, start_at DESC;";
				std::string comand = sd.str();
				if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
				PGresult* res = conn.PGexec(comand);
				//LOG_INFO(PdfLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					KPVL::SQL::GetCollumn(res);
					KPVL::SQL::GetSheet(conn, res, AllPfdSheet);
				}
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		}

		void CassettePdfClass::GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID)
		{
			try
			{
				std::tm TM_Temp ={0};
				std::string sBegTime2 = Start;
				std::stringstream sde;
				sde << "SELECT DISTINCT ON (id) create_at FROM todos WHERE id_name = " << ID;
				sde << " AND create_at <= '";
				sde << Start;
				sde << "' ORDER BY id DESC LIMIT 1;";
				std::string comand = sde.str();
				if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
				PGresult* res = conn.PGexec(comand);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					if(PQntuples(res))
						sBegTime2 = conn.PGgetvalue(res, 0, 0);
				}
				else
					LOG_ERR_SQL(PdfLogger, res, comand);
				PQclear(res);


				std::stringstream sdt;
				sdt << "SELECT DISTINCT ON (id) create_at, content FROM todos WHERE id_name = " << ID;
				if(sBegTime2.length())	sdt << " AND create_at >= '" << sBegTime2 << "'";
				if(Stop.length())	sdt << " AND create_at <= '" << Stop << "'";

				sdt << " ORDER BY id ASC;";

				comand = sdt.str();
				if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
				res = conn.PGexec(comand);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					if(line)
					{
						int i = 0;
						int64_t t = 0;

						float f = static_cast<float>(atof(conn.PGgetvalue(res, 0, 1).c_str()));
						DataTimeOfString(Start, FORMATTIME, TM_Temp);
						TM_Temp.tm_year -= 1900;
						TM_Temp.tm_mon -= 1;
						tr[Start] = std::pair(mktime(&TM_Temp), f);


						for(int l = 0; l < line; l++)
						{
							std::string sData = conn.PGgetvalue(res, l, 0);

							if(Start <= sData)
							{
								std::string sTemp = conn.PGgetvalue(res, l, 1);
								DataTimeOfString(sData, FORMATTIME, TM_Temp);
								TM_Temp.tm_year -= 1900;
								TM_Temp.tm_mon -= 1;

								f = static_cast<float>(atof(sTemp.c_str()));
								if(f != 0)
									tr[sData] = std::pair(mktime(&TM_Temp), f);
							}
						}


						DataTimeOfString(Stop, FORMATTIME, TM_Temp);
						TM_Temp.tm_year -= 1900;
						TM_Temp.tm_mon -= 1;
						tr[Stop] = std::pair(mktime(&TM_Temp), f);
					}
				}
				else
					LOG_ERR_SQL(PdfLogger, res, comand);

				PQclear(res);
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		}

		void CassettePdfClass::SqlTempActKPVL(T_SqlTemp& tr)
		{
			try
			{
				tr.erase(tr.begin(), tr.end());
				std::string Start = Sheet.Start_at;
				std::string Stop = Sheet.DataTime_End;
				int t = 0;
				std::tm TM_Temp ={0};

				if(Stop.length() < 1) return;
				if(Start.length() < 1)return;

				std::stringstream sdt;
				sdt << "SELECT create_at, content FROM todos WHERE (";
				sdt << "id_name = " << Hmi210_1.Htr1_1->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_2->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_3->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr1_4->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_1->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_2->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_3->ID << " OR ";
				sdt << "id_name = " << Hmi210_1.Htr2_4->ID << " ) ";

				sdt << " AND create_at >= '" << Start;
				sdt << "' AND create_at <= '" << Stop;
				sdt << "' ORDER BY create_at ASC ;";


				std::string comand = sdt.str();
				if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
				PGresult* res = conn.PGexec(comand);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					if(line)
					{
						if(!Stoi(Sheet.Temperature))
							SrTemp = 0.0f;
						for(int l = 0; l < line; l++)
						{
							std::string sData = conn.PGgetvalue(res, l, 0);
							std::string sTemp = conn.PGgetvalue(res, l, 1);
							float f =  atoi_t(float, atof, sTemp);

							if(Sheet.Start_at <= sData)
							{
								auto a = tr.find(sData);

								if(a != tr.end() && a._Ptr != NULL)
								{
									a->second.second = (f + a->second.second) / 2.0f;
								}
								else
								{
									if(f != 0.0)
									{
										if(!Stoi(Sheet.Temperature))
										{
											if(SrTemp == 0.0f)SrTemp = f;
											else SrTemp = (SrTemp + f) / 2.0f;
										}
										DataTimeOfString(sData, FORMATTIME, TM_Temp);
										TM_Temp.tm_year -= 1900;
										TM_Temp.tm_mon -= 1;
										tr[sData] = std::pair(mktime(&TM_Temp), f);
									}
								}
							}
						}
					}
				}
				else
					LOG_ERR_SQL(PdfLogger, res, comand);
				PQclear(res);

				if(!Stoi(Sheet.Temperature))
				{
					std::ostringstream oss;

					oss << std::setprecision(0) << std::fixed << SrTemp;

					std::string update = " temperature = " + oss.str();
					KPVL::Sheet::SetUpdateSheet(conn, Sheet, update, "");

					strSrTemp = oss.str();
				}
				else strSrTemp = Sheet.Temperature;
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		}

		void CassettePdfClass::DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat)
		{
			Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));

			std::wstring::const_iterator start = str.begin();
			std::wstring::const_iterator end = str.end();
			boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}):\\d{1,2}.*");
			boost::match_results<std::wstring::const_iterator> what;

			if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 1)
				temp.DrawString(what[1].str().c_str(), -1, &font1, Rect, &stringFormat, &Gdi_brush);
		}


		void CassettePdfClass::DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, double sd, std::wstring sDataBeg)
		{
			Gdiplus::PointF pt1 ={Rect.X + float(sd), Rect.Y};
			Gdiplus::PointF pt2 ={Rect.X + float(sd), Rect.Height - 10};
			temp.DrawLine(&Gdi_L1, pt1, pt2);

			Gdiplus::RectF RectText(Rect);

			Gdiplus::RectF boundRect;
			temp.MeasureString(sDataBeg.c_str(), 5, &font1, RectText, &stringFormat, &boundRect);

			boundRect.X = Rect.X + float(sd);
			boundRect.X -= 21;
			boundRect.Height += 3;

			//temp.DrawRectangle(&pen, boundRect);

			Gdiplus::Pen p = Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), 0.1f);

			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
			temp.DrawString(sDataBeg.c_str(), 5, &font1, boundRect, &stringFormat, &Gdi_brush);
		}

		void CassettePdfClass::DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st)
		{
			Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 0.5); //Черный
			Gdiplus::Pen Gdi_L2(clor, 1);
			auto b = st.begin();
			auto e = st.end();
			e--;
			double coeffW = (double)(Rect.Width) / double(maxd);
			double coeffH = (double)(Rect.Height - Rect.Y) / (double)(f_maxt - f_mint);


			//Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));
			//Gdiplus::StringFormat stringFormat;
			//stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			//stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

			//{
			//	float mY = Rect.Y + float((f_maxt - f_maxt) * coeffH);
			//	Gdiplus::PointF pt1 ={Rect.X - 5,				mY};
			//	Gdiplus::PointF pt2 ={Rect.X + Rect.Width + 5,	mY};
			//	temp.DrawLine(&Gdi_L1, pt1, pt2);
			//
			//	Gdiplus::RectF Rect2 ={0, mY - 8, 50, 20};
			//
			//	std::wstringstream sdw;
			//	sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << f_maxt;
			//	temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);
			//}

			//{
			//	float iY = Rect.Y + float((f_maxt - f_mint) * coeffH);
			//	Gdiplus::PointF pt1 ={Rect.X - 5,				iY};
			//	Gdiplus::PointF pt2 ={Rect.X + Rect.Width + 5,	iY};
			//	temp.DrawLine(&Gdi_L1, pt1, pt2);
			//
			//	Gdiplus::RectF Rect2 ={0, iY - 8, 50, 20};
			//
			//	std::wstringstream sdw;
			//	sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << f_mint;
			//	temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);
			//}

			Gdiplus::PointF p1 ={0, 0};
			Gdiplus::PointF p2;;
			p1.X = Rect.X;
			p1.Y = Rect.Y + float((f_maxt - b->second.second) * coeffH);

			for(auto& a : st)
			{
				p2.X =  Rect.X + float((a.second.first - mind) * coeffW);
				p2.Y =  Rect.Y + float((f_maxt - a.second.second) * coeffH);
				temp.DrawLine(&Gdi_L2, p1, p2);

				p1.X = p2.X;
				p1.Y = p2.Y;
			}
		}

		void CassettePdfClass::DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
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
			//}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		}

		void CassettePdfClass::DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG)
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

			Gdiplus::RectF Rect1 = RectG;

			temp.TranslateTransform(0, RectG.Height);
			temp.RotateTransform(-90);

			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

			std::wstring theString = L"Температура С°";

			Gdiplus::RectF boundRect ={0, 0, RectG.Height + 5, 20};
			temp.DrawString(theString.c_str(), -1, &font1, boundRect, &stringFormat, &Gdi_brush);
			temp.ResetTransform();

			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
			theString = L"Время час:мин";
			temp.DrawString(theString.c_str(), -1, &font1, Rect1, &stringFormat, &Gdi_brush);

			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}

		void CassettePdfClass::DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act, T_SqlTemp& Ref)
		{

			for(auto a : Ref)
			{
				maxt = std::fmaxl(maxt, a.second.second);
				mint = std::fminl(mint, a.second.second);
			}
			for(auto a : Act)
			{
				maxt = std::fmaxl(maxt, a.second.second);
				mint = std::fminl(mint, a.second.second);
			}

			f_mint = mint;
			f_maxt = maxt;


			fstep = 0.0;
			double fmaxt;
			double fmint;
			do
			{
				fstep += 50.0;
				fmaxt = f_maxt / fstep;
				fmint = f_mint / fstep;
				fmaxt = ceil(fmaxt);
				fmint = floor(fmint);
				//cstep = fmod((fmaxt - fmint), fstep);
				cstep = (fmaxt - fmint);
			} while(cstep >= 6.0);

			f_maxt = fmaxt * fstep;
			f_mint = fmint * fstep;

			//Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 0.5); //Черный
			//Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));
			//Gdiplus::StringFormat stringFormat;
			//stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			//stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);

			double coeffH = (double)(Rect.Height - Rect.Y) / (double)(f_maxt - f_mint);

			for(double d = f_mint; d <= f_maxt; d += fstep)
			{
				float mY = Rect.Y + float((f_maxt - d) * coeffH);
				Gdiplus::PointF pt1 ={Rect.X - 5,				mY};
				Gdiplus::PointF pt2 ={Rect.X + Rect.Width + 5,	mY};
				temp.DrawLine(&Gdi_L1, pt1, pt2);

				Gdiplus::RectF Rect2 ={Rect.X - 45, mY - 11, 40, 20};
				//Gdiplus::RectF Rect2 ={0, mY - 11, 40, 20};

				std::wstringstream sdw;
				sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << d;

				temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);

			}
			int tt = 0;
		}

		void CassettePdfClass::DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act)
		{
			auto tb = Act.begin();
			auto te = Act.rbegin();

			std::string st1 = std::string (tb->first.begin(), tb->first.end());
			std::string st2 = std::string (te->first.begin(), te->first.end());

			std::tm TM;
			time_t tm1 = DataTimeOfString(st1, FORMATTIME, TM);
			time_t tm2 = DataTimeOfString(st2, FORMATTIME, TM);

			double tm = difftime(tm2, tm1);
			double Step = 1.0;
			double Count = 1.0;

			Count = 6.0;
			Step = tm / Count;

			for(double e = 0; e <= Count; e++)
			{
				double sd = round(Rect.Width / (Count)*e);

				double f = Step * e;
				double tmf = tm1 + f;
				time_t tms = time_t(tmf);

				std::string s = GetDataTimeString(&tms);
				std::wstring sDataBeg = GetData(std::wstring(s.begin(), s.end()));

				DrawT(temp, Rect, sd, sDataBeg);
			}
		}


		void CassettePdfClass::PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage)
		{
			try
			{
				Gdiplus::REAL Width = 525;//Gdiplus::REAL(abs(rcBounds.right - rcBounds.left));
				Gdiplus::REAL Height = 205; // Gdiplus::REAL(abs(rcBounds.bottom - rcBounds.top));
				HDC hdc = GetDC(Global0);
				Gdiplus::Graphics g(hdc);
				Gdiplus::Bitmap backBuffer (INT(Width), INT(Height), &g);
				Gdiplus::Graphics temp(&backBuffer);
				Gdiplus::RectF RectG(0, 0, Width, Height);
				//Gdiplus::RectF RectBottom(0, 0, Width, Height);

				//Отчищаем
				temp.Clear(Gdiplus::Color(255, 255, 255));

				//Gdiplus::Pen Gdi_Bar(Gdiplus::Color(0, 0, 0), 1);
				if(!Act.size()) return;
				if(!Ref.size()) return;



				maxt = 0;
				mint = 9999;
				mind = (std::min)(Act.begin()->second.first, Ref.begin()->second.first);

				auto b = Act.begin();
				auto e = Act.rbegin();
				//e--;
				int64_t MaxSecCount = 0;
				maxd = (std::max)(MaxSecCount, e->second.first - b->second.first);
				//int64_t maxd = 0;// e->second.first - b->second.first;

				b = Ref.begin();
				e = Ref.rbegin();
				//e--;
				maxd = (std::max)(maxd, e->second.first - b->second.first);


				Gdiplus::Color Blue(0, 0, 255);
				Gdiplus::Color Red(255, 0, 0);

				Gdiplus::RectF RectG2(RectG);

				RectG2.Y += 5;
				RectG2.Height -= 40;

				RectG2.X += 55;
				RectG2.Width -= 75;

				Gdiplus::RectF RectG3(RectG2);
				RectG3.Height += 17;

				DrawGridTemp(temp, RectG2, Act, Ref);
				DrawGridTime(temp, RectG3, Act);
				DrawGridOssi(temp, RectG);

				DrawBottom(temp, RectG2, Red, Ref);	//Красный; Заданное значение температуры
				DrawBottom(temp, RectG2, Blue, Act);	//Синий; Фактическое значение температуры
				
				
				////double maxt = 0;
				////double mint = 500;
				//mind = (std::min)(Act.begin()->second.first, Ref.begin()->second.first);
				//
				////auto b = Act.begin();
				////auto e = Act.rbegin();
				//
				//int64_t maxcount = 0;
				////int64_t maxd = (std::max)(maxcount, Act.rbegin()->second.first - Act.begin()->second.first);
				////int64_t maxd = 0;// e->second.first - b->second.first;
				//
				////b = Ref.begin();
				////e = Ref.rbegin();
				//maxd = (std::max)(maxd, Ref.rbegin()->second.first - Ref.begin()->second.first);
				//
				//
				//for(auto& a : Ref)
				//{
				//	maxt = std::fmaxl(maxt, a.second.second);
				//	mint = std::fminl(mint, a.second.second);
				//}
				//for(auto& a : Act)
				//{
				//	maxt = std::fmaxl(maxt, a.second.second);
				//	mint = std::fminl(mint, a.second.second);
				//}
				//
				//Gdiplus::Color Blue(0, 0, 255);
				//Gdiplus::Color Red(255, 0, 0);
				//
				//Gdiplus::RectF RectG2(RectG);
				//RectG2.Y += 5;
				//RectG2.Height -= 25;
				//RectG2.X += 35;
				//RectG2.Width -= 40;
				//
				//DrawBottom(temp, RectG2, Red, Ref, mind, maxd, mint, maxt);	//Красный; Заданное значение температуры
				//DrawBottom(temp, RectG2, Blue, Act, mind, maxd, mint, maxt);	//Синий; Фактическое значение температуры
				//
				//Gdiplus::RectF RectG3(RectG);
				//RectG3.X = 100;
				//RectG3.Y = RectG.Height - 15;
				//DrawInfo(temp, RectG3);
				//
				////b = Act.begin();
				////e = Act.rbegin();
				////e--;
				//std::wstring sDataBeg(Act.begin()->first.begin(), Act.begin()->first.end());
				//std::wstring sDataEnd(Act.rbegin()->first.begin(), Act.rbegin()->first.end());
				//
				//Gdiplus::RectF RectText(RectG);
				//Gdiplus::StringFormat stringFormat;
				//stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
				//
				//stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
				//DrawTime(temp, RectText, sDataBeg, stringFormat);
				//
				//stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
				//DrawTime(temp, RectText, sDataEnd, stringFormat);


				std::wstring SaveFile(fImage.begin(), fImage.end());
				backBuffer.Save(SaveFile.c_str(), &guidJpeg, NULL);

				DeleteDC(hdc);
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		}


		HPDF_REAL CassettePdfClass::DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
		{
			HPDF_REAL Y = top - 0;
			try
			{
				std::string ss = Sheet.Start_at;
				std::vector <std::string>split;
				boost::split(split, ss, boost::is_any_of(" "), boost::token_compress_on);

				

				draw_text_rect (page, left + 0, Y, w, YP, "Дата и время загрузки");
				if(split.size() >= 2)
				{
					draw_text_rect (page, left + 270, Y, XP, YP, split[0]);  //Дата
					draw_text_rect (page, left + 340, Y, XP, YP, split[1]);  //Время
				}
				Y -= 25;
				draw_text (page, left + 10, Y, "Параметры");
				draw_text (page, left + 280, Y, "Задание");
				draw_text (page, left + 355, Y, "Факт");

				Y -= 20;
				draw_text_rect (page, left + 0, Y, w, YP, "Время нахождения листа в закалочной печи. мин");
				draw_text_rect (page, left + 270, Y, XP, YP, Sheet.TimeForPlateHeat);//Задание Время нахождения листа в закалочной печи. мин
				draw_text_rect (page, left + 340, Y, XP, YP, Sheet.DataTime_All);    //Факт Время нахождения листа в закалочной печи. мин

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Температура закалки, °С");
				draw_text_rect (page, left + 270, Y, XP, YP, Sheet.Temper);			//Задание Температуры закалки
				draw_text_rect (page, left + 340, Y, XP, YP, strSrTemp);				//Факт Температуры закалки

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Давление воды в коллекторе закал. машины, бар");
				draw_text_rect (page, left + 270, Y, XP, YP, Sheet.PresToStartComp);	//Задание Давления воды
				draw_text_rect (page, left + 340, Y, XP, YP, Sheet.Za_PT3);			//Факт Давления воды

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Температура закалочной жидкости, °С");
				draw_text_rect (page, left + 340, Y, XP, YP, Sheet.Za_TE3);			//Факт Температура воды, °С

				Y -= 10;
				HPDF_Page_MoveTo (page, 15, Y);
				HPDF_Page_LineTo (page, Width - 20, Y);
				HPDF_Page_Stroke (page);
				Y -= 20;
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);

			return Y;
		}

		HPDF_REAL CassettePdfClass::DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
		{
			HPDF_REAL Y = top - 0;
			try
			{
#if _DEBUG

				std::stringstream ssd;
				ssd << "Печь № " << Cassette.Peth << " Кассета № ";
				ssd << std::setw(4) << std::setfill('0') << Cassette.Year << "-";
				ssd << std::setw(2) << std::setfill('0') << Cassette.Month << "-";
				ssd << std::setw(2) << std::setfill('0') << Cassette.Day << "-";
				ssd << std::setw(2) << std::setfill('0') << Cassette.CassetteNo;
				draw_text_rect (page, left, Y, w, YP, ssd.str().c_str());
				Y -= 25;
#endif // _DEBUG


				std::string ss = Cassette.Run_at;
				std::vector <std::string>split;
				boost::split(split, ss, boost::is_any_of(" "), boost::token_compress_on);

				draw_text_rect (page, left + 0, Y, w, YP, "Дата и время загрузки");
				if(split.size() >= 2)
				{
					draw_text_rect (page, left + 270, Y, XP, YP, split[0]);  //Дата
					std::string ss = split[1];
					std::vector <std::string>split1;
					boost::split(split1, ss, boost::is_any_of(" "), boost::token_compress_on);

					if(split1.size())
						draw_text_rect (page, left + 340, Y, XP, YP, split1[0]);  //Время
					else
						draw_text_rect (page, left + 340, Y, XP, YP, split[1]);  //Время
				}

				Y -= 25;
				draw_text (page, left + 10, Y, "Параметры");
				draw_text (page, left + 280, Y, "Задание");
				draw_text (page, left + 355, Y, "Факт");

				Y -= 20;
				draw_text_rect (page, left + 0, Y, w, YP, "Время нагрева до температуры отпуска, мин");

				float PointTime1 = Stof(Cassette.PointTime_1);
				float PointTime2 = PointTime1 + 60;
				std::stringstream sss;
				sss << boost::format("%.1f - ") % PointTime1;
				sss << boost::format("%.1f") % PointTime2;

				

				draw_text_rect (page, left + 270, Y, XP, YP, sss.str());								//Задание
				draw_text_rect (page, left + 340, Y, XP, YP, Cassette.HeatAcc);							//Факт

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Время выдержки при заданной температуре, мин");
				draw_text_rect (page, left + 270, Y, XP, YP, Cassette.PointDTime_2);						//Задание
				draw_text_rect (page, left + 340, Y, XP, YP, Cassette.HeatWait);							//Факт

				Y -= 25;
				draw_text_rect (page, left + 0, Y, w, YP, "Температура отпуска, °С");
				draw_text_rect (page, left + 270, Y, XP, YP, Cassette.PointRef_1);						//Задание
				draw_text_rect (page, left + 340, Y, XP, YP, Cassette.facttemper);							//Факт
				Y -= 20;
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
			return Y;
		}

		//Создание нового листа PDF
		bool CassettePdfClass::NewPdf()
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

				 //HPDF_SetCurrentEncoder(pdf, "CP1251");

				font = HPDF_GetFont(pdf, detail_font_name, "UTF-8");


				page = HPDF_AddPage(pdf);

				//tw = HPDF_Page_TextWidth (page, page_title);
				HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);


				//210x297 мм
				//HPDF_Page_SetWidth (page, H);
				//HPDF_Page_SetHeight (page, W);

				//HPDF_Page_SetWidth (index_page, 300);
				//HPDF_Page_SetHeight (index_page, 220);

				Height = HPDF_Page_GetHeight (page);
				Width = HPDF_Page_GetWidth(page);
				coeff = Width / Height;

				HPDF_Page_SetLineWidth(page, 2); //Ширина линии 0.0 - 1.0
				HPDF_Page_Rectangle(page, 10, 10, Width - 20, Height - 20);
				HPDF_Page_Stroke(page);


				HPDF_Page_SetLineWidth(page, 0.5); //Ширина линии 0.0 - 1.0
				//HPDF_Page_SetRGBStroke (page, 0, 0, 0);
				//HPDF_Page_SetRGBFill (page, 0.75, 0.0, 0.0);
				return true;

			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
			return false;
		}


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


		//Сохранение листа PDF
		void CassettePdfClass::SavePDF()
		{
			std::stringstream urls;
			//urls << "\\\\192.168.9.63\\";
			std::stringstream temp;
			try
			{
				temp << lpLogPdf;
				urls << lpLogPdf;
				CheckDir(temp.str());
			}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

			std::tm TM;
			DataTimeOfString(Sheet.Start_at, FORMATTIME, TM);
			
			try
			{
				temp << "/" << TM.tm_year;
				urls << "/" << TM.tm_year;
				CheckDir(temp.str());
			}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

			try
			{
				temp << "/" << MonthName[TM.tm_mon];
				urls << "/" << MonthName[TM.tm_mon];
				CheckDir(temp.str());
			}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " " + std::to_string(TM.tm_mon) + " ");

			try
			{
				temp << "/" << std::setw(2) << std::setfill('0') << std::right << TM.tm_mday;
				urls << "/" << std::setw(2) << std::setfill('0') << std::right << TM.tm_mday;
				CheckDir(temp.str());
			}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

			//try
			//{
				//temp << "/";
				//temp << std::setw(4) << std::setfill('0') << std::right << Cassette.Year << "-";
				//temp << std::setw(2) << std::setfill('0') << std::right << Cassette.Month << "-";
				//temp << std::setw(2) << std::setfill('0') << std::right << Cassette.Day << "-";
				//temp << std::setw(2) << std::setfill('0') << std::right << Cassette.CassetteNo;
			//
	//В дебуге Удаляем катологи
	//#ifdef _DEBUG
				//if(!index)
				//{
				//	std::filesystem::path dir = temp.str();
				//	boost::system::error_code ec;
				//	std::filesystem::remove_all(dir, ec);
				//	std::string err = "";
				//	if(ec)
				//	{
				//		std::vector<boost::filesystem::path> patch = getDir(temp.str());
				//		for(auto& p : patch)
				//			remove(p.string().c_str());
				//		if(ec)
				//			LOG_ERROR(PdfLogger, "{:90}| Eor Delete dir: \"{}\"", FUNCTION_LINE_NAME, ec.message());
				//		patch.erase(patch.begin(), patch.end());
				//	}
				//}
	//#endif
				//CheckDir(temp.str());
			//}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + temp.str() + " ");

			std::stringstream tfile;
			std::stringstream ifile;
			try
			{
				tfile << std::setw(6) << std::setfill('0') << Sheet.Melt << "-";
				tfile << std::setw(3) << std::setfill('0') << Sheet.PartNo << "-";
				tfile << std::setw(3) << std::setfill('0') << Sheet.Pack << "-";
				tfile << std::setw(3) << std::setfill('0') << Sheet.Sheet << "-";
				tfile << std::setw(2) << std::setfill('0') << Sheet.SubSheet;

				ifile << std::setw(4) << std::setfill('0') << Cassette.Year << "-";
				ifile << std::setw(2) << std::setfill('0') << Cassette.Month << "-";
				ifile << std::setw(2) << std::setfill('0') << Cassette.Day << "-";
				ifile << std::setw(2) << std::setfill('0') << Cassette.CassetteNo;
				//tfile << ".pdf";
			}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + temp.str() + "/" + tfile.str());

			try
			{

	#pragma region Создаем 2 и последуюие файлы

				std::string ImgeName;
				FileName = temp.str() + "/" + tfile.str() + ".pdf";
				

				int n = 0;
				while(std::filesystem::exists(FileName))
				{
					n++;
					std::string s = FileName;
					std::vector <std::string>split;
					boost::split(split, s, boost::is_any_of("."));
					if(split.size() == 2)
						FileName = split[0] + "_" + std::to_string(n) + "." + split[1];
				}

				if(n)
				{
					ImgeName = temp.str() + "/" + ifile.str() + "_" + std::to_string(n) + ".jpg";
				}	
				else 
					ImgeName = temp.str() + "/" + ifile.str() + ".jpg";

	#pragma endregion

				try
				{
					if(std::filesystem::exists(furnImage))
						std::filesystem::copy_file(furnImage, ImgeName, std::filesystem::copy_options::skip_existing | std::filesystem::copy_options::overwrite_existing);
				}
				CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + FileName + " ");

				HPDF_SaveToFile (pdf, FileName.c_str());
				HPDF_Free (pdf);

				{
					std::string PdfName;
					if(n)	PdfName = urls.str() + "/" + tfile.str() + "_" + std::to_string(n) + ".pdf";
					else	PdfName = urls.str() + "/" + tfile.str() + ".pdf";


					std::stringstream ssd;
					ssd << "UPDATE sheet SET pdf = '" << PdfName;
					ssd << "' WHERE id = " << Sheet.id;
					std::string comand = ssd.str();
					PGresult* res = conn.PGexec(comand); 
					if(PQresultStatus(res) == PGRES_FATAL_ERROR)
						LOG_ERROR(PdfLogger, "{:89}| {}", (std::string(__FUNCTION__) + std::string(":") + std::to_string(1037)), comand);;
					PQclear(res);
				}
			}
			CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + FileName + " ");


		}

		HPDF_REAL CassettePdfClass::DrawHeder(HPDF_REAL left, HPDF_REAL top)
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
				draw_text_rect (page, left + 100, Y, XP, YP, Sheet.Alloy);

				draw_text (page, left + 220, Y, "Толщина, мм");
				draw_text_rect (page, left + 300, Y, XP, YP, Sheet.Thikness);


				Y -= 30;
				draw_text (page, left + 20, Y, "Плавка");
				draw_text_rect (page, left + 70, Y, XP, YP, Sheet.Melt);

				draw_text (page, left + 180, Y, "Партия");
				draw_text_rect (page, left + 230, Y, XP, YP, Sheet.PartNo);

				draw_text (page, left + 350, Y, "Пачка");
				draw_text_rect (page, left + 390, Y, XP, YP, Sheet.Pack);

				draw_text (page, left + 510, Y, "Лист");
				draw_text_rect (page, left + 550, Y, XP, YP, Sheet.Sheet + " / " + Sheet.SubSheet);

				draw_text (page, left + 680, Y, "Сляб");
				draw_text_rect (page, left + 720, Y, XP, YP, Sheet.Slab);

				Y -= 10;
				HPDF_Page_MoveTo (page, left + 15, Y);
				HPDF_Page_LineTo (page, left + Width - 20, Y);
				HPDF_Page_Stroke (page);
				Y -= 20;
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
			return Y;
		}

		//Рисуем Закалка
		HPDF_REAL CassettePdfClass::DrawKpvlPDF(HPDF_REAL left, HPDF_REAL top)
		{
			HPDF_REAL Y = top - 5;
			try
			{
				//График температуры закалки
				HPDF_Page_SetFontAndSize (page, font, 12);
				draw_text(page, 380, Y, "Закалка");
				Y -= 25;

				HPDF_Page_SetFontAndSize (page, font, 10);
				HPDF_REAL Y1 = DrawKpvl(410, Y, Width - 432);

				HPDF_REAL r1 = Y - Y1;
				HPDF_REAL r3 = top - Y1 + (Y - Y1);

				HPDF_Page_Rectangle(page, 20, r3 - 15/*Height - 270*/, 374, r1 - 10);
				HPDF_Page_Stroke(page);

				//std::ifstream ifs(tempImage);
				//if(!ifs.bad())
				std::fstream ifs(tempImage, std::fstream::binary | std::fstream::in);
				if(ifs.is_open())
				{
					ifs.close();
					HPDF_Image image1 = HPDF_LoadJpegImageFromFile(pdf, tempImage.c_str());
					HPDF_Page_DrawImage (page, image1, 22, r3 - 14/*Height - 269*/, 370, r1 - 13);
				}
				Y = r3 - 20;
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
			return Y;
		}

		//Рисуем Отпуск
		HPDF_REAL CassettePdfClass::DrawFurnPDF(HPDF_REAL left, HPDF_REAL top)
		{
			HPDF_REAL Y = top - 5;
			try
			{
		 //График температуры отпуска
				HPDF_Page_SetFontAndSize (page, font, 12);
				draw_text(page, 380, Y, "Отпуск");
				Y -= 25;

				HPDF_Page_SetFontAndSize (page, font, 10);
				HPDF_REAL Y1 = DrawFurn(410, Y, Width - 432);

				HPDF_REAL r0 = Height - 440 - (23 + 5);
				HPDF_REAL r1 = Y - Y1;
				HPDF_REAL r3 = top - Y1 + (Y - Y1);

				HPDF_Page_Rectangle(page, 20, r0, 374, 140);
				//HPDF_Page_Rectangle(page, 20, r3 - 15, 374, r1 - 10);

				HPDF_Page_Stroke(page);
				std::fstream ifs(furnImage, std::fstream::binary | std::fstream::in);
				//std::ifstream ifs(furnImage);
				//if(!ifs.bad())
				if(ifs.is_open())
				{
					ifs.close();
					HPDF_Image image2 = HPDF_LoadJpegImageFromFile(pdf, furnImage.c_str());
					HPDF_Page_DrawImage (page, image2, 22, r0 + 1, 370, 137);
					//HPDF_Page_DrawImage (page, image2, 22, r3 - 14, 370, r1 - 13);
				}
			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
			return Y;
		}

		CassettePdfClass::CassettePdfClass(TSheet& sheet, bool view)
		{
			try
			{
				stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar); 
				stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

	  #pragma region Готовим графики
				Sheet = sheet;

				if(!conn.connection())
					return;// throw std::exception(__FUN(std::string("Error SQL conn connection to GraffKPVL")));
				conn.Name = "test";

				std::stringstream sss;
				sss << "Кассета: "
					<< std::setw(4) << std::setfill('0') << Cassette.Year << "-"
					<< std::setw(2) << std::setfill('0') << Cassette.Month << "-"
					<< std::setw(2) << std::setfill('0') << Cassette.Day << " № "
					<< std::setw(2) << std::setfill('0') << Cassette.CassetteNo
					<< " Лист: "
					<< std::setw(6) << std::setfill('0') << Sheet.Melt << "-"
					//<< std::setw(3) << std::setfill('0') << Sheet.Slab << "-"
					<< std::setw(3) << std::setfill('0') << Sheet.PartNo << "-"
					<< std::setw(3) << std::setfill('0') << Sheet.Pack << "-"
					<< std::setw(3) << std::setfill('0') << Sheet.Sheet << "/"
					<< std::setw(2) << std::setfill('0') << Sheet.SubSheet;
				SetWindowText(hWndDebug, sss.str().c_str());


				if(Stoi(Sheet.Year) && Stoi(Sheet.Month) && Stoi(Sheet.Day) && Stoi(Sheet.CassetteNo))
					GetCassette(Cassette);


				std::stringstream ssd;
				ssd << std::setw(4) << std::setfill('0') << Cassette.Year << "-";
				ssd << std::setw(2) << std::setfill('0') << Cassette.Month << "-";
				ssd << std::setw(2) << std::setfill('0') << Cassette.Day << "-";
				ssd << std::setw(2) << std::setfill('0') << Cassette.CassetteNo << ".jpg";
				furnImage = ssd.str();

				std::stringstream ssh;
				ssh << std::setw(6) << std::setfill('0') << Sheet.Melt << "-";
				ssh << std::setw(3) << std::setfill('0') << Sheet.PartNo << "-";
				ssh << std::setw(3) << std::setfill('0') << Sheet.Pack << "-";
				ssh << std::setw(3) << std::setfill('0') << Sheet.Sheet << "-";
				ssh << std::setw(2) << std::setfill('0') << Sheet.SubSheet << ".jpg";


				FurnRef.erase(FurnRef.begin(), FurnRef.end());
				FurnAct.erase(FurnAct.begin(), FurnAct.end());
				TempAct.erase(TempAct.begin(), TempAct.end());
				TempRef.erase(TempRef.begin(), TempRef.end());

				int P = atoi(Cassette.Peth.c_str());

				if(Cassette.Run_at.length() && Cassette.Finish_at.length())
				{
					int Ref_ID = 0;
					int Act_ID = 0;

					//Первая отпускная печь
					if(P == 1)
					{
						Ref_ID = ForBase_RelFurn_1.TempRef->ID;
						Act_ID = ForBase_RelFurn_1.TempAct->ID;
					}

					//Вторая отпускная печь
					if(P == 2)
					{
						Ref_ID = ForBase_RelFurn_2.TempRef->ID;
						Act_ID = ForBase_RelFurn_2.TempAct->ID;
					}

					if(Ref_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, Ref_ID);
					if(Act_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, Act_ID);
				}
			
				//Рисуем график FURN
				PaintGraff(FurnAct, FurnRef, furnImage);

				//Закалка
				GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1->ID);
				SqlTempActKPVL(TempAct);

				//Рисуем график KPVL
				PaintGraff(TempAct, TempRef, tempImage);

	  #pragma endregion

				//Создание PFD файла
				if(NewPdf())
				{
					//Рисуем PDF заголовок
					HPDF_REAL Y = DrawHeder(0, Height);

					//Рисуем PDF Закалка
					DrawKpvlPDF(0, Height);

					//Рисуем PDF Отпуск
					DrawFurnPDF(0, Height);

					//Сохраняем PDF
					SavePDF();

					////Отображение PDF
					//if(view) std::system(("start " + FileName).c_str());
				}
				remove(furnImage.c_str());
				remove(tempImage.c_str());

			}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		}

		void OutDebugInfo(TCassette& Cassette, TSheet& Sheet)
		{
			std::stringstream sss;
			sss << "Кассета: "
				<< std::setw(4) << std::setfill('0') << Cassette.Year << "-"
				<< std::setw(2) << std::setfill('0') << Cassette.Month << "-"
				<< std::setw(2) << std::setfill('0') << Cassette.Day << " № "
				<< std::setw(2) << std::setfill('0') << Cassette.CassetteNo
				<< " Лист: "
				<< std::setw(6) << std::setfill('0') << Sheet.Melt << "-"
				//<< std::setw(3) << std::setfill('0') << Sheet.Slab << "-"
				<< std::setw(3) << std::setfill('0') << Sheet.PartNo << "-"
				<< std::setw(3) << std::setfill('0') << Sheet.Pack << "-"
				<< std::setw(3) << std::setfill('0') << Sheet.Sheet << "/"
				<< std::setw(2) << std::setfill('0') << Sheet.SubSheet;
			SetWindowText(hWndDebug, sss.str().c_str());

		}

		CassettePdfClass::CassettePdfClass(TCassette& TC)
		{
			try
			{
				Cassette = TC;

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
					ssd << std::setw(4) << std::setfill('0') << Cassette.Year << "-";
					ssd << std::setw(2) << std::setfill('0') << Cassette.Month << "-";
					ssd << std::setw(2) << std::setfill('0') << Cassette.Day << "-";
					ssd << std::setw(2) << std::setfill('0') << Cassette.CassetteNo << ".jpg";
					furnImage = ssd.str();
				}
				#pragma endregion


				if(!conn.connection()) return;
				GetSheet();

				#pragma region Номер печи для FurnRef и FurnAct
				{
					FurnRef.erase(FurnRef.begin(), FurnRef.end());
					FurnAct.erase(FurnAct.begin(), FurnAct.end());

					int P = atoi(Cassette.Peth.c_str());
					if(Cassette.Run_at.length() && Cassette.Finish_at.length())
					{

						int Ref_ID = 0;
						int Act_ID = 0;

						//Первая отпускная печь
						if(P == 1)
						{
							Ref_ID = ForBase_RelFurn_1.TempRef->ID;
							Act_ID = ForBase_RelFurn_1.TempAct->ID;
						}

						//Вторая отпускная печь
						if(P == 2)
						{
							Ref_ID = ForBase_RelFurn_2.TempRef->ID;
							Act_ID = ForBase_RelFurn_2.TempAct->ID;
						}

						if(Ref_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, Ref_ID);
						if(Act_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, Act_ID);

					}
					else return;

					//Рисуем график FURN
					PaintGraff(FurnAct, FurnRef, furnImage);
				}
				#pragma endregion

			#pragma endregion

				for(auto& a : AllPfdSheet)
				{
					if(!isRun) break;
					Sheet = a;

					#pragma region tempImage.jpg
					{
						std::stringstream ssh;
						ssh << std::setw(6) << std::setfill('0') << Sheet.Melt << "-";
						ssh << std::setw(3) << std::setfill('0') << Sheet.PartNo << "-";
						ssh << std::setw(3) << std::setfill('0') << Sheet.Pack << "-";
						ssh << std::setw(3) << std::setfill('0') << Sheet.Sheet << "-";
						ssh << std::setw(2) << std::setfill('0') << Sheet.SubSheet << ".jpg";
						tempImage = ssh.str();
					}
					#pragma endregion				

					OutDebugInfo(Cassette, Sheet);

					#pragma region Графики закалки
					{
						TempRef.erase(TempRef.begin(), TempRef.end());

						//Закалка
						GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1->ID);
						SqlTempActKPVL(TempAct);

						//Рисуем график KPVL
						PaintGraff(TempAct, TempRef, tempImage);

						GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1->ID);

						//TempAct.erase(TempAct.begin(), TempAct.end());
						SqlTempActKPVL(TempAct);

						//Рисуем график KPVL
						PaintGraff(TempAct, TempRef, tempImage);
					}
					#pragma endregion

					#pragma region Создание PFD файла

					if(NewPdf())
					{
						//Рисуем PDF заголовок
						HPDF_REAL Y1 = DrawHeder(0, Height);

						//Рисуем PDF Закалка
						HPDF_REAL Y2 = DrawKpvlPDF(0, Y1) - 30;

						//Рисуем PDF Отпуск
						HPDF_REAL Y3 = DrawFurnPDF(0, Y2);

						//auto index = a.index();
						//Сохраняем PDF
						SavePDF();

					}

					#pragma endregion

					remove(tempImage.c_str());
					//return;
				}
				remove(furnImage.c_str());
				{
					std::stringstream ssd;
					ssd << "UPDATE cassette SET pdf = now() WHERE";
					ssd << " year = '" << Cassette.Year << "' AND";
					ssd << " month = '" << Cassette.Month << "' AND";
					ssd << " day = '" << Cassette.Day << "' AND";
					ssd << " cassetteno = " << Cassette.CassetteNo;
					SETUPDATESQL(PdfLogger, conn, ssd);
				}

			}
			CATCH(PdfLogger, FUNCTION_LINE_NAME);
		};






		std::string GetPdf::GetVal(PGConnection& conn, int ID, std::string Run_at, std::string End_at)
		{
			std::string f = "0";
			try
			{
				std::stringstream sss;
				sss << "SELECT DISTINCT ON (id) content"
					//", (SELECT tag.comment FROM tag WHERE tag.id = todos.id_name)"
					" FROM todos WHERE "
					<< " id_name = " << ID 			//Фактическое значение температуры
					<< " AND create_at >= '" << Run_at << "'"
					<< " AND create_at < '" << End_at << "'"
					<< " ORDER BY id DESC LIMIT 1";
				std::string comand = sss.str();
				PGresult* res = conn.PGexec(comand);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					if(line)
						f = conn.PGgetvalue(res, line - 1, 0);
				}
				else
				{
					LOG_ERR_SQL(PdfLogger, res, comand);
				}
				PQclear(res);
			}
			CATCH(PdfLogger, "");;
			return f;
		}

		void GetPdf::GetVal(PGConnection& conn, TCassette& P, T_ForBase_RelFurn* Furn)
		{
			try
			{
	#pragma region Готовим запрос в базу

				std::stringstream sss;
				sss << "SELECT DISTINCT ON (id_name) id_name, content"
					", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)"
					" FROM todos WHERE ("
					<< " todos.id_name = " << Furn->ActTimeHeatAcc->ID << " OR"	    //Факт время нагрева
					<< " todos.id_name = " << Furn->ActTimeHeatWait->ID << " OR"	//Факт время выдержки
					<< " todos.id_name = " << Furn->ActTimeTotal->ID				//Факт общее время
					<< ")"	//Факт время выдержки
					<< " AND todos.create_at >= '" << P.Run_at << "'"
					<< " AND todos.create_at < '" << P.End_at << "'"
					<< " AND cast (content as numeric) <> 0"
					<< " ORDER BY todos.id_name, todos.id DESC"; // LIMIT 3

	#pragma endregion

	#pragma region Запрос в базу

				std::string comand = sss.str();
				PGresult* res = conn.PGexec(comand);
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
							if(id == Furn->PointRef_1->ID) P.PointRef_1 = f;
							if(id == Furn->ActTimeHeatAcc->ID) P.HeatAcc = f;
							if(id == Furn->ActTimeHeatWait->ID) P.HeatWait = f;
							if(id == Furn->ActTimeTotal->ID) P.Total = f;
						}
					}
				}
				else
				{
					LOG_ERR_SQL(PdfLogger, res, comand);
				}
				PQclear(res);

	#pragma endregion
			}
			CATCH(PdfLogger, "");

			try
			{
	#pragma region Гогтвим запрос в базу

				std::stringstream sss;
				sss << "SELECT DISTINCT ON (id_name) id_name, content"
					", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)"
					" FROM todos WHERE ("
					<< " todos.id_name = " << Furn->PointRef_1->ID << " OR"		//Уставка температуры
					<< " todos.id_name = " << Furn->PointTime_1->ID << " OR"		//Задание Время нагрева
					<< " todos.id_name = " << Furn->PointDTime_2->ID << " OR"	//Задание Время выдержки
					<< " todos.id_name = " << Furn->TimeProcSet->ID				//Полное время процесса (уставка), мин
					<< ")"	//Факт время выдержки
					<< " AND todos.create_at < '" << P.End_at << "'"
					<< " ORDER BY todos.id_name, todos.id DESC"; // LIMIT 4

	#pragma endregion

	#pragma region Запрос в базу

				std::string comand = sss.str();
				PGresult* res = conn.PGexec(comand);
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
							if(id == Furn->PointRef_1->ID) P.PointRef_1 = f;
							if(id == Furn->PointTime_1->ID)P.PointTime_1 = f;
							if(id == Furn->PointDTime_2->ID) P.PointDTime_2 = f;
							if(id == Furn->TimeProcSet->ID) P.TimeProcSet = f;
						}
					}
				}
				else
				{
					LOG_ERR_SQL(PdfLogger, res, comand);
				}
				PQclear(res);

	#pragma endregion
			}
			CATCH(PdfLogger, "");

			try
			{
				std::tm TM;
				time_t temp = DataTimeOfString(P.End_at, FORMATTIME, TM);
				temp  = (time_t)difftime(temp, 5 * 60); //Вычисть 5 минут до конца отпуска
				localtime_s(&TM, &temp);
				std::string End_at  = GetDataTimeString(TM);
				P.facttemper = GetVal(conn, Furn->TempAct->ID, P.Run_at, End_at);

			}
			CATCH(PdfLogger, "");
		}

		void GetPdf::EndCassette(PGConnection& conn, TCassette& P, int Petch, std::fstream& s1)
		{
			try
			{
				T_ForBase_RelFurn* Furn = NULL;
				if(Petch == 1)
					Furn = &ForBase_RelFurn_1;
				if(Petch == 2)
					Furn = &ForBase_RelFurn_2;
				if(Furn == NULL) 
					throw std::runtime_error(__FUN(("Error patametr Furn = ") + std::to_string(Petch)));

				if(S107::IsCassette(P) && P.Run_at.length())
				{
					std::tm TM_Run, TM_End, TM_All, TM_Fin;
					//struct tm TM_All;
					std::time_t tm_Fin;

					std::time_t tm_Run = DataTimeOfString(P.Run_at, FORMATTIME, TM_Run);
					std::time_t tm_End = DataTimeOfString(P.End_at, FORMATTIME, TM_End);
					std::time_t tm_All = (time_t)difftime(tm_End, tm_Run);

					gmtime_s(&TM_All, &tm_All);
					TM_All.tm_year -= 1900;
					TM_All.tm_mday -= 1;

					tm_Fin = DataTimeOfString(P.End_at, FORMATTIME, TM_Fin);
					tm_Fin = tm_End + (15 * 60);
					localtime_s(&TM_Fin, &tm_Fin);

					//P1.End_at
					P.Finish_at = GetDataTimeString(TM_Fin);

					std::stringstream sg2;
					GetVal(conn, P, Furn);

					s1 << P.Id << ";"
						<< " " << P.Run_at << ";"
						<< " " << P.End_at << ";"
						<< P.Year << ";"
						<< P.Month << ";"
						<< P.Day << ";"
						<< P.CassetteNo << ";"
						<< " " << P.Error_at << ";"

						<< P.PointRef_1 << ";"		//Уставка температуры
						<< P.facttemper << ";"		//Фактическое значение температуры

						<< P.PointTime_1 << ";"		//Задание Время нагрева
						<< P.HeatAcc << ";"			//Факт время нагрева
						<< P.PointDTime_2 << ";"	//Задание Время выдержки
						<< P.HeatWait << ";"		//Факт время выдержки

						<< P.TimeProcSet << ";"		//Полное время процесса (уставка), мин
						<< P.Total << ";"			//Факт общее время

						<< std::endl;


					P.Peth = std::to_string(Petch);
				}
			}
			CATCH(PdfLogger, "");
		}


		void GetPdf::GetCassetteData(PGConnection& conn, std::string id, TCassette& ct, TCassette& it)
		{
			try
			{
				std::stringstream com;
				com << "SELECT * FROM cassette WHERE"
					<< " day = " << it.Day
					<< " AND month = " << it.Month
					<< " AND year = " << it.Year
					<< " AND cassetteno = " << it.CassetteNo
					<< " AND correct IS NULL"
					<< " ORDER BY id";

				std::string comand = com.str();
				if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
				PGresult* res = conn.PGexec(comand);
				//LOG_INFO(PdfLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					S107::GetColl(res);
					if(PQntuples(res))
						S107::GetCassette(res, ct, 0);
				}
				else
				{
					LOG_ERR_SQL(PdfLogger, res, comand);
					PQclear(res);
				}
			}
			CATCH(PdfLogger, "");
		}

		void GetPdf::SaveFileSdg(MapRunn& CassetteTodos)
		{
	#ifdef _DEBUG
			try
			{
	#pragma region Запись в файл заголовка Sdg.csv
				std::fstream sdg("Sdg.csv", std::fstream::binary | std::fstream::out);

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
			CATCH(PdfLogger, "");
	#endif
		}


		std::fstream fUpdateCassette;
		void GetPdf::SaveDataBaseForId(PGConnection& conn, TCassette& ct, TCassette& it)
		{
			std::stringstream sg2;
			try
			{
				//sg2 << "Коррекция базы " << P0.size() << " id = " << it.Id;
				//SetWindowText(hWndDebug, sg2.str().c_str());

				if(it.Id.length())ct.Id = it.Id;
				if(it.Year.length())ct.Year = it.Year;
				if(it.Month.length())ct.Month =it.Month;
				if(it.Day.length())ct.Day = it.Day;
				if(it.CassetteNo.length())ct.CassetteNo = it.CassetteNo;

				if(it.Peth.length())ct.Peth = it.Peth;
				if(it.Run_at.length())ct.Run_at = it.Run_at;
				if(it.Error_at.length())ct.Error_at = it.Error_at;
				if(it.End_at.length())ct.End_at = it.End_at;
				if(it.PointTime_1.length())ct.PointTime_1 = it.PointTime_1;    //Время разгона
				if(it.PointRef_1.length())ct.PointRef_1 = it.PointRef_1;      //Уставка температуры
				if(it.TimeProcSet.length())ct.TimeProcSet = it.TimeProcSet;    //Полное время процесса (уставка), мин
				if(it.PointDTime_2.length())ct.PointDTime_2 =it.PointDTime_2;   //Время выдержки
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
					ssd << "finish_at = '" << ct.Finish_at << "', correct = now(), pdf = DEFAULT";
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


				if(Stof(ct.PointDTime_2))
					ssd << ", pointdtime_2 = " << ct.PointDTime_2;
				if(Stof(ct.HeatWait))
					ssd << ", heatwait = " << ct.HeatWait;

				if(Stof(ct.TimeProcSet))
					ssd << ", timeprocset = " << ct.TimeProcSet;
				if(Stof(ct.Total))
					ssd << ", total = " << ct.Total;

				ssd << " WHERE id = " << ct.Id;
				std::string comand = ssd.str();

				SETUPDATESQL(PdfLogger, conn, ssd);


				fUpdateCassette << ssd.str() << std::endl;
				fUpdateCassette.flush();

				PrintCassettePdfAuto(ct);
			}
			CATCH(PdfLogger, "")
		}

		
		void GetPdf::SaveDataBaseNotId(PGConnection& conn, TCassette& ct, TCassette& it)
		{
			try
			{
				if(it.Id.length())ct.Id = it.Id;
				if(it.Year.length())ct.Year = it.Year;
				if(it.Month.length())ct.Month =it.Month;
				if(it.Day.length())ct.Day = it.Day;
				if(it.CassetteNo.length())ct.CassetteNo = it.CassetteNo;

				if(it.Peth.length())ct.Peth = it.Peth;
				if(it.Run_at.length())ct.Run_at = it.Run_at;
				if(it.Error_at.length())ct.Error_at = it.Error_at;
				if(it.End_at.length())ct.End_at = it.End_at;

				if(it.PointTime_1.length())ct.PointTime_1 = it.PointTime_1;    //Время разгона
				if(it.PointRef_1.length())ct.PointRef_1 = it.PointRef_1;      //Уставка температуры
				if(it.TimeProcSet.length())ct.TimeProcSet = it.TimeProcSet;    //Полное время процесса (уставка), мин
				if(it.PointDTime_2.length())ct.PointDTime_2 =it.PointDTime_2;   //Время выдержки
				if(it.facttemper.length())ct.facttemper = it.facttemper;			//Факт температуры за 5 минут до конца отпуска
				if(it.Finish_at.length())ct.Finish_at = it.Finish_at;        //Завершение процесса + 15 минут
				if(it.HeatAcc.length())ct.HeatAcc = it.HeatAcc;			//Факт время нагрева
				if(it.HeatWait.length())ct.HeatWait = it.HeatWait;          //Факт время выдержки
				if(it.Total.length())ct.Total = it.Total;				//Факт общее время

	//#ifndef TESTPDF2
				std::stringstream ssd;
				ssd << "INSERT INTO cassette ";
				ssd << "("
					"create_at, "
					"event, "
					"year, "
					"month, "
					"day, "
					"cassetteno, "
					"sheetincassette, "
					"peth, "
					"run_at, "
					"error_at, "
					"end_at, "
					"finish_at, "
					"pointtime_1, "
					"pointref_1, "
					"timeprocset, "
					"pointdtime_2, "
					"facttemper, "
					"heatacc, "
					"heatwait, "
					"total, "
					"correct"
					") VALUES (";

				if(ct.Create_at.length())
					ssd << "'" << ct.Create_at << "', ";
				else if(ct.Run_at.length())
					ssd << "'" << ct.Run_at << "', ";
				else
					ssd << "now(), ";

				ssd << "6, ";
				ssd << Stoi(ct.Year) << ", ";
				ssd << Stoi(ct.Month) << ", ";
				ssd << Stoi(ct.Day) << ", ";
				ssd << Stoi(ct.CassetteNo) << ", ";
				if(Stoi(ct.SheetInCassette))
					ssd << Stoi(ct.SheetInCassette) << ", ";
				else
					ssd <<  "-1 , ";

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
				ssd << Stof(ct.PointDTime_2) << ", ";
				ssd << Stof(ct.facttemper) << ", ";
				ssd << Stof(ct.HeatAcc) << ", ";
				ssd << Stof(ct.HeatWait) << ", ";
				ssd << Stof(ct.Total) << ", ";
				ssd << "now());";

				std::string comand = ssd.str();

				//std::string comand = sss.str();

				SETUPDATESQL(PdfLogger, conn, ssd);


				std::stringstream ssg;
				ssg << "SELECT id FROM cassette ";
				ssg << "WHERE day = " << ct.Day;
				ssg << " AND month = " << ct.Month;
				ssg << " AND year = " << ct.Year;
				ssg << " AND cassetteno = " << ct.CassetteNo;
				ssg << " ORDER BY id LIMIT 1";
				comand = ssg.str();

				if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
				PGresult* res = conn.PGexec(comand);
				if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
					it.Id = conn.PGgetvalue(res, 0, 0);
				PQclear(res);
				ct.Id = it.Id;

				fUpdateCassette << ssd.str();
				fUpdateCassette << "\t" << ct.Id << std::endl;
				fUpdateCassette.flush();

	//#endif
				PrintCassettePdfAuto(ct);
			}
			CATCH(PdfLogger, "");
		}

		void GetPdf::SaveDataBase(PGConnection& conn, TCassette& ct, TCassette& it)
		{
			//Запись в базу
			std::stringstream ssg;
			ssg << "SELECT id FROM cassette ";
			ssg << "WHERE day = " << it.Day;
			ssg << " AND month = " << it.Month;
			ssg << " AND year = " << it.Year;
			ssg << " AND cassetteno = " << it.CassetteNo;
			ssg << " ORDER BY id LIMIT 1";
			std::string comand = ssg.str();

			if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
			PGresult* res = conn.PGexec(comand);
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

		void GetPdf::SaveFileCass()
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
							<< it.second.CassetteNo << ";"
							<< " " << it.second.Error_at << ";"
							<< it.second.Peth << ";"
							<< it.second.PointRef_1 << ";"		//Уставка температуры
							<< it.second.facttemper << ";"		//Фактическое значение температуры
							<< it.second.PointTime_1 << ";"		//Задание Время нагрева
							<< it.second.HeatAcc << ";"			//Факт время нагрева
							<< it.second.PointDTime_2 << ";"	//Задание Время выдержки
							<< it.second.HeatWait << ";"		//Факт время выдержки
							<< it.second.TimeProcSet << ";"		//Полное время процесса (уставка), мин
							<< it.second.Total << ";"			//Факт общее время

							<< std::endl;
					}
				}
				s1.close();
	#pragma endregion
			}
			CATCH(PdfLogger, "");
	//#endif
		}


		int lin = 1;
		void GetPdf::SaveBaseCassete(PGConnection& conn, TCassette& tc)
		{
			TCassette ct;
			if(tc.Run_at.length() && tc.End_at.length() && S107::IsCassette(tc))
			{
				//std::string sg = std::string("Получение кассеты из базы ") + std::to_string(P0.size()) + std::string(" :") + std::to_string(lin++);
				//SetWindowText(hWndDebug, sg.c_str());

				GetCassetteData(conn, tc.Id, ct, tc);
				if(ct.facttemper.length())	tc.facttemper = ct.facttemper;
				if(ct.Id.length())			tc.Id = ct.Id;

				if(!tc.Finish_at.length())
				{
					std::tm TM;
					time_t tm = DataTimeOfString(tc.End_at, FORMATTIME, TM);
					tm += (60 * 15);
					tc.Finish_at = GetDataTimeString(tm);
				}
				SaveDataBase(conn, ct, tc);
			}
		}

		void GetPdf::GetCassette(PGConnection& conn, MapRunn& CassetteTodos, int Petch)
		{
			TCassette P;
			T_ForBase_RelFurn* Furn = NULL;
			try
			{
				if(Petch == 1)
					Furn = &ForBase_RelFurn_1;
				if(Petch == 2)
					Furn = &ForBase_RelFurn_2;
				if(Furn == NULL) return;

				std::stringstream ssd;
				ssd << "SELECT todos.create_at, todos.id, todos.id_name, todos.content"
					", (SELECT type FROM tag WHERE tag.id = todos.id_name) "
					", (SELECT comment AS name FROM tag WHERE tag.id = todos.id_name) "
					"FROM todos WHERE create_at >= '" << DateStart << "' AND ("
					<< "id_name = " << Furn->ProcEnd->ID
					<< " OR id_name = " << Furn->ProcRun->ID
					<< " OR id_name = " << Furn->ProcFault->ID

					<< " OR id_name = " << Furn->Cassette.Day->ID
					<< " OR id_name = " << Furn->Cassette.Month->ID
					<< " OR id_name = " << Furn->Cassette.Year->ID
					<< " OR id_name = " << Furn->Cassette.CassetteNo->ID
					<< ") AND content <> '0' AND content <> 'false' ORDER BY id";

				std::string comand = ssd.str();
				PGresult* res = conn.PGexec(comand);
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					TodosColumn(res);

					int line = PQntuples(res);
					std::string old_value = "";
					for(int l = 0; l < line && isRun; l++)
					{
						//std::stringstream ssd;
						//ssd << "SaveCassette: " << line << ":" << l;
						//SetWindowText(hWndDebug, ssd.str().c_str());


						T_Todos ct;
						ct.value = conn.PGgetvalue(res, l, TODOS::content);
						ct.create_at = conn.PGgetvalue(res, l, TODOS::create_at);
						ct.id = Stoi(conn.PGgetvalue(res, l, TODOS::id));
						//ct.id_name_at = "";
						ct.id_name = Stoi(conn.PGgetvalue(res, l, TODOS::id_name));
						ct.type =  Stoi(conn.PGgetvalue(res, l, TODOS::type));
						ct.content = PDF::GetVarVariant((OpcUa::VariantType)ct.type, ct.value);
						ct.id_name_at = conn.PGgetvalue(res, l, TODOS::name);
						ct.Petch = Petch;
						CassetteTodos[ct.id] = ct;
					}
				}
				else
				{
					LOG_ERR_SQL(PdfLogger, res, comand);
				}
				PQclear(res);
			}
			CATCH(PdfLogger, "");

			try
			{
				std::string name = "cass" + std::to_string(Petch) + ".csv";
				std::fstream s1(name, std::fstream::binary | std::fstream::out);

				int lin = 0;
				auto size = CassetteTodos.size();
				for(auto& a : CassetteTodos)
				{
					if(!isRun)break;
					lin++;

					//std::stringstream ssd;
					//ssd << "GetCassette:CassetteTodos " << Petch << ", " << size << ":" << lin;
					//SetWindowText(hWndDebug, ssd.str().c_str());

					//Furn->Cassette.Day->ID
					try
					{
						if(a.second.id_name == Furn->Cassette.Day->ID)
						{
							if(!P.Run_at.size())
								P.Day = a.second.value;
							else if(P.Day != a.second.value)
							{
								P.End_at = a.second.create_at;
								//EndCassette(conn, P, Petch, s1);
								P0[a.first] = P;

								P.Run_at = "";
								P.End_at = "";
								P.Error_at = "";
								P.Day = a.second.value;
							}
						}
					}
					CATCH(PdfLogger, "");

					//Furn->Cassette.Month->ID
					try
					{
						if(a.second.id_name == Furn->Cassette.Month->ID)
						{
							if(!P.Run_at.size())
								P.Month = a.second.value;
							else if(P.Month != a.second.value)
							{
								P.End_at = a.second.create_at;
								//EndCassette(conn, P, Petch, s1);
								P0[a.first] = P;

								P.Run_at = "";
								P.End_at = "";
								P.Error_at = "";
								P.Month = a.second.value;
							}
						}
					}
					CATCH(PdfLogger, "");

					//Furn->Cassette.Year->I
					try
					{
						if(a.second.id_name == Furn->Cassette.Year->ID)
						{
							if(!P.Run_at.size())
								P.Year = a.second.value;
							else if(P.Year != a.second.value)
							{

								P.End_at = a.second.create_at;
								//EndCassette(conn, P, Petch, s1);
								P0[a.first] = P;

								P.Run_at = "";
								P.End_at = "";
								P.Error_at = "";
								P.Year = a.second.value;
							}
						}
					}
					CATCH(PdfLogger, "");

					//Furn->Cassette.CassetteNo->ID
					try
					{
						if(a.second.id_name == Furn->Cassette.CassetteNo->ID)
						{
							if(!P.Run_at.size())
								P.CassetteNo = a.second.value;
							else if(P.CassetteNo != a.second.value)
							{
								P.End_at = a.second.create_at;
								//EndCassette(conn, P, Petch, s1);
								P0[a.first] = P;
								P.Run_at = "";
								P.End_at = "";
								P.Error_at = "";
								P.CassetteNo = a.second.value;
							}
						}
					}
					CATCH(PdfLogger, "");

					//Furn->ProcRun->ID
					try
					{
						if(a.second.id_name == Furn->ProcRun->ID && a.second.content.As<bool>())
						{
							if(S107::IsCassette(P))
							{
								if(!P.Run_at.size())
								{
									P.Run_at = a.second.create_at;
								}
								else
								{
									//P.End_at = a.second.create_at;
									//EndCassette(conn, P, Petch, s1);
									//P0[a.first] = P;
									//P = TCassette();
									//
									//P.Run_at = a.second.create_at;
								}
							}
						}
					}
					CATCH(PdfLogger, "");

					//urn->ProcEnd->ID
					try
					{
						if(a.second.id_name == Furn->ProcEnd->ID && a.second.content.As<bool>())
						{
							P.End_at = a.second.create_at;
							EndCassette(conn, P, Petch, s1);
							P0[a.first] = P;

							SaveBaseCassete(conn, P);
							P = TCassette();
						}
					}
					CATCH(PdfLogger, "");

					//Furn->ProcFault->ID
					try
					{
						if(a.second.id_name == Furn->ProcFault->ID && a.second.content.As<bool>())
						{
							if(!P.Error_at.length() && S107::IsCassette(P))
							{
								P.Error_at = a.second.create_at;
							}
						}
					}
					CATCH(PdfLogger, "");

				}
				s1.close();
			}
			CATCH(PdfLogger, "");
		}

		void GetPdf::GetCassette(PGConnection& conn)
		{
			MapRunn CassetteTodos;
			SetWindowText(hWndDebug, "SaveCassette");
			try
			{
				GetCassette(conn, CassetteTodos, 1);
				GetCassette(conn, CassetteTodos, 2);

				SaveFileSdg(CassetteTodos);
			}
			CATCH(PdfLogger, "");
		}


		void GetPdf::CorrectSQL(PGConnection& conn)
		{
			try
			{
				//MapRunn CassetteTodos;
				P0.erase(P0.begin(), P0.end());

				SetWindowText(hWndDebug, "connection");

				GetCassette(conn);

				SaveFileCass();

				//int lin = 0;
				//for(auto& it : P0)
				//{
				//	if(!isRun)return;
				//	lin++;
				//
				//	if(!S107::IsCassette(it.second)) continue;
				//}

			}
			CATCH(PdfLogger, "");

			SetWindowText(hWndDebug, "Закончили коррекцию");
		}

		GetPdf::GetPdf(PGConnection& conn, std::string datestart)
		{
			try
			{
//#ifndef _DEBUG
				DateStart = datestart;
				//SELECT create_at FROM cassette WHERE correct IS NULL AND finish_at IS NOT NULL AND  delete_at IS NULL ORDER BY id ASC LIMIT 1
				//std::stringstream ssa;
				//ssa << "SELECT create_at FROM cassette WHERE ";
				//ssa << "(event = 2 OR event = 4) AND ";
				//ssa << "correct IS NULL AND ";
				////ssa << " create_at >= '2024-04-16' AND";
				//ssa << "finish_at IS NOT NULL AND ";
				//ssa << " delete_at IS NULL";
				//ssa << " ORDER BY id ASC LIMIT 1";
				//std::string comand = ssa.str();

				////Удаление инфы о коррекции кассет
				//std::stringstream ssa;
				//ssa << "UPDATE cassette SET  correct = DEFAULT, pdf = DEFAULT WHERE run_at >= '2024-05-01';";
				//SETUPDATESQL(PdfLogger, conn, ssa);
				
				//std::string comand = "SELECT create_at FROM cassette WHERE correct IS NULL AND finish_at IS NOT NULL AND  delete_at IS NULL ORDER BY id ASC LIMIT 1";


				//std::string comand = "SELECT * FROM cassette WHERE finish_at IS NULL ORDER BY id ASC LIMIT 1";
				//PGresult* res = conn.PGexec(comand);
				//if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
				//	DateStart = conn.PGgetvalue(res, 0, 0);
				//PQclear(res);
				//DateStart = "2024-05-31";
				
				//DateStart = "2024-05-25 04:30:00";
//#else
//				DateStart = "2024-04-16 00:00:00";
//#endif

				if(!DateStart.length())
				{
//#ifndef _DEBUG
					return;
//#else
//					std::time_t st = time(NULL);
//					st = (std::time_t)difftime(st, 60 * 60 * 24 * 1); //за 1 суток
//					DateStart = GetDataTimeString(&st);
//#endif // !_DEBUG
				}

				remove("Cass.csv");
				remove("cass1.csv");
				remove("cass2.csv");
				remove("Sdg.csv");
				remove("all_tag.csv");
				remove("UpdateCassette.txt");

				LOG_INFO(PdfLogger, "{:90}| DateStart = {}", FUNCTION_LINE_NAME, DateStart);
				fUpdateCassette = std::fstream("UpdateCassette.txt", std::fstream::binary | std::fstream::out | std::ios::app);
				CorrectSQL(conn);
				fUpdateCassette.close();
				LOG_INFO(PdfLogger, "{:90}| End CassetteSQL", FUNCTION_LINE_NAME);
			}
			CATCH(PdfLogger, "");

		}






		//Автоматическое создание по кассете
		void PrintCassettePdfAuto(TCassette& TC)
		{
			try
			{
				if(TC.Run_at.length() && TC.Finish_at.length())
				{
					std::stringstream sss;
					sss << "PrintPdfAuto ";
					sss << boost::format("%|04|-") % TC.Year;
					sss << boost::format("%|02|-") % TC.Month;
					sss << boost::format("%|02| ") % TC.Day;
					sss << " № " << TC.CassetteNo;
					SetWindowText(hWndDebug, sss.str().c_str());
					PDF::Cassette::CassettePdfClass pdf(TC);
				}
			}
			CATCH(PdfLogger, FUNCTION_LINE_NAME);
		}

		//Открывается по клику на лист
		void PrintCassettePdfAuto(TSheet& Sheet)
		{
			try
			{
				PDF::Cassette::CassettePdfClass* pdf  = new PDF::Cassette::CassettePdfClass(Sheet);
			}
			CATCH(PdfLogger, std::string("PrintPdf: "));
		}

		//Автоматическое создание по листам
		void RunAlCassettelPdfAuto(TSheet& Sheet, bool view)
		{
			try
			{
				PDF::Cassette::CassettePdfClass pdf(Sheet, view);
			}
			CATCH(PdfLogger, std::string("PrintPdf: "));
		}

//Отключаем поиск кассет
#define NOCASSETTE

	};


	typedef std::vector<T_IdSheet>V_IdSheet;
	//std::vector <T_IdSheet> IdSheet;
	

#define NOLIST
	namespace SHEET
	{

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
		const int st3_3 = 3;	//Выдача заготовки
		const int st3_4 = 4;	//Окончание цикла обработки

		int AllId;
		int iAllId;

		std::string StartSheet;
		std::string StopSheet;

		//const std::string StartSheet = "2024-05-13 14:40:00";
		//const std::string StopSheet = "2024-05-13 16:00:00";

		//const std::string StartSheet = "2024-05-16 05:56:20";

		typedef std::vector<T_Todos> MapSheetTodos;

		void GetTodosSQL(PGConnection& conn, MapSheetTodos& mt, std::string& comand)
		{

			if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
			PGresult* res = conn.PGexec(comand);
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
						if(nFields >= 6)
							td.id_name_at = conn.PGgetvalue(res, l, 5);

						mt.push_back(td);
					}
				}
			}
			PQclear(res);
		}

		void SaveBodyCsv(std::fstream& s1, T_IdSheet& ids, std::string Error)
		{

			std::stringstream ssd;

			ssd << ids.id << ";";

			ssd << " " << GetStringData(ids.Start1) << ";";
			ssd << " " << GetStringData(ids.Start2) << ";";
			ssd << " " << GetStringData(ids.DataTime_End) << ";";
			//ssd << " " << GetStringData(ids.Start4) << ";";

			ssd << std::fixed << std::setprecision(1) << ids.DataTime_All << ";";
			ssd << std::fixed << std::setprecision(1) << ids.TimeForPlateHeat << ";";
			ssd << " " << GetStringData(ids.DataTime) << ";";

			ssd << ids.Alloy << ";";
			ssd << ids.Thikness << ";";

			ssd << ids.Melt << ";";
			ssd << ids.PartNo << ";";
			ssd << ids.Pack << ";";
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
		}

		std::fstream SaveHeadCsv(std::string name)
		{
			std::fstream ss1(name, std::fstream::binary | std::fstream::out);

			//Дата, время загрузки листа в закалочную печь
			//Марка стали
			//Толщина листа, мм
			//Плавка
			//партия
			//пачка
			//номер листа
			//Заданная температура
			//скорость выдачи, мм/с
			
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
				//<< "В охлаждение;"
				<< "В кантовку;"
				<< "DataTime_All;"
				<< "TimeForPlateHeat;"
				<< "TimeStart;"

				<< "Марка;"
				<< "Толщина;"
				<< "Плавка;"
				<< "Партия;"
				<< "Пачка;"
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
				<< "Ошибка;"
				<< "TimeTest;"

				<< std::endl;
			return ss1;
		}

		bool isSheet(T_IdSheet& t)
		{
			return t.Melt && t.Pack && t.PartNo && t.Sheet;
		}
		T_IdSheet GetIdSheet(PGConnection& conn, std::string Start, size_t count, size_t i)
		{
			T_IdSheet ids;
			{
				std::stringstream ssd;
				ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content ";
				ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
				ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";

				ssd << "FROM todos WHERE";
				ssd << " (id_name = " << PlateData[0].Melt->ID;
				ssd << " OR id_name = " << PlateData[0].Pack->ID;
				ssd << " OR id_name = " << PlateData[0].PartNo->ID;
				ssd << " OR id_name = " << PlateData[0].Sheet->ID;

				ssd << " OR id_name = " << PlateData[0].AlloyCodeText->ID;
				ssd << " OR id_name = " << PlateData[0].ThiknessText->ID;

				//ssd << " OR id_name = " << PlateData[0].SubSheet->ID;
				ssd << ") AND ";
				ssd << " create_at <= '" << Start << "'";
				ssd << " AND content <> '0'";
				ssd << "ORDER BY todos.id_name, todos.id DESC;";

				std::string comand1 = ssd.str();

				MapSheetTodos idSheet1;
				GetTodosSQL(conn, idSheet1, comand1);
				for(auto t : idSheet1)
				{
					if(t.id_name == PlateData[0].AlloyCodeText->ID) ids.Alloy = t.value;
					if(t.id_name == PlateData[0].ThiknessText->ID) ids.Thikness = t.value;

					if(t.id_name == PlateData[0].Melt->ID)
					{
						//ids.sMelt = t.value;
						ids.Melt = t.content.As<int32_t>();
					}
					if(t.id_name == PlateData[0].Pack->ID)
					{
						//ids.sPack = t.value;
						ids.Pack = t.content.As<int32_t>();
					}
					if(t.id_name == PlateData[0].PartNo->ID)
					{
						//ids.sPartNo = t.value;
						ids.PartNo = t.content.As<int32_t>();
					}
					if(t.id_name == PlateData[0].Sheet->ID)
					{
						//ids.sSheet = t.value;
						ids.Sheet = t.content.As<int32_t>();
					}
				}
			}
			{
				std::stringstream ssd;
				ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content ";
				ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
				ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";

				ssd << "FROM todos WHERE";
				ssd << " id_name = " << PlateData[0].SubSheet->ID;
				ssd << " AND ";
				ssd << " create_at <= '" << Start << "'";
				//ssd << " AND content <> '0'";
				ssd << "ORDER BY todos.id_name, todos.id DESC;";

				std::string comand = ssd.str();

				MapSheetTodos idSheet;
				GetTodosSQL(conn, idSheet, comand);
				for(auto t : idSheet)
				{
					if(t.id_name == PlateData[0].SubSheet->ID)
					{
						//ids.sSubSheet = t.value;
						ids.SubSheet = t.content.As<int32_t>();
					}
				}
			}

			{
				//std::stringstream ssq;
				//ssq << count << ": " << i << " (";
				//ssq << GetStringData(Start) << ") ";
				//ssq << std::setw(6) << std::setfill('0') << ids.Melt << "-";
				//ssq << std::setw(3) << std::setfill('0') << ids.PartNo << "-";
				//ssq << std::setw(3) << std::setfill('0') << ids.Pack << "-";
				//ssq << std::setw(3) << std::setfill('0') << ids.Sheet << "/";
				//ssq << std::setw(1) << std::setfill('0') << ids.SubSheet;
				////ssq << ids.DataTime_All;
				//SetWindowText(hWndDebug, ssq.str().c_str());
			}
			return ids;
		}

		float GetTime(std::string Start, std::vector<T_Todos>& allTime)
		{
			for(auto& a : allTime)
			{
				if(a.create_at <= Start)
				{
					return a.content.As<float>();
				}
			}
			return 0.0f;
		}

		void GetAllTime(PGConnection& conn, std::vector<T_Todos>& allTime)
		{
			std::stringstream ssd;
			ssd << "SELECT create_at, id, id_name, content,";
			ssd << " (SELECT type FROM tag WHERE tag.id = todos.id_name),";
			ssd << " (SELECT comment FROM tag WHERE tag.id = todos.id_name) FROM todos WHERE";
			ssd << " id_name = " << Par_Gen.TimeForPlateHeat->ID;
			ssd << " AND CAST(content AS DOUBLE PRECISION) <> 0.0";
			ssd << " ORDER BY id DESC;";
			std::string comand = ssd.str();

			if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
			PGresult* res = conn.PGexec(comand);
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

		void GetMask(uint16_t v, std::string& MaskV)
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
		}

		//Фиксируем на начало входа в печь State_1 = 3;
		void GetDataSheet1(PGConnection& conn, T_IdSheet& ids)
		{
			MapSheetTodos DataSheetTodos;

			std::stringstream ssd;
			ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE create_at < '" << ids.Start1 << "' AND (";
			ssd << "id_name = " << GenSeqFromHmi.TempSet1->ID << " OR ";
			ssd << "id_name = " << Par_Gen.UnloadSpeed->ID << " OR ";
			ssd << "id_name = " << Par_Gen.PresToStartComp->ID << " OR ";
			ssd << "id_name = " << HMISheetData.SpeedSection.Top->ID << " OR ";
			ssd << "id_name = " << HMISheetData.SpeedSection.Bot->ID << " OR ";
			ssd << "id_name = " << HMISheetData.LaminarSection1.Top->ID << " OR ";
			ssd << "id_name = " << HMISheetData.LaminarSection1.Bot->ID << " OR ";
			ssd << "id_name = " << HMISheetData.LaminarSection2.Top->ID << " OR ";
			ssd << "id_name = " << HMISheetData.LaminarSection2.Bot->ID << " OR ";
			ssd << "id_name = " << HMISheetData.Valve_1x->ID << " OR ";
			ssd << "id_name = " << HMISheetData.Valve_2x->ID << ") ";
			ssd << "ORDER BY id_name, id DESC;";

			std::string comand = ssd.str();
			GetTodosSQL(conn, DataSheetTodos, comand);

			for(auto a : DataSheetTodos)
			{
				if(a.id_name == GenSeqFromHmi.TempSet1->ID) ids.Temper = a.content.As<float>();
				if(a.id_name == Par_Gen.UnloadSpeed->ID) ids.Speed = a.content.As<float>();
				if(a.id_name == Par_Gen.PresToStartComp->ID) ids.PresToStartComp = a.content.As<float>();
				if(a.id_name == HMISheetData.SpeedSection.Top->ID) ids.SpeedTopSet = a.content.As<float>();
				if(a.id_name == HMISheetData.SpeedSection.Bot->ID) ids.SpeedBotSet = a.content.As<float>();
				if(a.id_name == HMISheetData.LaminarSection1.Top->ID) ids.Lam1PosClapanTop = a.content.As<float>();
				if(a.id_name == HMISheetData.LaminarSection1.Bot->ID) ids.Lam1PosClapanBot = a.content.As<float>();
				if(a.id_name == HMISheetData.LaminarSection2.Top->ID) ids.Lam2PosClapanTop = a.content.As<float>();
				if(a.id_name == HMISheetData.LaminarSection2.Bot->ID) ids.Lam2PosClapanBot = a.content.As<float>();
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
		}

		//Фиксируем на начало входа в печь State_2 = 5;
		void GetDataSheet2(PGConnection& conn, T_IdSheet& ids)
		{
			MapSheetTodos DataSheetTodos;

			std::stringstream ssd;
			ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE create_at < '" << ids.DataTime_End << "' AND (";
			ssd << "id_name = " << AI_Hmi_210.Za_TE3->ID << " OR ";
			ssd << "id_name = " << AI_Hmi_210.Za_PT3->ID << " OR ";
			ssd << "id_name = " << AI_Hmi_210.LAM_TE1->ID;
			ssd << ") ORDER BY id_name, id DESC;";

			std::string comand = ssd.str();
			GetTodosSQL(conn, DataSheetTodos, comand);

			for(auto a : DataSheetTodos)
			{
				if(a.id_name == AI_Hmi_210.Za_TE3->ID) ids.Za_TE3 = a.content.As<float>();
				if(a.id_name == AI_Hmi_210.Za_PT3->ID) ids.Za_PT3 = a.content.As<float>();
				if(a.id_name == AI_Hmi_210.LAM_TE1->ID) ids.LAM_TE1 = a.content.As<float>();
			}
		}

		//Фиксируем на начало входа в печь State_2 = 5 плюс 5 секунд;
		void GetDataSheet3(PGConnection& conn, T_IdSheet& ids)
		{
			MapSheetTodos DataSheetTodos;
			std::tm tmp;
			time_t t1 = DataTimeOfString(ids.DataTime_End, FORMATTIME, tmp) + 5;
			std::string Start = GetDataTimeString(&t1);

			std::stringstream ssd;
			ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE create_at < '" << Start << "' AND (";
			ssd << "id_name = " << AI_Hmi_210.LaminPressTop->ID << " OR ";
			ssd << "id_name = " << AI_Hmi_210.LaminPressBot->ID;
			ssd << ") ORDER BY id_name, id DESC;";

			std::string comand = ssd.str();
			GetTodosSQL(conn, DataSheetTodos, comand);

			for(auto a : DataSheetTodos)
			{
				if(a.id_name == AI_Hmi_210.LaminPressTop->ID) ids.LaminPressTop = a.content.As<float>();
				if(a.id_name == AI_Hmi_210.LaminPressBot->ID) ids.LaminPressBot = a.content.As<float>();
			}
		}

		void GetSeq_1(PGConnection& conn, MapSheetTodos& allSheetTodos)
		{
#pragma region Подготовка запроса в базу
			std::stringstream ssd;

			ssd << "SELECT create_at, id, id_name, content ";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE";
			ssd << " id_name = " << GenSeqToHmi.Seq_1_StateNo->ID;
			ssd << " AND CAST(content AS INTEGER) >= " << st1_3;
			//ssd << " AND CAST(content AS INTEGER) <> " << st1_6;
			ssd << " AND create_at >= '" << StartSheet << "'";
			if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
			ssd << " ORDER BY id;";

#pragma endregion

			std::string comand = ssd.str();
			GetTodosSQL(conn, allSheetTodos, comand);

		}
		void GetSeq_2(PGConnection& conn, MapSheetTodos& allSheetTodos)
		{
#pragma region Подготовка запроса в базу
			std::stringstream ssd;

			ssd << "SELECT create_at, id, id_name, content ";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE";
			ssd << " Id_name = " << GenSeqToHmi.Seq_2_StateNo->ID;
			ssd << " AND CAST(content AS integer) >= " << st2_3;
			ssd << " AND CAST(content AS integer) <> " << st2_4;
			ssd << " AND create_at >= '" << StartSheet << "'";
			if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
			ssd << " ORDER BY id;";

#pragma endregion

			std::string comand = ssd.str();
			GetTodosSQL(conn, allSheetTodos, comand);
		}

		float GetDataTime_All(std::string TimeStart, std::string Start3)
		{
			//std::tm tmp;
			//time_t t1 = DataTimeOfString(TimeStart, FORMATTIME, tmp);
			//time_t t2 = DataTimeOfString(Start3, FORMATTIME, tmp);
			float t1 = float(DataTimeDiff(Start3, TimeStart)) / 60.0f;
			return t1;
		}

		void Get_ID_S(PGConnection& conn, T_IdSheet& td)
		{
			{
				std::stringstream ssd;
				ssd << "SELECT id, day, month, year, cassetteno FROM sheet ";
				ssd << "WHERE melt = " << td.Melt;
				ssd << " AND partno = " << td.PartNo;
				ssd << " AND pack = " << td.Pack;
				ssd << " AND sheet = " << td.Sheet;
				ssd << " AND subsheet = " << td.SubSheet;
				ssd << " ORDER BY id LIMIT 1";
				std::string comand = ssd.str();
				if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
				PGresult* res = conn.PGexec(comand);
				if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
				{
					td.id = Stoi(conn.PGgetvalue(res, 0, 0));
					td.day = Stoi(conn.PGgetvalue(res, 0, 1));
					td.month = Stoi(conn.PGgetvalue(res, 0, 2));
					td.year = Stoi(conn.PGgetvalue(res, 0, 3));
					td.cassetteno = Stoi(conn.PGgetvalue(res, 0, 4));
				}
				PQclear(res);
			}
		}

		int Get_ID_C(PGConnection& conn, T_IdSheet& td)
		{
			int id = 0;
			std::stringstream ssd;
			ssd << "SELECT id FROM cassette ";
			ssd << "WHERE day = " << td.day;
			ssd << " AND month = " << td.month;
			ssd << " AND year = " << td.year;
			ssd << " AND cassetteno = " << td.cassetteno;
			ssd << " ORDER BY id LIMIT 1";
			std::string comand = ssd.str();

			if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
			PGresult* res = conn.PGexec(comand);
			if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
				id = Stoi(conn.PGgetvalue(res, 0, 0));
			PQclear(res);
			return id;
		}

		std::fstream fUpdateSheet;
		void UpdateSheet(PGConnection& conn, T_IdSheet& td)
		{
			if(td.id)
			{
				std::stringstream ssd;
				ssd << "UPDATE sheet SET";
				ssd << " start_at = '" << td.Start1 << "'";				//Дата, время загрузки листа в закалочную печь
				ssd << ", datatime_end = '" << td.DataTime_End << "'";		//Дата, время выдачи листа из закалочной печи
				if(td.Start2.length()) ssd << ", secondpos_at = '" << td.Start2 << "'";		//Преход во вторую зону
				ssd << ", alloy = '" << td.Alloy << "'";				//Марка стали
				ssd << ", thikness = '" << td.Thikness << "'";			//Толщина листа, мм

				ssd << ", timeforplateheat = " << td.TimeForPlateHeat;	//Задание Время нахождения листа в закалочной печи. мин
				ssd << ", datatime_all = " << td.DataTime_All;			//Факт Время нахождения листа в закалочной печи. мин

				ssd << ", speed = " << td.Speed;					//Скорость выгрузки
				ssd << ", temper = " << td.Temper;					//Уставка температуры
				ssd << ", prestostartcomp = " << td.PresToStartComp;					//Скорость выгрузки
				ssd << ", posclapantop = " << td.SpeedTopSet;			//Клапан. Скоростная секция. Верх
				ssd << ", posclapanbot = " << td.SpeedBotSet;			//Клапан. Скоростная секция. Низ
				ssd << ", lam1posclapantop = " << td.Lam1PosClapanTop;		//Клапан. Ламинарная секция 1. Верх
				ssd << ", lam1posclapanbot = " << td.Lam1PosClapanBot;		//Клапан. Ламинарная секция 1. Низ
				ssd << ", lam2posclapantop = " << td.Lam2PosClapanTop;		//Клапан. Ламинарная секция 2. Верх
				ssd << ", lam2posclapanbot = " << td.Lam2PosClapanBot;		//Клапан. Ламинарная секция 2. Низ
				ssd << ", mask = '" << td.Mask << "'";					//Режим работы клапана
				//Фиксируем на выходе из печи State_2 = 5;
				ssd << ", lam_te1 = " << td.LAM_TE1;					//Температура воды в поддоне
				ssd << ", za_te3 = " << td.Za_TE3;						//Температура воды в баке
				ssd << ", za_pt3 = " << td.Za_PT3;						//Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)

				//Фиксируем на выходе из печи State_2 = 5 плюс 5 секунд;
				ssd << ", lampresstop = " << td.LaminPressTop;			//Давление в верхнем коллекторе
				ssd << ", lampressbot = " << td.LaminPressBot;			//Давление в нижнем коллекторе

				ssd << ", correct = now(), pdf = '' WHERE id = " << td.id;
				SetWindowText(hWndDebug, ssd.str().c_str());


				fUpdateSheet << ssd.str() << std::endl;
				fUpdateSheet.flush();
				//LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, ssd.str());
				SETUPDATESQL(PdfLogger, conn, ssd);

				if(td.day && td.month && td.year && td.cassetteno)
				{
					int ID_C = Get_ID_C(conn, td);
					if(ID_C)
					{
						std::stringstream ssd;
						ssd << "UPDATE cassette SET correct = DEFAULT, pdf = DEFAULT WHERE id = " << ID_C;
						SETUPDATESQL(PdfLogger, conn, ssd);
					}
				}

				//TSheet Sheet;
				//SetSheet(td, Sheet);
				//PDF::Cassette::CassettePdfClass pdf(Sheet);
			}
			else
			{
				//Свободные id
				//3394
				//3428
				//3669
				//3928
				//3952
				//3954
				//3955
				//3973

				if(iAllId) td.id = iAllId++;
				std::stringstream ssd;
				ssd << "INSERT INTO sheet (";
				//if(td.id)
				//	ssd << "id, ";
				ssd << "create_at, ";				//Дата, время загрузки листа в закалочную печь
				ssd << "start_at, ";				//Дата, время загрузки листа в закалочную печь
				ssd << "datatime_end, ";			//Дата, время выдачи листа из закалочной печи
				ssd << "datatime_all,";			//Факт Время нахождения листа в закалочной печи. мин
				if(td.Start2.length()) ssd << "secondpos_at, ";		//Преход во вторую зону
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

				ssd << "lampresstop, ";									//Давление в верхнем коллекторе
				ssd << "lampressbot, ";									//Давление в нижнем коллекторе
				ssd << "lam_te1, ";					//Температура воды в поддоне
				ssd << "za_te3, ";						//Температура воды в баке
				ssd << "za_pt3, ";						//Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)
				ssd << "correct, ";
				ssd << "pdf, ";
				ssd << " pos";

				ssd << ") VALUES (";
				//if(td.id)
				//	ssd << td.id << ", ";
				ssd << "'" << td.DataTime << "', ";
				ssd << "'" << td.DataTime << "', ";
				ssd << "'" << td.DataTime_End << "', ";
				ssd << td.DataTime_All << ", ";
				if(td.Start2.length()) ssd << "'" << td.Start2 << "', ";		//Преход во вторую зону
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

				ssd << "now(), '', 7);";

				SetWindowText(hWndDebug, ssd.str().c_str());

				fUpdateSheet << ssd.str() << std::endl;
				fUpdateSheet.flush();


				SETUPDATESQL(SQLLogger, conn, ssd);

				if(td.day && td.month && td.year && td.cassetteno)
				{
					int ID_C = Get_ID_C(conn, td);
					if(ID_C)
					{
						std::stringstream ssd;
						ssd << "UPDATE cassette SET correct = DEFAULT, pdf = DEFAULT WHERE id = " << ID_C;
						SETUPDATESQL(PdfLogger, conn, ssd);
					}
				}

				//TSheet Sheet;
				//SetSheet(td, Sheet);
				//PDF::Cassette::CassettePdfClass pdf(Sheet);

				//if(iAllId == AllId)
				//{
				//	iAllId = 0;
				//	MessageBox(Global0, "Достигнуд придел ID", "Внимание", 0);
				//	int tt = 0;
				//}

			}

		}


		bool InZone1(PGConnection& conn, T_IdSheet& ids1, std::fstream& s1, std::string create_at, size_t count, size_t i)
		{
			T_IdSheet ids = GetIdSheet(conn, create_at, count, i);
			if(isSheet(ids))
			{
				if(isSheet(ids1))
				{
					//SaveBodyCsv(s1, ids1, "Потерян в 1-й зоне");
					return false;
				}

				ids1 = ids;
				ids1.Start1 = create_at;
				ids1.DataTime = create_at;
			}
			return true;
		}

		bool InZone2(PGConnection& conn, T_IdSheet& ids1, T_IdSheet& ids2, std::fstream& s1, std::string create_at)
		{
			if(isSheet(ids1))
			{
				if(isSheet(ids2))
				{
					//SaveBodyCsv(s1, ids2, "Потерян в 2-й зоне 0");
					return false;
				}

				ids2 = ids1;
				ids2.Start2 = create_at;
				ids1 = T_IdSheet();
			}
			return true;
		}

		void InZone3(PGConnection& conn, T_IdSheet& ids1, V_IdSheet& vids, V_Todos& allTime, std::fstream& s1, std::string create_at)
		{
			if(isSheet(ids1))
			{
				T_IdSheet ids = ids1;
				ids.DataTime_End = create_at;
				ids1 = T_IdSheet();

				GetDataSheet1(conn, ids);
				GetDataSheet2(conn, ids);
				GetDataSheet3(conn, ids);

				ids.TimeForPlateHeat = GetTime(ids.Start2, allTime);

				ids.DataTime_All = GetDataTime_All(ids.DataTime, ids.DataTime_End);
				float f = fabsf(ids.DataTime_All - ids.TimeForPlateHeat);
				if(f > 2.0f)
					ids.DataTime_All = GetDataTime_All(ids.Start1, ids.DataTime_End);
				Get_ID_S(conn, ids);

				SaveBodyCsv(s1, ids, "");

				UpdateSheet(conn, ids);
				vids.push_back(ids);
			}
		}

		bool cmpData(T_Todos& first, T_Todos& second)
		{
			return first.id < second.id;
		}

#pragma region SaveStep
		std::fstream SaveStepHeadCsv(std::string name)
		{
			std::fstream ss1(name, std::fstream::binary | std::fstream::out);
			ss1 << "create_at;";
			ss1 << "id;";
			ss1 << "value;";
			ss1 << "id_name;";
			ss1 << "comment;";
			ss1 << std::endl;
			return ss1;
		}
		void SaveStepBodyCsv(std::fstream& ss1, T_Todos& td)
		{
			ss1 << " " << td.create_at << ";";
			ss1 << td.id << ";";
			ss1 << td.value << ";";
			ss1 << td.id_name << ";";
			ss1 << td.id_name_at << ";";
			ss1 << std::endl;
		}
#pragma endregion

#pragma region SaveT_IdSheet
		std::fstream SaveT_IdSheetHeadCsv(std::string name)
		{
			std::fstream ss1(name, std::fstream::binary | std::fstream::out);
			ss1 << " " << "Start1" << ";";
			ss1 << " " << "TimeStart" << ";";
			ss1 << "Melt" << ";";
			ss1 << "PartNo" << ";";
			ss1 << "Pack" << ";";
			ss1 << "Sheet" << ";";
			ss1 << "SubSheet" << ";";
			ss1 << std::endl;
			return ss1;
		}
		void SaveT_IdSheetBodyCsv(std::fstream& ss1, T_IdSheet& ids)
		{
			ss1 << " " << ids.Start1 << ";";
			ss1 << " " << ids.DataTime << ";";
			ss1 << ids.Melt << ";";
			ss1 << ids.PartNo << ";";
			ss1 << ids.Pack << ";";
			ss1 << ids.Sheet<< ";";
			ss1 << ids.SubSheet << ";";
			ss1 << std::endl;
		}
#pragma endregion

		/*
		void SetSheet(T_IdSheet& td, TSheet& Sheet)
		{
			Sheet.Alloy = td.Alloy;
			Sheet.DataTime = td.DataTime;

			Sheet.DataTime_All = (boost::format("%.1f") % td.DataTime_All).str();
			Sheet.DataTime_End = td.DataTime_End;

			Sheet.id = (boost::format("%u") % td.id).str();
			Sheet.Melt = (boost::format("%u") % td.Melt).str();
			Sheet.Pack = (boost::format("%u") % td.Pack).str();
			Sheet.PartNo = (boost::format("%u") % td.PartNo).str();
			Sheet.Sheet = (boost::format("%u") % td.Sheet).str();
			Sheet.SubSheet = (boost::format("%u") % td.SubSheet).str();
			Sheet.Slab = (boost::format("%u") % td.Slab).str();

			Sheet.Lam1PosClapanTop = (boost::format("%.1f") % td.Lam1PosClapanTop).str();
			Sheet.Lam1PosClapanBot = (boost::format("%.1f") % td.Lam1PosClapanBot).str();
			Sheet.Lam2PosClapanTop = (boost::format("%.1f") % td.Lam2PosClapanTop).str();
			Sheet.Lam2PosClapanBot = (boost::format("%.1f") % td.Lam2PosClapanBot).str();
			Sheet.LaminPressTop = (boost::format("%.1f") % td.LaminPressTop).str();
			Sheet.LaminPressBot = (boost::format("%.1f") % td.LaminPressBot).str();

			Sheet.LAM_TE1 = (boost::format("%.1f") % td.LAM_TE1).str();
			Sheet.Mask = td.Mask;
			Sheet.Pos = (boost::format("%u") % td.Pos).str();
			Sheet.Speed = (boost::format("%.0f") % td.Speed).str();
			Sheet.Start_at = td.DataTime;
			Sheet.Temper = (boost::format("%.0f") % td.Temper).str();

			Sheet.TimeForPlateHeat = (boost::format("%.1f") % td.TimeForPlateHeat).str();
			Sheet.Za_PT3 = (boost::format("%.1f") % td.Za_PT3).str();
			Sheet.Za_TE3 = (boost::format("%.1f") % td.Za_TE3).str();
		}
		*/
		void GetRawSheet(PGConnection& conn)
		{

			//std::stringstream ssd;
			//ssd << "SELECT datatime_end, id FROM sheet WHERE pdf <> '' ORDER BY id DESC LIMIT 1";
			//PGresult* res = conn.PGexec(ssd.str());
			//if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
			//		StartSheet = conn.PGgetvalue(res, 0, 0);
			//PQclear(res);

			MapSheetTodos allSheetTodos;

			V_Todos allTime;

			T_IdSheet ids1;
			T_IdSheet ids2;

			V_IdSheet ids4;

			remove("UpdateSheet.txt");
			fUpdateSheet = std::fstream("UpdateSheet.txt", std::fstream::binary | std::fstream::out | std::ios::app);


			std::fstream ss1 = SaveHeadCsv("Sheet.csv");


			//StartSheet = "2024-05-13 18:40:00.0";

			GetAllTime(conn, allTime);
			//GenSeqToHmi.Seq_1_StateNo
			GetSeq_1(conn, allSheetTodos);
			//GenSeqToHmi.Seq_2_StateNo
			GetSeq_2(conn, allSheetTodos);

			std::sort(allSheetTodos.begin(), allSheetTodos.end(), cmpData);
			////GenSeqToHmi.Seq_3_StateNo
			//GetSeq_3(conn);


			size_t count = allSheetTodos.size();
			size_t i = count;
			std::fstream ff1 = SaveStepHeadCsv("Step.csv");
			std::fstream ff2 = SaveT_IdSheetHeadCsv("T_IdSheet.csv");


			for(MapSheetTodos::iterator it = allSheetTodos.begin(); isRun && it != allSheetTodos.end(); it++)
			{
				T_Todos& td = *it;

				SaveStepBodyCsv(ff1, td);

				if(td.id_name == GenSeqToHmi.Seq_1_StateNo->ID) //Загрузка в печь
				{
					int16_t st = td.content.As<int16_t>();
					if(st == st1_3) //Открыть входную дверь
					{
						if(!InZone1(conn, ids1, ss1, td.create_at, count, i))
							if(!InZone2(conn, ids1, ids2, ss1, td.create_at))
								InZone3(conn, ids1, ids4, allTime, ss1, td.create_at);

						InZone1(conn, ids1, ss1, td.create_at, count, i);
						SaveT_IdSheetBodyCsv(ff2, ids1);
					}
					else if(st == st1_4 || st == st1_5 || st == st1_6) //"Загрузка в печь" || "Закрыть входную дверь" || "Нагрев листа"
					{
						ids1.DataTime = td.create_at;
					}
					else if(st == st1_7 || st == st1_8) 	//"Передача на 2 рольганг" || "Передача на 2-й рольганг печи"
					{
						if(!InZone2(conn, ids1, ids2, ss1, td.create_at))
							InZone3(conn, ids1, ids4, allTime, ss1, td.create_at);
					}
				}

				else if(td.id_name == GenSeqToHmi.Seq_2_StateNo->ID) //Выгрузка из печи
				{
					int16_t st = td.content.As<int16_t>();
					if(st == st2_3)	//3 = Прием заготовки с 1-го рольганга печи
					{
						if(!InZone2(conn, ids1, ids2, ss1, td.create_at))
							InZone3(conn, ids1, ids4, allTime, ss1, td.create_at);
					}
					else if(st == st2_5 || st == st2_6 || st == st2_7) //"Открыть выходную дверь" || Выдача в линию закалки" || "Закрыть выходную дверь"
					{
						InZone3(conn, ids2, ids4, allTime, ss1, td.create_at);
					}
				}
				i--;
			}
			ff2.close();
			ff1.close();
			ss1.close();
			fUpdateSheet.close();
			//for(V_IdSheet::iterator it = ids4.begin(); isRun && it != ids4.end(); it++)
			//{
			//	T_IdSheet td = *it;
			//	UpdateSheet(conn, td);
			//}
			INT II = 0;
		}

	};


	//Для ручного добавления листа
	void HendInsetr(PGConnection& conn)
	{
		std::string comand = "";

		std::vector<int> csId;

		comand = "SELECT id FROM cassette ORDER BY create_at DESC;"; //Все кассеты
		PGresult* res = conn.PGexec(comand);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			for(int l = 0; l < line; l++)
			{
				int id = Stoi(conn.PGgetvalue(res, l, 0));
				csId.push_back(id);
			}
		}
		PQclear(res);

		comand = "SELECT * FROM cassette WHERE id = 371"; //2024-05-25-06
		res = conn.PGexec(comand);
		TCassette Cassette;
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			S107::GetColl(res);
			if(conn.PQntuples(res))
				S107::GetCassette(res, Cassette, 0);
		}
		else
			LOG_ERR_SQL(PdfLogger, res, comand);
		PQclear(res);
		
		try
		{
			 //Удаление всех файлов в каталоге кассеты
			std::stringstream temp;
			temp << lpLogPdf;
			temp << "/" << Stoi(Cassette.Year);
			temp << "/" << Cassette::MonthName[Stoi(Cassette.Month)];
			temp << "/" << std::setw(2) << std::setfill('0') << std::right << Stoi(Cassette.Day);
			std::Pathc patch = getDirContents (temp.str());
			for(auto p : patch)
			{
				if(p.string().length())
					remove(p.string().c_str());
			}
		}
		CATCH(PdfLogger, FUNCTION_LINE_NAME);
		//Cassette.Peth = "1";
		Cassette::CassettePdfClass cc = Cassette::CassettePdfClass(Cassette);

		//std::stringstream ssa;
		//ssa << "SELECT * FROM cassette WHERE ";
		//ssa << "correct IS NULL AND ";
		//ssa << "finish_at IS NOT NULL AND ";
		//ssa << " delete_at IS NULL";
		//ssa << " ORDER BY id ASC LIMIT 1";
		//
		//T_IdSheet td;
		//td.id = 2928;
		//td.DataTime = "2024-05-14 11:26:45";
		//td.DataTime_End = "2024-05-14 11:45:04";
		//td.Alloy = "А3";
		//td.Thikness = "4,2";
		//td.DataTime_All = 18.3;
		//td.TimeForPlateHeat = 18;
		//td.Melt = 107235;
		//td.Slab = 0;
		//td.PartNo = 402;
		//td.Pack = 3;
		//td.Sheet = 65;
		//td.SubSheet = 0;
		//td.Temper = 900;
		//td.Speed = 700;
		//td.Za_PT3 = 4.1;
		//td.Za_TE3 = 34.4;
		//td.LaminPressTop = 3.7;
		//td.LaminPressBot = 1.8;
		//td.SpeedTopSet = 2;
		//td.SpeedBotSet = 10;
		//td.Mask = "001111111 001111111";
		//td.Lam1PosClapanTop = 2.5;
		//td.Lam1PosClapanBot = 10;
		//td.Lam2PosClapanTop = 2.5;
		//td.Lam2PosClapanBot = 10;
		//td.LAM_TE1 = 34.3;
		//std::stringstream ssd;
		//ssd << "INSERT INTO sheet (";
		//ssd << "id, ";
		//ssd << "create_at, ";				//Дата, время загрузки листа в закалочную печь
		//ssd << "start_at, ";				//Дата, время загрузки листа в закалочную печь
		//ssd << "datatime_end, ";			//Дата, время выдачи листа из закалочной печи
		//ssd << "datatime_all,";			//Факт Время нахождения листа в закалочной печи. мин
		//ssd << "alloy, ";
		//ssd << "thikness, ";
		//ssd << "melt, ";
		//ssd << "slab, ";
		//ssd << "partno, ";
		//ssd << "pack, ";
		//ssd << "sheet, ";
		//ssd << "subsheet, ";
		//ssd << "temper, ";
		//ssd << "speed, ";
		//ssd << "timeforplateheat, ";
		//ssd << "prestostartcomp, ";
		//ssd << "posclapantop, ";
		//ssd << "posclapanbot, ";
		//ssd << "lam1posclapantop, ";
		//ssd << "lam1posclapanbot, ";
		//ssd << "lam2posclapantop, ";
		//ssd << "lam2posclapanbot, ";
		//ssd << "mask, ";
		//ssd << "lampresstop, ";									//Давление в верхнем коллекторе
		//ssd << "lampressbot, ";									//Давление в нижнем коллекторе
		//ssd << "lam_te1, ";					//Температура воды в поддоне
		//ssd << "za_te3, ";						//Температура воды в баке
		//ssd << "za_pt3, ";						//Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)
		//ssd << "correct, ";
		//ssd << "pdf, ";
		//ssd << " pos";
		//
		//ssd << ") VALUES (";
		//ssd << td.id << ", ";
		//ssd << "'" << td.DataTime << "', ";
		//ssd << "'" << td.DataTime << "', ";
		//ssd << "'" << td.DataTime_End << "', ";
		//ssd << td.DataTime_All << ", ";
		//ssd << "'" << td.Alloy << "', ";
		//ssd << "'" << td.Thikness << "', ";
		//ssd << td.Melt << ", ";
		//ssd << td.Slab << ", "; //Slab
		//ssd << td.PartNo << ", ";
		//ssd << td.Pack << ", ";
		//ssd << td.Sheet << ", ";
		//ssd << td.SubSheet << ", ";
		//ssd << td.Temper << ", ";
		//ssd << td.Speed << ", ";
		//ssd << td.TimeForPlateHeat << ", ";
		//ssd << td.PresToStartComp << ", ";
		//ssd << td.SpeedTopSet << ", ";
		//ssd << td.SpeedBotSet << ", ";
		//ssd << td.Lam1PosClapanTop << ", ";
		//ssd << td.Lam1PosClapanBot << ", ";
		//ssd << td.Lam2PosClapanTop << ", ";
		//ssd << td.Lam2PosClapanBot << ", ";
		//ssd << "'" + td.Mask + "', ";
		//ssd << td.LaminPressTop << ", ";
		//ssd << td.LaminPressBot << ", ";
		//ssd << td.LAM_TE1 << ", ";
		//ssd << td.Za_TE3 << ", ";
		//ssd << td.Za_PT3 << ", ";
		//ssd << "now(), '', 7);";
		//
		//SETUPDATESQL(SQLLogger, conn, ssd);
		//PDF::Cassette::CassettePdfClass pdf(td);
	}


	//Поток автоматической корректировки
	DWORD WINAPI RunCassettelPdf(LPVOID)
	{
#ifndef _DEBUG
		return 0;
#endif
#ifdef NOCASSETTE
		try
		{
			//return 0;
			PdfLogger = InitLogger("Pdf Debug");
			PGConnection conn_pdf;
			conn_pdf.connection();

			////Для ручного тестирования
			//HendInsetr(conn_pdf);
			//isRun = false;
			//return 0;

			while(isRun)
			{
				PDF::Correct = true;
				//PDF::Correct = false;
				if(PDF::Correct)
				{
					////Проверяем и востанавливаем все листы
					SHEET::StartSheet = "2024-01-01 00:00:00";
					//SHEET::AllId = 1356;
					//SHEET::iAllId = 1;
					
					//Найти Потерянные листы:
					//SELECT * FROM sheet WHERE create_at > '2024-05-15 17:25:13.433' AND news <> 1 ORDER BY id DESC
					//PDF::SHEET::GetRawSheet(conn_pdf);
					

					//SHEET::StartSheet = "2024-05-25 04:30:00";

					////Для автоматической коррекции
					int id = 0;
					//std::string comand = "SELECT create_at, id FROM sheet WHERE correct IS NOT NULL ORDER BY id DESC LIMIT 1";
					//std::string comand = "SELECT create_at, id FROM sheet WHERE correct IS NOT NULL ORDER BY id DESC LIMIT 1";
					//PGresult* res = conn_pdf.PGexec(comand);
					//if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
					//{
					//	SHEET::StartSheet = conn_pdf.PGgetvalue(res, 0, 0);
					//	id = Stoi(conn_pdf.PGgetvalue(res, 0, 1));
					//}
					//PQclear(res);

					//comand = "SELECT create_at, id FROM sheet WHERE id > " + std::to_string(id) + " ORDER BY id ASC LIMIT 1";
					//res = conn_pdf.PGexec(comand);
					//if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
					//	SHEET::StartSheet = conn_pdf.PGgetvalue(res, 0, 0);
					//PQclear(res);
					
					SHEET::StartSheet = "2024-05-31 00:00:00.00";
					//PDF::SHEET::GetRawSheet(conn_pdf);

					PDF::Cassette::GetPdf getpdf(conn_pdf, SHEET::StartSheet);

					PDF::Correct = false;
				}
#ifdef _DEBUG
				//В дебаге один проход и выход из программы
				isRun = false;
#else
				int TimeCount = 0;
				while(TimeCount++ < 60 && isRun)
					std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
				LOG_INFO(PdfLogger, "{:90}| Stop Pdf Debug", FUNCTION_LINE_NAME);
			}
		}
		CATCH(PdfLogger, "");;
#endif

		return 0;
	}

}



