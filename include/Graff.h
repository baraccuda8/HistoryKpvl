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
	T_SqlTemp TempRef ={};
	T_SqlTemp TempAct ={};
	std::string Name;
	//std::map<std::string, float>Temper;
	PGConnection* conn = NULL;
	Graff(std::string n):Name(n){};
	void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st, int64_t mind, int64_t maxd, double mint, double maxt);
	void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);

	void DrawTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG);
	void Paint(HWND hWnd);

	//void SqlTemp(T_SqlTemp& st, Value* val);
	//void GetGrTemp(T_ForBase_RelFurn& app);

	void DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat&);
};

void InitGrafWindow(HWND hWnd);
void Open_GRAFF_FURN(TCassette& p);
void StopGraff();


//extern Graff GraffKPVL;
//extern Graff GraffFurn1;
//extern Graff GraffFurn2;
