#pragma once

//#include "ValueTag.h"
#include <gdiplusimagecodec.h>
#include "GdiPlusInit.h"
#include "sql.h"

typedef std::map<std::string, std::pair<int64_t, float>> T_SqlTemp;

class Graff{
public:
	HWND gHwnd;
	bool full = false;
	int64_t MaxSecCount = 0;
	T_SqlTemp TempRef ={}; 	//Задание
	T_SqlTemp TempAct ={};	//Актуальное
	std::string Name;

	Gdiplus::StringFormat stringFormat;

	Gdiplus::Pen pen = Gdiplus::Pen(Gdiplus::Color(255, 0, 0), 1);
	Gdiplus::SolidBrush Gdi_brush = Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
	Gdiplus::Pen Gdi_L1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 0.5); //Черный


	double f_mint = 0.0;
	double f_maxt = 0.0;

	double mint = 0.0;
	double maxt = 0.0;
	double cstep = 0.0;
	double fstep = 0.0;
	int64_t mind = 0;
	int64_t maxd = 0;
	//std::map<std::string, float>Temper;
	PGConnection* conn = NULL;
	Graff(std::string n):Name(n)
	{
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	};

	void DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat&);

	void DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);
	void DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG);
	void DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);

	void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st);
	void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);

	void DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, double sd, std::wstring sDataBeg);
	void DrawGraf(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG);
	void Paint(HWND hWnd);

	//void SqlTemp(T_SqlTemp& st, Value* val);
	//void GetGrTemp(T_ForBase_RelFurn& app);

};

void InitGrafWindow(HWND hWnd);
void Open_GRAFF_FURN(TCassette& p);
void StopGraff();


//extern Graff GraffKPVL;
//extern Graff GraffFurn1;
//extern Graff GraffFurn2;
