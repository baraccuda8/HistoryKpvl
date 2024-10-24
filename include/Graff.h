#pragma once

//#include "ValueTag.h"
#include <gdiplusimagecodec.h>
#include "GdiPlusInit.h"
#include "sql.h"

typedef std::map<std::string, std::pair<int64_t, float>> T_SqlTemp;

class Graff{
public:
	HWND gHwnd;
	HWND tHwnd;
	HWND bHwnd;
	std::string AddTime = "";
	std::string StartKPVL = "";
	std::string StopKPVL = "";

	bool full = false;
	int64_t MaxSecCount = 0LL;
	T_SqlTemp TempRef ={}; 	//�������
	T_SqlTemp TempAct ={};	//����������
	std::string Name;

	Gdiplus::StringFormat stringFormat;

	Gdiplus::Pen pen = Gdiplus::Pen(Gdiplus::Color(255, 0, 0), 1);
	Gdiplus::SolidBrush Gdi_brush = Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
	Gdiplus::Pen Gdi_L1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 0.5); //������


	float f_mint = 0.0;
	float f_maxt = 0.0;

	float mint = 0.0;
	float maxt = 0.0;
	float cstep = 0.0;
	float fstep = 0.0;
	int64_t mind = 0LL;
	int64_t maxd = 0LL;
	//std::map<std::string, float>Temper;
	PGConnection* conn = NULL;
	Graff(std::string n);

	void DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat&);

	void DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);
	void DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG);
	void DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);

	void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st);
	void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);

	void DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, double sd, std::wstring sDataBeg);
	void DrawGraf(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG);
	void Paint(HWND hWnd);

};

void InitGrafWindow(HWND hWnd);
void Open_GRAFF_FURN(TCassette& p);
void StopGraff();
