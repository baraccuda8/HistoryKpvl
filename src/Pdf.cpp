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

	PdfClass(TSheet& sheet);

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
	std::string command = sde.str();
	PGresult* res = conn.PGexec(command);
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		if(PQntuples(res))
			sBegTime2 = conn.PGgetvalue(res, 0, 0);
	}
	else
	{
		LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
		LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
	}
	PQclear(res);


	std::stringstream sdt;
	sdt << "SELECT create_at, content FROM todos WHERE id_name = " << val->ID;
	if(sBegTime2.length())	sdt << " AND create_at >= '" << sBegTime2 << "'";
	if(Stop.length())	sdt << " AND create_at <= '" << Stop << "'";

	sdt << " ORDER BY create_at ASC;";

	command = sdt.str();
	res = conn.PGexec(command);
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
	PQclear(res);
}

void PdfClass::SqlTempActKPVL(T_SqlTemp& tr)
{
	std::string Start = Sheet.Start_at;
	std::string Stop = Sheet.DataTime_End;
	int t = 0;
	std::tm TM_Temp ={0};

	if(Stop.length() < 1)
		throw std::exception(__FUN(std::string("Error: Sheet.Sheet.DataTime_End")));
	if(Start.length() < 1)
		throw std::exception(__FUN(std::string("Error: Sheet.Start_at")));


	std::stringstream sdt;
	sdt << "SELECT create_at, content FROM todos WHERE (";
	
	sdt << "id_name = " << Hmi210_1.Htr_1->ID << " OR ";
	sdt << "id_name = " << Hmi210_1.Htr_2->ID << " OR ";
	sdt << "id_name = " << Hmi210_1.Htr_3->ID << " OR ";
	sdt << "id_name = " << Hmi210_1.Htr_4->ID << " OR ";
	sdt << "id_name = " << Hmi210_1.Htr2_1->ID << " OR ";
	sdt << "id_name = " << Hmi210_1.Htr2_2->ID << " OR ";
	sdt << "id_name = " << Hmi210_1.Htr2_3->ID << " OR ";
	sdt << "id_name = " << Hmi210_1.Htr2_4->ID << " ) ";
	
	sdt << " AND create_at >= '";
	sdt << Start;
	sdt << "' AND create_at <= '";
	sdt << Stop;
	sdt << "' ORDER BY create_at ASC ;";


	std::string command = sdt.str();
	PGresult* res = conn.PGexec(command);
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
	PQclear(res);

	std::ostringstream oss;
	oss << std::setprecision(1) << std::fixed << SrTemp;
	strSrTemp = oss.str();


	//float allsec = atof(Sheet.DataTime_All.c_str()) * 60.0f;
	//
	//
	//GraffKPVL.TempAct.erase(GraffKPVL.TempAct.begin(), GraffKPVL.TempAct.end()); //Синий; Фактическое значение температуры
	//
	//std::tm TM_End ={0};
	//std::tm TM_Beg ={0};
	//std::tm TM_Temp ={0};
	//std::string TecTime;
	//time_t tStop1 = time(NULL);
	//localtime_s(&TM_End, &tStop1);
	//time_t tStop = _mkgmtime(&TM_End);
	//time_t tStart = (time_t)difftime(tStop, GraffKPVL.MaxSecCount);
	//gmtime_s(&TM_Beg, &tStart);
	//
	//std::stringstream sdw;
	//sdw << boost::format("%|04|-") % (TM_End.tm_year + 1900);
	//sdw << boost::format("%|02|-") % (TM_End.tm_mon + 1);
	//sdw << boost::format("%|02| ") % TM_End.tm_mday;
	//sdw << boost::format("%|02|:") % TM_End.tm_hour;
	//sdw << boost::format("%|02|:") % TM_End.tm_min;
	//sdw << boost::format("%|02|") % TM_End.tm_sec;
	////sdw << "+05";
	//std::string sEndTime = sdw.str();
	//
	//sdw.str(std::string());
	//sdw << boost::format("%|04|-") % (TM_Beg.tm_year + 1900);
	//sdw << boost::format("%|02|-") % (TM_Beg.tm_mon + 1);
	//sdw << boost::format("%|02| ") % TM_Beg.tm_mday;
	//sdw << boost::format("%|02|:") % TM_Beg.tm_hour;
	//sdw << boost::format("%|02|:") % TM_Beg.tm_min;
	//sdw << boost::format("%|02|") % TM_Beg.tm_sec;
	////sdw << "+05";
	//std::string sBegTime = sdw.str();
	//
	////std::string sBegTime2 = SqlTempKPVL(sBegTime);
	//
	//std::stringstream sdt;
	//sdt << "SELECT create_at, content FROM todos WHERE (";
	//
	//sdt << "id_name = " << Hmi210_1.Htr_1->ID << " OR ";
	//sdt << "id_name = " << Hmi210_1.Htr_2->ID << " OR ";
	//sdt << "id_name = " << Hmi210_1.Htr_3->ID << " OR ";
	//sdt << "id_name = " << Hmi210_1.Htr_4->ID << " OR ";
	//sdt << "id_name = " << Hmi210_1.Htr2_1->ID << " OR ";
	//sdt << "id_name = " << Hmi210_1.Htr2_2->ID << " OR ";
	//sdt << "id_name = " << Hmi210_1.Htr2_3->ID << " OR ";
	//sdt << "id_name = " << Hmi210_1.Htr2_4->ID << " ) ";
	//
	//sdt << " AND create_at >= '";
	//sdt << sBegTime;
	//sdt << "' AND create_at <= '";
	//sdt << sEndTime;
	//sdt << "' ORDER BY create_at ASC ;";
	//
	//std::string command = sdt.str();
	//PGresult* res = GraffKPVL.conn->PGexec(command);
	////LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
	//if(PQresultStatus(res) == PGRES_TUPLES_OK)
	//{
	//	int line = PQntuples(res);
	//	if(line)
	//	{
	//		for(int l = 0; l < line; l++)
	//		{
	//			std::string sData = GraffKPVL.conn->PGgetvalue(res, l, 0);
	//			if(sBegTime <= sData)
	//			{
	//				auto a = GraffKPVL.TempAct.find(sData);
	//				std::string sTemp = GraffKPVL.conn->PGgetvalue(res, l, 1);
	//				float f =  atoi_t(float, atof, sTemp); //static_cast<float>(atof(sTemp.c_str()));
	//
	//				if(a != GraffKPVL.TempAct.end() && a._Ptr != NULL)
	//				{
	//					a->second.second = (f + a->second.second) / 2.0f;
	//				}
	//				else
	//				{
	//					if(f != 0)
	//					{
	//						DataTimeOfString(sData, FORMATTIME, TM_Temp.tm_year, TM_Temp.tm_mon, TM_Temp.tm_mday, TM_Temp.tm_hour, TM_Temp.tm_min, TM_Temp.tm_sec);
	//						TM_Temp.tm_year -= 1900;
	//						TM_Temp.tm_mon -= 1;
	//						GraffKPVL.TempAct[sData] = std::pair(mktime(&TM_Temp), f);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	//PQclear(res);
	//
	//
	//GraffKPVL.full = true;
	//InvalidateRect(GraffKPVL.gHwnd, NULL, false);


}

void PdfClass::DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st, int64_t mind, int64_t maxd, double mint, double maxt)
{
	Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 0.5); //Черный
	Gdiplus::Pen Gdi_L2(clor, 1);
	auto b = st.begin();
	auto e = st.end();
	e--;
	double coffW = (double)(Rect.Width) / double(maxd);
	double coffH = (double)(Rect.Height - Rect.Y) / (double)(maxt - mint);


	Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));
	Gdiplus::StringFormat stringFormat;
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);


	float mY = Rect.Y + float((maxt - maxt) * coffH);
	Gdiplus::PointF pt1 ={Rect.X - 5,				mY};
	Gdiplus::PointF pt2 ={Rect.X + Rect.Width + 5,	mY};
	temp.DrawLine(&Gdi_L1, pt1, pt2);
	Gdiplus::RectF Rect2 ={0, mY - 8, 50, 20};

	std::wstringstream sdw;
	sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << maxt;

	temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);

	float iY = Rect.Y + float((maxt - mint) * coffH);
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
	p1.Y = Rect.Y + float((maxt - b->second.second) * coffH);

	for(auto& a : st)
	{
		p2.X =  Rect.X + float((a.second.first - mind) * coffW);
		p2.Y =  Rect.Y + float((maxt - a.second.second) * coffH);
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

void GetCassete(TSheet& p, TCassette& сassette)
{
	
	if(!p.Year.length()) return;
	if(!p.Month.length()) return;
	if(!p.Day.length()) return;
	if(!p.CassetteNo.length()) return;

	std::string FilterComand1 = "SELECT * FROM cassette ";
	FilterComand1 += "WHERE ";
	FilterComand1 += "year = '" + p.Year + "' AND ";
	FilterComand1 += "month = '" + p.Month + "' AND ";
	FilterComand1 += "day = '" + p.Day + "' AND ";
	FilterComand1 += "cassetteno = '" + p.CassetteNo + "' ";
	FilterComand1 += "ORDER BY create_at DESC";
	FilterComand1 += ";";

	PGresult* res = conn_kpvl.PGexec(FilterComand1);
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		int line = PQntuples(res);
		int nFields = PQnfields(res);
		for(int l = 0; l < line; l++)
		{
			сassette.Create_at = GetStringData(conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::create_at));
			сassette.Id = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::id);
			сassette.Event = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::event);
			сassette.Day = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::day);
			сassette.Month = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::month);
			сassette.Year = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::year);
			сassette.CassetteNo = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::cassetteno);
			сassette.SheetInCassette = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::sheetincassette);
			сassette.Close_at = GetStringData(conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::close_at));
			сassette.Peth = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::peth);
			сassette.Run_at = GetStringData(conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::run_at));
			сassette.Error_at = GetStringData(conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::error_at));
			сassette.End_at = GetStringData(conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::end_at));
			сassette.Delete_at = GetStringData(conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::delete_at));

			сassette.TempRef = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::tempref);
			сassette.PointTime_1 = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::pointtime_1);
			сassette.PointRef_1 = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::pointref_1);
			сassette.TimeProcSet = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::timeprocset);
			сassette.PointDTime_2 = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::pointdtime_2);
			сassette.f_temper = conn_kpvl.PGgetvalue(res, l, casCassette::emCassette::f_temper);

		}
	}
	else
	{
		std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
	}
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

PdfClass::PdfClass(TSheet& sheet)
{
	Sheet = sheet;

	if(!conn.connection())
		throw std::exception(__FUN(std::string("Error SQL conn_temp connection to GraffKPVL")));

	GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1);
	SqlTempActKPVL(TempAct);

	
	GetCassete(Sheet, сassette);
	int P = atoi(сassette.Peth.c_str());
	while(P >= 10) P -= 10;

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
		throw std::exception(__FUN(std::string("error: cannot create PdfDoc object")));

	if(setjmp(env))
	{
		HPDF_Free (pdf);
		throw std::exception(__FUN(std::string("error: cannot setjmp PdfDoc")));
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
	HPDF_Image image = HPDF_LoadJpegImageFromFile(pdf, tempImage.c_str());
	HPDF_Page_DrawImage (page, image, 22, Height - 239, 370, 137);

	//График температуры отпуска
	draw_text(page, 20, Height - 270, "Отпуск");
	HPDF_Page_Rectangle(page, 20, Height - 410, 374, 140);
	HPDF_Page_Stroke(page);


	/* save the document to a file */
	HPDF_SaveToFile (pdf, (lpLogPdf+fname).c_str());
	/* clean up */
	HPDF_Free (pdf);
#pragma endregion


	if(!std::system((lpLogPdf + fname).c_str()))
		throw std::exception(__FUN(std::string("std::system: fname") + lpLogPdf + "\\" + fname));
};


void PrintPdf(TSheet& Sheet)
{
	try
	{
//#ifdef TESTPFD
		PdfClass pdf (Sheet);
//#endif
	}
	CATCH(AllLogger, std::string("PrintPdf: "));
}
