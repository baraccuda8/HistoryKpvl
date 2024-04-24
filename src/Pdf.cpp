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

const char* tempImage = "t_kpvl.jpg";
const char* furnImage = "t_furn.jpg";

extern std::string lpLogPdf;
extern std::string  FORMATTIME;
extern Gdiplus::Font font1;
extern Gdiplus::Font font2;

extern GUID guidBmp;
extern GUID guidJpeg;
extern GUID guidGif;
extern GUID guidTiff;
extern GUID guidPng;

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
	//printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
	LOG_ERROR(AllLogger, "{:90}| ERROR: error_no={}, detail_no={}", FUNCTION_LINE_NAME, (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
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

	T_SqlTemp FurnRef ={};	//�������
	T_SqlTemp FurnAct ={};	//����������

	T_SqlTemp TempRef ={};	//�������
	T_SqlTemp TempAct ={};	//����������

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

		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
		PGresult* res = conn.PGexec(comand);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			S107::GetColl(res);
			if(conn.PQntuples(res))
				S107::GetCassette(res, cassette, 0);
		}
		else
			LOG_ERR_SQL(SQLLogger, res, comand);
		PQclear(res);
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
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
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
		PGresult* res = conn.PGexec(comand);
		//LOG_INFO(SQLLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, FilterComand.str());
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			KPVL::SQL::GetCollumn(res);
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

				sheet.Day = conn.PGgetvalue(res, l, Col_Sheet_day);
				sheet.Month = conn.PGgetvalue(res, l, Col_Sheet_month);
				sheet.Year = conn.PGgetvalue(res, l, Col_Sheet_year);
				sheet.CassetteNo = conn.PGgetvalue(res, l, Col_Sheet_cassetteno);
				sheet.SheetInCassette = conn.PGgetvalue(res, l, Col_Sheet_sheetincassette);

				sheet.Start_at = GetStringData(conn.PGgetvalue(res, l, Col_Sheet_start_at));
				sheet.TimeForPlateHeat = conn.PGgetvalue(res, l, Col_Sheet_timeforplateheat);
				sheet.PresToStartComp = conn.PGgetvalue(res, l, Col_Sheet_prestostartcomp);

				AllPfdSheet.push_back(sheet);
			}
		}
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
}

void PdfClass::GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID)
{
	try
	{
		std::tm TM_Temp ={0};
		std::string sBegTime2 = Start;
		std::stringstream sde;
		sde << "SELECT max(create_at) FROM todos WHERE id_name = " << ID;
		sde << " AND create_at <= '";
		sde << Start;
		sde << "';";
		std::string comand = sde.str();
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
		PGresult* res = conn.PGexec(comand);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			if(PQntuples(res))
				sBegTime2 = conn.PGgetvalue(res, 0, 0);
		}
		else
			LOG_ERR_SQL(SQLLogger, res, comand);
		PQclear(res);


		std::stringstream sdt;
		sdt << "SELECT create_at, content FROM todos WHERE id_name = " << ID;
		if(sBegTime2.length())	sdt << " AND create_at >= '" << sBegTime2 << "'";
		if(Stop.length())	sdt << " AND create_at <= '" << Stop << "'";

		sdt << " ORDER BY create_at ASC;";

		comand = sdt.str();
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
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
				tr[GetStringData(Start)] = std::pair(mktime(&TM_Temp), f);


				for(int l = 0; l < line; l++)
				{
					std::string sData = GetStringData(conn.PGgetvalue(res, l, 0));

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
				tr[GetStringData(Stop)] = std::pair(mktime(&TM_Temp), f);
			}
		}
		else
			LOG_ERR_SQL(SQLLogger, res, comand);

		PQclear(res);
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
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
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
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
					std::string sData = GetStringData(conn.PGgetvalue(res, l, 0));
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
			LOG_ERR_SQL(SQLLogger, res, comand);
		PQclear(res);

		if(!std::stoi(Sheet.Temperature))
		{
			std::ostringstream oss;

			oss << std::setprecision(0) << std::fixed << SrTemp;

			std::string update = " temperature = " + oss.str();
			KPVL::Sheet::SetUpdateSheet(conn_kpvl, Sheet, update, "");

			strSrTemp = oss.str();
		}
		else strSrTemp = Sheet.Temperature;
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
}

void PdfClass::DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st, int64_t mind, int64_t maxd, double mint, double maxt)
{
	try
	{
		Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 0.5); //������
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
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
}

void PdfClass::DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
{
	try
	{
		Gdiplus::Pen Gdi_L1(Gdiplus::Color(255, 0, 0), 2); //�������
		Gdiplus::Pen Gdi_L2(Gdiplus::Color(0, 0, 255), 2); //�����

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
		temp.DrawString(L"�������", -1, &font2, Rect2, &stringFormat, &Gdi_brush);

		Rect2 = Rect;
		Rect2.X += 125;
		Rect2.Y -= 5;
		Rect2.Height = 20;
		temp.DrawString(L"����", -1, &font2, Rect2, &stringFormat, &Gdi_brush);
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
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
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
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

		//��������
		temp.Clear(Gdiplus::Color(255, 255, 255));

		Gdiplus::Pen Gdi_Bar(Gdiplus::Color(0, 0, 0), 1);
		if(!Act.size()) return;
		if(!Ref.size()) return;

		double maxt = 0;
		double mint = 2000;
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

		DrawBottom(temp, RectG2, Red, Ref, mind, maxd, mint, maxt);	//�������; �������� �������� �����������
		DrawBottom(temp, RectG2, Blue, Act, mind, maxd, mint, maxt);	//�����; ����������� �������� �����������

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
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
}

void PdfClass::DrawHeder(HPDF_REAL left, HPDF_REAL top)
{
	try
	{
		draw_text (page, left + 20, top - 40, "����� �����");
		draw_text_rect (page, left + 100, top - 40, XP, YP, Sheet.Alloy);

		draw_text (page, left + 220, top - 40, "������ �����");
		draw_text_rect (page, left + 300, top - 40, XP, YP, Sheet.Thikness);


		draw_text (page, left + 20, top - 70, "������");
		draw_text_rect (page, left + 70, top - 70, XP, YP, Sheet.Melt);

		draw_text (page, left + 180, top - 70, "������");
		draw_text_rect (page, left + 230, top - 70, XP, YP, Sheet.PartNo);

		draw_text (page, left + 350, top - 70, "�����");
		draw_text_rect (page, left + 390, top - 70, XP, YP, Sheet.Pack);

		draw_text (page, left + 510, top - 70, "����");
		draw_text_rect (page, left + 550, top - 70, XP, YP, Sheet.Sheet + " / " + Sheet.SubSheet);

		draw_text (page, left + 680, top - 70, "����");
		draw_text_rect (page, left + 720, top - 70, XP, YP, Sheet.Slab);

		HPDF_Page_MoveTo (page, left + 15, top - 80);
		HPDF_Page_LineTo (page, left + Width - 20, top - 80);
		HPDF_Page_Stroke (page);
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
}

void PdfClass::DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
{
	try
	{
		std::string ss = Sheet.Start_at;
		std::vector <std::string>split;
		boost::split(split, ss, boost::is_any_of(" "), boost::token_compress_on);

		draw_text_rect (page, left + 0, top - 0, w, YP, "���� � ����� ��������");
		if(split.size() >= 2)
		{
			draw_text_rect (page, left + 270, top - 0, XP, YP, split[0]);  //����
			draw_text_rect (page, left + 340, top - 0, XP, YP, split[1]);  //�����
		}

		draw_text (page, left + 10, top - 25, "���������");
		draw_text (page, left + 280, top - 25, "�������");
		draw_text (page, left + 355, top - 25, "����");

		draw_text_rect (page, left + 0, top - 45, w, YP, "����� ���������� ����� � ���������� ����. ���");
		draw_text_rect (page, left + 270, top - 45, XP, YP, Sheet.TimeForPlateHeat);//������� ����� ���������� ����� � ���������� ����. ���
		draw_text_rect (page, left + 340, top - 45, XP, YP, Sheet.DataTime_All);    //���� ����� ���������� ����� � ���������� ����. ���

		draw_text_rect (page, left + 0, top - 70, w, YP, "����������� �������, ��");
		draw_text_rect (page, left + 270, top - 70, XP, YP, Sheet.Temper);			//������� ����������� �������
		draw_text_rect (page, left + 340, top - 70, XP, YP, strSrTemp);				//���� ����������� �������

		draw_text_rect (page, left + 0, top - 95, w, YP, "�������� ���� � ���������� �����. ������, ���");
		draw_text_rect (page, left + 270, top - 95, XP, YP, Sheet.PresToStartComp);	//������� �������� ����
		draw_text_rect (page, left + 340, top - 95, XP, YP, Sheet.Za_PT3);			//���� �������� ����

		draw_text_rect (page, left + 0, top - 120, w, YP, "����������� ���������� ��������, ��");
		draw_text_rect (page, left + 340, top - 120, XP, YP, Sheet.Za_TE3);			//���� ����������� ����, ��

		HPDF_Page_MoveTo (page, 15, top - 130);
		HPDF_Page_LineTo (page, Width - 20, top - 130);
		HPDF_Page_Stroke (page);
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
}

void PdfClass::DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
{
	try
	{
		std::string ss = Cassette.Run_at;
		std::vector <std::string>split;
		boost::split(split, ss, boost::is_any_of(" "), boost::token_compress_on);

		draw_text_rect (page, left + 0, top - 0, w, YP, "���� � ����� ��������");
		if(split.size() >= 2)
		{
			draw_text_rect (page, left + 270, top - 0, XP, YP, split[0]);  //����
			draw_text_rect (page, left + 340, top - 0, XP, YP, split[1]);  //�����
		}

		draw_text (page, left + 10, top - 25, "���������");
		draw_text (page, left + 280, top - 25, "�������");
		draw_text (page, left + 355, top - 25, "����");

		draw_text_rect (page, left + 0, top - 45, w, YP, "����� ������� �� ����������� �������, ���");
		draw_text_rect (page, left + 270, top - 45, XP, YP, Cassette.PointTime_1);						//�������
		draw_text_rect (page, left + 340, top - 45, XP, YP, Cassette.HeatAcc);							//����

		draw_text_rect (page, left + 0, top - 70, w, YP, "����� �������� ��� �������� �����������, ���");
		draw_text_rect (page, left + 270, top - 70, XP, YP, Cassette.PointDTime_2);						//�������
		draw_text_rect (page, left + 340, top - 70, XP, YP, Cassette.HeatWait);							//����

		draw_text_rect (page, left + 0, top - 95, w, YP, "����������� �������, ��");
		draw_text_rect (page, left + 270, top - 95, XP, YP, Cassette.PointRef_1);						//�������
		draw_text_rect (page, left + 340, top - 95, XP, YP, Cassette.f_temper);							//����
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
}


//�������� ������ ����� PDF
bool PdfClass::NewPdf()
{
	pdf = HPDF_New (error_handler, NULL);
	if(!pdf)
	{
		return false; //throw std::exception(__FUN(std::string("error: cannot create PdfDoc object")));
	}

	if(setjmp(env))
	{
		HPDF_Free (pdf);
		return false;	// throw std::exception(__FUN(std::string("error: cannot setjmp PdfDoc")));
	}

	// create default-font

	HPDF_UseUTFEncodings(pdf);
	HPDF_SetCurrentEncoder(pdf, "UTF-8");
	char* detail_font_name = (char*)HPDF_LoadTTFontFromFile (pdf, "arial.ttf", HPDF_TRUE);

	//HPDF_SetCurrentEncoder(pdf, "CP1251");

	font = HPDF_GetFont(pdf, detail_font_name, "UTF-8");


	page = HPDF_AddPage(pdf);

	HPDF_Page_SetFontAndSize (page, font, 10);

	//tw = HPDF_Page_TextWidth (page, page_title);
	HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);


	//21x29.7 ��
	//coeff = W / H;

	//HPDF_Page_SetWidth (page, H);
	//HPDF_Page_SetHeight (page, W);

	//HPDF_Page_SetWidth (index_page, 300);
	//HPDF_Page_SetHeight (index_page, 220);

	Height = HPDF_Page_GetHeight (page);
	Width = HPDF_Page_GetWidth(page);
	coeff = Width / Height;

	HPDF_Page_SetLineWidth(page, 2); //������ ����� 0.0 - 1.0
	HPDF_Page_Rectangle(page, 10, 10, Width - 20, Height - 20);
	HPDF_Page_Stroke(page);


	HPDF_Page_SetLineWidth(page, 0.5); //������ ����� 0.0 - 1.0
	//HPDF_Page_SetRGBStroke (page, 0, 0, 0);
	//HPDF_Page_SetRGBFill (page, 0.75, 0.0, 0.0);

	return true;
}

//���������� ����� PDF
void PdfClass::SavePDF()
{
	std::stringstream temp;
	try
	{
		temp << lpLogPdf;
		CheckDir(temp.str());
	}CATCH(AllLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

	try
	{
		temp << "/" << Sheet.Year;
		CheckDir(temp.str());
	}CATCH(AllLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

	try
	{
		temp << "/" << MonthName[Sheet.Month];
		CheckDir(temp.str());
	}CATCH(AllLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " " + Sheet.Month +" ");

	try
	{
		temp << "/" << std::setw(2) << std::setfill('0') << std::right << Sheet.Day;
		CheckDir(temp.str());
	}CATCH(AllLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

	try
	{
		temp << "/" << std::setw(4) << std::setfill('0') << std::right << Cassette.Year << "-";
		temp << std::setw(2) << std::setfill('0') << std::right << Cassette.Month << "-";
		temp << std::setw(2) << std::setfill('0') << std::right << Cassette.Day << "-";
		temp << std::setw(2) << std::setfill('0') << std::right << Cassette.CassetteNo;
		CheckDir(temp.str());
	}CATCH(AllLogger, FUNCTION_LINE_NAME + " File1: " + temp.str() + " ");

	try
	{
		temp << "/";
		temp << std::setw(6) << std::setfill('0') << Sheet.Melt << "-";
		temp << std::setw(3) << std::setfill('0') << Sheet.Slab << "-";
		temp << std::setw(3) << std::setfill('0') << Sheet.PartNo << "-";
		temp << std::setw(3) << std::setfill('0') << Sheet.Pack << "-";
		temp << std::setw(3) << std::setfill('0') << Sheet.Sheet << "-";
		temp << std::setw(2) << std::setfill('0') << Sheet.SubSheet << ".pdf";
	}CATCH(AllLogger, FUNCTION_LINE_NAME + " File1: " + temp.str() + " ");

		try
		{
			FileName = temp.str();
			HPDF_SaveToFile (pdf, FileName.c_str());
			HPDF_Free (pdf);
		}CATCH(AllLogger, FUNCTION_LINE_NAME + " File1: " + FileName + " ");
}

//������ �������
void PdfClass::DrawKpvlPDF()
{
	try
	{
		DrawKpvl(410, Height - 120, Width - 432);

		//������ ����������� �������
		draw_text(page, 20, Height - 100, "�������");
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
	}CATCH(AllLogger, FUNCTION_LINE_NAME);

}

//������ ������
void PdfClass::DrawFurnPDF()
{
	try
	{
 //������ ����������� �������
		DrawFurn(410, Height - 290, Width - 432);
		draw_text(page, 20, Height - 270, "������");
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
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
}

PdfClass::PdfClass(TSheet& sheet, bool view)
{
	try
	{
#pragma region ������� �������
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
			//MessageBox(GlobalWindow, "���� ��� ����� �� �������", "������", MB_OK | MB_ICONWARNING | MB_APPLMODAL);
			LOG_INFO(AllLogger, "{:90}| ���� {:06}-{:03}-{:03}-{:03}-{:03}/{:03}, ������ {:04}-{:02}-{:02}-{:02} ��� �� ���� � ��������� ����",
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
			//������ ��������� ����
			if(P == 1)
			{
				GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, ForBase_RelFurn_1.TempRef->ID);
				GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, ForBase_RelFurn_1.TempAct->ID);
			}

			//������ ��������� ����
			if(P == 2)
			{
				GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, ForBase_RelFurn_2.TempRef->ID);
				GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, ForBase_RelFurn_2.TempAct->ID);
			}
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
				//T_SqlTemp TempFurnAct ={};	//��������� ����������
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

		//������ ������ FURN
		PaintGraff(FurnAct, FurnRef, furnImage);

		//�������
		GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1->ID);
		SqlTempActKPVL(TempAct);

		//������ ������ KPVL
		PaintGraff(TempAct, TempRef, tempImage);


#pragma endregion

	//�������� PFD �����
		if(NewPdf())
		{
			//������ PDF ���������
			DrawHeder(0, Height);

			//������ PDF �������
			DrawKpvlPDF();

			//������ PDF ������
			DrawFurnPDF();

			//��������� PDF
			SavePDF();

			//����������� PDF
			if(view) std::system(("start " + FileName).c_str());
		}
		remove(furnImage);
		remove(tempImage);
	}CATCH(AllLogger, FUNCTION_LINE_NAME);

};

PdfClass::PdfClass(TCassette& TC)
{
	try
	{
#pragma region ������� �������
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
		//������ ��������� ����
			if(P == 1)
			{
				GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, ForBase_RelFurn_1.TempRef->ID);
				GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, ForBase_RelFurn_1.TempAct->ID);
			}

			//������ ��������� ����
			if(P == 2)
			{
				GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, ForBase_RelFurn_2.TempRef->ID);
				GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, ForBase_RelFurn_2.TempAct->ID);
			}
		}
		//������ ������ FURN
		PaintGraff(FurnAct, FurnRef, furnImage);

		for(auto& a : AllPfdSheet)
		{
			Sheet = a;
			//�������
			TempRef.erase(TempRef.begin(), TempRef.end());
			GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1->ID);

			//TempAct.erase(TempAct.begin(), TempAct.end());
			SqlTempActKPVL(TempAct);

			//������ ������ KPVL
			PaintGraff(TempAct, TempRef, tempImage);

			//������ PDF ������

#pragma endregion

		//�������� PFD �����
			if(NewPdf())
			{
				//������ PDF ���������
				DrawHeder(0, Height);

				//������ PDF �������
				DrawKpvlPDF();

				//������ PDF ������
				DrawFurnPDF();

				//��������� PDF
				SavePDF();
			}
		}
		remove(furnImage);
		remove(tempImage);
	}CATCH(AllLogger, FUNCTION_LINE_NAME);
};


//����������� �� ����� �� ����
void PrintPdf(TSheet& Sheet)
{
	try
	{
		PdfClass* pdf  = new PdfClass(Sheet);
	}
	CATCH(AllLogger, std::string("PrintPdf: "));
}

//�������������� �������� �� ������
void PrintPdfAuto(TSheet& Sheet, bool view)
{
	try
	{
		PdfClass pdf(Sheet, view);
	}
	CATCH(AllLogger, std::string("PrintPdf: "));
}

//�������������� �������� �� �������
void PrintPdfAuto(TCassette& TC)
{
	try
	{
		if(TC.Run_at.length() && TC.Finish_at.length())
			PdfClass pdf(TC);
		//else
		//	LOG_INFO(AllLogger, "{:90}| ERROR: TC.Run_at.length()={}, TC.Finish_at.length()={}", FUNCTION_LINE_NAME, TC.Run_at.length(), TC.Finish_at.length())
	}
	CATCH(AllLogger, FUNCTION_LINE_NAME);
}

