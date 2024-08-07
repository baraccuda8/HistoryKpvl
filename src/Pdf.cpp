#include "pch.h"
#include <optional>
#include <setjmp.h>
//#include <codecvt>

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

//��� ���������� ������, �������� ����� � ������� ������ ��������� "correct IS NULL AND"
#define TESTPDF

////��� ���������� ������, �� ��������� � ����]
#define TESTPDF2


extern std::string lpLogPdf;
extern std::string lpLogPdf2;
//extern const std::string FORMATTIME;
extern Gdiplus::Font font1;
extern Gdiplus::Font font2;

extern GUID guidBmp;
extern GUID guidJpeg;
extern GUID guidGif;
extern GUID guidTiff;
extern GUID guidPng;

extern std::map<int, std::string> GenSeq1;
extern std::map<int, std::string> GenSeq2;
extern std::map<int, std::string> GenSeq3;

namespace PDF
{
	typedef struct T_IdSheet{
		int id = 0;
		std::string Start1 = "";		//����, ����� �������� ����� � ���������� ����
		std::string Start2 = "";		//����, ����� �������� ����� �� ������ ����
		std::string Start3 = "";		//����, ����� �������� ����� �� ������ ����
		std::string Start5 = "";		//����, ����� �������� ����� �� ����������

		std::string Nagrev = "";		//����, ����� ������ �������
		std::string CloseInDor = "";	//������� ������� �����
		
		std::string InCant = "";		//����, ����� �������� ����� �� ��������
		std::string Cant = "";			//����, ����� ��������
		std::string DataTime_End = "";	//����, ����� �������� ����� �� ���� 

		std::string TimeTest = "";		//��� ������
		std::string DataTime = "";		//����� �������

		int Melt = 0;					//������
		int Pack = 0;					//�����
		int PartNo = 0;					//������
		int Sheet = 0;					//����� �����
		int SubSheet = 0;				//����� ��������
		int Slab = 0;					//����

		std::string Alloy = "���";		//����� �����
		std::string Thikness = "���";	//������� �����, ��

		//float HeatTime_Z2 = 0.0f;		//����� ������� � ���� 2
		float TimeForPlateHeat = 0.0f;	//������� ����� ���������� ����� � ���������� ����. ���
		float DataTime_All = 0.0f;		//���� ����� ���������� ����� � ���������� ����. ���
		float PresToStartComp = 0.0f;	//������� �������� ����

		//��������� �� ������ ����� � ���� State_1 = 3;
		float Temper = 0.0f;			//������� �����������
		float Speed = 0.0f;		//�������� ��������
		float SpeedTopSet = 0.0f;		//������. ���������� ������. ����
		float SpeedBotSet = 0.0f;		//������. ���������� ������. ���
		float Lam1PosClapanTop = 0.0f;		//������. ���������� ������ 1. ����
		float Lam1PosClapanBot = 0.0f;		//������. ���������� ������ 1. ���
		float Lam2PosClapanTop = 0.0f;		//������. ���������� ������ 2. ����
		float Lam2PosClapanBot = 0.0f;		//������. ���������� ������ 2. ���
		uint16_t Valve_1x = 0;			//����� ������ ������� 1
		uint16_t Valve_2x = 0;			//����� ������ ������� 2
		std::string Mask1 = "000000000";//����� ������ ������� 1
		std::string Mask2 = "000000000";//����� ������ ������� 2
		std::string Mask = "";			//����� ������ �������
		//��������� �� ������ �� ���� State_2 = 5;
		float LAM_TE1 = 0.0f;			//����������� ���� � �������
		float Za_TE3 = 0.0f;			//����������� ���� � ����
		float Za_PT3 = 0.0f;			//�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)

		//��������� �� ������ �� ���� State_2 = 5 ���� 5 ������;
		float LaminPressTop = 0.0f;		//�������� � ������� ���������� 
		float LaminPressBot = 0.0f;		//�������� � ������ ����������

		int Pos = 0;

		int hour = 0;
		int day = 0;
		int month = 0;
		int year = 0;
		int cassetteno = 0;
		int sheetincassette = 0;
		int news = 0;
	}T_IdSheet;
	typedef std::vector<T_IdSheet>V_IdSheet;
	typedef struct T_fTemp{
		float t0 = 0;
		float t1 = 0;
		float t2 = 0;
		float t3 = 0;
		float t4 = 0;
	}T_fTemp;

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

	typedef struct PFS{
		int sec = 0;
		float temper = 0.f;
		std::string data = "";
		int count = 0;
		int oldStep = 0;
	}PFS;

	typedef std::vector<PFS> VPFS;

	class PdfClass{
	public:

		HPDF_Doc  pdf;
		HPDF_Font font;
		HPDF_Page page;
		HPDF_REAL Height = 0;
		HPDF_REAL Width = 0;
		HPDF_REAL coeff = 0;
		float SrTemp = 0.0f;
		std::string strSrTemp = "";
		std::string FileName;
		int StepSec = 5;


		PGConnection conn;
		TSheet Sheet;
		std::deque<TSheet>AllPfdSheet;
		TCassette Cassette;

		int64_t MaxSecCount = 0LL;
		const int XP = 70;
		const int YP = 18;

		T_SqlTemp FurnRef ={};	//�������
		T_SqlTemp FurnAct ={};	//����������

		T_SqlTemp TempRef ={};	//�������
		T_SqlTemp TempAct ={};	//����������

		float f_mint = 0.0f;
		float f_maxt = 0.0f;
		float fstep = 0.0f;

		float mint = 0.0f;
		float maxt = 0.0f;
		float cstep = 0.0f;
		int64_t mind = 0LL;
		int64_t maxd = 0LL;


		PdfClass(TCassette& TC);
		//CassettePdfClass(TSheet& sheet, bool view = true);
		//CassettePdfClass(T_IdSheet& sheet, bool view = false);
		~PdfClass()
		{
			//conn.PGDisConnection();
		};

		Gdiplus::StringFormat stringFormat;

		Gdiplus::Pen pen = Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0));
		Gdiplus::SolidBrush Gdi_brush = Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
		Gdiplus::Pen Gdi_L1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 1); //������
		Gdiplus::Pen Gdi_D1 = Gdiplus::Pen(Gdiplus::Color(192, 192, 192), 2); //������
		Gdiplus::Pen Gdi_R1 = Gdiplus::Pen(Gdiplus::Color(192, 0, 0), 0.5); //������

		void GetSrTemper(std::vector<PFS>& pF, std::map<int, PFS>& mF);
		void GetTempRef(std::string Start, std::string Stop, T_SqlTemp& tr, int ID);
		void SqlTempActKPVL1(std::string Stop, T_fTemp& fT);
		void SqlTempActKPVL1(std::string Start, std::string Stop, VPFS& pfs1);
		void SqlTempActKPVL2(std::string Stop, T_fTemp& fT);
		void SqlTempActKPVL2(std::string Start, std::string Stop, VPFS& pfs2);
		void SqlTempActKPVL3(std::string Start, std::string Stop, VPFS& pfs);
		void SqlTempActKPVL(T_SqlTemp& tr);

		void DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);
		void DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, float sd, std::wstring sDataBeg);
		void DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st);
		void DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect);
		void DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act, T_SqlTemp& Ref);
		void DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG, std::wstring s1, std::wstring s2);
		void DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, int msec);

		//void DrawTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat);
		void PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage, int msec, std::wstring s1, std::wstring s2);

		bool NewPdf();
		void SavePDF();
		HPDF_REAL DrawHeder(HPDF_REAL left, HPDF_REAL top);

		HPDF_REAL DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
		HPDF_REAL DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w);
		HPDF_REAL DrawKpvlPDF(HPDF_REAL left, HPDF_REAL top);

		HPDF_REAL DrawFurnPDF(HPDF_REAL left, HPDF_REAL top);

		void UpdateTemperature(T_SqlTemp& tr);
		void GetSheet();
		void GetCassette(TCassette& cassette);
	};

	void PdfClass::GetCassette(TCassette& cassette)
	{
		try
		{
			cassette = TCassette();
			if(Sheet.Year.empty() || !Sheet.Year.length()) return;
			if(Sheet.Month.empty() || !Sheet.Month.length()) return;
			if(Sheet.Day.empty() || !Sheet.Day.length()) return;
			if(Sheet.Hour.empty() || !Sheet.Hour.length()) return;
			if(Sheet.CassetteNo.empty() || !Sheet.CassetteNo.length()) return;

			std::string comand = "SELECT * FROM cassette ";
			comand += "WHERE ";
			comand += "year = " + Sheet.Year + " AND ";
			comand += "month = " + Sheet.Month + " AND ";
			comand += "day = " + Sheet.Day + " AND ";
			comand += "hour = " + Sheet.Hour + " AND ";
			comand += "cassetteno = " + Sheet.CassetteNo + " ";
			comand += "ORDER BY run_at DESC";
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
			sd << "hour = '" << Cassette.Hour << "' AND ";
			sd << "day = '" << Cassette.Day << "' AND ";
			sd << "month = '" << Cassette.Month << "' AND ";
			sd << "year = '" << Cassette.Year << "' AND ";
			sd << "cassetteno = " << Cassette.CassetteNo << " ";
			sd << "ORDER BY start_at DESC, pos DESC;";
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

					float f = static_cast<float>(atof(conn.PGgetvalue(res, 0, 1).c_str()));
					time_t t1 = DataTimeOfString(Start);
					tr[Start] = std::pair(0, f);


					for(int l = 0; l < line; l++)
					{
						std::string sData = conn.PGgetvalue(res, l, 0);

						if(Start <= sData)
						{
							std::string sTemp = conn.PGgetvalue(res, l, 1);
							time_t t2 = DataTimeOfString(sData);

							f = static_cast<float>(atof(sTemp.c_str()));
							if(f != 0)
								tr[sData] = std::pair(int(difftime(t2, t1)), f);
						}
					}


					time_t t3 = DataTimeOfString(Stop);
					tr[Stop] = std::pair(int(difftime(t3, t1)), f);
				}
			}
			else
				LOG_ERR_SQL(PdfLogger, res, comand);

			PQclear(res);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::SqlTempActKPVL1(std::string Stop, T_fTemp& fT)
	{
		std::stringstream sdt;
		sdt << "SELECT DISTINCT ON(id_name) id_name, create_at, content FROM todos WHERE (";

		sdt << "id_name = " << Hmi210_1.Htr2_1->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr2_2->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr2_3->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr2_4->ID;

		sdt << ") AND create_at <= '" << Stop;
		sdt << "' ORDER BY id_name DESC, id ASC;";
		std::string comand = sdt.str();
		if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
		PGresult* res = conn.PGexec(comand);

		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			for(int l = 0; l < line; l++)
			{
				PFS pfs;
				int id_name		= Stoi(conn.PGgetvalue(res, l, 0));
				pfs.data		= conn.PGgetvalue(res, l, 1);
				pfs.temper		= Stof(conn.PGgetvalue(res, l, 2));

				if(id_name == Hmi210_1.Htr2_1->ID) fT.t1 = pfs.temper; else
				if(id_name == Hmi210_1.Htr2_2->ID) fT.t2 = pfs.temper; else
				if(id_name == Hmi210_1.Htr2_3->ID) fT.t3 = pfs.temper; else
				if(id_name == Hmi210_1.Htr2_4->ID) fT.t4 = pfs.temper;
			}
		}
		else
			LOG_ERR_SQL(PdfLogger, res, comand);
		PQclear(res);
		fT.t0 = (fT.t1 + fT.t2 + fT.t3 + fT.t4) / 4.0f;
	}
	void PdfClass::SqlTempActKPVL2(std::string Stop, T_fTemp& fT)
	{
		std::stringstream sdt;
		sdt << "SELECT DISTINCT ON(id_name) id_name, create_at, content FROM todos WHERE (";

		sdt << "id_name = " << Hmi210_1.Htr1_1->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr1_2->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr1_3->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr1_4->ID;

		sdt << ") AND create_at <= '" << Stop;
		sdt << "' ORDER BY id_name DESC, id ASC;";
		std::string comand = sdt.str();
		if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
		PGresult* res = conn.PGexec(comand);

		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			for(int l = 0; l < line; l++)
			{
				PFS pfs;
				int id_name		= Stoi(conn.PGgetvalue(res, l, 0));
				pfs.data		= conn.PGgetvalue(res, l, 1);
				pfs.temper		= Stof(conn.PGgetvalue(res, l, 2));

				if(id_name == Hmi210_1.Htr1_1->ID) fT.t1 = pfs.temper; else
					if(id_name == Hmi210_1.Htr1_2->ID) fT.t2 = pfs.temper; else
						if(id_name == Hmi210_1.Htr1_3->ID) fT.t3 = pfs.temper; else
							if(id_name == Hmi210_1.Htr1_4->ID) fT.t4 = pfs.temper;
			}
		}
		else
			LOG_ERR_SQL(PdfLogger, res, comand);
		PQclear(res);
		fT.t0 = (fT.t1 + fT.t2 + fT.t3 + fT.t4) / 4.0f;
	}


	void PdfClass::SqlTempActKPVL1(std::string Start, std::string Stop, VPFS& pF1)
	{
		T_fTemp fT;
		SqlTempActKPVL1(Start, fT);
		PFS pfs;
		pfs.data = Start;

		pfs.temper = fT.t1;
		pF1.push_back(pfs);
		pfs.temper = fT.t2;
		pF1.push_back(pfs);
		pfs.temper = fT.t3;
		pF1.push_back(pfs);
		pfs.temper = fT.t4;
		pF1.push_back(pfs);

		std::stringstream sdt;
		sdt << "SELECT id_name, create_at, content FROM todos WHERE (";
		sdt << "id_name = " << Hmi210_1.Htr2_1->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr2_2->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr2_3->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr2_4->ID;
		sdt << ") AND create_at >= '" << Start;
		sdt << "' AND create_at < '" << Stop;
		sdt << "' ORDER BY id ASC;";

		std::string comand = sdt.str();
		if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
		PGresult* res = conn.PGexec(comand);

		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			if(line)
			{
				if(!Stoi(Sheet.Temperature)) SrTemp = 0.0f;

				for(int l = 0; l < line; l++)
				{
					PFS pfs;
					int id_name		= Stoi(conn.PGgetvalue(res, l, 0));
					pfs.data		= conn.PGgetvalue(res, l, 1);
					pfs.temper		= Stof(conn.PGgetvalue(res, l, 2));

						 if(id_name == Hmi210_1.Htr2_1->ID) pF1.push_back(pfs);
					else if(id_name == Hmi210_1.Htr2_2->ID) pF1.push_back(pfs); 
					else if(id_name == Hmi210_1.Htr2_3->ID) pF1.push_back(pfs); 
					else if(id_name == Hmi210_1.Htr2_4->ID) pF1.push_back(pfs);
				}
			}
		}
		else
			LOG_ERR_SQL(PdfLogger, res, comand);
		PQclear(res);

	}

	void PdfClass::SqlTempActKPVL2(std::string Start, std::string Stop, VPFS& pF2)
	{
		T_fTemp fT;
		SqlTempActKPVL2(Start, fT);
		PFS pfs;
		pfs.data = Start;

		pfs.temper = fT.t1;
		pF2.push_back(pfs);
		pfs.temper = fT.t2;
		pF2.push_back(pfs);
		pfs.temper = fT.t3;
		pF2.push_back(pfs);
		pfs.temper = fT.t4;
		pF2.push_back(pfs);

		std::stringstream sdt;
		sdt << "SELECT id_name, create_at, content FROM todos WHERE (";
		sdt << "id_name = " << Hmi210_1.Htr1_1->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr1_2->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr1_3->ID << " OR ";
		sdt << "id_name = " << Hmi210_1.Htr1_4->ID;
		sdt << ") AND create_at >= '" << Start;
		sdt << "' AND create_at < '" << Stop;
		sdt << "' ORDER BY id ASC;";


		std::string comand = sdt.str();
		if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
		PGresult* res = conn.PGexec(comand);

		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			if(line)
			{
				if(!Stoi(Sheet.Temperature)) SrTemp = 0.0f;


				PFS vpfs[4];
				for(int l = 0; l < line; l++)
				{
					PFS pfs;
					int id_name		= Stoi(conn.PGgetvalue(res, l, 0));
					pfs.data		= conn.PGgetvalue(res, l, 1);
					pfs.temper		= Stof(conn.PGgetvalue(res, l, 2));

					if(id_name == Hmi210_1.Htr1_1->ID) { vpfs[0] = pfs; pF2.push_back(pfs); }
					else if(id_name == Hmi210_1.Htr1_2->ID) { vpfs[1] = pfs; pF2.push_back(pfs); }
					else if(id_name == Hmi210_1.Htr1_3->ID) { vpfs[2] = pfs; pF2.push_back(pfs); }
					else if(id_name == Hmi210_1.Htr1_4->ID) { vpfs[3] = pfs; pF2.push_back(pfs); }

				}
				vpfs[0].data = Stop;
				vpfs[1].data = Stop;
				vpfs[2].data = Stop;
				vpfs[3].data = Stop;
				pF2.push_back(vpfs[0]);
				pF2.push_back(vpfs[1]);
				pF2.push_back(vpfs[2]);
				pF2.push_back(vpfs[3]);
			}
		}
		else
			LOG_ERR_SQL(PdfLogger, res, comand);
		PQclear(res);

	}

	void PdfClass::SqlTempActKPVL3(std::string Start, std::string Stop, VPFS& pF)
	{
		//std::stringstream sdt;
		//sdt << "SELECT id_name, create_at, content FROM todos WHERE (";
		//
		//sdt << "id_name = " << Hmi210_1.Htr1_1->ID << " OR ";
		//sdt << "id_name = " << Hmi210_1.Htr1_2->ID << " OR ";
		//sdt << "id_name = " << Hmi210_1.Htr1_3->ID << " OR ";
		//sdt << "id_name = " << Hmi210_1.Htr1_4->ID << " OR ";
		//sdt << "id_name = " << Hmi210_1.Htr2_1->ID << " OR ";
		//sdt << "id_name = " << Hmi210_1.Htr2_2->ID << " OR ";
		//sdt << "id_name = " << Hmi210_1.Htr2_3->ID << " OR ";
		//sdt << "id_name = " << Hmi210_1.Htr2_4->ID;
		//
		//sdt << ")  AND create_at >= '" << Start;
		//sdt << "' AND create_at <= '" << Stop;
		//sdt << "' ORDER BY id ASC;";
		//
		//
		//std::string comand = sdt.str();
		//if(DEB)LOG_INFO(PdfLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
		//PGresult* res = conn.PGexec(comand);
		//
		//if(PQresultStatus(res) == PGRES_TUPLES_OK)
		//{
		//	int line = PQntuples(res);
		//	if(line)
		//	{
		//		if(!Stoi(Sheet.Temperature)) SrTemp = 0.0f;
		//
		//		for(int l = 0; l < line; l++)
		//		{
		//			PFS pfs;
		//			int id_name		= Stoi(conn.PGgetvalue(res, l, 0));
		//			pfs.data		= conn.PGgetvalue(res, l, 1);
		//			pfs.temper		= Stof(conn.PGgetvalue(res, l, 2));
		//
		//			if(id_name == Hmi210_1.Htr1_1->ID) pF.push_back(pfs); else
		//			if(id_name == Hmi210_1.Htr1_2->ID) pF.push_back(pfs); else
		//			if(id_name == Hmi210_1.Htr1_2->ID) pF.push_back(pfs); else
		//			if(id_name == Hmi210_1.Htr1_3->ID) pF.push_back(pfs); else
		//			if(id_name == Hmi210_1.Htr1_4->ID) pF.push_back(pfs); else
		//			if(id_name == Hmi210_1.Htr2_1->ID) pF.push_back(pfs); else
		//			if(id_name == Hmi210_1.Htr2_2->ID) pF.push_back(pfs); else
		//			if(id_name == Hmi210_1.Htr2_3->ID) pF.push_back(pfs); else
		//			if(id_name == Hmi210_1.Htr2_4->ID) pF.push_back(pfs);
		//		}
		//	}
		//}
		//else
		//	LOG_ERR_SQL(PdfLogger, res, comand);
		//PQclear(res);

	}

	void PdfClass::GetSrTemper(std::vector<PFS>& pF, std::map<int, PFS>& mF)
	{
		std::tm TM;
		if(pF.size())
		{
			time_t tS1 = DataTimeOfString(pF.begin()->data, TM);

			int oldStep = 0;
			for(auto& a : pF)
			{
				time_t tS = DataTimeOfString(a.data, TM);
				a.sec = int(difftime(tS, tS1));

				int st = a.sec / StepSec;
				if(oldStep == st)
				{
					if(a.temper > 0)
					{
						mF[st].temper += a.temper;
						mF[st].data = a.data;
						mF[st].count++;
					}
				}
				else
				{
					if(a.temper > 0)
					{
						oldStep = st;
						mF[st].temper += a.temper;
						mF[st].data = a.data;
						mF[st].count = 1;
					}
				}
			}
			for(auto& a : mF)
				if(a.second.count)
					a.second.temper /= a.second.count;

		}
	}

	void PdfClass::UpdateTemperature(T_SqlTemp& tr)
	{
		SrTemp = tr.rbegin()->second.second;
		Sheet.Temperature = std::to_string(SrTemp);
		std::ostringstream oss;

		oss << std::setprecision(0) << std::fixed << SrTemp;
		std::string update = " temperature = " + oss.str();
		KPVL::Sheet::SetUpdateSheet(conn, Sheet, update, "");

		strSrTemp = oss.str();
	}

	void PdfClass::SqlTempActKPVL(T_SqlTemp& tr)
	{
		try
		{
			tr.erase(tr.begin(), tr.end());
			std::string Pos1 = Sheet.Start_at;
			std::string Pos2 = Sheet.SecondPos_at;
			std::string Stop = Sheet.DataTime_End;
			//std::tm TM;

			if(Stop.length() < 1) return;
			if(Pos1.length() < 1)return;


			std::vector<PFS>pF1;
			std::vector<PFS>pF2;

			std::map<int, PFS>mF1;
			std::map<int, PFS>mF2;

			//T_fTemp fT1;
			//T_fTemp fT2;
			//SqlTempActKPVL1(Stop, fT1);
			//SqlTempActKPVL2(Stop, fT2);

			//if(Pos2.length())
			//{
				SqlTempActKPVL1(Pos1, Pos2, pF1);
				SqlTempActKPVL2(Pos2, Stop, pF2);
			//}
			//else
			//{
			//	SqlTempActKPVL3(Pos1, Stop, pF1);
			//}

			GetSrTemper(pF1, mF1);
			GetSrTemper(pF2, mF2);

			int64_t t0 = int64_t(DataTimeOfString(mF1.begin()->second.data));
			for(auto a : mF1)
			{
				int64_t t = int64_t(DataTimeOfString(a.second.data)) - t0;
				tr[a.second.data] = std::pair(t, a.second.temper);
			}

			for(auto a : mF2)
			{
				int64_t t = int64_t(DataTimeOfString(a.second.data)) - t0;
				tr[a.second.data] = std::pair(t, a.second.temper);
			}
			
			UpdateTemperature(tr);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat)
	{
		Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));

		std::wstring::const_iterator start = str.begin();
		std::wstring::const_iterator end = str.end();
		boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}):\\d{1,2}.*");
		boost::match_results<std::wstring::const_iterator> what;

		if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 1)
			temp.DrawString(what[1].str().c_str(), -1, &font1, Rect, &stringFormat, &Gdi_brush);
	}


	void PdfClass::DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, float sd, std::wstring sDataBeg)
	{
		Gdiplus::RectF RectText(Rect);

		Gdiplus::RectF boundRect;
		temp.MeasureString(sDataBeg.c_str(), 5, &font1, RectText, &stringFormat, &boundRect);

		boundRect.X = Rect.X + float(sd);
		boundRect.X -= boundRect.Width / 2;
		boundRect.Height += 3;

		//temp.DrawRectangle(&pen, boundRect);

		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		temp.DrawString(sDataBeg.c_str(), 5, &font1, boundRect, &stringFormat, &Gdi_brush);
	}


	void PdfClass::DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st)
	{
		Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 1); //������
		Gdiplus::Pen Gdi_L2(clor, 1);
		auto b = st.begin();

		double coeffW = (double)(Rect.Width) / double(maxd);
		double coeffH = (double)(Rect.Height - Rect.Y) / (double)(f_maxt - f_mint);

		Gdiplus::PointF p1;
		Gdiplus::PointF p2;
		p1.X = Rect.X;
		p1.Y = Rect.Y + float((f_maxt - b->second.second) * coeffH);

		Gdiplus::GraphicsPath path;
		path.StartFigure();
		for(auto& a : st)
		{
			p2.X =  Rect.X + float((a.second.first - mind) * coeffW);
			p2.Y =  Rect.Y + float((f_maxt - a.second.second) * coeffH);
			path.AddLine(p1, p2);
			p1 = p2;
		}
		temp.DrawPath(&Gdi_L2, &path);
		path.Reset();
	}

	void PdfClass::DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
	{
		//try
		//{
		//	Gdiplus::Pen Gdi_L1(Gdiplus::Color(255, 0, 0), 2); //�������
		//	Gdiplus::Pen Gdi_L2(Gdiplus::Color(0, 0, 255), 2); //�����
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
		//	temp.DrawString(L"�������", -1, &font2, Rect2, &stringFormat, &Gdi_brush);
		//
		//	Rect2 = Rect;
		//	Rect2.X += 125;
		//	Rect2.Y -= 5;
		//	Rect2.Height = 20;
		//	temp.DrawString(L"����", -1, &font2, Rect2, &stringFormat, &Gdi_brush);
		//}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

	void PdfClass::DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG, std::wstring s1, std::wstring s2)
	{
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

		Gdiplus::RectF Rect1 = RectG;

		temp.TranslateTransform(0, RectG.Height);
		temp.RotateTransform(-90);

		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

		//std::wstring theString = L"����������� Ѱ";

		Gdiplus::RectF boundRect ={0, 0, RectG.Height + 5, 20};
		temp.DrawString(s1.c_str(), -1, &font1, boundRect, &stringFormat, &Gdi_brush);
		temp.ResetTransform();

		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		//theString = L"����� ���:���";
		temp.DrawString(s2.c_str(), -1, &font1, Rect1, &stringFormat, &Gdi_brush);

		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	}

	void PdfClass::DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, T_SqlTemp& Act, T_SqlTemp& Ref)
	{

		for(auto a : Ref)
		{
			maxt = std::fmaxf(maxt, a.second.second);
			mint = std::fminf(mint, a.second.second); 
		}
		for(auto a : Act)
		{
			maxt = std::fmaxf(maxt, a.second.second);
			mint = std::fminf(mint, a.second.second);
		}

		f_mint = mint;
		f_maxt = maxt;


		fstep = 0.0;
		float fmaxt;
		float fmint;
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

		stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);

		double coeffH = (double)(Rect.Height - Rect.Y) / (double)(f_maxt - f_mint);

		for(double d = f_mint; d <= f_maxt; d += fstep)
		{
			float mY = Rect.Y + float((f_maxt - d) * coeffH);
			Gdiplus::PointF pt1 ={Rect.X - 5,				mY};
			Gdiplus::PointF pt2 ={Rect.X + Rect.Width,	mY};
			temp.DrawLine(&Gdi_L1, pt1, pt2);

			Gdiplus::RectF Rect2 ={Rect.X - 45, mY - 11, 40, 20};

			std::wstringstream sdw;
			sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << d;

			temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);

		}
		int tt = 0;
	}

	void PdfClass::DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, int msec)
	{
		float W = float(msec) / Rect.Width;

		float h1 = 60.0f;
		float h2 = 60.0f * 5.0f;
		float h3 = 60.0f;

		if((msec / 60) > 60)
		{
			h1 = 60.0f * 15.0f;
			h2 = 60.0f * 60.0f;
			h3 = 60.0f * 60.0f;
		}

		for(float e = 0; e <= Rect.Width; e++)
		{
			float sd = W * e;
			bool b1 = std::fmod(sd, h1) < W;
			bool b2 = std::fmod(sd, h2) < W;

			if(b1 /*&& (e + 15) < Rect.Width*/)
			{
				if(b2)
				{
					Gdiplus::PointF pt1 ={Rect.X + float(e), Rect.Y};
					Gdiplus::PointF pt2 ={Rect.X + float(e), Rect.Height - (b2 ? 10 : 18)};
					temp.DrawLine(&Gdi_D1, pt1, pt2);
				}
				else
				{
					Gdiplus::PointF pt1 ={Rect.X + float(e), Rect.Y};
					Gdiplus::PointF pt2 ={Rect.X + float(e), Rect.Height - (b2 ? 10 : 18)};
					temp.DrawLine(&Gdi_L1, pt1, pt2);
				}

			}
			if(b2)
			{
				//float X = float(e) + 20;
				//if(X < Rect.Width)
				{
					std::wstringstream sdw;
					sdw << std::fixed << std::setprecision(0) << (sd / h3);
					std::wstring sDataBeg = sdw.str();
					DrawT(temp, Rect, e, sDataBeg);
				}
			}
		}

		//int sd = int(W * Rect.Width);
		//Gdiplus::PointF pt1 ={Rect.X + Rect.Width, Rect.Y};
		//Gdiplus::PointF pt2 ={Rect.X + Rect.Width, Rect.Height - 10};
		//
		//temp.DrawLine(&Gdi_D1, pt1, pt2);
		//
		//std::wstringstream sdw;
		//sdw << std::fixed << std::setprecision(1) << (sd / h3);
		//std::wstring sDataBeg = sdw.str();
		//DrawT(temp, Rect, Rect.Width, sDataBeg);

	}

	void PdfClass::PaintGraff(T_SqlTemp& Act, T_SqlTemp& Ref, std::string fImage, int msec, std::wstring s1, std::wstring s2)
	{
		try
		{
			Gdiplus::REAL Width = 525;
			Gdiplus::REAL Height = 205;
			HDC hdc = GetDC(Global0);
			Gdiplus::Graphics g(hdc);
			Gdiplus::Bitmap backBuffer (INT(Width), INT(Height), &g);
			Gdiplus::Graphics temp(&backBuffer);
			Gdiplus::RectF RectG(0, 0, Width, Height);

			//��������
			temp.Clear(Gdiplus::Color(255, 255, 255));

			if(!Act.size()) return;
			if(!Ref.size()) return;

			maxt = 0;
			mint = 9999;
			mind = (std::min)(Act.begin()->second.first, Ref.begin()->second.first);

			auto b = Act.begin();
			auto e = Act.rbegin();

			int64_t MaxSecCount = 0;
			maxd = std::max<int64_t>(MaxSecCount, e->second.first - b->second.first);


			b = Ref.begin();
			e = Ref.rbegin();
			maxd = std::max<int64_t>(maxd, e->second.first - b->second.first);


			Gdiplus::Color Blue(0, 0, 255);
			Gdiplus::Color Red(255, 0, 0);

			Gdiplus::RectF RectG2(RectG);

			RectG2.Y += 5;
			RectG2.Height -= 40;

			RectG2.X += 55;
			RectG2.Width -= 65;

			Gdiplus::RectF RectG3(RectG2);
			RectG3.Height += 17;

			DrawGridTemp(temp, RectG2, Act, Ref);
			DrawGridTime(temp, RectG3, msec);
			DrawGridOssi(temp, RectG, s1, s2);

			DrawBottom(temp, RectG2, Red, Ref);	//�������; �������� �������� �����������
			DrawBottom(temp, RectG2, Blue, Act);	//�����; ����������� �������� �����������
			
			std::wstring SaveFile(fImage.begin(), fImage.end());
			backBuffer.Save(SaveFile.c_str(), &guidJpeg, NULL);

			DeleteDC(hdc);
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}

		

	HPDF_REAL PdfClass::DrawKpvl(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
	{
		HPDF_REAL Y = top - 0;
		try
		{
			draw_text_rect (page, left + 0, Y, w, YP, "���� � ����� ��������");
			std::string outDate = "";
			std::string outTime = "";
			GetDataTimeStr(Sheet.Start_at, outDate, outTime);
			draw_text_rect (page, left + 270, Y, XP, YP, outDate);  //����
			draw_text_rect (page, left + 340, Y, XP, YP, outTime);  //�����


			//std::string str = Sheet.Start_at;
			//boost::regex xRegEx("^(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}:\\d{2}:\\d{2}).*");
			//boost::match_results<std::string::const_iterator>what;
			//boost::regex_search(str, what, xRegEx, boost::match_default) && what.size();
			//if(what.size() > 4)
			//{
			//	std::string year = what[1].str();
			//	std::string month = what[2].str();
			//	std::string day = what[3].str();
			//	std::string time = what[4].str();
			//	std::string data = day + "-" + month + "-" + year;
			//
			//	draw_text_rect (page, left + 270, Y, XP, YP, data);  //����
			//	draw_text_rect (page, left + 340, Y, XP, YP, time);  //�����
			//}
			//else
			//{
			//	std::string ss = Sheet.Start_at;
			//	std::vector <std::string>split;
			//	boost::split(split, ss, boost::is_any_of(" "), boost::token_compress_on);
			//	if(split.size() >= 2)
			//	{
			//		draw_text_rect (page, left + 270, Y, XP, YP, split[0]);  //����
			//		draw_text_rect (page, left + 340, Y, XP, YP, split[1]);  //�����
			//	}
			//}
			Y -= 25;
			draw_text (page, left + 10, Y, "���������");
			draw_text (page, left + 280, Y, "�������");
			draw_text (page, left + 355, Y, "����");

			Y -= 20;
			draw_text_rect (page, left + 0, Y, w, YP, "����� ���������� ����� � ���������� ����. ���");
			draw_text_rect (page, left + 270, Y, XP, YP, Sheet.TimeForPlateHeat);//������� ����� ���������� ����� � ���������� ����. ���
			

			//Sheet.DataTime_All = (boost::format("%.1f") % (double(DataTimeDiff(Sheet.DataTime_End, Sheet.Start_at)) / 60.0)).str();
			//Sheet.DataTime_All = std::to_string(double(DataTimeDiff(Sheet.DataTime_End, Sheet.Start_at)) / 60.0);
			draw_text_rect (page, left + 340, Y, XP, YP, Sheet.DataTime_All);    //���� ����� ���������� ����� � ���������� ����. ���

			Y -= 25;
			draw_text_rect (page, left + 0, Y, w, YP, "����������� �������, ��");
			draw_text_rect (page, left + 270, Y, XP, YP, Sheet.Temper);			//������� ����������� �������
			draw_text_rect (page, left + 340, Y, XP, YP, strSrTemp);				//���� ����������� �������

			Y -= 25;
			draw_text_rect (page, left + 0, Y, w, YP, "�������� ���� � ���������� �����. ������, ���");
			draw_text_rect (page, left + 270, Y, XP, YP, Sheet.PresToStartComp);	//������� �������� ����
			draw_text_rect (page, left + 340, Y, XP, YP, Sheet.Za_PT3);			//���� �������� ����

			Y -= 25;
			draw_text_rect (page, left + 0, Y, w, YP, "����������� ���������� ��������, ��");
			draw_text_rect (page, left + 340, Y, XP, YP, Sheet.Za_TE3);			//���� ����������� ����, ��

			Y -= 10;
			HPDF_Page_MoveTo (page, 15, Y);
			HPDF_Page_LineTo (page, Width - 20, Y);
			HPDF_Page_Stroke (page);
			Y -= 20;
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);

		return Y;
	}
		
	HPDF_REAL PdfClass::DrawFurn(HPDF_REAL left, HPDF_REAL top, HPDF_REAL w)
	{
		HPDF_REAL Y = top - 0;
		try
		{
#if _DEBUG

			std::stringstream ssd;
			ssd << "���� � " << Cassette.Peth << " ������� � ";
			ssd << std::setw(2) << std::setfill('0') << Cassette.Hour << "-";
			ssd << std::setw(2) << std::setfill('0') << Cassette.Day << "-";
			ssd << std::setw(2) << std::setfill('0') << Cassette.Month << "-";
			ssd << std::setw(4) << std::setfill('0') << Cassette.Year << "-";
			ssd << std::setw(2) << std::setfill('0') << Cassette.CassetteNo;
			draw_text_rect (page, left, Y, w, YP, ssd.str().c_str());
			Y -= 25;
#endif // _DEBUG

			draw_text_rect (page, left + 0, Y, w, YP, "���� � ����� ��������");
			std::string outDate = "";
			std::string outTime = "";
			GetDataTimeStr(Cassette.Run_at, outDate, outTime);
			draw_text_rect (page, left + 270, Y, XP, YP, outDate);  //����
			draw_text_rect (page, left + 340, Y, XP, YP, outTime);  //�����

			//std::string str = Cassette.Run_at;
			//boost::regex xRegEx("^(\\d{1,4})-(\\d{1,2})-(\\d{1,2}) (\\d{1,2}:\\d{1,2}:\\d{1,2}).*");
			//boost::match_results<std::string::const_iterator>what;
			//boost::regex_search(str, what, xRegEx, boost::match_default) && what.size();
			//if(what.size() > 4)
			//{
			//	std::string year = what[1].str();
			//	std::string month = what[2].str();
			//	std::string day = what[3].str();
			//	std::string time = what[4].str();
			//	std::string data = day + "-" + month + "-" + year;
			//
			//	draw_text_rect (page, left + 270, Y, XP, YP, data);  //����
			//	draw_text_rect (page, left + 340, Y, XP, YP, time);  //�����
			//}
			//else
			//{
			//	std::string ss = Cassette.Run_at;
			//	std::vector <std::string>split;
			//	boost::split(split, ss, boost::is_any_of(" "), boost::token_compress_on);
			//
			//	if(split.size() >= 2)
			//	{
			//		draw_text_rect (page, left + 270, Y, XP, YP, split[0]);  //����
			//		std::string ss = split[1];
			//		std::vector <std::string>split1;
			//		boost::split(split1, ss, boost::is_any_of("."), boost::token_compress_on);
			//
			//		if(split1.size())
			//			draw_text_rect (page, left + 340, Y, XP, YP, split1[0]);  //�����
			//		else
			//			draw_text_rect (page, left + 340, Y, XP, YP, split[1]);  //�����
			//	}
			//}

			Y -= 25;
			draw_text (page, left + 10, Y, "���������");
			draw_text (page, left + 280, Y, "�������");
			draw_text (page, left + 355, Y, "����");

			Y -= 20;
			draw_text_rect (page, left + 0, Y, w, YP, "����� ������� �� ����������� �������, ���");

			float PointTime1 = Stof(Cassette.PointTime_1);
			float PointTime2 = PointTime1 + 60;
			std::stringstream sss;
			sss << boost::format("%.1f - ") % PointTime1;
			sss << boost::format("%.1f") % PointTime2;

			draw_text_rect (page, left + 270, Y, XP, YP, sss.str());								//�������
			draw_text_rect (page, left + 340, Y, XP, YP, Cassette.HeatAcc);							//����

			Y -= 25;
			draw_text_rect (page, left + 0, Y, w, YP, "����� �������� ��� �������� �����������, ���");
			draw_text_rect (page, left + 270, Y, XP, YP, Cassette.PointDTime_2);						//�������
			draw_text_rect (page, left + 340, Y, XP, YP, Cassette.HeatWait);							//����

			Y -= 25;
			draw_text_rect (page, left + 0, Y, w, YP, "����������� �������, ��");
			draw_text_rect (page, left + 270, Y, XP, YP, Cassette.PointRef_1);						//�������
			draw_text_rect (page, left + 340, Y, XP, YP, Cassette.facttemper);							//����
			Y -= 20;
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		return Y;
	}
		
	//�������� ������ ����� PDF
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

			//tw = HPDF_Page_TextWidth (page, page_title);
			HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);


			//210x297 ��
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

		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		return false;
	}


	//���������� ����� PDF
	void PdfClass::SavePDF()
	{
		std::stringstream urls;
		//urls << "\\\\192.168.9.63\\";
		std::stringstream temp;
		try
		{
			temp << lpLogPdf2;
			urls << lpLogPdf2;
			CheckDir(temp.str());
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File: " + temp.str() + " ");

		std::tm TM;
		DataTimeOfString(Sheet.Start_at, TM);
			
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
//� ������ ������� ��������
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
			ifile << std::setw(2) << std::setfill('0') << Cassette.Hour << "-";
			ifile << std::setw(2) << std::setfill('0') << Cassette.CassetteNo;
		}CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + temp.str() + "/" + tfile.str());

		FileName = temp.str() + "/" + tfile.str() + ".pdf";
		std::string ImgeName = temp.str() + "/" + ifile.str() + ".jpg";
		std::string PdfName = urls.str() + "/" + tfile.str() + ".pdf";

#pragma region ������� 2-� � ���������� �����
		try
		{
			int n = 0;
			while(std::filesystem::exists(FileName))
			{
				n++;
				std::string sn = "_" + std::to_string(n);
				FileName = temp.str() + "/" + tfile.str() + sn + ".pdf";
				ImgeName = temp.str() + "/" + ifile.str() + sn + ".jpg";
				PdfName = urls.str() + "/" + tfile.str() + sn + ".pdf";
			}
		}
		CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + FileName + " ");
#pragma endregion

		try
		{

			try
			{
				if(std::filesystem::exists(furnImage))
					std::filesystem::copy_file(furnImage, ImgeName, std::filesystem::copy_options::skip_existing | std::filesystem::copy_options::overwrite_existing);
			}
			CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + FileName + " ");

			HPDF_SaveToFile (pdf, FileName.c_str());
			HPDF_Free (pdf);

			
			boost::replace_all(PdfName, lpLogPdf2, lpLogPdf);
			std::stringstream ssd;
			ssd << "UPDATE sheet SET pdf = '" << PdfName;
			ssd << "' WHERE id = " << Sheet.id;
			std::string comand = ssd.str();
			PGresult* res = conn.PGexec(comand);
			if(PQresultStatus(res) == PGRES_FATAL_ERROR)
				LOG_ERROR(PdfLogger, "{:89}| {}", (std::string(__FUNCTION__) + std::string(":") + std::to_string(1037)), comand);;
			PQclear(res);
		}
		CATCH(PdfLogger, FUNCTION_LINE_NAME + " File1: " + FileName + " ");


	}

	HPDF_REAL PdfClass::DrawHeder(HPDF_REAL left, HPDF_REAL top)
	{
		HPDF_REAL Y = top - 40;
		try
		{
			HPDF_Page_SetFontAndSize (page, font, 14);
				
			//842 / 2 = 
			draw_text (page, left + 365, Y, "�������");


			HPDF_Page_SetFontAndSize (page, font, 10);
			Y -= 30;
			draw_text (page, left + 20, Y, "����� �����");
			draw_text_rect (page, left + 100, Y, XP, YP, Sheet.Alloy);

			draw_text (page, left + 220, Y, "�������, ��");
			draw_text_rect (page, left + 300, Y, XP, YP, Sheet.Thikness);


			Y -= 30;
			draw_text (page, left + 20, Y, "������");
			draw_text_rect (page, left + 70, Y, XP, YP, Sheet.Melt);

			draw_text (page, left + 180, Y, "������");
			draw_text_rect (page, left + 230, Y, XP, YP, Sheet.PartNo);

			draw_text (page, left + 350, Y, "�����");
			draw_text_rect (page, left + 390, Y, XP, YP, Sheet.Pack);

			draw_text (page, left + 510, Y, "����");
			draw_text_rect (page, left + 550, Y, XP, YP, Sheet.Sheet + " / " + Sheet.SubSheet);

			draw_text (page, left + 680, Y, "����");
			draw_text_rect (page, left + 720, Y, XP, YP, Sheet.Slab);

			Y -= 10;
			HPDF_Page_MoveTo (page, left + 15, Y);
			HPDF_Page_LineTo (page, left + Width - 20, Y);
			HPDF_Page_Stroke (page);
			Y -= 20;
		}CATCH(PdfLogger, FUNCTION_LINE_NAME);
		return Y;
	}

	//������ �������
	HPDF_REAL PdfClass::DrawKpvlPDF(HPDF_REAL left, HPDF_REAL top)
	{
		HPDF_REAL Y = top - 5;
		try
		{
			//������ ����������� �������
			HPDF_Page_SetFontAndSize (page, font, 12);
			draw_text(page, 380, Y, "�������");
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

	//������ ������
	HPDF_REAL PdfClass::DrawFurnPDF(HPDF_REAL left, HPDF_REAL top)
	{
		HPDF_REAL Y = top - 5;
		try
		{
		//������ ����������� �������
			HPDF_Page_SetFontAndSize (page, font, 12);
			draw_text(page, 380, Y, "������");
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



	void OutDebugInfo(TCassette& Cassette, TSheet& Sheet)
	{
		std::stringstream sss;
		sss << "�������: "
			<< std::setw(4) << std::setfill('0') << Cassette.Year << "-"
			<< std::setw(2) << std::setfill('0') << Cassette.Month << "-"
			<< std::setw(2) << std::setfill('0') << Cassette.Day << "-"
			<< std::setw(2) << std::setfill('0') << Cassette.Hour << "� "
			<< std::setw(2) << std::setfill('0') << Cassette.CassetteNo
			<< " ����: "
			<< std::setw(6) << std::setfill('0') << Sheet.Melt << "-"
			//<< std::setw(3) << std::setfill('0') << Sheet.Slab << "-"
			<< std::setw(3) << std::setfill('0') << Sheet.PartNo << "-"
			<< std::setw(3) << std::setfill('0') << Sheet.Pack << "-"
			<< std::setw(3) << std::setfill('0') << Sheet.Sheet << "/"
			<< std::setw(2) << std::setfill('0') << Sheet.SubSheet;
		SetWindowText(hWndDebug, sss.str().c_str());

	}

	PdfClass::PdfClass(TCassette& TC)
	{
		try
		{
			Cassette = TC;

		#pragma region ������� �������

			#pragma region ����� stringFormat ��� ��������
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
				ssd << std::setw(2) << std::setfill('0') << Cassette.Hour << "-";
				ssd << std::setw(2) << std::setfill('0') << Cassette.CassetteNo << ".jpg";
				furnImage = ssd.str();
			}
			#pragma endregion


			if(!conn.connection()) return;
			GetSheet();

			#pragma region ����� ���� ��� FurnRef � FurnAct
			{
				FurnRef.erase(FurnRef.begin(), FurnRef.end());
				FurnAct.erase(FurnAct.begin(), FurnAct.end());

				int P = atoi(Cassette.Peth.c_str());
				if(Cassette.Run_at.length() && Cassette.Finish_at.length())
				{

					int Ref_ID = 0;
					int Act_ID = 0;

					//������ ��������� ����
					if(P == 1)
					{
						Ref_ID = ForBase_RelFurn_1.TempRef->ID;
						Act_ID = ForBase_RelFurn_1.TempAct->ID;
					}

					//������ ��������� ����
					if(P == 2)
					{
						Ref_ID = ForBase_RelFurn_2.TempRef->ID;
						Act_ID = ForBase_RelFurn_2.TempAct->ID;
					}

					if(Ref_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnRef, Ref_ID);
					if(Act_ID) GetTempRef(Cassette.Run_at, Cassette.Finish_at, FurnAct, Act_ID);

				}
				else return;

				//������ ������ FURN
				time_t t1 = DataTimeOfString(Cassette.Run_at);
				time_t t2 = DataTimeOfString(Cassette.End_at);
				int t = int(difftime(t2, t1));

				PaintGraff(FurnAct, FurnRef, furnImage, t, L"����������� Ѱ", L"����� ���");
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

				#pragma region ������� �������
				{
					TempRef.erase(TempRef.begin(), TempRef.end());
					TempAct.erase(TempAct.begin(), TempAct.end());

					//�������
					SqlTempActKPVL(TempAct);
					GetTempRef(Sheet.Start_at, Sheet.DataTime_End, TempRef, GenSeqFromHmi.TempSet1->ID);

					//TempAct.erase(TempAct.begin(), TempAct.end());
					//SqlTempActKPVL(TempAct);

					//������ ������ KPVL
				
					time_t t1 = DataTimeOfString(Sheet.Start_at);
					time_t t2 = DataTimeOfString(Sheet.DataTime_End);
					int t = int(difftime(t2, t1));
		//std::wstring theString = L"����������� Ѱ";
		//theString = L"����� ���:���";
					PaintGraff(TempAct, TempRef, tempImage, t, L"����������� Ѱ", L"����� ���");
				}
				#pragma endregion

				#pragma region �������� PFD �����

				if(NewPdf())
				{
					//������ PDF ���������
					HPDF_REAL Y1 = DrawHeder(0, Height);

					//������ PDF �������
					HPDF_REAL Y2 = DrawKpvlPDF(0, Y1) - 30;

					//������ PDF ������
					HPDF_REAL Y3 = DrawFurnPDF(0, Y2);

					//auto index = a.index();
					//��������� PDF
					SavePDF();

				}

				#pragma endregion

				remove(tempImage.c_str());
#ifdef HENDINSERT
				std::string url = FileName;
				boost::replace_all(url, "/", "\\");
				ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
				return;
#endif
			}
			remove(furnImage.c_str());
			{
				std::stringstream ssd;
				ssd << "UPDATE cassette SET pdf = now() WHERE";
				ssd << " year = '" << Cassette.Year << "' AND";
				ssd << " month = '" << Cassette.Month << "' AND";
				ssd << " day = '" << Cassette.Day << "' AND";
				ssd << " hour = '" << Cassette.Hour << "' AND";
				ssd << " cassetteno = " << Cassette.CassetteNo;
				SETUPDATESQL(PdfLogger, conn, ssd);
			}

		}
		CATCH(PdfLogger, FUNCTION_LINE_NAME);
	};
	


	//�������������� �������� �� �������
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
				sss << boost::format("%|02|-") % TC.Day;
				sss << boost::format("%|02| ") % TC.Hour;
				sss << " � " << TC.CassetteNo;
				SetWindowText(hWndDebug, sss.str().c_str());
				PdfClass pdf(TC);
			}
		}
		CATCH(PdfLogger, FUNCTION_LINE_NAME);
	}
		

	/*
	//����������� �� ����� �� ����
	void PrintCassettePdfAuto(TSheet& Sheet)
	{
		try
		{
			PDF::Cassette::CassettePdfClass* pdf  = new PDF::Cassette::CassettePdfClass(Sheet);
		}
		CATCH(PdfLogger, std::string("PrintPdf: "));
	}
		
	//�������������� �������� �� ������
	void RunAlCassettelPdfAuto(TSheet& Sheet, bool view)
	{
		try
		{
			PDF::Cassette::CassettePdfClass pdf(Sheet, view);
		}
		CATCH(PdfLogger, std::string("PrintPdf: "));
	}
	*/


	namespace CASSETTE
	{
		class GetCassettes
		{
		public:

			std::string DateStart = "";
			std::string DateStop = "";
			TCASS P0;
			std::fstream fUpdateCassette;
			int lin = 1;

			std::string GetVal(PGConnection& conn, int ID, std::string Run_at, std::string End_at);
			void GetVal(PGConnection& conn, TCassette& P, T_ForBase_RelFurn* Furn);
			void EndCassette(PGConnection& conn, TCassette& P, int Petch, std::fstream& s1);
			void GetCassetteData(PGConnection& conn, std::string id, TCassette& ct, TCassette& it);
			void SaveFileSdg(MapRunn& CassetteTodos);
			void SaveDataBaseForId(PGConnection& conn, TCassette& ct, TCassette& it);
			void SaveDataBaseNotId(PGConnection& conn, TCassette& ct, TCassette& it);
			void SaveDataBase(PGConnection& conn, TCassette& ct, TCassette& it);
			void SaveFileCass();
			void GetCasset(PGConnection& conn, MapRunn& CassetteTodos, int Petch);
			void GetCasset(PGConnection& conn);
			void SaveBaseCassete(PGConnection& conn, TCassette& tc);
			void CorrectSQL(PGConnection& conn);
			GetCassettes(PGConnection& conn, std::string datestart = "", std::string datestop = "");
		};

		std::string GetCassettes::GetVal(PGConnection& conn, int ID, std::string Run_at, std::string End_at)
		{
			std::string f = "0";
			try
			{
				std::stringstream sss;
				sss << "SELECT DISTINCT ON (id) content"
					//", (SELECT tag.comment FROM tag WHERE tag.id = todos.id_name)"
					" FROM todos WHERE "
					<< " id_name = " << ID 			//����������� �������� �����������
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

		void GetCassettes::GetVal(PGConnection& conn, TCassette& P, T_ForBase_RelFurn* Furn)
		{
			try
			{
#pragma region ������� ������ � ����

				std::stringstream sss;
				sss << "SELECT DISTINCT ON (id_name) id_name, content"
					", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)"
					" FROM todos WHERE ("
					<< " todos.id_name = " << Furn->ActTimeHeatAcc->ID << " OR"	    //���� ����� �������
					<< " todos.id_name = " << Furn->ActTimeHeatWait->ID << " OR"	//���� ����� ��������
					<< " todos.id_name = " << Furn->ActTimeTotal->ID				//���� ����� �����
					<< ")"	//���� ����� ��������
					<< " AND todos.create_at >= '" << P.Run_at << "'"
					<< " AND todos.create_at < '" << P.End_at << "'"
					<< " AND cast (content as numeric) <> 0"
					<< " ORDER BY todos.id_name, todos.id DESC"; // LIMIT 3

#pragma endregion

#pragma region ������ � ����

				std::string comand = sss.str();
				PGresult* res = conn.PGexec(comand);
				//��������� ������
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					//��������� ������
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
#pragma region ������� ������ � ����

				std::stringstream sss;
				sss << "SELECT DISTINCT ON (id_name) id_name, content"
					", (SELECT tag.comment AS name FROM tag WHERE tag.id = todos.id_name)"
					" FROM todos WHERE ("
					<< " todos.id_name = " << Furn->PointRef_1->ID << " OR"		//������� �����������
					<< " todos.id_name = " << Furn->PointTime_1->ID << " OR"		//������� ����� �������
					<< " todos.id_name = " << Furn->PointDTime_2->ID << " OR"	//������� ����� ��������
					<< " todos.id_name = " << Furn->TimeProcSet->ID				//������ ����� �������� (�������), ���
					<< ")"	//���� ����� ��������
					<< " AND todos.create_at < '" << P.End_at << "'"
					<< " ORDER BY todos.id_name, todos.id DESC"; // LIMIT 4

#pragma endregion

#pragma region ������ � ����

				std::string comand = sss.str();
				PGresult* res = conn.PGexec(comand);
				//��������� ������
				if(PQresultStatus(res) == PGRES_TUPLES_OK)
				{
					int line = PQntuples(res);
					//��������� ������
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
				time_t temp = DataTimeOfString(P.End_at, TM);
				temp  = (time_t)difftime(temp, 5 * 60); //������� 5 ����� �� ����� �������
				localtime_s(&TM, &temp);
				std::string End_at  = GetDataTimeString(TM);
				P.facttemper = GetVal(conn, Furn->TempAct->ID, P.Run_at, End_at);

			}
			CATCH(PdfLogger, "");
		}

		void GetCassettes::EndCassette(PGConnection& conn, TCassette& P, int Petch, std::fstream& s1)
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

					std::time_t tm_Run = DataTimeOfString(P.Run_at, TM_Run);
					std::time_t tm_End = DataTimeOfString(P.End_at, TM_End);
					std::time_t tm_All = (time_t)difftime(tm_End, tm_Run);

					gmtime_s(&TM_All, &tm_All);
					TM_All.tm_year -= 1900;
					TM_All.tm_mday -= 1;

					tm_Fin = DataTimeOfString(P.End_at, TM_Fin);
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
						<< P.Hour << ";"
						<< P.CassetteNo << ";"
						<< " " << P.Error_at << ";"

						<< P.PointRef_1 << ";"		//������� �����������
						<< P.facttemper << ";"		//����������� �������� �����������

						<< P.PointTime_1 << ";"		//������� ����� �������
						<< P.HeatAcc << ";"			//���� ����� �������
						<< P.PointDTime_2 << ";"	//������� ����� ��������
						<< P.HeatWait << ";"		//���� ����� ��������

						<< P.TimeProcSet << ";"		//������ ����� �������� (�������), ���
						<< P.Total << ";"			//���� ����� �����

						<< std::endl;


					P.Peth = std::to_string(Petch);
				}
			}
			CATCH(PdfLogger, "");
		}


		void GetCassettes::GetCassetteData(PGConnection& conn, std::string id, TCassette& ct, TCassette& it)
		{
			try
			{
				std::stringstream com;
				com << "SELECT * FROM cassette WHERE"
					<< " hour = " << it.Hour
					<< " AND day = " << it.Day
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

		void GetCassettes::SaveFileSdg(MapRunn& CassetteTodos)
		{
#ifdef _DEBUG
			try
			{
#pragma region ������ � ���� ��������� Sdg.csv
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

#pragma region ������ � ���� ���� Sdg.csv
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


		void GetCassettes::SaveDataBaseForId(PGConnection& conn, TCassette& ct, TCassette& it)
		{
			std::stringstream sg2;
			try
			{
				//sg2 << "��������� ���� " << P0.size() << " id = " << it.Id;
				//SetWindowText(hWndDebug, sg2.str().c_str());

				if(it.Id.length())ct.Id = it.Id;
				if(it.Year.length())ct.Year = it.Year;
				if(it.Month.length())ct.Month =it.Month;
				if(it.Day.length())ct.Day = it.Day;
				if(it.Hour.length())ct.Hour = it.Hour;
				if(it.CassetteNo.length())ct.CassetteNo = it.CassetteNo;

				if(it.Peth.length())ct.Peth = it.Peth;
				if(it.Run_at.length())ct.Run_at = it.Run_at;
				if(it.Error_at.length())ct.Error_at = it.Error_at;
				if(it.End_at.length())ct.End_at = it.End_at;
				if(it.PointTime_1.length())ct.PointTime_1 = it.PointTime_1;    //����� �������
				if(it.PointRef_1.length())ct.PointRef_1 = it.PointRef_1;      //������� �����������
				if(it.TimeProcSet.length())ct.TimeProcSet = it.TimeProcSet;    //������ ����� �������� (�������), ���
				if(it.PointDTime_2.length())ct.PointDTime_2 =it.PointDTime_2;   //����� ��������
				if(it.facttemper.length())ct.facttemper = it.facttemper;			//���� ����������� �� 5 ����� �� ����� �������
				if(it.Finish_at.length())ct.Finish_at = it.Finish_at;        //���������� �������� + 15 �����
				if(it.HeatAcc.length())ct.HeatAcc = it.HeatAcc;			//���� ����� �������
				if(it.HeatWait.length())ct.HeatWait = it.HeatWait;          //���� ����� ��������
				if(it.Total.length())ct.Total = it.Total;				//���� ����� �����

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
					ssd << ", facttemper = " << ct.facttemper; //���� ����������� �� 5 ����� �� ����� �������


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


		void GetCassettes::SaveDataBaseNotId(PGConnection& conn, TCassette& ct, TCassette& it)
		{
			try
			{
				if(it.Id.length())ct.Id = it.Id;
				if(it.Year.length())ct.Year = it.Year;
				if(it.Month.length())ct.Month =it.Month;
				if(it.Day.length())ct.Day = it.Day;
				if(it.Hour.length())ct.Hour = it.Hour;
				if(it.CassetteNo.length())ct.CassetteNo = it.CassetteNo;

				if(it.Peth.length())ct.Peth = it.Peth;
				if(it.Run_at.length())ct.Run_at = it.Run_at;
				if(it.Error_at.length())ct.Error_at = it.Error_at;
				if(it.End_at.length())ct.End_at = it.End_at;

				if(it.PointTime_1.length())ct.PointTime_1 = it.PointTime_1;    //����� �������
				if(it.PointRef_1.length())ct.PointRef_1 = it.PointRef_1;      //������� �����������
				if(it.TimeProcSet.length())ct.TimeProcSet = it.TimeProcSet;    //������ ����� �������� (�������), ���
				if(it.PointDTime_2.length())ct.PointDTime_2 =it.PointDTime_2;   //����� ��������
				if(it.facttemper.length())ct.facttemper = it.facttemper;			//���� ����������� �� 5 ����� �� ����� �������
				if(it.Finish_at.length())ct.Finish_at = it.Finish_at;        //���������� �������� + 15 �����
				if(it.HeatAcc.length())ct.HeatAcc = it.HeatAcc;			//���� ����� �������
				if(it.HeatWait.length())ct.HeatWait = it.HeatWait;          //���� ����� ��������
				if(it.Total.length())ct.Total = it.Total;				//���� ����� �����

	//#ifndef TESTPDF2
				std::stringstream ssd;
				ssd << "INSERT INTO cassette ";
				ssd << "("
					"create_at, "
					"event, "
					"year, "
					"month, "
					"day, "
					"hour, "
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
				ssd << Stoi(ct.Hour) << ", ";
				ssd << Stoi(ct.CassetteNo) << ", ";

				int count = -1;
				if(Stoi(ct.SheetInCassette))
					ssd << Stoi(ct.SheetInCassette) << ", ";
				else
				{
					std::stringstream set;
					set << "SELECT COUNT(*) FROM sheet WHERE ";
					set << "year = '" << Stoi(ct.Year) << "' ";
					set << "month = '" << Stoi(ct.Month) << "' ";
					set << "AND day = '" << Stoi(ct.Day) << "' ";
					set << "AND hour = '" << Stoi(ct.Hour) << "' ";
					set << "AND cassetteno = " << Stoi(ct.CassetteNo);;
					std::string comand = set.str();
					PGresult* res = conn.PGexec(comand);
					if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
						count = Stoi(conn.PGgetvalue(res, 0, 0));
					PQclear(res);

				}
					ssd << count << ", ";

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
				ssg << "WHERE";
				//ssg << " hour = " << ct.Hour << " AND";
				ssg << " day = " << ct.Day;
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

		void GetCassettes::SaveDataBase(PGConnection& conn, TCassette& ct, TCassette& it)
		{
			//������ � ����
			std::stringstream ssg;
			ssg << "SELECT id FROM cassette ";
			ssg << "WHERE ";
			//ssg << " hour = " << it.Hour << " AND";
			ssg << " day = " << it.Day;
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

		void GetCassettes::SaveFileCass()
		{

	//#ifdef _DEBUG
			try
			{
#pragma region ������ � ���� Cass.csv ���������
				std::fstream s1("Cass.csv", std::fstream::binary | std::fstream::out);
				s1 << "������;"
					<< "�����;"
					<< "����;"
					<< "�����;"
					<< "���;"
					<< "�����;"
					<< "����;"
					<< "�������;"
					<< "������;"
					<< "����;"
					<< "������� T;"			//������� �����������
					<< "���� �;"			//����������� �������� �����������
					<< "������� �������;"	//������� ����� �������
					<< "���� �������;"		//���� ����� �������
					<< "������� ��������;"	//������� ����� ��������
					<< "���� ��������;"		//���� ����� ��������
					<< "������� ��������;"	//������ ����� �������� (�������), ���
					<< "���� ��������;"		//���� ����� �����
					<< std::endl;
#pragma endregion

#pragma region ������ � ���� Cass.csv ����
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
							<< it.second.Hour << ";"
							<< it.second.CassetteNo << ";"
							<< " " << it.second.Error_at << ";"
							<< it.second.Peth << ";"
							<< it.second.PointRef_1 << ";"		//������� �����������
							<< it.second.facttemper << ";"		//����������� �������� �����������
							<< it.second.PointTime_1 << ";"		//������� ����� �������
							<< it.second.HeatAcc << ";"			//���� ����� �������
							<< it.second.PointDTime_2 << ";"	//������� ����� ��������
							<< it.second.HeatWait << ";"		//���� ����� ��������
							<< it.second.TimeProcSet << ";"		//������ ����� �������� (�������), ���
							<< it.second.Total << ";"			//���� ����� �����

							<< std::endl;
					}
				}
				s1.close();
#pragma endregion
			}
			CATCH(PdfLogger, "");
	//#endif
		}


		void GetCassettes::SaveBaseCassete(PGConnection& conn, TCassette& tc)
		{
			TCassette ct;
			if(tc.Run_at.length() && tc.End_at.length() && S107::IsCassette(tc))
			{
				//std::string sg = std::string("��������� ������� �� ���� ") + std::to_string(P0.size()) + std::string(" :") + std::to_string(lin++);
				//SetWindowText(hWndDebug, sg.c_str());

				GetCassetteData(conn, tc.Id, ct, tc);
				if(ct.facttemper.length())	tc.facttemper = ct.facttemper;
				if(ct.Id.length())			tc.Id = ct.Id;

				if(!tc.Finish_at.length())
				{
					std::tm TM;
					time_t tm = DataTimeOfString(tc.End_at, TM);
					tm += (60 * 15);
					tc.Finish_at = GetDataTimeString(tm);
				}
				SaveDataBase(conn, ct, tc);
			}
		}

		void GetCassettes::GetCasset(PGConnection& conn, MapRunn& CassetteTodos, int Petch)
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
				ssd << "SELECT todos.create_at, todos.id, todos.id_name, todos.content";
				ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
				ssd << ", (SELECT comment AS name FROM tag WHERE tag.id = todos.id_name) ";
				ssd << "FROM todos WHERE ";
				ssd << "create_at >= '" << DateStart << "' AND ";

				if(DateStop.length())
					ssd << "create_at < '" << DateStop << "' AND ";

				ssd << "(id_name = " << Furn->ProcEnd->ID;
				ssd << " OR id_name = " << Furn->ProcRun->ID;
				ssd << " OR id_name = " << Furn->ProcFault->ID;

				ssd << " OR id_name = " << Furn->Cassette.Hour->ID;
				ssd << " OR id_name = " << Furn->Cassette.Day->ID;
				ssd << " OR id_name = " << Furn->Cassette.Month->ID;
				ssd << " OR id_name = " << Furn->Cassette.Year->ID;
				ssd << " OR id_name = " << Furn->Cassette.CassetteNo->ID
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

					//Furn->Cassette.Hour->ID
					try
					{
						if(a.second.id_name == Furn->Cassette.Hour->ID)
						{
							if(!P.Run_at.size())
								P.Hour = a.second.value;
							else if(P.Hour != a.second.value)
							{
								P.End_at = a.second.create_at;
								//EndCassette(conn, P, Petch, s1);
								P0[a.first] = P;

								P.Run_at = "";
								P.End_at = "";
								P.Error_at = "";
								P.Hour = a.second.value;
							}
						}
					}
					CATCH(PdfLogger, "");
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

		void GetCassettes::GetCasset(PGConnection& conn)
		{
			MapRunn CassetteTodos;
			SetWindowText(hWndDebug, "SaveCassette");
			try
			{
				GetCasset(conn, CassetteTodos, 1);
				GetCasset(conn, CassetteTodos, 2);

				SaveFileSdg(CassetteTodos);
			}
			CATCH(PdfLogger, "");
		}


		void GetCassettes::CorrectSQL(PGConnection& conn)
		{
			try
			{
				P0.erase(P0.begin(), P0.end());

				SetWindowText(hWndDebug, "connection");

				GetCasset(conn);

				SaveFileCass();
			}
			CATCH(PdfLogger, "");

			SetWindowText(hWndDebug, "��������� ���������");
		}

		GetCassettes::GetCassettes(PGConnection& conn, std::string datestart, std::string datestop)
		{
			try
			{
				DateStart = datestart;
				DateStop = datestop;

				if(!DateStart.length())
					return;

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

	};
		
	typedef std::vector<T_Todos> MapTodos;


	void GetTodosSQL(PGConnection& conn, MapTodos& mt, std::string& comand)
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

	namespace SHEET
	{


		class GetSheets
		{
		public:
			//GenSeqToHmi.Seq_1_StateNo
			const int st1_3 = 3;	//������� ������� �����
			const int st1_4 = 4;	//�������� � ����
			const int st1_5 = 5;	//������� ������� �����
			const int st1_6 = 6;	//������ �����
			const int st1_7 = 7;	//�������� �� 2 ��������
			const int st1_8 = 8;	//�������� �� 2-� �������� ����

			//GenSeqToHmi.Seq_2_StateNo
			const int st2_3 = 3;	//����� ��������� � 1-�� ��������� ����
			const int st2_4 = 4;	//���������.������ �����
			const int st2_5 = 5;	//������� �������� �����
			const int st2_6 = 6;	//������ � ����� �������
			const int st2_7 = 7;	//������� �������� �����

			//GenSeqToHmi.Seq_3_StateNo
			const int st3_3 = 3;	//������ ���������
			const int st3_4 = 4;	//��������� ����� ���������

			int AllId;
			int iAllId;

			std::string StartSheet;
			std::string StopSheet;

			void SaveBodyCsv(std::fstream& s1, T_IdSheet& ids, std::string Error);
			std::fstream SaveHeadCsv(std::string name);
			bool isSheet(T_IdSheet& t);

			T_IdSheet GetIdSheet(PGConnection& conn, std::string Start, size_t count, size_t i);

			float GetTime(std::string Start, std::vector<T_Todos>& allTime);

			void GetAllTime(PGConnection& conn, std::vector<T_Todos>& allTime);

			void GetMask(uint16_t v, std::string& MaskV);

			//��������� �� ������ ����� � ���� State_1 = 3;
			void GetDataSheet1(PGConnection& conn, T_IdSheet& ids);

			//��������� �� ������ ����� � ���� State_2 = 5;
			void GetDataSheet2(PGConnection& conn, T_IdSheet& ids);

			//��������� �� ������ ����� � ���� State_2 = 5 ���� 5 ������;
			void GetDataSheet3(PGConnection& conn, T_IdSheet& ids);

			void GetSeq_1(PGConnection& conn, MapTodos& allSheetTodos);
			void GetSeq_2(PGConnection& conn, MapTodos& allSheetTodos);
			void GetSeq_3(PGConnection& conn, MapTodos& allSheetTodos);
			void GetSeq_6(PGConnection& conn, MapTodos& allSheetTodos);

			float GetDataTime_All(std::string TimeStart, std::string Start3);

			void Get_ID_S(PGConnection& conn, T_IdSheet& td);
			int Get_ID_C(PGConnection& conn, T_IdSheet& td);

			std::fstream fUpdateSheet;
			void UpdateSheet(PGConnection& conn, T_IdSheet& td);

			bool InZone1(PGConnection& conn, std::string create_at, size_t count, size_t i);
			bool InZone2(PGConnection& conn, std::string create_at);
			bool InZone3(PGConnection& conn, std::string create_at);
			bool InZone5(PGConnection& conn, std::string create_at);

			void InZone62(PGConnection& conn, T_IdSheet& ids, std::string create_at);
			void InZone6(PGConnection& conn, std::string create_at);

			std::fstream SaveStepHeadCsv(std::string name);
			void SaveStepBodyCsv(std::fstream& ss1, T_Todos& td);

			std::fstream SaveT_IdSheetHeadCsv(std::string name);
			void SaveT_IdSheetBodyCsv(std::fstream& ss1, T_IdSheet& ids);
			void GetSheetCassette(PGConnection& conn, T_IdSheet& ids);

			GetSheets(PGConnection& conn, std::string datestart, std::string datestop = "");

			V_Todos allTime;

			T_IdSheet Ids1;
			T_IdSheet Ids2;
			T_IdSheet Ids3;

			T_IdSheet Ids5;

			V_IdSheet Ids6;

			std::fstream ss1;
		};



		void GetSheets::SaveBodyCsv(std::fstream& s1, T_IdSheet& ids, std::string Error)
		{

			std::stringstream ssd;

			ssd << ids.id << ";";

			ssd << " " << GetStringData(ids.Start1) << ";";
			ssd << " " << GetStringData(ids.Start2) << ";";
			ssd << " " << GetStringData(ids.Start3) << ";";
			ssd << " " << GetStringData(ids.Start5) << ";";


			ssd << " " << GetStringData(ids.DataTime_End) << ";";
			ssd << " " << GetStringData(ids.InCant) << ";";
			ssd << " " << GetStringData(ids.Cant) << ";";
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

			ssd << ids.year << ";";
			ssd << ids.month << ";";
			ssd << ids.day << ";";
			ssd << ids.cassetteno << ";";
			ssd << ids.sheetincassette << ";";
			ssd << ids.Pos << ";";

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

		std::fstream GetSheets::SaveHeadCsv(std::string name)
		{
			std::fstream ss1(name, std::fstream::binary | std::fstream::out);

			//����, ����� �������� ����� � ���������� ����
			//����� �����
			//������� �����, ��
			//������
			//������
			//�����
			//����� �����
			//�������� �����������
			//�������� ������, ��/�
			//
			//���������� ������. �������� ���� � ����
			//���������� ������. ����������� ���� � ����
			//���������� ������. �������� � ������� ����������
			//���������� ������. �������� ������ ����������
			//���������� ������. ��������� ������� ����
			//���������� ������. ��������� ������� ���
			// 
			//���������� ������1  ��������� ������� ����
			//���������� ������1  ��������� ������� ���
			//���������� ������2  ��������� ������� ����
			//���������� ������2  ��������� ������� ���
			//����������� ���� � �������

			ss1
				<< "Id;"
				<< "� ���� 1;"
				<< "� ���� 2;"
				<< "� ����������;"
				<< "� ��������;"
				<< "DataTime_End;"
				<< "� ��������;"
				<< "��������;"

				<< "DataTime_All;"
				<< "TimeForPlateHeat;"
				<< "TimeStart;"

				<< "�����;"
				<< "�������;"
				<< "������;"
				<< "������;"
				<< "�����;"
				<< "����;"
				<< "�������;"
				<< "TempSet1;"
				<< "UnloadSpeed;"
				<< "Za_PT3;"
				<< "Za_TE3;"
				<< "LaminPressTop;"
				<< "LaminPressBot;"
				<< "SpeedTopSet;"
				<< "SpeedBotSet;"
				<< "������������;"
				<< "LAM1_BotSet;"
				<< "LAM1_TopSet;"
				<< "LAM2_TopSet;"
				<< "LAM2_BotSet;"
				<< "LAM_TE1;"

				<< "Year;"
				<< "Month;"
				<< "Day;"
				<< "CassetteNo;"
				<< "SheetInCassette;"
				<< "Pos;"

				<< "������;"
				<< "TimeTest;"

				<< std::endl;
			return ss1;
		}

		bool GetSheets::isSheet(T_IdSheet& t)
		{
			return t.Melt && t.Pack && t.PartNo && t.Sheet;
		}

		T_IdSheet GetSheets::GetIdSheet(PGConnection& conn, std::string Start, size_t count, size_t i)
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

				MapTodos idSheet1;
				GetTodosSQL(conn, idSheet1, comand1);
				for(auto t : idSheet1)
				{
					if(t.id_name == PlateData[0].AlloyCodeText->ID) ids.Alloy = t.value;
					if(t.id_name == PlateData[0].ThiknessText->ID) ids.Thikness = t.value;

					if(t.id_name == PlateData[0].Melt->ID)		ids.Melt = t.content.As<int32_t>();
					if(t.id_name == PlateData[0].Pack->ID)		ids.Pack = t.content.As<int32_t>();
					if(t.id_name == PlateData[0].PartNo->ID)	ids.PartNo = t.content.As<int32_t>();
					if(t.id_name == PlateData[0].Sheet->ID)		ids.Sheet = t.content.As<int32_t>();
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

				MapTodos idSheet;
				GetTodosSQL(conn, idSheet, comand);
				for(auto t : idSheet)
				{
					if(t.id_name == PlateData[0].SubSheet->ID) ids.SubSheet = t.content.As<int32_t>();
				}
			}
			return ids;
		}

		float GetSheets::GetTime(std::string Start, std::vector<T_Todos>& allTime)
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

		void GetSheets::GetAllTime(PGConnection& conn, std::vector<T_Todos>& allTime)
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

		void GetSheets::GetMask(uint16_t v, std::string& MaskV)
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

		//��������� �� ������ ����� � ���� State_1 = 3;
		void GetSheets::GetDataSheet1(PGConnection& conn, T_IdSheet& ids)
		{
			MapTodos DataSheetTodos;

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

		//��������� �� ������ ����� � ���� State_2 = 5;
		void GetSheets::GetDataSheet2(PGConnection& conn, T_IdSheet& ids)
		{
			MapTodos DataSheetTodos;

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

		//��������� �� ������ ����� � ���� State_2 = 5 ���� 5 ������;
		void GetSheets::GetDataSheet3(PGConnection& conn, T_IdSheet& ids)
		{
			MapTodos DataSheetTodos;
			std::tm tmp;
			time_t t1 = DataTimeOfString(ids.DataTime_End, tmp) + 5;
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

		void GetSheets::GetSeq_1(PGConnection& conn, MapTodos& allSheetTodos)
		{
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
			std::string comand = ssd.str();
			GetTodosSQL(conn, allSheetTodos, comand);

		}
		void GetSheets::GetSeq_2(PGConnection& conn, MapTodos& allSheetTodos)
		{
			std::stringstream ssd;
			ssd << "SELECT create_at, id, id_name, content ";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE";
			ssd << " id_name = " << GenSeqToHmi.Seq_2_StateNo->ID;
			ssd << " AND CAST(content AS integer) >= " << st2_3;
			ssd << " AND CAST(content AS integer) <> " << st2_4;
			ssd << " AND create_at >= '" << StartSheet << "'";
			if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
			ssd << " ORDER BY id;";

			std::string comand = ssd.str();
			GetTodosSQL(conn, allSheetTodos, comand);
		}

		void GetSheets::GetSeq_3(PGConnection& conn, MapTodos& allSheetTodos)
		{
			std::stringstream ssd;
			ssd << "SELECT create_at, id, id_name, content ";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE";
			ssd << " id_name = " << GenSeqToHmi.Seq_3_StateNo->ID;
			ssd << " AND (CAST(content AS integer) = " << st3_3;
			ssd << " OR CAST(content AS integer) = " << st3_4;
			ssd << " ) AND create_at >= '" << StartSheet << "'";
			if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
			ssd << " ORDER BY id;";

			std::string comand = ssd.str();
			GetTodosSQL(conn, allSheetTodos, comand);
		}
		void GetSheets::GetSeq_6(PGConnection& conn, MapTodos& allSheetTodos)
		{
			std::stringstream ssd;
			ssd << "SELECT create_at, id, id_name, content ";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE";
			ssd << " id_name = " << HMISheetData.NewData->ID;
			ssd << " AND create_at >= '" << StartSheet << "'";
			if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
			ssd << " ORDER BY id;";

			std::string comand = ssd.str();
			GetTodosSQL(conn, allSheetTodos, comand);
		}


		float GetSheets::GetDataTime_All(std::string TimeStart, std::string Start3)
		{
			return float(DataTimeDiff(Start3, TimeStart)) / 60.0f;
		}

		void GetSheets::Get_ID_S(PGConnection& conn, T_IdSheet& td)
		{
			std::stringstream ssd;
			ssd << "SELECT id, day, month, year, cassetteno, sheetincassette, pos, news FROM sheet ";
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
				td.sheetincassette = Stoi(conn.PGgetvalue(res, 0, 5));
				td.Pos = Stoi(conn.PGgetvalue(res, 0, 6));
				td.news = Stoi(conn.PGgetvalue(res, 0, 7));

			}
			PQclear(res);
		}

		int GetSheets::Get_ID_C(PGConnection& conn, T_IdSheet& td)
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

		void GetSheets::UpdateSheet(PGConnection& conn, T_IdSheet& td)
		{
			if(!td.Start1.length() || !td.DataTime_End.length()) return;

			if(td.id)
			{
				std::stringstream ssd;
				ssd << "UPDATE sheet SET";
				
				ssd << " start_at = '" << td.Start1 << "'";				//����, ����� �������� ����� � ���������� ����
				ssd << ", datatime_end = '" << td.DataTime_End << "'";		//����, ����� ������ ����� �� ���������� ����

				if(td.Start2.length())			ssd << ", secondpos_at = '" << td.Start2 << "'";		//������ �� ������ ����
				if(td.InCant.length())			ssd << ", incant_at = '" << td.InCant << "'";		//������ �� ��������
				if(td.Cant.length())			ssd << ", cant_at = '" << td.Cant << "'";			//���������

				ssd << ", alloy = '" << td.Alloy << "'";				//����� �����
				ssd << ", thikness = '" << td.Thikness << "'";			//������� �����, ��

				ssd << ", timeforplateheat = " << td.TimeForPlateHeat;	//������� ����� ���������� ����� � ���������� ����. ���
				ssd << ", datatime_all = " << td.DataTime_All;			//���� ����� ���������� ����� � ���������� ����. ���

				ssd << ", speed = " << td.Speed;						//�������� ��������
				ssd << ", temper = " << td.Temper;						//������� �����������
				ssd << ", prestostartcomp = " << td.PresToStartComp;				//�������� ��������
				ssd << ", posclapantop = " << td.SpeedTopSet;			//������. ���������� ������. ����
				ssd << ", posclapanbot = " << td.SpeedBotSet;			//������. ���������� ������. ���
				ssd << ", lam1posclapantop = " << td.Lam1PosClapanTop;		//������. ���������� ������ 1. ����
				ssd << ", lam1posclapanbot = " << td.Lam1PosClapanBot;		//������. ���������� ������ 1. ���
				ssd << ", lam2posclapantop = " << td.Lam2PosClapanTop;		//������. ���������� ������ 2. ����
				ssd << ", lam2posclapanbot = " << td.Lam2PosClapanBot;		//������. ���������� ������ 2. ���
				ssd << ", mask = '" << td.Mask << "'";					//����� ������ �������
				//��������� �� ������ �� ���� State_2 = 5;
				ssd << ", lam_te1 = " << td.LAM_TE1;					//����������� ���� � �������
				ssd << ", za_te3 = " << td.Za_TE3;						//����������� ���� � ����
				ssd << ", za_pt3 = " << td.Za_PT3;						//�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)

				//��������� �� ������ �� ���� State_2 = 5 ���� 5 ������;
				ssd << ", lampresstop = " << td.LaminPressTop;			//�������� � ������� ����������
				ssd << ", lampressbot = " << td.LaminPressBot;			//�������� � ������ ����������

				ssd << ", hour = '" << td.hour << "'";
				ssd << ", day = '" << td.day << "'";
				ssd << ", month = '" << td.month << "'";
				ssd << ", year = '" << td.year << "'";
				ssd << ", cassetteno = " << td.cassetteno;
				ssd << ", sheetincassette = " << td.sheetincassette;
				ssd << ", pos = " << td.Pos;
				ssd << ", news = " << td.news;
				
				

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
			}
			else
			{
				if(iAllId) td.id = iAllId++;
				std::stringstream ssd;
				ssd << "INSERT INTO sheet (";
				ssd << "create_at, ";				//����, ����� �������� ����� � ���������� ����
				ssd << "start_at, ";				//����, ����� �������� ����� � ���������� ����
				"datatime_end, ";					//����, ����� ������ ����� �� ���������� ����

				if(td.Start2.length())			ssd << "secondpos_at, ";	//������ �� ������ ����
				if(td.InCant.length())			ssd << "incant_at, ";		//������ �� ��������
				if(td.Cant.length())			ssd << "cant_at, ";			//���������

				ssd << "datatime_all,";			//���� ����� ���������� ����� � ���������� ����. ���
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

				ssd << "lampresstop, ";				//�������� � ������� ����������
				ssd << "lampressbot, ";				//�������� � ������ ����������
				ssd << "lam_te1, ";					//����������� ���� � �������
				ssd << "za_te3, ";					//����������� ���� � ����
				ssd << "za_pt3, ";					//�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)

				ssd << " day, ";
				ssd << " month, ";
				ssd << " year, ";
				ssd << " cassetteno, ";
				ssd << " sheetincassette, ";


				ssd << "correct, ";
				ssd << "pdf, ";
				ssd << "pos, ";
				ssd << "news";


				ssd << ") VALUES (";
				ssd << "'" << td.DataTime << "', ";
				ssd << "'" << td.Start1 << "', ";
				ssd << "'" << td.DataTime_End << "', ";

				if(td.Start2.length())			ssd << "'" << td.Start2 << "', ";	//������ �� ������ ����
				if(td.InCant.length())			ssd << "'" << td.InCant << "', ";	//������ �� ��������
				if(td.Cant.length())			ssd << "'" << td.Cant << "', ";		//���������

				ssd << td.DataTime_All << ", ";
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

				ssd << "'" << td.day << "', ";
				ssd << "'" << td.month << "', ";
				ssd << "'" << td.year << "', ";
				ssd << td.cassetteno << ", ";
				ssd << td.sheetincassette << ", ";

				ssd << "now(), '', ";
				ssd << td.Pos << ", ";
				ssd << td.news << ");";

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
			}
		}

		bool GetSheets::InZone1(PGConnection& conn, std::string create_at, size_t count, size_t i)
		{
			T_IdSheet ids = GetIdSheet(conn, create_at, count, i);

			std::stringstream sss;
			sss << "InZone1 �" << i;
			sss << " Id: " << ids.id;
			sss << " Melt: " << ids.Melt;
			sss << " PartNo: " << ids.PartNo;
			sss << " Pack: " << ids.Pack;
			sss << " Sheet: " << ids.Sheet;
			sss << " SubSheet: " << ids.SubSheet;
			SetWindowText(hWndDebug, sss.str().c_str());

			if(isSheet(ids))
			{
				if(isSheet(Ids1))
				{
					InZone2(conn, create_at);
				}

				Ids1 = ids;
				Ids1.DataTime = create_at;
				Ids1.Start1 = create_at;

			}
			return true;
		}

		bool GetSheets::InZone2(PGConnection& conn, std::string create_at)
		{
			if(isSheet(Ids1))
			{
				if(isSheet(Ids2))
				{
					InZone3(conn, create_at);
				}

				Ids2 = Ids1;
				Ids2.Start2 = create_at;
				Ids1 = T_IdSheet();

				std::stringstream sss;
				sss << "InZone2";
				sss << " Id: " << Ids2.id;
				sss << " Melt: " << Ids2.Melt;
				sss << " PartNo: " << Ids2.PartNo;
				sss << " Pack: " << Ids2.Pack;
				sss << " Sheet: " << Ids2.Sheet;
				sss << " SubSheet: " << Ids2.SubSheet;
				SetWindowText(hWndDebug, sss.str().c_str());

			}
			return true;
		}

		bool GetSheets::InZone3(PGConnection& conn, std::string create_at)
		{
			if(isSheet(Ids2))
			{
				if(isSheet(Ids3))
				{
					InZone5(conn, create_at);
				}
				Ids3 = Ids2;
				Ids3.DataTime_End = create_at;
				Ids3.Start3 = create_at;
				Ids2 = T_IdSheet();

				std::stringstream sss;
				sss << "InZone3";
				sss << " Id: " << Ids3.id;
				sss << " Melt: " << Ids3.Melt;
				sss << " PartNo: " << Ids3.PartNo;
				sss << " Pack: " << Ids3.Pack;
				sss << " Sheet: " << Ids3.Sheet;
				sss << " SubSheet: " << Ids3.SubSheet;
				SetWindowText(hWndDebug, sss.str().c_str());
			}
			return true;
		}
		bool GetSheets::InZone5(PGConnection& conn, std::string create_at)
		{
			if(isSheet(Ids3))
			{
				if(isSheet(Ids5))
				{
					InZone6(conn, create_at);
				}

				if(isSheet(Ids3))
				{
					Ids5 = Ids3;
					//Ids5.Start5 = create_at;
					Ids5.Start5 = create_at;
					Ids5.InCant = create_at;
					Ids3 = T_IdSheet();

					std::stringstream sss;
					sss << "InZone5";
					sss << " Id: " << Ids5.id;
					sss << " Melt: " << Ids5.Melt;
					sss << " PartNo: " << Ids5.PartNo;
					sss << " Pack: " << Ids5.Pack;
					sss << " Sheet: " << Ids5.Sheet;
					sss << " SubSheet: " << Ids5.SubSheet;
					SetWindowText(hWndDebug, sss.str().c_str());
				}
			}
			return true;
		}
		void GetSheets::GetSheetCassette(PGConnection& conn, T_IdSheet& ids)
		{
			int Hour = 0;
			int Day = 0;
			int Month = 0;
			int Year = 0;
			int CassetteNo = 0;
			int SheetInCassette = 0;

			MapTodos DataSheetTodos;
			std::stringstream ssd;
			ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content";
			ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
			ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
			ssd << "FROM todos WHERE ";
			//ssd << "create_at >= '" << ids.Start3 << "' AND ";
			ssd << "create_at < '" << ids.Cant << "' AND (";
			ssd << "id_name = " << HMISheetData.Cassette.Hour->ID << " OR ";
			ssd << "id_name = " << HMISheetData.Cassette.Day->ID << " OR ";
			ssd << "id_name = " << HMISheetData.Cassette.Month->ID << " OR ";
			ssd << "id_name = " << HMISheetData.Cassette.Year->ID << " OR ";
			ssd << "id_name = " << HMISheetData.Cassette.CassetteNo->ID << " OR ";
			ssd << "id_name = " << HMISheetData.Cassette.SheetInCassette->ID << ") ";
			ssd << "ORDER BY id_name, id DESC;";

			std::string comand = ssd.str();
			GetTodosSQL(conn, DataSheetTodos, comand);

			for(auto a : DataSheetTodos)
			{
				if(a.id_name == HMISheetData.Cassette.Hour->ID) Hour = Stoi(a.value);
				if(a.id_name == HMISheetData.Cassette.Day->ID) Day = Stoi(a.value);
				if(a.id_name == HMISheetData.Cassette.Month->ID) Month = Stoi(a.value);
				if(a.id_name == HMISheetData.Cassette.Year->ID) Year = Stoi(a.value);
				if(a.id_name == HMISheetData.Cassette.CassetteNo->ID) CassetteNo = Stoi(a.value);
				if(a.id_name == HMISheetData.Cassette.SheetInCassette->ID) SheetInCassette = Stoi(a.value);
			}

			if(Day && Month && Year && CassetteNo)
			{
				ids.hour = Hour;
				ids.day = Day;
				ids.month = Month;
				ids.year = Year;
				ids.cassetteno = CassetteNo;
				ids.sheetincassette = SheetInCassette + 1;
			}

			if(ids.day && ids.month && ids.year && ids.cassetteno)
			{
				ids.news = 1;
				ids.Pos = 7;
			}
			else
			{
				ids.news = 0;
				ids.Pos = 16;
			}
		}


		void GetSheets::InZone62(PGConnection& conn, T_IdSheet& Ids, std::string create_at)
		{
			T_IdSheet ids = Ids;

			std::stringstream sss;
			sss << "InZone6 ";
			sss << "Id: " << ids.id;
			sss << "Melt: " << ids.Melt;
			sss << "PartNo: " << ids.PartNo;
			sss << "Pack: " << ids.Pack;
			sss << "Sheet: " << ids.Sheet;
			sss << "SubSheet: " << ids.SubSheet;
			SetWindowText(hWndDebug, sss.str().c_str());

			Ids = T_IdSheet();

			ids.Cant = create_at;
			GetDataSheet1(conn, ids);
			GetDataSheet2(conn, ids);
			GetDataSheet3(conn, ids);
			Get_ID_S(conn, ids);

			//if(!ids.DataTime_End.length()) return;
			if(!ids.Start2.length())
			{
				std::tm TM;
				time_t tm1 = DataTimeOfString(ids.DataTime_End, TM);
				time_t tm2 = DataTimeOfString(ids.Start1, TM);

				time_t tm = time_t(tm1 + (difftime(tm1, tm2) / 2.0));
				ids.Start2 = GetDataTimeString(&tm);
			}

			ids.TimeForPlateHeat = GetTime(ids.Start2, allTime);

			ids.DataTime_All = GetDataTime_All(ids.Start1, ids.DataTime_End);
			float f = fabsf(ids.DataTime_All - ids.TimeForPlateHeat);
			if(f > 2.0f)
				ids.DataTime_All = GetDataTime_All(ids.Start1, ids.DataTime_End);


			GetSheetCassette(conn, ids);
			SaveBodyCsv(ss1, ids, "");

			UpdateSheet(conn, ids);
			Ids6.push_back(ids);
		}

		void GetSheets::InZone6(PGConnection& conn, std::string create_at)
		{
			if(isSheet(Ids5))
			{
				InZone62(conn, Ids5, create_at);
			}
			else if(isSheet(Ids3))
			{
				InZone62(conn, Ids3, create_at);
			}
		}

		std::fstream GetSheets::SaveStepHeadCsv(std::string name)
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
		void GetSheets::SaveStepBodyCsv(std::fstream& ss1, T_Todos& td)
		{
			ss1 << " " << td.create_at << ";";
			ss1 << td.id << ";";
			if(td.id_name == GenSeqToHmi.Seq_1_StateNo->ID)
				ss1 << GenSeq1[Stoi(td.value)] << ";";
			else if(td.id_name == GenSeqToHmi.Seq_2_StateNo->ID)
				ss1 << GenSeq2[Stoi(td.value)] << ";";
			else if(td.id_name == GenSeqToHmi.Seq_3_StateNo->ID)
				ss1 << GenSeq3[Stoi(td.value)] << ";";
			else
				ss1 << td.value << ";";
			ss1 << td.id_name << ";";
			ss1 << td.id_name_at << ";";
			ss1 << std::endl;
		}

		std::fstream GetSheets::SaveT_IdSheetHeadCsv(std::string name)
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
		void GetSheets::SaveT_IdSheetBodyCsv(std::fstream& ss1, T_IdSheet& ids)
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

		bool cmpData(T_Todos& first, T_Todos& second)
		{
			return first.id < second.id;
		}

		GetSheets::GetSheets(PGConnection& conn, std::string datestart, std::string datestop)
		{
			StartSheet = datestart;
			StopSheet = datestop;

			MapTodos allSheetTodos;

			
			remove("UpdateSheet.txt");
			fUpdateSheet = std::fstream("UpdateSheet.txt", std::fstream::binary | std::fstream::out | std::ios::app);


			ss1 = SaveHeadCsv("Sheet.csv");


			GetAllTime(conn, allTime);

			//GenSeqToHmi.Seq_1_StateNo
			GetSeq_1(conn, allSheetTodos);

			//GenSeqToHmi.Seq_2_StateNo
			GetSeq_2(conn, allSheetTodos);
			
			//GenSeqToHmi.Seq_3_StateNo
			GetSeq_3(conn, allSheetTodos);

			//HMISheetData.NewData
			GetSeq_6(conn, allSheetTodos);


			//��������� �� �������
			std::sort(allSheetTodos.begin(), allSheetTodos.end(), cmpData);

			////GenSeqToHmi.Seq_3_StateNo
			//GetSeq_3(conn);


			size_t count = allSheetTodos.size();
			size_t i = count;
			std::fstream ff1 = SaveStepHeadCsv("Step.csv");
			std::fstream ff2 = SaveT_IdSheetHeadCsv("T_IdSheet.csv");


			for(MapTodos::iterator it = allSheetTodos.begin(); isRun && it != allSheetTodos.end(); it++)
			{
				T_Todos& td = *it;

				SaveStepBodyCsv(ff1, td);

				if(td.id_name == GenSeqToHmi.Seq_1_StateNo->ID) //�������� � ����
				{
					int16_t st = td.content.As<int16_t>();
					if(st == st1_3) //������� ������� �����
					{
						//if(!InZone1(conn, Ids1, td.create_at, count, i))
						//{
						//	if(!InZone2(conn, Ids1, Ids2, td.create_at))
						//		if(!InZone3(conn, Ids1, Ids3, td.create_at))
						//			InZone6(conn, Ids1, Ids6, allTime, ss1, td.create_at);
						//
						//	InZone1(conn, Ids1, td.create_at, count, i);
						//}
						InZone1(conn, td.create_at, count, i);
						SaveT_IdSheetBodyCsv(ff2, Ids1);
					}
					else if(st == st1_4 || st == st1_5) //"�������� � ����" || "������� ������� �����"
					{
						if(Ids1.CloseInDor.length())
						{
							if(!isSheet(Ids1))
							{
								InZone1(conn, td.create_at, count, i);
								SaveT_IdSheetBodyCsv(ff2, Ids1);
							}
							if(isSheet(Ids1))
							{
								Ids1.CloseInDor = td.create_at;
								Ids1.Start1 = td.create_at;
							}
						}
					}
					else if(st == st1_6) //"������ �����"
					{
						if(!Ids1.Nagrev.length())
						{
							if(!isSheet(Ids1))
							{
								InZone1(conn, td.create_at, count, i);
								SaveT_IdSheetBodyCsv(ff2, Ids1);
							}
							if(isSheet(Ids1))
							{
								Ids1.Nagrev = td.create_at;
								Ids1.Start1 = td.create_at;
							}
						}
					}
					else if(st == st1_7 || st == st1_8) 	//"�������� �� 2 ��������" || "�������� �� 2-� �������� ����"
					{
						//if(!InZone2(conn, Ids1, Ids2, td.create_at))
						//{
						//	if(!InZone3(conn, Ids2, Ids3, td.create_at))
						//		InZone6(conn, Ids2, Ids6, allTime, ss1, td.create_at);
						//	InZone2(conn, Ids1, Ids2, td.create_at);
						//}
						InZone2(conn, td.create_at);
					}
				}

				else if(td.id_name == GenSeqToHmi.Seq_2_StateNo->ID) //������ ���� ����
				{
					int16_t st = td.content.As<int16_t>();
					if(st == st2_3)	//3 = ����� ��������� � 1-�� ��������� ����
					{
						//if(!InZone2(conn, Ids1, Ids2, td.create_at))
						//{
						//	if(!InZone3(conn, Ids2, Ids3, td.create_at))
						//		InZone6(conn, Ids2, Ids6, allTime, ss1, td.create_at);
						//
						//	InZone2(conn, Ids1, Ids2, td.create_at);
						//}
						InZone2(conn, td.create_at);
					}
					else if(st == st2_5 || st == st2_6 || st == st2_7) //�������� �� ���� - "������� �������� �����" || ������ � ����� �������" || "������� �������� �����"
					{
						//if(!InZone3(conn, Ids2, Ids3, td.create_at))
						//{
						//	InZone6(conn, Ids3, Ids6, allTime, ss1, td.create_at);
						//	InZone3(conn, Ids2, Ids3, td.create_at);
						//}
						InZone3(conn, td.create_at);
					}
				}
				else if(td.id_name == GenSeqToHmi.Seq_3_StateNo->ID) //����������
				{
					int16_t st = td.content.As<int16_t>();
					if(st == st2_3 || st == st2_4)	//3 = ������ ���������; 4 - ��������� ����� ���������
						InZone5(conn, td.create_at);
				}
				else if(td.id_name == HMISheetData.NewData->ID)
				{
					InZone6(conn, td.create_at);
				}

				i--;
			}
			ff2.close();
			ff1.close();
			ss1.close();
			fUpdateSheet.close();
			INT II = 0;
		}

	};

		//������������ �����������. ���������� ������ ����� �������
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

	void CopyAllFile()
	{
		//namespace fs = std::filesystem;
		const char* dir1 = lpLogPdf2.c_str();
		const char* dir2 = lpLogPdf.c_str();

		try
		{
			std::filesystem::copy(dir1, dir2,
						std::filesystem::copy_options::update_existing
					 //|  std::filesystem::copy_options::overwrite_existing
					 |  std::filesystem::copy_options::recursive);
		}
		CATCH(PdfLogger, "");;
	}
	void DelAllPdf(std::filesystem::path dir)
	{
		boost::system::error_code ec;
		std::filesystem::remove_all(dir, ec);
		//std::string err = "";
		//if(ec)
		//{
		//	std::vector<boost::filesystem::path> patch = getDir(dir.string());
		//	for(auto& p : patch)
		//		remove(p.string().c_str());
		//	if(ec)
		//		LOG_ERROR(PdfLogger, "{:90}| Eor Delete dir: \"{}\"", FUNCTION_LINE_NAME, ec.message());
		//	patch.erase(patch.begin(), patch.end());
		//}
	}

#define HENDINSERT 0
#if HENDINSERT


	typedef struct T_casset{
		std::string Create;
		std::string End;
		int Hour = 0;
		int Day = 0;
		int Month = 0;
		int Year = 0;
		int CassetteNo = 0;
	}T_casset;

	typedef std::vector<T_casset>VT_casset;

	void GetSeq1(PGConnection& conn, MapTodos& allTodos)
	{
		std::stringstream ssd;
		ssd << "SELECT create_at, id, id_name, content ";
		ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
		ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
		ssd << "FROM todos WHERE";
		ssd << " id_name = " << HMISheetData.CasseteIsFill->ID;
		//ssd << " AND CAST(content AS INTEGER) >= " << st1_3;
		//ssd << " AND CAST(content AS INTEGER) <> " << st1_6;
		//ssd << " AND create_at >= '" << StartSheet << "'";
		//if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
		ssd << " ORDER BY id;";
		std::string comand = ssd.str();
		GetTodosSQL(conn, allTodos, comand);

	}
	void GetSeq2(PGConnection& conn, MapTodos& allTodos, std::string Start)
	{
		std::stringstream ssd;
		ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content ";
		ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
		ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
		ssd << "FROM todos WHERE (";
		ssd << " id_name = " << HMISheetData.Cassette.Hour->ID;
		ssd << " OR id_name = " << HMISheetData.Cassette.Day->ID;
		ssd << " OR id_name = " << HMISheetData.Cassette.Month->ID;
		ssd << " OR id_name = " << HMISheetData.Cassette.Year->ID;
		ssd << " OR id_name = " << HMISheetData.Cassette.CassetteNo->ID;
		//ssd << " AND CAST(content AS INTEGER) >= " << st1_3;
		//ssd << " AND CAST(content AS INTEGER) <> " << st1_6;
		ssd << ") AND create_at <= '" << Start << "'";
		//if(StopSheet.length())	ssd << " AND create_at < '" << StopSheet << "'";
		ssd << " ORDER BY id_name DESC, id;";
		std::string comand = ssd.str();
		GetTodosSQL(conn, allTodos, comand);
	}

	//��� ������� ���������� �����
	void HendInsetr(PGConnection& conn)
	{
		//MapTodos allCassetTodos;
		////MapTodos allTodos;
		//std::vector <T_casset> all_casset;
		//GetSeq1(conn, allCassetTodos);
		//
		//for(auto a : allCassetTodos)
		//{
		//	if(a.content.As<bool>())
		//	{
		//		std::stringstream ssd;
		//		ssd << "SELECT DISTINCT ON (id_name) create_at, id, id_name, content ";
		//		ssd << ", (SELECT type FROM tag WHERE tag.id = todos.id_name) ";
		//		ssd << ", (SELECT comment FROM tag WHERE tag.id = todos.id_name) ";
		//		ssd << "FROM todos WHERE (";
		//		ssd << " id_name = " << HMISheetData.Cassette.Hour->ID;
		//		ssd << " OR id_name = " << HMISheetData.Cassette.Day->ID;
		//		ssd << " OR id_name = " << HMISheetData.Cassette.Month->ID;
		//		ssd << " OR id_name = " << HMISheetData.Cassette.Year->ID;
		//		ssd << " OR id_name = " << HMISheetData.Cassette.CassetteNo->ID;
		//		ssd << ") AND create_at <= '" << a.create_at << "'";
		//		ssd << " ORDER BY id_name DESC, id DESC;";
		//		std::string comand = ssd.str();
		//		SetWindowText(hWndDebug, comand.c_str());
		//		PGresult* res = conn.PGexec(comand); 
		//		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		//		{
		//			int nFields = PQnfields(res);
		//			int line = PQntuples(res);
		//			if(line)
		//			{
		//				//float f = static_cast<float>(atof(conn.PGgetvalue(res, 0, 1).c_str()));
		//				T_casset tc;
		//				for(int l = 0; l < line; l++)
		//				{
		//					T_Todos td;
		//					td.create_at = conn.PGgetvalue(res, l, 0);
		//					td.id = Stoi(conn.PGgetvalue(res, l, 1));
		//					td.id_name = Stoi(conn.PGgetvalue(res, l, 2));
		//					td.value = conn.PGgetvalue(res, l, 3);
		//					int type = Stoi(conn.PGgetvalue(res, l, 4));
		//					td.content = PDF::GetVarVariant((OpcUa::VariantType)type, td.value);
		//					if(nFields >= 6)
		//						td.id_name_at = conn.PGgetvalue(res, l, 5);
		//
		//					if(td.id_name == HMISheetData.Cassette.Hour->ID)tc.Hour = Stoi(td.value);
		//					if(td.id_name == HMISheetData.Cassette.Day->ID)tc.Day = Stoi(td.value);
		//					if(td.id_name == HMISheetData.Cassette.Month->ID)tc.Month = Stoi(td.value);
		//					if(td.id_name == HMISheetData.Cassette.Year->ID)tc.Year = Stoi(td.value);
		//					if(td.id_name == HMISheetData.Cassette.CassetteNo->ID)tc.CassetteNo = Stoi(td.value);
		//				}
		//				if(tc.Day && tc.Month && tc.Year && tc.CassetteNo)
		//				{
		//					tc.Create = a.create_at;
		//					all_casset.push_back(tc);
		//				}
		//			}
		//		}
		//		PQclear(res);
		//		//GetSeq2(conn, allTodos, a.create_at);
		//		//T_casset tc;
		//		//std::stringstream ssd;
		//		//ssd << "SELECT create_at, hour, day, month, year, cassetteno, WHERE create_at <=" << a.create_at << " ORDER BY id DESC id LIMIT 1";
		//	}
		//}
		//
		//int tt = 0;
		//{
			std::string comand = "SELECT * FROM cassette WHERE id = 664"; //2024-05-25-06
			PGresult* res = conn.PGexec(comand);
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
			PdfClass sdc(Cassette);
	}
#endif // HENDINSERT

	//����� �������������� �������������
	DWORD WINAPI RunCassettelPdf(LPVOID)
	{
		//return 0;
#ifndef _DEBUG
		
#else

		try
		{
			//return 0;
			PdfLogger = InitLogger("Pdf Debug");
			PGConnection conn_pdf;
			conn_pdf.connection();

#if HENDINSERT
			//��� ������� ������������
			DelAllPdf(lpLogPdf2);
			HendInsetr(conn_pdf);
			isRun = false;
			return 0;
#else

			while(isRun)
			{
				PDF::Correct = true;
				//PDF::Correct = false;
				if(PDF::Correct)
				{
					////��������� � �������������� ��� �����
					//SHEET::StartSheet = "2024-01-01 00:00:00";
					//SHEET::AllId = 1356;
					//SHEET::iAllId = 1;
					//
					//����� ���������� �����:
					//SELECT * FROM sheet WHERE create_at > '2024-05-15 17:25:13.433' AND news <> 1 ORDER BY id DESC
					//PDF::SHEET::GetRawSheet(conn_pdf);
					//
					//
					//SHEET::StartSheet = "2024-05-25 04:30:00";
					//
					////��� �������������� ���������
					//int id = 0;
					//std::string comand = "SELECT create_at, id FROM sheet WHERE correct IS NOT NULL ORDER BY id DESC LIMIT 1";
					//std::string comand = "SELECT create_at, id FROM sheet WHERE correct IS NOT NULL ORDER BY id DESC LIMIT 1";
					//PGresult* res = conn_pdf.PGexec(comand);
					//if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
					//{
					//	SHEET::StartSheet = conn_pdf.PGgetvalue(res, 0, 0);
					//	id = Stoi(conn_pdf.PGgetvalue(res, 0, 1));
					//}
					//PQclear(res);
					//
					//comand = "SELECT create_at, id FROM sheet WHERE id > " + std::to_string(id) + " ORDER BY id ASC LIMIT 1";
					//res = conn_pdf.PGexec(comand);
					//if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
					//	SHEET::StartSheet = conn_pdf.PGgetvalue(res, 0, 0);
					//PQclear(res);
					//
					//SHEET::StartSheet = "2024-06-02 00:00:00.00";
					//SHEET::StopSheet  = "2024-05-31 11:00:00.00";
					//std::string StartSheet = "2024-05-15 00:00:00.00";
					//std::string comand = "SELECT create_at, id, correct FROM sheet WHERE correct <= '2024-06-05' AND create_at  > '2024-04-01' ORDER BY id ASC LIMIT 1";
					//PGresult* res = conn_pdf.PGexec(comand);
					//if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
					//	StartSheet = conn_pdf.PGgetvalue(res, 0, 0);
					//PQclear(res);

					std::string start = "2024-08-05 00:00:00";
					std::string stop = "";
					SHEET::GetSheets getsheet(conn_pdf, "2024-08-05 00:00:00", stop); // , "2024-03-30 00:00:00.00");// , "2024-05-19 01:00:00.00");

					DelAllPdf(lpLogPdf2);
					CASSETTE::GetCassettes getpdf(conn_pdf, start, stop); // , "2024-03-30 00:00:00.00");
					CopyAllFile();

					Correct = false;
				}
#ifdef _DEBUG
				//� ������ ���� ������ � ����� �� ���������
				isRun = false;
#else
				int TimeCount = 0;
				while(TimeCount++ < 60 && isRun)
					std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
				LOG_INFO(PdfLogger, "{:90}| Stop Pdf Debug", FUNCTION_LINE_NAME);
			}
#endif // HENDINSERT
		}
		CATCH(PdfLogger, "");;
#endif


		return 0;
	}
}



