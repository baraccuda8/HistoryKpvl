#pragma once
#include "hpdf.h"

namespace PDF
{
	void TodosColumn(PGresult* res);

	OpcUa::Variant GetVarVariant(OpcUa::VariantType Type, std::string value);

	bool cmpData(T_Todos& first, T_Todos& second);

	void GetTodosSQL(PGConnection& conn, MapTodos& mt, std::string& command, int p = 0);

	namespace PASSPORT
	{

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

			T_SqlTemp FurnRef = {};	//Çàäàíèå
			T_SqlTemp FurnAct = {};	//Àêòóàëüíîå

			T_SqlTemp TempRef = {};	//Çàäàíèå
			T_SqlTemp TempAct = {};	//Àêòóàëüíîå

			float f_mint = 0.0f;
			float f_maxt = 0.0f;
			float f_step = 0.0f;

			int64_t mind = 0LL;
			int64_t maxd = 0LL;

			Gdiplus::StringFormat stringFormat;

			Gdiplus::Pen pen = Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0));
			Gdiplus::SolidBrush Gdi_brush = Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
			Gdiplus::Pen Gdi_L1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 1); //×åðíûé
			Gdiplus::Pen Gdi_D1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 2); //×åðíûé
			Gdiplus::Pen Gdi_R1 = Gdiplus::Pen(Gdiplus::Color(192, 0, 0), 0.5); //×åðíûé

			~PdfClass(){};

			void GetCassette(TCassette& cassette, TSheet& Sheet);

			void GetSheet(TCassette& Cassette);

			float GetStartTime(std::string start, int ID);


			void GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID);


			void SqlTempActKPVL1(std::string Stop, T_fTemp& fT);

			void SqlTempActKPVL2(std::string Stop, T_fTemp& fT);

			void SqlTempActKPVL1(std::string Start, std::string Stop, VPFS& pF1, int Temperature);

			void SqlTempActKPVL2(std::string Start, std::string Stop, VPFS& pF2, int Temperature);

			void GetSrTemper(std::vector<PFS>& pF, std::map<int, PFS>& mF);

			void UpdateTemperature(TSheet& Sheet);


			void SqlTempActKPVL(T_SqlTemp& tr, TSheet& Sheet);


			void DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);

			void DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, float sd, std::wstring sDataBeg);

			void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st);

			void DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG, std::wstring s1, std::wstring s2);

			void DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act, T_SqlTemp& Ref);

			void DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, int msec);

			void GetMinMax(T_SqlTemp& Act, T_SqlTemp& Ref);

			void PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage, int msec, std::wstring s1, std::wstring s2);

			void DrawHeap(HPDF_REAL left, HPDF_REAL Y);

			HPDF_REAL DrawKpvl(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);

			HPDF_REAL DrawFurn(TCassette& Cassette, HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
		
			//Создание нового листа PDF
			bool NewPdf();
		
			//Сохранение листа PDF
			void SavePDF(TSheet& Sheet);


			HPDF_REAL DrawHeder(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top);

			//Рисуем Закалка
			HPDF_REAL DrawKpvlPDF(TSheet& Sheet, HPDF_REAL left, HPDF_REAL top);

			//Рисуем Отпуск
			HPDF_REAL DrawFurnPDF(TCassette& Cassette, HPDF_REAL left, HPDF_REAL top);

			PdfClass(TCassette& Cassette, bool end);


			PdfClass(TSheet& Sheet, bool end);

		};

	}
	
	//Поток автоматической корректировки
	DWORD WINAPI RunCassettelPdf(LPVOID);
}