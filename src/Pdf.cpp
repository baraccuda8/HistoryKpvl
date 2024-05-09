#include "pch.h"
#include <setjmp.h>

#include "win.h"
#include "main.h"
#include "file.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "term.h"
#include "hard.h"
#include "KPVL.h"
#include "Furn.h"
#include "Graff.h"


#include "hpdf.h"
#include "Pdf.h"

#include <filesystem>

std::shared_ptr<spdlog::logger> PdfLogger;



HANDLE hAllPlf = NULL;

typedef struct T_cass{
	int nom = 0;
	int id = 0;
	std::string Run_at = "";
	std::string End_at = "";
	std::string Err_at = "";
	std::string Fin_at = "";

	std::tm TM_Run = std::tm();
	std::tm TM_End = std::tm();
	std::tm TM_Fin = std::tm();
	std::tm TM_All = std::tm();
	time_t tm_Run = 0;
	time_t tm_End = 0;
	time_t tm_Fin = 0;
	time_t tm_All = 0;

	float f_temper = 0.0f;

	int Repeat_at = 0;

	int Day = 0;
	int Month = 0;
	int Year = 0;
	int CassetteNo = 0;
	int Petch = 0;

	float PointRef_1 = 0.0f;		//Уставка Tемпературы
	float TempAct = 0.0f;			//Фактическое значение температуры

	float PointTime_1 = 0.0f;		//Заданное нагрева
	float ActTimeHeatAcc = 0.0f;	//Факт время нагрева

	float PointDTime_2 = 0.0f;		//Заданное Выдержка
	float ActTimeHeatWait = 0.0f;	//Факт время выдержки

	float TimePocSet = 0.0f;		//Полное время процесса (уставка), мин
	float ActTimeTotal = 0.0f;		//Факт общее время
};

typedef std::map<int, T_cass> TCASS;

extern std::string lpLogPdf;
extern std::string  FORMATTIME;
extern Gdiplus::Font font1;
extern Gdiplus::Font font2;

extern GUID guidBmp;
extern GUID guidJpeg;
extern GUID guidGif;
extern GUID guidTiff;
extern GUID guidPng;

std::string DateStart = "";
typedef std::map <int, T_Todos> MapRunn;
TCASS P0;


namespace PDF
{
	const char* tempImage = "t_kpvl.jpg";
	const char* furnImage = "t_furn.jpg";

	std::map <std::string, std::string> MonthName{
		{"1", "January"},
		{"2", "February"},
		{"3", "March"},
		{"4", "April"},
		{"5", "May"},
		{"6", "June"},
		{"7", "July"},
		{"8", "August"},
		{"9", "September"},
		{"10", "October"},
		{"11", "November"},
		{"12", "December"},
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

		//HPDF_Page_Rectangle(page, x, y, 220, 18);
	}

	void draw_text_rect(HPDF_Page page, double x, double y, double w, double h, std::string label)
	{
		draw_text(page, x, y, label);
		HPDF_Page_Rectangle(page, HPDF_REAL(x), HPDF_REAL(y), HPDF_REAL(w), HPDF_REAL(h));
		HPDF_Page_Stroke (page);
	}

	class PdfClass{
	public:

		HPDF_Doc  pdf;
		HPDF_Font font;
		HPDF_Page page;
		std::string FileName;
		HPDF_REAL Height = 0;
		HPDF_REAL Width = 0;
		HPDF_REAL coeff = 0;
		float SrTemp = 0;
		std::string strSrTemp = "";


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

		PdfClass(TCassette& TC);
		PdfClass(TSheet& sheet, bool view = true);
		~PdfClass()
		{
			//conn.PGDisConnection();
		};

		void GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID);
		void SqlTempActKPVL(T_SqlTemp& tr);

		void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st, int64_t mind, int64_t maxd, double mint, double maxt);;
		void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);
		void DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);
		void PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage);
		void DrawHeder(HPDF_REAL left, HPDF_REAL top);
		void DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
		void DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);

		bool NewPdf();
		void SavePDF();
		void DrawKpvlPDF();
		void DrawFurnPDF();

		void GetSheet();
		void GetCassete(TCassette& cassette);
	};

	void PdfClass::GetCassete(TCassette& cassette)
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

	void PdfClass::GetSheet()
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

	void PdfClass::GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID)
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

	void PdfClass::SqlTempActKPVL(T_SqlTemp& tr)
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
					if(!std::stoi(Sheet.Temperature))
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
									if(!std::stoi(Sheet.Temperature))
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

			if(!std::stoi(Sheet.Temperature))
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

	void PdfClass::DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st, int64_t mind, int64_t maxd, double mint, double maxt)
	{
		try
		{
			Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 0.5); //Черный
			Gdiplus::Pen Gdi_L2(clor, 1);
			//auto b = st.begin();
			//auto e = st.end();
			//e--;

			double coeffW = (double)(Rect.Width) / double(maxd);
			double coeffH = (double)(Rect.Height - Rect.Y) / (double)(maxt - mint);


			Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));
			Gdiplus::StringFormat stringFormat;
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);


			float mY = Rect.Y + float((maxt - maxt) * coeffH);
			Gdiplus::PointF pt1 ={Rect.X - 5,				mY};
			Gdiplus::PointF pt2 ={Rect.X + Rect.Width + 5,	mY};
			temp.DrawLine(&Gdi_L1, pt1, pt2);
			Gdiplus::RectF Rect2 ={0, mY - 8, 50, 20};

			std::wstringstream sdw;
			sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << maxt;

			temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);

			float iY = Rect.Y + float((maxt - mint) * coeffH);
			pt1 ={Rect.X - 5,				iY};
			pt2 ={Rect.X + Rect.Width + 5,	iY};
			temp.DrawLine(&Gdi_L1, pt1, pt2);


			Rect2 ={0, iY - 8, 50, 20};

			sdw.str(std::wstring());
			sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << mint;
			temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);

			Gdiplus::PointF p1 ={0, 0};
			Gdiplus::PointF p2;;
			p1.X = Rect.X;
			p1.Y = Rect.Y + float((maxt - st.begin()->second.second) * coeffH);

			for(auto& a : st)
			{
				p2.X =  Rect.X + float((a.second.first - mind) * coeffW);
				p2.Y =  Rect.Y + float((maxt - a.second.second) * coeffH);
				temp.DrawLine(&Gdi_L2, p1, p2);

				p1.X = p2.X;
				p1.Y = p2.Y;
			}
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
	{
		try
		{
			Gdiplus::Pen Gdi_L1(Gdiplus::Color(255, 0, 0), 2); //Красный
			Gdiplus::Pen Gdi_L2(Gdiplus::Color(0, 0, 255), 2); //Синий

			Gdiplus::PointF pt1 ={Rect.X + 0, Rect.Y + 5};
			Gdiplus::PointF pt2 ={Rect.X + 20, Rect.Y + 5};

			temp.DrawLine(&Gdi_L1, pt1, pt2);

			pt1 ={Rect.X + 100, Rect.Y + 5};
			pt2 ={Rect.X + 120, Rect.Y + 5};

			temp.DrawLine(&Gdi_L2, pt1, pt2);


			Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));
			Gdiplus::StringFormat stringFormat;
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

			Gdiplus::RectF Rect2 = Rect;
			Rect2.X += 25;
			Rect2.Y -= 5;
			Rect2.Height = 20;
			temp.DrawString(L"Задание", -1, &font2, Rect2, &stringFormat, &Gdi_brush);

			Rect2 = Rect;
			Rect2.X += 125;
			Rect2.Y -= 5;
			Rect2.Height = 20;
			temp.DrawString(L"Факт", -1, &font2, Rect2, &stringFormat, &Gdi_brush);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat)
	{
		try
		{
			Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));

			std::wstring::const_iterator start = str.begin();
			std::wstring::const_iterator end = str.end();
			boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}:\\d{1,2}).*");
			boost::match_results<std::wstring::const_iterator> what;

			if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 1)
				temp.DrawString(what[1].str().c_str(), -1, &font1, Rect, &stringFormat, &Gdi_brush);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage)
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

			Gdiplus::Pen Gdi_Bar(Gdiplus::Color(0, 0, 0), 1);
			if(!Act.size()) return;
			if(!Ref.size()) return;

			double maxt = 0;
			double mint = 500;
			int64_t mind = (std::min)(Act.begin()->second.first, Ref.begin()->second.first);

			//auto b = Act.begin();
			//auto e = Act.rbegin();

			int64_t maxcount = 0;
			int64_t maxd = (std::max)(maxcount, Act.rbegin()->second.first - Act.begin()->second.first);
			//int64_t maxd = 0;// e->second.first - b->second.first;

			//b = Ref.begin();
			//e = Ref.rbegin();
			maxd = (std::max)(maxd, Ref.rbegin()->second.first - Ref.begin()->second.first);


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

			Gdiplus::Color Blue(0, 0, 255);
			Gdiplus::Color Red(255, 0, 0);

			Gdiplus::RectF RectG2(RectG);
			RectG2.Y += 5;
			RectG2.Height -= 25;
			RectG2.X += 35;
			RectG2.Width -= 40;

			DrawBottom(temp, RectG2, Red, Ref, mind, maxd, mint, maxt);	//Красный; Заданное значение температуры
			DrawBottom(temp, RectG2, Blue, Act, mind, maxd, mint, maxt);	//Синий; Фактическое значение температуры

			Gdiplus::RectF RectG3(RectG);
			RectG3.X = 100;
			RectG3.Y = RectG.Height - 15;
			DrawInfo(temp, RectG3);

			//b = Act.begin();
			//e = Act.rbegin();
			//e--;
			std::wstring sDataBeg(Act.begin()->first.begin(), Act.begin()->first.end());
			std::wstring sDataEnd(Act.rbegin()->first.begin(), Act.rbegin()->first.end());

			Gdiplus::RectF RectText(RectG);
			Gdiplus::StringFormat stringFormat;
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);

			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
			DrawTime(temp, RectText, sDataBeg, stringFormat);

			stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
			DrawTime(temp, RectText, sDataEnd, stringFormat);


			std::wstring SaveFile(fImage.begin(), fImage.end());
			backBuffer.Save(SaveFile.c_str(), &guidJpeg, NULL);

			DeleteDC(hdc);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::DrawHeder(HPDF_REAL left, HPDF_REAL top)
	{
		try
		{
			draw_text (page, left + 20, top - 40, "Марка стали");
			draw_text_rect (page, left + 100, top - 40, XP, YP, Sheet.Alloy);

			draw_text (page, left + 220, top - 40, "Размер листа");
			draw_text_rect (page, left + 300, top - 40, XP, YP, Sheet.Thikness);


			draw_text (page, left + 20, top - 70, "Плавка");
			draw_text_rect (page, left + 70, top - 70, XP, YP, Sheet.Melt);

			draw_text (page, left + 180, top - 70, "Партия");
			draw_text_rect (page, left + 230, top - 70, XP, YP, Sheet.PartNo);

			draw_text (page, left + 350, top - 70, "Пачка");
			draw_text_rect (page, left + 390, top - 70, XP, YP, Sheet.Pack);

			draw_text (page, left + 510, top - 70, "Лист");
			draw_text_rect (page, left + 550, top - 70, XP, YP, Sheet.Sheet + " / " + Sheet.SubSheet);

			draw_text (page, left + 680, top - 70, "Сляб");
			draw_text_rect (page, left + 720, top - 70, XP, YP, Sheet.Slab);

			HPDF_Page_MoveTo (page, left + 15, top - 80);
			HPDF_Page_LineTo (page, left + Width - 20, top - 80);
			HPDF_Page_Stroke (page);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
	{
		try
		{
			std::string ss = Sheet.Start_at;
			std::vector <std::string>split;
			boost::split(split, ss, boost::is_any_of(" "), boost::token_compress_on);

			draw_text_rect (page, left + 0, top - 0, w, YP, "Дата и время загрузки");
			if(split.size() >= 2)
			{
				draw_text_rect (page, left + 270, top - 0, XP, YP, split[0]);  //Дата
				draw_text_rect (page, left + 340, top - 0, XP, YP, split[1]);  //Время
			}

			draw_text (page, left + 10, top - 25, "Параметры");
			draw_text (page, left + 280, top - 25, "Задание");
			draw_text (page, left + 355, top - 25, "Факт");

			draw_text_rect (page, left + 0, top - 45, w, YP, "Время нахождения листа в закалочной печи. мин");
			draw_text_rect (page, left + 270, top - 45, XP, YP, Sheet.TimeForPlateHeat);//Задание Время нахождения листа в закалочной печи. мин
			draw_text_rect (page, left + 340, top - 45, XP, YP, Sheet.DataTime_All);    //Факт Время нахождения листа в закалочной печи. мин

			draw_text_rect (page, left + 0, top - 70, w, YP, "Температура закалки, °С");
			draw_text_rect (page, left + 270, top - 70, XP, YP, Sheet.Temper);			//Задание Температуры закалки
			draw_text_rect (page, left + 340, top - 70, XP, YP, strSrTemp);				//Факт Температуры закалки

			draw_text_rect (page, left + 0, top - 95, w, YP, "Давление воды в коллекторе закал. машины, бар");
			draw_text_rect (page, left + 270, top - 95, XP, YP, Sheet.PresToStartComp);	//Задание Давления воды
			draw_text_rect (page, left + 340, top - 95, XP, YP, Sheet.Za_PT3);			//Факт Давления воды

			draw_text_rect (page, left + 0, top - 120, w, YP, "Температура закалочной жидкости, °С");
			draw_text_rect (page, left + 340, top - 120, XP, YP, Sheet.Za_TE3);			//Факт Температура воды, °С

			HPDF_Page_MoveTo (page, 15, top - 130);
			HPDF_Page_LineTo (page, Width - 20, top - 130);
			HPDF_Page_Stroke (page);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
	{
		try
		{
			std::string ss = Cassette.Run_at;
			std::vector <std::string>split;
			boost::split(split, ss, boost::is_any_of(" "), boost::token_compress_on);

			draw_text_rect (page, left + 0, top - 0, w, YP, "Дата и время загрузки");
			if(split.size() >= 2)
			{
				draw_text_rect (page, left + 270, top - 0, XP, YP, split[0]);  //Дата
				draw_text_rect (page, left + 340, top - 0, XP, YP, split[1]);  //Время
			}

			draw_text (page, left + 10, top - 25, "Параметры");
			draw_text (page, left + 280, top - 25, "Задание");
			draw_text (page, left + 355, top - 25, "Факт");

			draw_text_rect (page, left + 0, top - 45, w, YP, "Время нагрева до температуры отпуска, мин");
			draw_text_rect (page, left + 270, top - 45, XP, YP, Cassette.PointTime_1);						//Задание
			draw_text_rect (page, left + 340, top - 45, XP, YP, Cassette.HeatAcc);							//Факт

			draw_text_rect (page, left + 0, top - 70, w, YP, "Время выдержки при заданной температуре, мин");
			draw_text_rect (page, left + 270, top - 70, XP, YP, Cassette.PointDTime_2);						//Задание
			draw_text_rect (page, left + 340, top - 70, XP, YP, Cassette.HeatWait);							//Факт

			draw_text_rect (page, left + 0, top - 95, w, YP, "Температура отпуска, °С");
			draw_text_rect (page, left + 270, top - 95, XP, YP, Cassette.PointRef_1);						//Задание
			draw_text_rect (page, left + 340, top - 95, XP, YP, Cassette.f_temper);							//Факт
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
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

			//HPDF_SetCurrentEncoder(pdf, "CP1251");

			font = HPDF_GetFont(pdf, detail_font_name, "UTF-8");


			page = HPDF_AddPage(pdf);

			HPDF_Page_SetFontAndSize (page, font, 10);

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

	//Сохранение листа PDF
	void PdfClass::SavePDF()
	{
		std::stringstream temp;
		try
		{
			temp << lpLogPdf;
			CheckDir(temp.str());
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

		try
		{
			temp << "/" << Sheet.Year;
			CheckDir(temp.str());
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

		try
		{
			temp << "/" << MonthName[Sheet.Month];
			CheckDir(temp.str());
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " " + Sheet.Month +" ");

		try
		{
			temp << "/" << std::setw(2) << std::setfill('0') << std::right << Sheet.Day;
			CheckDir(temp.str());
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

		try
		{
			temp << "/" << std::setw(4) << std::setfill('0') << std::right << Cassette.Year << "-";
			temp << std::setw(2) << std::setfill('0') << std::right << Cassette.Month << "-";
			temp << std::setw(2) << std::setfill('0') << std::right << Cassette.Day << "-";
			temp << std::setw(2) << std::setfill('0') << std::right << Cassette.CassetteNo;
			CheckDir(temp.str());
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + temp.str() + " ");

		try
		{
			std::stringstream temp2;
			temp2 << temp.str() << "/" << furnImage;
			std::filesystem::copy_file(furnImage, temp2.str(), std::filesystem::copy_options::skip_existing | std::filesystem::copy_options::overwrite_existing);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + temp.str() + " ");

		try
		{
			temp << "/";
			temp << std::setw(6) << std::setfill('0') << Sheet.Melt << "-";
			temp << std::setw(3) << std::setfill('0') << Sheet.Slab << "-";
			temp << std::setw(3) << std::setfill('0') << Sheet.PartNo << "-";
			temp << std::setw(3) << std::setfill('0') << Sheet.Pack << "-";
			temp << std::setw(3) << std::setfill('0') << Sheet.Sheet << "-";
			temp << std::setw(2) << std::setfill('0') << Sheet.SubSheet;
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + temp.str() + " ");

			try
			{
				int n = 1;
				if(std::filesystem::exists(temp.str() + ".pdf"))
				{
					FileName = temp.str() + "_" + std::to_string(n) + ".pdf";
				}
				else
					FileName = temp.str() + ".pdf";

				HPDF_SaveToFile (pdf, FileName.c_str());
				HPDF_Free (pdf);
			}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + FileName + " ");
	}

	//Рисуем Закалка
	void PdfClass::DrawKpvlPDF()
	{
		try
		{
			DrawKpvl(410, Height - 120, Width - 432);

			//График температуры закалки
			draw_text(page, 20, Height - 100, "Закалка");
			HPDF_Page_Rectangle(page, 20, Height - 240, 374, 140);
			HPDF_Page_Stroke(page);

			//std::ifstream ifs(tempImage);
			//if(!ifs.bad())
			std::fstream ifs(tempImage, std::fstream::binary | std::fstream::in);
			if(ifs.is_open())
			{
				ifs.close();
				HPDF_Image image1 = HPDF_LoadJpegImageFromFile(pdf, tempImage);
				HPDF_Page_DrawImage (page, image1, 22, Height - 239, 370, 137);
			}
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);

	}

	//Рисуем Отпуск
	void PdfClass::DrawFurnPDF()
	{
		try
		{
	 //График температуры отпуска
			DrawFurn(410, Height - 290, Width - 432);
			draw_text(page, 20, Height - 270, "Отпуск");
			HPDF_Page_Rectangle(page, 20, Height - 410, 374, 140);
			HPDF_Page_Stroke(page);
			std::fstream ifs(furnImage, std::fstream::binary | std::fstream::in);
			//std::ifstream ifs(furnImage);
			//if(!ifs.bad())
			if(ifs.is_open())
			{
				ifs.close();
				HPDF_Image image2 = HPDF_LoadJpegImageFromFile(pdf, furnImage);
				HPDF_Page_DrawImage (page, image2, 22, Height - 409, 370, 137);
			}
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	PdfClass::PdfClass(TSheet& sheet, bool view)
	{
		try
		{
	#pragma region Готовим графики
			Sheet = sheet;

			if(!conn.connection())
				return;// throw std::exception(__FUN(std::string("Error SQL conn connection to GraffKPVL")));
			conn.Name = "test";

			if(!Sheet.Year.length() || !Sheet.Month.length() || !Sheet.Day.length() || !Sheet.CassetteNo.length())
			{
				return;
			}

			GetCassete(Cassette);
			if(!Cassette.Run_at.length() || !Cassette.Finish_at.length())
			{
				//MessageBox(GlobalWindow, "Лист еще небыл на отпуске", "Ошибка", MB_OK | MB_ICONWARNING | MB_APPLMODAL);
				LOG_INFO(PdfLogger, "{:90}| Лист {:06}-{:03}-{:03}-{:03}-{:03}/{:03}, Касета {:04}-{:02}-{:02}-{:02} еще не была в отпускной печи",
						 FUNCTION_LINE_NAME,
						 std::stoi(Sheet.Melt), std::stoi(Sheet.Slab), std::stoi(Sheet.PartNo), std::stoi(Sheet.Pack), std::stoi(Sheet.Sheet), std::stoi(Sheet.SubSheet),
						 std::stoi(Cassette.Year), std::stoi(Cassette.Month), std::stoi(Cassette.Day), std::stoi(Cassette.CassetteNo));
				return;
			}

			int P = atoi(Cassette.Peth.c_str());

			FurnRef.erase(FurnRef.begin(), FurnRef.end());
			FurnAct.erase(FurnAct.begin(), FurnAct.end());
			TempAct.erase(TempAct.begin(), TempAct.end());
			TempRef.erase(TempRef.begin(), TempRef.end());

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

				if(!std::stof(Cassette.f_temper))
				{
					//std::tm TM_Temp ={0};
					//int m = 60 * 5;
					//if(Cassette.End_at.length())
					//	DataTimeOfString(Cassette.End_at, FORMATTIME, TM_Temp);
					//else
					//{
					//
					//	DataTimeOfString(Cassette.End_at, FORMATTIME, TM_Temp);
					//	m = 60 * 15;
					//}
					//
					//TM_Temp.tm_year -= 1900;
					//TM_Temp.tm_mon -= 1;
					//std::time_t st = (std::time_t)difftime(mktime(&TM_Temp), m); 
					////localtime_s(&TM_Temp, &st);
					//std::string fd = GetDataTimeString(st);
					//
					//T_SqlTemp TempFurnAct ={};	//Временное Актуальное
					//if(P == 1)
					//	GetTempRef(Cassette.Run_at, fd, TempFurnAct, ForBase_RelFurn_1.TempAct->ID);
					//if(P == 2)
					//	GetTempRef(Cassette.Run_at, fd, TempFurnAct, ForBase_RelFurn_2.TempAct->ID);
					//
					//auto b = FurnAct.rbegin();
					//float f = b->second.second;
					//int tt = 0;
				}
			}
			//return;

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
				DrawHeder(0, Height);

				//Рисуем PDF Закалка
				DrawKpvlPDF();

				//Рисуем PDF Отпуск
				DrawFurnPDF();

				//Сохраняем PDF
				SavePDF();

				//Отображение PDF
				if(view) std::system(("start " + FileName).c_str());
			}
			remove(furnImage);
			remove(tempImage);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);

	};

	PdfClass::PdfClass(TCassette& TC)
	{
		try
		{
	#pragma region Готовим графики
			Cassette = TC;

			//Sheet = sheet;

			if(!conn.connection())
				return;// throw std::exception(__FUN(std::string("Error SQL conn connection to GraffKPVL")));
			conn.Name = "test";
			GetSheet();


			FurnRef.erase(FurnRef.begin(), FurnRef.end());
			FurnAct.erase(FurnAct.begin(), FurnAct.end());

			//GetCassete(cassette);
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

			for(auto& a : AllPfdSheet)
			{
				if(!isRun) break;
				Sheet = a;

				std::stringstream sss;
				sss << "Кассета: "
					<< std::setw(4) << std::setfill('0') << Cassette.Year << "-"
					<< std::setw(2) << std::setfill('0') << Cassette.Month << "-"
					<< std::setw(2) << std::setfill('0') << Cassette.Day << " № "
					<< std::setw(2) << std::setfill('0') << Cassette.CassetteNo
					<< " Лист: "
					<< std::setw(6) << std::setfill('0') << Sheet.Melt << "-"
					<< std::setw(3) << std::setfill('0') << Sheet.Slab << "-"
					<< std::setw(3) << std::setfill('0') << Sheet.PartNo << "-"
					<< std::setw(3) << std::setfill('0') << Sheet.Pack << "-"
					<< std::setw(3) << std::setfill('0') << Sheet.Sheet << "/"
					<< std::setw(2) << std::setfill('0') << Sheet.SubSheet;


				SetWindowText(hWndDebug, sss.str().c_str());

				//Закалка
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

				//Рисуем PDF Отпуск

	#pragma endregion

			//Создание PFD файла
				if(NewPdf())
				{
					//Рисуем PDF заголовок
					DrawHeder(0, Height);

					//Рисуем PDF Закалка
					DrawKpvlPDF();

					//Рисуем PDF Отпуск
					DrawFurnPDF();

					//Сохраняем PDF
					SavePDF();
				}
			}
			remove(furnImage);
			remove(tempImage);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	};
};

//Открывается по клику на лист
void PrintPdf(TSheet& Sheet)
{
	try
	{
		PDF::PdfClass* pdf  = new PDF::PdfClass(Sheet);
	}
	CATCH(PdfLogger, std::string("PrintPdf: "));
}

//Автоматическое создание по листам
void PrintPdfAuto(TSheet& Sheet, bool view)
{
	try
	{
		PDF::PdfClass pdf(Sheet, view);
	}
	CATCH(PdfLogger, std::string("PrintPdf: "));
}

//Автоматическое создание по кассете
void PrintPdfAuto(TCassette& TC)
{
	try
	{
		if(TC.Run_at.length() && TC.Finish_at.length())
		{
			std::stringstream sss;
			sss << "PrintPdfAuto "
				<< boost::format("%|04|-") % TC.Year
				<< boost::format("%|02|-") % TC.Month
				<< boost::format("%|02| ") % TC.Day
				<< " № " << TC.CassetteNo;
			SetWindowText(hWndDebug, sss.str().c_str());
			PDF::PdfClass pdf(TC);
		}
		//else
		//	LOG_INFO(PdfLogger, "{:90}| ERROR: TC.Run_at.length()={}, TC.Finish_at.length()={}", FUNCTION_LINE_NAME, TC.Run_at.length(), TC.Finish_at.length())
	}
	CATCH(PdfLogger, FUNCTION_LINE_NAME);
}


void AllPdf(PGConnection& conn)
{
	SetWindowText(hWndDebug, "Start PrintPdfAuto");

	std::deque<TCassette> allCassette;

	//std::string comand = "SELECT * FROM cassette WHERE event = 5 AND run_at IS NOT NULL AND finish_at IS NOT NULL ORDER BY id;";
	std::stringstream com;
	com << "SELECT * FROM cassette WHERE finish_at IS NOT NULL";
	com << " AND run_at >= '" + DateStart + "'";
	com << " AND delete_at IS NULL ";
	com << " AND pdf IS NULL ";
	com << "ORDER BY id;";
	std::string comand = com.str();
	if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
	PGresult* res = conn.PGexec(comand);
	//LOG_INFO(PdfLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		S107::GetColl(res);
		allCassette.erase(allCassette.begin(), allCassette.end());
		int line =  PQntuples(res);
		for(int l = 0; l < line && isRun; l++)
		{
			TCassette cassette;
			S107::GetCassette(res, cassette, l);
			allCassette.push_back(cassette);
		}
		PQclear(res);
		for(auto ac : allCassette)
		{
			if(!isRun) break;
			PrintPdfAuto(ac);
			std::stringstream ss;
			ss << "UPDATE cassette SET pdf = now() WHERE id = " << ac.Id << ";";
			SETUPDATESQL(PdfLogger, conn, ss);
		}
	}
	else
	{
		LOG_ERR_SQL(PdfLogger, res, comand);
		PQclear(res);
	}
	SetWindowText(hWndDebug, "Stop PrintPdfAuto");
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

bool isCassete(T_cass& tc)
{
	return tc.Day && tc.Month && tc.Year && tc.CassetteNo;
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

void GetVal(PGConnection& conn, float& f, int ID, std::string Run_at, std::string End_at)
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
		for(int l = 0; l < line; l++)
		{
			f = std::stof(conn.PGgetvalue(res, l, 0));
		}
	}
	else
	{
		LOG_ERR_SQL(PdfLogger, res, comand);
	}
	PQclear(res);

}

void GetVal(PGConnection& conn, T_cass& P, T_ForBase_RelFurn* Furn)
{
	std::tm TM;
	time_t temp;// = DataTimeOfString(P.End_at, FORMATTIME, TM);
	//temp  = (time_t)difftime(temp, 60); //Вычисть 1 минут до конца отпуска что бы не пропустить данных из контроллера
	//localtime_s(&TM, &temp);
	std::string End_at = ""; // GetDataTimeString(TM);

	{
		if(P.Day == 2 && P.Month == 5 && P.CassetteNo == 13)
		{
			int tt = 0;
		}
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
			<< " ORDER BY todos.id_name, todos.id DESC";
		std::string comand = sss.str();
		PGresult* res = conn.PGexec(comand);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			for(int l = 0; l < line; l++)
			{
				int id = std::stoi(conn.PGgetvalue(res, l, 0));
				float f = std::stof(conn.PGgetvalue(res, l, 1));

				if(id == Furn->ActTimeHeatAcc->ID) P.ActTimeHeatAcc = f;
				if(id == Furn->ActTimeHeatWait->ID) P.ActTimeHeatWait = f;
				if(id == Furn->ActTimeTotal->ID) P.ActTimeTotal = f;
			}
		}
		else
		{
			LOG_ERR_SQL(PdfLogger, res, comand);
		}
		PQclear(res);
	}

	{
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
			<< " ORDER BY todos.id_name, todos.id DESC";
		std::string comand = sss.str();
		PGresult* res = conn.PGexec(comand);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			for(int l = 0; l < line; l++)
			{
				int id = std::stoi(conn.PGgetvalue(res, l, 0));
				float f = std::stof(conn.PGgetvalue(res, l, 1));

				if(id == Furn->PointRef_1->ID) P.PointRef_1 = f;
				if(id == Furn->PointTime_1->ID)P.PointTime_1 = f;
				if(id == Furn->PointDTime_2->ID) P.PointDTime_2 = f;
				if(id == Furn->TimeProcSet->ID) P.TimePocSet = f;
			}
		}
		else
		{
			LOG_ERR_SQL(PdfLogger, res, comand);
		}
		PQclear(res);
	}

	temp = DataTimeOfString(P.End_at, FORMATTIME, TM);
	temp  = (time_t)difftime(temp, 5 * 60); //Вычисть 5 минут до конца отпуска
	localtime_s(&TM, &temp);
	End_at  = GetDataTimeString(TM);
	GetVal(conn, P.TempAct, Furn->TempAct->ID, P.Run_at, End_at);

	int tt = 1;
}

void EndCassette(PGConnection& conn, T_cass& P, int Petch, std::fstream& s1)
{
	T_ForBase_RelFurn* Furn = NULL;
	if(Petch == 1)
		Furn = &ForBase_RelFurn_1;
	if(Petch == 2)
		Furn = &ForBase_RelFurn_2;
	if(Furn == NULL) return;

	if(isCassete(P))
	{
		if(P.Run_at.size())
		{
			P.tm_Run = DataTimeOfString(P.Run_at, FORMATTIME, P.TM_Run);
			P.tm_End = DataTimeOfString(P.End_at, FORMATTIME, P.TM_End);
			P.tm_All = (time_t)difftime(P.tm_End, P.tm_Run);
			gmtime_s(&P.TM_All, &P.tm_All);
			P.TM_All.tm_year -= 1900;
			P.TM_All.tm_mday -= 1;

			P.tm_Fin = DataTimeOfString(P.End_at, FORMATTIME, P.TM_Fin);
			P.tm_Fin = P.tm_End + (15 * 60);
			localtime_s(&P.TM_Fin, &P.tm_Fin);

			//P1.End_at
			P.Fin_at = GetDataTimeString(P.TM_Fin);

			std::stringstream sg2;
			GetVal(conn, P, Furn);

			s1 << P.id << ";"
				<< " " << P.Run_at << ";"
				<< " " << P.End_at << ";"
				<< P.Year << ";"
				<< P.Month << ";"
				<< P.Day << ";"
				<< P.CassetteNo << ";"
				<< " " << P.Err_at << ";"

				<< Formats(P.PointRef_1) << ";"		//Уставка температуры
				<< Formats(P.TempAct) << ";"			//Фактическое значение температуры

				<< Formats(P.PointTime_1) << ";"		//Задание Время нагрева
				<< Formats(P.ActTimeHeatAcc) << ";"	//Факт время нагрева
				<< Formats(P.PointDTime_2) << ";"		//Задание Время выдержки
				<< Formats(P.ActTimeHeatWait) << ";"	//Факт время выдержки

				<< Formats(P.TimePocSet) << ";"		//Полное время процесса (уставка), мин
				<< Formats(P.ActTimeTotal) << ";"		//Факт общее время

				<< std::endl;


			P.Petch = Petch;
		}
	}
}

void SaveCassete(PGConnection& conn, MapRunn& CasseteTodos, int Petch)
{
	T_cass P;
	T_ForBase_RelFurn* Furn = NULL;
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
		for(int l = 0; l < line; l++)
		{

			T_Todos ct;
			ct.value = conn.PGgetvalue(res, l, TODOS::content);
			ct.create_at = conn.PGgetvalue(res, l, TODOS::create_at);
			ct.id = std::stoi(conn.PGgetvalue(res, l, TODOS::id));
			//ct.id_name_at = "";
			ct.id_name = std::stoi(conn.PGgetvalue(res, l, TODOS::id_name));
			ct.type =  std::stoi(conn.PGgetvalue(res, l, TODOS::type));
			ct.content = GetVarVariant((OpcUa::VariantType)ct.type, ct.value);
			ct.id_name_at = conn.PGgetvalue(res, l, TODOS::name);
			ct.Petch = Petch;
			CasseteTodos[ct.id] = ct;
		}
	}
	else
	{
		LOG_ERR_SQL(PdfLogger, res, comand);
	}
	PQclear(res);


	std::string name = "cass" + std::to_string(Petch) + ".csv";
	std::fstream s1(name, std::fstream::binary | std::fstream::out);

	int lin = 0;
	for(auto a : CasseteTodos)
	{
		lin++;
		if(a.second.id_name == Furn->Cassette.Day->ID)
		{
			if(!P.Run_at.size())
				P.Day = a.second.content.As<int32_t>();
			else if(P.Day != a.second.content.As<int32_t>())
			{
				P.End_at = a.second.create_at;
				EndCassette(conn, P, Petch, s1);
				P0[a.first] = P;

				P.Run_at = "";
				P.End_at = "";
				P.Err_at = "";
				P.Day = a.second.content.As<int32_t>();
			}
		}
		if(a.second.id_name == Furn->Cassette.Month->ID)
		{
			if(!P.Run_at.size())
				P.Month = a.second.content.As<int32_t>();
			else if(P.Month != a.second.content.As<int32_t>())
			{
				P.End_at = a.second.create_at;
				EndCassette(conn, P, Petch, s1);
				P0[a.first] = P;

				P.Run_at = "";
				P.End_at = "";
				P.Err_at = "";
				P.Month = a.second.content.As<int32_t>();
			}
		}
		if(a.second.id_name == Furn->Cassette.Year->ID)
		{
			if(!P.Run_at.size())
				P.Year = a.second.content.As<int32_t>();
			else if(P.Year != a.second.content.As<int32_t>())
			{

				P.End_at = a.second.create_at;
				EndCassette(conn, P, Petch, s1);
				P0[a.first] = P;

				P.Run_at = "";
				P.End_at = "";
				P.Err_at = "";
				P.Year = a.second.content.As<int32_t>();
			}
		}
		if(a.second.id_name == Furn->Cassette.CassetteNo->ID)
		{
			if(!P.Run_at.size())
				P.CassetteNo = a.second.content.As<int32_t>();
			else if(P.CassetteNo != a.second.content.As<int32_t>())
			{
				P.End_at = a.second.create_at;
				EndCassette(conn, P, Petch, s1);
				P0[a.first] = P;
				P.Run_at = "";
				P.End_at = "";
				P.Err_at = "";
				P.CassetteNo = a.second.content.As<int32_t>();
			}
		}

		if(a.second.id_name == Furn->ProcRun->ID && a.second.content.As<bool>())
		{
			if(isCassete(P))
			{
				if(!P.Run_at.size())
				{
					P.Run_at = a.second.create_at;
				}
			}
		}
		if(a.second.id_name == Furn->ProcEnd->ID && a.second.content.As<bool>())
		{
			P.End_at = a.second.create_at;
			EndCassette(conn, P, Petch, s1);
			P0[a.first] = P;
			P = T_cass();
		}

		if(a.second.id_name == Furn->ProcFault->ID && a.second.content.As<bool>())
		{
			if(!P.Err_at.length() && isCassete(P))
			{
				P.Err_at = a.second.create_at;
			}
		}
	}
	s1.close();
}

void CorrectSQL(PGConnection& conn)
{
	try
	{
		MapRunn CasseteTodos;
		P0.erase(P0.begin(), P0.end());

		SetWindowText(hWndDebug, "connection");

		//PGConnection conn;
		//conn.connection();
		//if(!conn.connections)
		//{
		//	throw std::exception((FUNCTION_LINE_NAME + "Error SqlCassette.connection").c_str());
		//}

		SetWindowText(hWndDebug, "SaveCassete");
		SaveCassete(conn, CasseteTodos, 1);
		SaveCassete(conn, CasseteTodos, 2);


#pragma region Запись в файл заголовка
		std::fstream sdg("sdg.csv", std::fstream::binary | std::fstream::out);

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

		for(auto& a : CasseteTodos)
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

		//sort (P0.begin(), P0.end(), comp);

		std::string sg = "Получение кассеты из базы " + std::to_string(P0.size());
		SetWindowText(hWndDebug, sg.c_str());
		std::fstream s1("cass.csv", std::fstream::binary | std::fstream::out);
		for(TCASS::iterator it = P0.begin(); it != P0.end() && isRun;)
		{
			if(it->second.Run_at.length() && it->second.End_at.length() && it->second.Year && it->second.Month && it->second.Day && it->second.CassetteNo)
			{
				s1 << it->first << ";"
					<< " " << it->second.Run_at << ";"
					<< " " << it->second.End_at << ";"
					<< it->second.Year << ";"
					<< it->second.Month << ";"
					<< it->second.Day << ";"
					<< it->second.CassetteNo << ";"
					<< " " << it->second.Err_at << ";"

					<< boost::format("%.1f") % it->second.PointRef_1 << ";"		//Уставка температуры
					<< boost::format("%.1f") % it->second.TempAct << ";"			//Фактическое значение температуры

					<< boost::format("%.1f") % it->second.PointTime_1 << ";"		//Задание Время нагрева
					<< boost::format("%.1f") % it->second.ActTimeHeatAcc << ";"	//Факт время нагрева
					<< boost::format("%.1f") % it->second.PointDTime_2 << ";"		//Задание Время выдержки
					<< boost::format("%.1f") % it->second.ActTimeHeatWait << ";"	//Факт время выдержки

					<< boost::format("%.1f") % it->second.TimePocSet << ";"		//Полное время процесса (уставка), мин
					<< boost::format("%.1f") % it->second.ActTimeTotal << ";"		//Факт общее время

					<< std::endl;
			}

#pragma region Получение кассеты из базы

			std::stringstream sss;
			sss << "SELECT id, tempref FROM cassette WHERE"
				<< " day = " << it->second.Day
				<< " AND month = " << it->second.Month
				<< " AND year = " << it->second.Year
				<< " AND cassetteno = " << it->second.CassetteNo
				<< " AND correct IS NULL"
				<< " ORDER BY id";
			std::string comand = sss.str();
			PGresult* res = conn.PGexec(comand);
			if(PQresultStatus(res) == PGRES_TUPLES_OK)
			{
				int line = PQntuples(res);
				if(line)
				{
					std::string s = conn.PGgetvalue(res, line - 1, 0);
					std::string f = conn.PGgetvalue(res, line - 1, 1);
					if(s.length())
					{
						it->second.id = std::stoi(s);
					}
					if(f.length())
					{
						it->second.f_temper = std::stof(f);
					}
				}
			}
			else
			{
				LOG_ERR_SQL(PdfLogger, res, comand);
				throw std::exception(FUNCTION_LINE_NAME.c_str());
			}

#pragma endregion

			it++;
		}
		s1.close();
		//return 0;


#pragma region Запись в файл заголовка
		std::fstream ss1("cass0.csv", std::fstream::binary | std::fstream::out);
		ss1 << "Номер;" << "Индекс;"
			<< "Старт;" << "Стоп;" << "Финиш;"
			<< "Год;" << "Месяц;" << "День;" << "Кассета;" << "Авария;" << "Печь;" << "Повтор;"
			//<< "Год;" << "Месяц;" << "День;" << "Час;" << "Мин;" << "Сек;" 
			//<< "Год;" << "Месяц;" << "День;" << "Час;" << "Мин;" << "Сек;" 
			//<< "Год;" << "Месяц;" << "День;" 
			//<< "Час;" << "Мин;" << "Сек;"
			<< "Уставка T;"
			<< "Факт Т;"
			<< "Время нагрева;"
			<< "Факт нагрева;"
			<< "Время выдержки;"
			<< "Факт выдержки;"
			<< "Время процесса;"
			<< "Факт процесса;"
			<< "Время;"
			<< std::endl;
#pragma endregion

		//int nom = 2;
		sg = "Индексация " + std::to_string(P0.size());
		SetWindowText(hWndDebug, sg.c_str());
		for(auto a : P0 | boost::adaptors::indexed(2))
			a.value().second.nom = (int)a.index();


		sg = "Поиск повторов " + std::to_string(P0.size());
		SetWindowText(hWndDebug, sg.c_str());
		int lin = 0;
		for(TCASS::iterator it = P0.begin(); it != P0.end() && isRun; it++)
		{
			if(it->second.Run_at.length() && it->second.End_at.length() && it->second.Year && it->second.Month && it->second.Day && it->second.CassetteNo)
			{
#pragma region Проверка повторов 
				TCASS::iterator iy = it;
				iy++;
				for(TCASS::iterator iit = iy; iit != P0.end(); iit++)
				{
					if(iit->second.Run_at.length() && iit->second.End_at.length() && iit->second.Year && iit->second.Month && iit->second.Day && iit->second.CassetteNo)
					{
						if(
							iit->second.Year == it->second.Year &&
							iit->second.Month == it->second.Month &&
							iit->second.Day == it->second.Day &&
							iit->second.CassetteNo == it->second.CassetteNo
							)
						{
							it->second.Repeat_at = iit->second.nom;
							iit->second.Repeat_at = it->second.nom;
						}
					}
				}
#pragma endregion

#pragma region Запись в файл

				ss1 << it->second.nom << ";"
					<< it->second.id << ";"
					<< " " << it->second.Run_at << ";"
					<< " " << it->second.End_at << ";"
					<< " " << it->second.Fin_at << ";"
					<< it->second.Year << ";"
					<< it->second.Month << ";"
					<< it->second.Day << ";"
					<< it->second.CassetteNo << ";"
					<< " " << it->second.Err_at << ";"
					<< it->second.Petch << ";"
					<< it->second.Repeat_at << ";";

				ss1 << Formats(it->second.PointRef_1) << ";"		//Уставка температуры

					<< Formats(it->second.TempAct) << ";"			//Фактическое значение температуры

					<< Formats(it->second.PointTime_1) << ";"		//Задание Время нагрева
					<< Formats(it->second.ActTimeHeatAcc) << ";"	//Факт время нагрева

					<< Formats(it->second.PointDTime_2) << ";"		//Задание Время выдержки
					<< Formats(it->second.ActTimeHeatWait) << ";"	//Факт время выдержки

					<< Formats(it->second.TimePocSet) << ";"		//Полное время процесса (уставка), мин
					<< Formats(it->second.ActTimeTotal) << ";"		//Факт общее время

					<< Formats(it->second.tm_All / 60.0f) << ";";	//Время в часах
#pragma endregion
				lin++;

#pragma region Запись в базу
				if(it->second.id)
				{
					std::stringstream sg2;
					sg2 << "Коррекция базы " << P0.size() << ":" << lin << " id = " << it->second.id;
					SetWindowText(hWndDebug, sg2.str().c_str());

					std::stringstream sss;
					sss << "UPDATE cassette SET ";
					sss << "";

					if(it->second.Run_at.length())
						sss << "run_at = '" << it->second.Run_at << "', ";
					else
						sss << "run_at = DEFAULT, ";

					if(it->second.End_at.length())
						sss << "end_at = '" << it->second.End_at << "', ";
					else
						sss << "end_at = DEFAULT, ";

					if(it->second.Fin_at.length())
						sss << "finish_at = '" << it->second.Fin_at << "', correct = now(), pdf = DEFAULT";
					else 
						sss << "finish_at = DEFAULT, correct = DEFAULT";


					if(it->second.PointRef_1)sss << ", pointref_1 = " << it->second.PointRef_1;
					if(it->second.TempAct && it->second.f_temper == 0.0f) sss << ", facttemper = " << it->second.TempAct;

					if(it->second.PointTime_1) sss << ", pointtime_1 = " << it->second.PointTime_1;
					if(it->second.ActTimeHeatAcc)sss << ", heatacc = " << it->second.ActTimeHeatAcc;


					if(it->second.PointDTime_2)sss << ", pointdtime_2 = " << it->second.PointDTime_2;
					if(it->second.ActTimeHeatWait)sss << ", heatwait = " << it->second.ActTimeHeatWait;

					if(it->second.TimePocSet)sss << ", timeprocset = " << it->second.TimePocSet;
					if(it->second.ActTimeTotal)sss << ", total = " << it->second.ActTimeTotal;

					sss << " WHERE id = " << it->second.id;
					std::string comand = sss.str();
					SETUPDATESQL(PdfLogger, conn, sss);
				}
#pragma endregion

#pragma region Запись в файл конца строки
				ss1 << std::endl;

#pragma endregion
			}
		}
#pragma region Закрытие файла
		ss1.close();
#pragma endregion

	}
#pragma region catch of try
	catch(std::exception& exc)
	{
		LOG_ERROR(PdfLogger, "{:89}| Error {}", FUNCTION_LINE_NAME, exc.what());

	}
	catch(...)
	{
		LOG_ERROR(PdfLogger, "{:89}| Error {}", FUNCTION_LINE_NAME, "Unknown");
	}
#pragma endregion

	SetWindowText(hWndDebug, "Закончили коррекцию");
}

void RunAllPdf()
{
	PdfLogger = InitLogger("Pdf Debug");
	PGConnection conn_pdf;
	conn_pdf.connection();
	DateStart = "";
	while(isRun)
	{
		std::stringstream ssq;
		//ssq << "SELECT DISTINCT ON (id) content FROM todof WHERE";
		//ssq << "id_name";
		//ssq << " ORDER BY id DESC";

		std::stringstream ssa;
		ssa << "SELECT DISTINCT ON (id) create_at FROM cassette WHERE";
		//ssa << "(event = 2 OR event = 4)";
		ssa << " AND correct IS NULL";
		ssa << " AND delete_at IS NULL";
		ssa << " ORDER BY id ASC LIMIT 1";
		std::string comand = ssa.str();
		PGresult* res = conn_pdf.PGexec(comand);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			if(PQntuples(res))
				DateStart = conn_pdf.PGgetvalue(res, 0, 0);
		}

		if(!DateStart.length())
		{
			std::time_t st = time(NULL);
			st = (std::time_t)difftime(st, 60 * 60 * 24 * 1); //за 1 суток
			DateStart = GetDataTimeString(&st);
		}

		LOG_INFO(PdfLogger, "{:90}| DateStart = {}", FUNCTION_LINE_NAME, DateStart);
		CorrectSQL(conn_pdf);
		AllPdf(conn_pdf);

		LOG_INFO(PdfLogger, "{:90}| End CassetteSQL", FUNCTION_LINE_NAME);
		int t = 0;
		while(t++ < 60 && isRun)
			std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	LOG_INFO(PdfLogger, "{:90}| Stop Pdf Debug", FUNCTION_LINE_NAME);
	return;
}