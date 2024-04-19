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

extern std::string lpLogPdf;
extern std::string  FORMATTIME;
extern Gdiplus::Font font1;
extern Gdiplus::Font font2;

extern GUID guidBmp;
extern GUID guidJpeg;
extern GUID guidGif;
extern GUID guidTiff;
extern GUID guidPng;

std::string tempImage = "temp.jpg";
std::string furnImage = "furn.jpg";

std::map <int, std::string> MonthName{
	{0, "Unknown"},
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
	//printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
	LOG_ERROR(AllLogger, "{:90}| ERROR: error_no={}, detail_no={}", FUNCTION_LINE_NAME, (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
	longjmp(env, 1);
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
	std::string fname = "test.pdf";
	HPDF_REAL Height = 0;
	HPDF_REAL Width = 0;
	HPDF_REAL coeff = 0;
	float SrTemp = 0;
	std::string strSrTemp = "";

	PGConnection conn;
	TSheet Sheet;
	TCassette сassette;

	PdfClass(TSheet& sheet, bool view = true);
	~PdfClass()
	{
		//conn.PGDisConnection();
	};

	int MaxSecCount = 0;
	const int XP = 70;
	const int YP = 18;

	T_SqlTemp FurnRef ={};	//Задание
	T_SqlTemp FurnAct ={};	//Актуальное

	T_SqlTemp TempRef ={};	//Задание
	T_SqlTemp TempAct ={};	//Актуальное

	void GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, Value* val);
	void SqlTempActKPVL(T_SqlTemp& tr);

	void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st, int64_t mind, int64_t maxd, double mint, double maxt);;
	void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);
	void DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);
	void PaintGraffKPVL();
	void DrawHeder(HPDF_REAL left, HPDF_REAL top);
	void DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
	void DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
	void GetCassete(TCassette& сassette);
};


void PdfClass::GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, Value* val)
{
	std::tm TM_Temp ={0};
	std::string sBegTime2 = Start;
	std::stringstream sde;
	sde << "SELECT max(create_at) FROM todos WHERE id_name = " << val->ID;
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
	sdt << "SELECT create_at, content FROM todos WHERE id_name = " << val->ID;
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
}

void PdfClass::SqlTempActKPVL(T_SqlTemp& tr)
{
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
							if(SrTemp == 0.0f)SrTemp = f;
							else SrTemp = (SrTemp + f) / 2.0f;

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

	std::ostringstream oss;
	oss << std::setprecision(1) << std::fixed << SrTemp;
	strSrTemp = oss.str();
}

void PdfClass::DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st, int64_t mind, int64_t maxd, double mint, double maxt)
{
	Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 0.5); //Черный
	Gdiplus::Pen Gdi_L2(clor, 1);
	auto b = st.begin();
	auto e = st.end();
	e--;
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
	p1.Y = Rect.Y + float((maxt - b->second.second) * coeffH);

	for(auto& a : st)
	{
		p2.X =  Rect.X + float((a.second.first - mind) * coeffW);
		p2.Y =  Rect.Y + float((maxt - a.second.second) * coeffH);
		temp.DrawLine(&Gdi_L2, p1, p2);

		p1.X = p2.X;
		p1.Y = p2.Y;
	}
}

void PdfClass::DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
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

}

void PdfClass::DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat)
{
	Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));

	std::wstring::const_iterator start = str.begin();
	std::wstring::const_iterator end = str.end();
	boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}:\\d{1,2}).*");
	boost::match_results<std::wstring::const_iterator> what;

	if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 1)
		temp.DrawString(what[1].str().c_str(), -1, &font1, Rect, &stringFormat, &Gdi_brush);
}


void PdfClass::PaintGraffKPVL()
{
	Gdiplus::REAL Width = 525;//Gdiplus::REAL(abs(rcBounds.right - rcBounds.left));
	Gdiplus::REAL Height = 205; // Gdiplus::REAL(abs(rcBounds.bottom - rcBounds.top));
	HDC hdc = GetDC(Global0);
	Gdiplus::Graphics g(hdc);
	Gdiplus::Bitmap backBuffer (INT(Width), INT(Height), &g);
	Gdiplus::Graphics temp(&backBuffer);
	Gdiplus::RectF RectG(0, 0, Width, Height);
	//Gdiplus::RectF RectBottom(0, 0, Width, Height);
	temp.Clear(Gdiplus::Color(255, 255, 255));

	Gdiplus::Pen Gdi_Bar(Gdiplus::Color(0, 0, 0), 1);
	if(!TempAct.size()) return;
	if(!TempRef.size()) return;
	
	double maxt = 0;
	double mint = 2000;
	int64_t mind = (std::min)(TempAct.begin()->second.first, TempRef.begin()->second.first);

	auto b = TempAct.begin();
	auto e = TempAct.end();
	e--;
	int64_t maxcount = 0;
	int64_t maxd = (std::max)(maxcount, e->second.first - b->second.first);
	//int64_t maxd = 0;// e->second.first - b->second.first;

	b = TempRef.begin();
	e = TempRef.end();
	e--;
	maxd = (std::max)(maxd, e->second.first - b->second.first);


	for(auto a : TempRef)
	{
		maxt = std::fmaxl(maxt, a.second.second);
		mint = std::fminl(mint, a.second.second);
	}
	for(auto a : TempAct)
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

	DrawBottom(temp, RectG2, Red, TempRef, mind, maxd, mint, maxt);	//Красный; Заданное значение температуры
	DrawBottom(temp, RectG2, Blue, TempAct, mind, maxd, mint, maxt);	//Синий; Фактическое значение температуры

	Gdiplus::RectF RectG3(RectG);
	RectG3.X = 100;
	RectG3.Y = RectG.Height - 15;
	DrawInfo(temp, RectG3);

	b = TempAct.begin();
	e = TempAct.end();
	e--;
	std::wstring sDataBeg(b->first.begin(), b->first.end());
	std::wstring sDataEnd(e->first.begin(), e->first.end());

	Gdiplus::RectF RectText(RectG);
	Gdiplus::StringFormat stringFormat;
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);

	stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	DrawTime(temp, RectText, sDataBeg, stringFormat);

	stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
	DrawTime(temp, RectText, sDataEnd, stringFormat);


	std::wstring SaveFile(tempImage.begin(), tempImage.end());

	backBuffer.Save(SaveFile.c_str(), &guidJpeg, NULL);

	//backBuffer.
	//g.DrawImage(&backBuffer, RectG);
	DeleteDC(hdc);
}

void PdfClass::DrawHeder(HPDF_REAL left, HPDF_REAL top)
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

}

void PdfClass::DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
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

}

void PdfClass::GetCassete(TCassette& сassette)
{
	
	if(Sheet.Year.empty() /*|| !p.Year.length()*/) return;
	if(Sheet.Month.empty() /*|| !p.Month.length()*/) return;
	if(Sheet.Day.empty() /*|| !p.Day.length()*/) return;
	if(Sheet.CassetteNo.empty() /*|| !p.CassetteNo.length()*/) return;

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


		int line = conn.PQntuples(res);
		//for(int l = 0; l < line; l++)
		if(line)
		{
			//TCassette cassette;
			S107::GetCassette(res, сassette, 0);
		}
	}
	else
		LOG_ERR_SQL(SQLLogger, res, comand);
	PQclear(res);

}

void PdfClass::DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
{
	std::string ss = сassette.Run_at;
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
	draw_text_rect (page, left + 270, top - 45, XP, YP, сassette.PointTime_1);			//Задание
	draw_text_rect (page, left + 340, top - 45, XP, YP, "");							//Факт

	draw_text_rect (page, left + 0, top - 70, w, YP, "Время выдержки при заданной температуре, мин");
	draw_text_rect (page, left + 270, top - 70, XP, YP, сassette.PointDTime_2);			//Задание
	draw_text_rect (page, left + 340, top - 70, XP, YP, "");							//Факт

	draw_text_rect (page, left + 0, top - 95, w, YP, "Температура отпуска, °С");
	draw_text_rect (page, left + 270, top - 95, XP, YP, сassette.PointRef_1);			//Задание
	draw_text_rect (page, left + 340, top - 95, XP, YP, сassette.f_temper);				//Факт

}

PdfClass::PdfClass(TSheet& sheet, bool view)
{
	Sheet = sheet;

	if(!conn.connection())
		return;// throw std::exception(__FUN(std::string("Error SQL conn connection to GraffKPVL")));
	conn.Name = "test";

	GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1);
	SqlTempActKPVL(TempAct);


	GetCassete(сassette);
	int P = atoi(сassette.Peth.c_str());
	//while(P >= 10) P -= 10;

	if(P == 1)
	{
		GetTempRef(сassette.Run_at, сassette.End_at, FurnRef, ForBase_RelFurn_1.TempRef);
		GetTempRef(сassette.Run_at, сassette.End_at, FurnAct, ForBase_RelFurn_1.TempAct);
	}
	else if(P == 2)
	{
		GetTempRef(сassette.Run_at, сassette.End_at, FurnRef, ForBase_RelFurn_2.TempRef);
		GetTempRef(сassette.Run_at, сassette.End_at, FurnAct, ForBase_RelFurn_2.TempAct);
	}

	//Рисуем график
	PaintGraffKPVL();

#pragma region Создание PFD файла


	pdf = HPDF_New (error_handler, NULL);
	if(!pdf)
		return; //throw std::exception(__FUN(std::string("error: cannot create PdfDoc object")));

	if(setjmp(env))
	{
		HPDF_Free (pdf);
		return;// throw std::exception(__FUN(std::string("error: cannot setjmp PdfDoc")));
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


	//21x29.7 см
	//coeff = W / H;

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

	DrawHeder(0, Height);
	DrawKpvl(410, Height - 120, Width - 432);
	DrawFurn(410, Height - 290, Width - 432);



	//График температуры закалки
	draw_text(page, 20, Height - 100, "Закалка");
	HPDF_Page_Rectangle(page, 20, Height - 240, 374, 140);
	HPDF_Page_Stroke(page);
	HPDF_Image image1 = HPDF_LoadJpegImageFromFile(pdf, tempImage.c_str());
	HPDF_Page_DrawImage (page, image1, 22, Height - 239, 370, 137);

	//График температуры отпуска
	draw_text(page, 20, Height - 270, "Отпуск");
	HPDF_Page_Rectangle(page, 20, Height - 410, 374, 140);
	HPDF_Page_Stroke(page);
	HPDF_Image image2 = HPDF_LoadJpegImageFromFile(pdf, tempImage.c_str());
	HPDF_Page_DrawImage (page, image2, 22, Height - 409, 370, 137);


	/* save the document to a file */
	//326581-0402-007-158-000000
	std::stringstream temp;
	temp << lpLogPdf;

	CheckDir(temp.str());
	//temp.fill('0');
	//std::fill ('*');
	temp << std::string("/") << Sheet.Year;
	CheckDir(temp.str());
	temp << std::string("/") << MonthName[std::stoi(sheet.Month)];
	CheckDir(temp.str());
	temp << std::string("/") << std::setw(2) << std::setfill('0') << std::right << Sheet.Day;
	CheckDir(temp.str());

	std::stringstream fname;
	fname << temp.str() + "/";
	fname << std::setw(6) << std::setfill('0') << Sheet.Melt << "-";
	fname << std::setw(3) << std::setfill('0') << Sheet.Slab << "-";
	fname << std::setw(3) << std::setfill('0') << Sheet.Pack << "-";
	fname << std::setw(3) << std::setfill('0') << Sheet.PartNo << "-";
	fname << std::setw(3) << std::setfill('0') << Sheet.Sheet << "-";
	fname << std::setw(2) << std::setfill('0') << Sheet.SubSheet << ".pdf";


	HPDF_SaveToFile (pdf, fname.str().c_str());
	HPDF_Free (pdf);
#pragma endregion

	if(view)
	{
		if(
			!std::system(("start " + fname.str()).c_str())
		   )
		{
		}
	}
};


void PrintPdf(TSheet& Sheet)
{
	try
	{
//#ifdef TESTPFD
		PdfClass* pdf  = new PdfClass(Sheet);
//#endif
	}
	CATCH(AllLogger, std::string("PrintPdf: "));
}

void PrintPdfAuto(TSheet& Sheet, bool view)
{
	try
	{
//#ifdef TESTPFD
		PdfClass pdf(Sheet, view);
//#endif
	}
	CATCH(AllLogger, std::string("PrintPdf: "));
}

