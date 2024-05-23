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
	Graff(std::string n):Name(n){};

	void DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat&);
	void DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);

	void Grid(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);
	void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st);
	void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);

	void DrawTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG);
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
