#include "pch.h"
#include "win.h"
#include "main.h"
#include "StringData.h"
#include "hard.h"
#include "term.h"

#include "Graff.h"

std::shared_ptr<spdlog::logger> Graphics = NULL;

const int64_t SecCount1 = 60 * 60 * 24; //24 �����
const int64_t SecCount2 = int64_t(60.0f * 60.0f * 0.5f); //1 ���

HANDLE hGGraff1 = NULL;
HANDLE hGGraff2 = NULL;
HANDLE hGGraff3 = NULL;


HRESULT GetGdiplusEncoderClsid(const std::wstring& format, GUID* pGuid)
{
	try
	{
		HRESULT hr = S_OK;
		UINT  nEncoders = 0;          // number of image encoders
		UINT  nSize = 0;              // size of the image encoder array in bytes
		std::vector<BYTE> spData;
		Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
		Gdiplus::Status status;
		bool found = false;

		if(format.empty() || !pGuid)
		{
			hr = E_INVALIDARG;
		}

		if(SUCCEEDED(hr))
		{
			*pGuid = GUID_NULL;
			status = Gdiplus::GetImageEncodersSize(&nEncoders, &nSize);

			if((status != Gdiplus::Ok) || (nSize == 0))
			{
				hr = E_FAIL;
			}
		}

		if(SUCCEEDED(hr))
		{

			spData.resize(nSize);
			pImageCodecInfo = (Gdiplus::ImageCodecInfo*)&spData.front();
			status = Gdiplus::GetImageEncoders(nEncoders, nSize, pImageCodecInfo);

			if(status != Gdiplus::Ok)
			{
				hr = E_FAIL;
			}
		}

		if(SUCCEEDED(hr))
		{
			for(UINT j = 0; j < nEncoders && !found; j++)
			{
				if(pImageCodecInfo[j].MimeType == format)
				{
					*pGuid = pImageCodecInfo[j].Clsid;
					found = true;
				}
			}

			hr = found ? S_OK : E_FAIL;
		}

		return hr; \
	}
	CATCH(Graphics, "");
	return S_FALSE;
}

const int CountPoint = 86400; //����������� ������ � ������


GUID guidBmp ={};
GUID guidJpeg ={};
GUID guidGif ={};
GUID guidTiff ={};
GUID guidPng ={};

extern CGdiPlusInit init;

const int Xsize = 350;
const int Ysize1 = 137;
const int Ysize2 = 107;


Gdiplus::Font font0(L"Tahoma", 9, Gdiplus::FontStyleRegular);
Gdiplus::Font font1(L"Tahoma", 10, Gdiplus::FontStyleBold);
Gdiplus::Font font2(L"Tahoma", 10, Gdiplus::FontStyleRegular);


PGConnection connKPVL;
PGConnection connFurn1;
PGConnection connFurn2;
PGConnection connFurn;


Graff GraffKPVL("KPV");
Graff GraffFurn1("Furn1");
Graff GraffFurn2("Furn2");
Graff GraffFurn("Furn");

const float Mashtab = 1.5;

Graff::Graff(std::string n):Name(n)
{
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
	stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
};

void Graff::DrawGridOssi(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG)
{
	try
	{
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

		Gdiplus::RectF Rect1 = RectG;

		temp.TranslateTransform(0, RectG.Height);
		temp.RotateTransform(-90);

		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

		std::wstring theString = L"����������� Ѱ";

		Gdiplus::RectF boundRect ={0, 0, RectG.Height + 35, 20};
		temp.DrawString(theString.c_str(), -1, &font2, boundRect, &stringFormat, &Gdi_brush);
		temp.ResetTransform();

		Rect1.Height -= 2;

		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		theString = L"����� ���:���";
		temp.DrawString(theString.c_str(), -1, &font2, Rect1, &stringFormat, &Gdi_brush);

		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	}
	CATCH(Graphics, Name);
}

void Graff::DrawGridTemp(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
{
	try
	{
		for(auto a : TempRef)
		{
			maxt = std::fmaxf(maxt, a.second.second);
			mint = std::fminf(mint, a.second.second);
		}
		for(auto a : TempAct)
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
		} while(cstep > 6.0);

		f_maxt = fmaxt * fstep;
		f_mint = fmint * fstep;

		//Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 0.5); //������
		//Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));
		//Gdiplus::StringFormat stringFormat;
		//stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);

		double coeffH = (double)(Rect.Height - Rect.Y) / (double)(f_maxt - f_mint);


		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
		for(double d = f_mint; d <= f_maxt; d += fstep)
		{
			float mY = Rect.Y + float((f_maxt - d) * coeffH);
			Gdiplus::PointF pt1 ={Rect.X - 5,				mY};
			Gdiplus::PointF pt2 ={Rect.X + Rect.Width + 5,	mY};
			temp.DrawLine(&Gdi_L1, pt1, pt2);
			Gdiplus::RectF Rect2 ={Rect.X - 45, mY - 11, 40, 20};
			std::wstringstream sdw;
			sdw << std::setprecision(0) << std::setiosflags(std::ios::fixed) << d;
			temp.DrawString(sdw.str().c_str(), -1, &font1, Rect2, &stringFormat, &Gdi_brush);
		}
	}
	CATCH(Graphics, Name);
}

void Graff::DrawBottom(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, Gdiplus::Color& clor, T_SqlTemp& st)
{
	try
	{
		Gdiplus::Pen Gdi_L1(Gdiplus::Color(192, 192, 192), 0.5); //������
		Gdiplus::Pen Gdi_L2(clor, 1);
		auto b = st.begin();
		auto e = st.rbegin();
		//e--;
		double coeffW = (double)(Rect.Width) / double(maxd);
		double coeffH = (double)(Rect.Height - Rect.Y) / (double)(f_maxt - f_mint);

		Gdiplus::PointF p1 ={0, 0};
		Gdiplus::PointF p2;;
		p1.X = Rect.X;
		p1.Y = Rect.Y + float((f_maxt - b->second.second) * coeffH);

		for(auto& a : st)
		{
			p2.X =  Rect.X + float((a.second.first - mind) * coeffW);
			p2.Y =  Rect.Y + float((f_maxt - a.second.second) * coeffH);
			temp.DrawLine(&Gdi_L2, p1, p2);

			p1.X = p2.X;
			p1.Y = p2.Y;
		}
	}
	CATCH(Graphics, Name);
}

void Graff::DrawTimeText(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, std::wstring str, Gdiplus::StringFormat& stringFormat)
{
	try
	{
		Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));

		std::wstring::const_iterator start = str.begin();
		std::wstring::const_iterator end = str.end();
		boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}):\\d{1,2}.*");
		boost::match_results<std::wstring::const_iterator> what;

		if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 1)
			temp.DrawString(what[1].str().c_str(), -1, &font1, Rect, &stringFormat, &Gdi_brush);
	}
	CATCH(Graphics, Name);
}


void Graff::DrawT(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect, double sd, std::wstring sDataBeg)
{
	try
	{
		Gdiplus::RectF RectText(Rect);

		Gdiplus::RectF boundRect;
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

		temp.MeasureString(sDataBeg.c_str(), -1, &font1, RectText, &stringFormat, &boundRect);

		boundRect.X = Rect.X + float(sd);
		//boundRect.X -= 21;
		boundRect.X -= boundRect.Width / 2;
		boundRect.Height += 3;

		//temp.DrawRectangle(&pen, boundRect);

		Gdiplus::Pen p = Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), 0.1f);

		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		temp.DrawString(sDataBeg.c_str(), -1, &font1, boundRect, &stringFormat, &Gdi_brush);
	}
	CATCH(Graphics, Name);
}

void Graff::DrawGridTime(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
{
	try
	{
		auto tb = TempAct.begin();
		auto te = TempAct.rbegin();

		//std::tm TM;
		//std::string st1 = std::string (tb->first.begin(), tb->first.end());
		//std::string st2 = std::string (te->first.begin(), te->first.end());
		//time_t tm1 = DataTimeOfString(st1, TM);
		//time_t tm2 = DataTimeOfString(st2, TM);

		time_t tm1 = tb->second.first;
		time_t tm2 = te->second.first;


		double tm = difftime(tm2, tm1);
		double Count = 12.0;
		double Step = tm / Count;

		for(double e = 0; e <= Count; e++)
		{
			double sd = round(Rect.Width / Count * e);
			bool b = (bool)std::fmod(e, 2);

			Gdiplus::PointF pt1 ={Rect.X + float(sd), Rect.Y};
			Gdiplus::PointF pt2 ={Rect.X + float(sd), Rect.Height - (b ? 18 : 10)};
			temp.DrawLine(&Gdi_L1, pt1, pt2);

			if(!b)
			{
				double f = Step * e;
				double tmf = tm1 + f;
				time_t tms = time_t(tmf);

				std::string s = GetStringOfDataTime(&tms);
				std::wstring sDataBeg = GetData(std::wstring(s.begin(), s.end()));
				DrawT(temp, Rect, sd, sDataBeg);
			}
		}
	}
	CATCH(Graphics, Name);
}

void Graff::DrawInfo(Gdiplus::Graphics& temp, Gdiplus::RectF& Rect)
{
	//try
	//{
	//	Gdiplus::Pen Gdi_L1(Gdiplus::Color(255, 0, 0), 2); //�������
	//	Gdiplus::Pen Gdi_L2(Gdiplus::Color(0, 0, 255), 2); //�����
	//
	//	Gdiplus::PointF pt1 ={Rect.X + 0, Rect.Y + 0};
	//	Gdiplus::PointF pt2 ={Rect.X + 20, Rect.Y + 0};
	//	temp.DrawLine(&Gdi_L1, pt1, pt2);
	//
	//	pt1 ={Rect.X + 100, Rect.Y + 0};
	//	pt2 ={Rect.X + 120, Rect.Y + 0};
	//	temp.DrawLine(&Gdi_L2, pt1, pt2);
	//
	//
	//	Gdiplus::SolidBrush Gdi_brush(Gdiplus::Color(0, 0, 0));
	//	Gdiplus::StringFormat stringFormat;
	//	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
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
	//}
	//CATCH(Graphics, Name);
}


void Graff::DrawGraf(Gdiplus::Graphics& temp, Gdiplus::RectF& RectG)
{
	try
	{
		maxt = 0;
		mint = 9999;
		mind = std::min<int64_t>(TempAct.begin()->second.first, TempRef.begin()->second.first);

		auto b = TempAct.begin();
		auto e = TempAct.rbegin();
		//e--;
		maxd = std::max<int64_t>(MaxSecCount, e->second.first - b->second.first);
		//int64_t maxd = 0;// e->second.first - b->second.first;

		b = TempRef.begin();
		e = TempRef.rbegin();
		//e--;
		maxd = (std::max)(maxd, e->second.first - b->second.first);


		Gdiplus::Color Blue(0, 0, 255);
		Gdiplus::Color Red(255, 0, 0);

		Gdiplus::RectF RectG2(RectG);
		RectG2.Y += 5;
		RectG2.Height -= 40;

		RectG2.X += 50;
		RectG2.Width -= 75;

		Gdiplus::RectF RectG3(RectG2);
		RectG3.Height += 17;

		DrawGridTemp(temp, RectG2);
		DrawGridTime(temp, RectG3);
		DrawGridOssi(temp, RectG);

		DrawBottom(temp, RectG2, Red, TempRef);	//�������; �������� �������� �����������
		DrawBottom(temp, RectG2, Blue, TempAct);	//�����; ����������� �������� �����������
	}
	CATCH(Graphics, Name);
}

void Graff::Paint(HWND hWnd)
{
	try
	{
		if(!init.Good())return;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		RECT rcBounds;
		GetWindowRect(hWnd, &rcBounds);

		Gdiplus::REAL Width = Gdiplus::REAL(abs(rcBounds.right - rcBounds.left));
		Gdiplus::REAL Height = Gdiplus::REAL(abs(rcBounds.bottom - rcBounds.top));

		Gdiplus::Graphics g(hdc);
		Gdiplus::Bitmap backBuffer (INT(Width * Mashtab), INT(Height * Mashtab), &g);
		Gdiplus::Graphics temp(&backBuffer);
		Gdiplus::RectF RectG(0, 0, Width * Mashtab, Height * Mashtab);


		Gdiplus::RectF RectBottom(0, 0, Width, Height);
		temp.Clear(Gdiplus::Color(255, 255, 255));


		if(TempRef.size() && TempAct.size() && full)
		{
			DrawGraf(temp, RectG);

			std::wstring SaveFile(Name.begin(), Name.end());
			SaveFile += L".jpg";
			backBuffer.Save(SaveFile.c_str(), &guidJpeg, NULL);
		}

		g.DrawImage(&backBuffer, RectBottom);

		EndPaint(hWnd, &ps);
	}
	CATCH(Graphics, Name);
}

void SqlTempFURN(PGConnection* conn, T_SqlTemp& st, Value* val, int64_t SecCount)
{
	try
	{
		st.erase(st.begin(), st.end());

		std::tm TM_End ={0};
		std::tm TM_Beg ={0};
		std::tm TM_Temp ={0};
		std::string TecTime;
		time_t tStop1 = time(NULL);
		localtime_s(&TM_End, &tStop1);
		time_t tStop = _mkgmtime(&TM_End);
		time_t tStart = (time_t)difftime(tStop, SecCount);
		gmtime_s(&TM_Beg, &tStart);



		std::stringstream sdw;
		sdw << boost::format("%|04|-") % (TM_End.tm_year + 1900);
		sdw << boost::format("%|02|-") % (TM_End.tm_mon + 1);
		sdw << boost::format("%|02| ") % TM_End.tm_mday;
		sdw << boost::format("%|02|:") % TM_End.tm_hour;
		sdw << boost::format("%|02|:") % TM_End.tm_min;
		sdw << boost::format("%|02|") % TM_End.tm_sec;
		//sdw << "+05";
		std::string sEndTime = sdw.str();


		sdw.str(std::string());
		sdw << boost::format("%|04|-") % (TM_Beg.tm_year + 1900);
		sdw << boost::format("%|02|-") % (TM_Beg.tm_mon + 1);
		sdw << boost::format("%|02| ") % TM_Beg.tm_mday;
		sdw << boost::format("%|02|:") % TM_Beg.tm_hour;
		sdw << boost::format("%|02|:") % TM_Beg.tm_min;
		sdw << boost::format("%|02|") % TM_Beg.tm_sec;
		//sdw << "+05";
		std::string sBegTime = sdw.str();

		std::stringstream sde;
		sde << "SELECT create_at FROM todos WHERE id_name = " << val->ID;
		sde << " AND create_at <= '";
		sde << sBegTime;
		sde << "' ORDER BY create_at DESC LIMIT 1;";
		std::string sBegTime2 = sBegTime;
		std::string command = sde.str();
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		PGresult* res = conn->PGexec(command);
		//LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			if(PQntuples(res))
				sBegTime2 = conn->PGgetvalue(res, 0, 0);
		}
		else
			LOG_ERR_SQL(SQLLogger, res, command);

		PQclear(res);


		std::stringstream sdt;
		sdt << "SELECT create_at, content FROM todos WHERE id_name = " << val->ID;
		if(sBegTime2.length())	sdt << " AND create_at >= '" << sBegTime2 << "'";
		if(sEndTime.length())	sdt << " AND create_at <= '" << sEndTime << "'";

		sdt << " ORDER BY create_at ASC ;";

		command = sdt.str();
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		res = conn->PGexec(command);
		//LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			if(line)
			{
				int i = 0;
				int64_t t = 0;

				float f = Stof(conn->PGgetvalue(res, 0, 1));
				DataTimeOfString(sBegTime, TM_Temp);
				TM_Temp.tm_year -= 1900;
				TM_Temp.tm_mon -= 1;


				st[sBegTime] = std::pair(mktime(&TM_Temp), f);


				for(int l = 0; l < line; l++)
				{
					std::string sData = conn->PGgetvalue(res, l, 0);

					if(sBegTime <= sData)
					{
						std::string sTemp = conn->PGgetvalue(res, l, 1);
						DataTimeOfString(sData, TM_Temp);
						TM_Temp.tm_year -= 1900;
						TM_Temp.tm_mon -= 1;

						f = Stof(sTemp);
						if(f > 0 && f < 2000)
							st[sData] = std::pair(mktime(&TM_Temp), f);
					}
				}
				st[sEndTime] = std::pair(tStop1, f);
			}
		}
		else
			LOG_ERR_SQL(SQLLogger, res, command);

		PQclear(res);
	}
	CATCH(Graphics, "");
}

void GetGrTempFURN(Graff& gr, T_ForBase_RelFurn& app)
{
	try
	{
		SqlTempFURN(gr.conn, gr.TempAct, app.TempAct, gr.MaxSecCount);
		SqlTempFURN(gr.conn, gr.TempRef, app.TempRef, gr.MaxSecCount);
		InvalidateRect(gr.gHwnd, NULL, false);
		gr.full = true;
	}
	CATCH(Graphics, "");
}


std::string SqlTempKPVL2(std::string sBegTime)
{
	try
	{
		std::stringstream sde;
		sde << "SELECT create_at FROM todos WHERE id_name = " << GenSeqFromHmi.TempSet1->ID;
		sde << " AND create_at <= '";
		sde << sBegTime;
		sde << "' ORDER BY create_at ASC LIMIT 1;";
		std::string sBegTime2 = sBegTime;
		std::string command = sde.str();
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		PGresult* res = GraffKPVL.conn->PGexec(command);
		//LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			if(PQntuples(res))
				sBegTime2 = GraffKPVL.conn->PGgetvalue(res, 0, 0);
		}
		else
			LOG_ERR_SQL(SQLLogger, res, command);

		PQclear(res);

		return sBegTime2;
	}
	CATCH(Graphics, "");
	return "";
}

void GetGrTempKPVLTempAct()
{
	try
	{
		if(GraffKPVL.AddTime.length())
			GraffKPVL.MaxSecCount = Stoi(GraffKPVL.AddTime) * 60;
		else
			GraffKPVL.MaxSecCount = SecCount2;

		
		SqlTempFURN(GraffKPVL.conn, GraffKPVL.TempRef, GenSeqFromHmi.TempSet1, GraffKPVL.MaxSecCount);
		int t = 0;

		GraffKPVL.TempAct.erase(GraffKPVL.TempAct.begin(), GraffKPVL.TempAct.end()); //�����; ����������� �������� �����������

		std::string sBegTime = "";
		std::string sEndTime = "";

		std::tm TM_Temp ={0};

		if(TestDataTimeOfString(GraffKPVL.StartKPVL))
		{
			sBegTime = DataYarDaySwap(GraffKPVL.StartKPVL);
			std::tm TM_End ={0};
			sEndTime = "";
			time_t tStart = DataTimeOfString(sBegTime);
			time_t tStop1 = tStart + GraffKPVL.MaxSecCount;
			localtime_s(&TM_End, &tStop1);

			std::stringstream sdw;
			sdw << boost::format("%|04|-") % (TM_End.tm_year + 1900);
			sdw << boost::format("%|02|-") % (TM_End.tm_mon + 1);
			sdw << boost::format("%|02| ") % TM_End.tm_mday;
			sdw << boost::format("%|02|:") % TM_End.tm_hour;
			sdw << boost::format("%|02|:") % TM_End.tm_min;
			sdw << boost::format("%|02|") % TM_End.tm_sec;
			sEndTime = sdw.str();
		}
		else
		{
			std::tm TM_End ={0};
			std::tm TM_Beg ={0};
			std::string TecTime;
			time_t tStop1 = time(NULL);
			localtime_s(&TM_End, &tStop1);
			time_t tStop = _mkgmtime(&TM_End);
			time_t tStart = (time_t)difftime(tStop, GraffKPVL.MaxSecCount);
			gmtime_s(&TM_Beg, &tStart);

			std::stringstream sdw;
			sdw << boost::format("%|04|-") % (TM_End.tm_year + 1900);
			sdw << boost::format("%|02|-") % (TM_End.tm_mon + 1);
			sdw << boost::format("%|02| ") % TM_End.tm_mday;
			sdw << boost::format("%|02|:") % TM_End.tm_hour;
			sdw << boost::format("%|02|:") % TM_End.tm_min;
			sdw << boost::format("%|02|") % TM_End.tm_sec;
			//sdw << "+05";
			sEndTime = sdw.str();

			sdw.str(std::string());
			sdw << boost::format("%|04|-") % (TM_Beg.tm_year + 1900);
			sdw << boost::format("%|02|-") % (TM_Beg.tm_mon + 1);
			sdw << boost::format("%|02| ") % TM_Beg.tm_mday;
			sdw << boost::format("%|02|:") % TM_Beg.tm_hour;
			sdw << boost::format("%|02|:") % TM_Beg.tm_min;
			sdw << boost::format("%|02|") % TM_Beg.tm_sec;
			//sdw << "+05";
			sBegTime = sdw.str();
			//SetWindowText(GraffKPVL.bHwnd, sBegTime.c_str());
		}

		//std::string sBegTime2 = SqlTempKPVL(sBegTime);

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
		sdt << " AND create_at >= '";
		sdt << sBegTime;
		sdt << "' AND create_at <= '";
		sdt << sEndTime;
		sdt << "' ORDER BY create_at ASC ;";

		std::string command = sdt.str();
		PGresult* res = GraffKPVL.conn->PGexec(command);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			if(line)
			{
				for(int l = 0; l < line; l++)
				{
					std::string sData = GraffKPVL.conn->PGgetvalue(res, l, 0);
					if(sBegTime <= sData)
					{
						auto a = GraffKPVL.TempAct.find(sData);
						std::string sTemp = GraffKPVL.conn->PGgetvalue(res, l, 1);
						float f =  Stof(sTemp);
						if(f > 0 && f < 2000)
						{
							if(a != GraffKPVL.TempAct.end() && a._Ptr != NULL)
							{
								a->second.second = (f + a->second.second) / 2.0f;
							}
							else
							{
								if(f != 0)
								{
									DataTimeOfString(sData, TM_Temp);
									TM_Temp.tm_year -= 1900;
									TM_Temp.tm_mon -= 1;
									GraffKPVL.TempAct[sData] = std::pair(mktime(&TM_Temp), f);
								}
							}
						}
					}
				}
			}
		}
		else
			LOG_ERR_SQL(SQLLogger, res, command);
		PQclear(res);


		GraffKPVL.full = true;
		InvalidateRect(GraffKPVL.gHwnd, NULL, false);
	}
	CATCH(Graphics, "");
}

namespace GRAGG{
	LRESULT Paint(HWND hWnd)
	{
		try
		{
			Graff* graff = (Graff*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(graff)
				graff->Paint(hWnd);
		}
		CATCH(Graphics, "");
		return 0;
	}

	LRESULT CALLBACK GrafProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if(message == WM_PAINT) return Paint(hWnd);
		return DefWindowProc(hWnd, message, wParam, lParam);;
	}

	void RegisterClassGraf()
	{
		WNDCLASS  wc ={0};
		wc.style = 0;
		wc.lpfnWndProc = (WNDPROC)GrafProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "GrafClass";
		RegisterClass(&wc);
	}

};


DWORD WINAPI Open_GRAFF_FURN1(LPVOID)
{
	try
	{
		CONNECTION2(GraffFurn1.conn, Graphics);
		GraffFurn1.MaxSecCount = SecCount1;
		while(isRun)
		{
			GetGrTempFURN(GraffFurn1, AppFurn1);
			int f = 10;
			while(isRun && f--)
				std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	CATCH(Graphics, "");
	return 0;
}

DWORD WINAPI Open_GRAFF_FURN2(LPVOID)
{
	try
	{
		CONNECTION2(GraffFurn2.conn, Graphics);
		GraffFurn2.MaxSecCount = SecCount1;

		while(isRun)
		{
			GetGrTempFURN(GraffFurn2, AppFurn2);
			int f = 10;
			while(isRun && f--)
				std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	CATCH(Graphics, "");
	return 0;
}

DWORD WINAPI Open_GRAFF_KPVL(LPVOID)
{
	try
	{
		CONNECTION2(GraffKPVL.conn, Graphics);
		GraffKPVL.MaxSecCount = SecCount2;
		while(isRun)
		{
			GetGrTempKPVLTempAct();
			int f = 10;
			while(isRun && f--)
				std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	CATCH(Graphics, "");
	return 0;
}


void SqlTempFURN0(PGConnection* conn, T_SqlTemp& st, Value* val, T_ForBase_RelFurn& app, TCassette& TC)
{
	try
	{
		st.erase(st.begin(), st.end());
		std::tm TM_Temp ={0};
		time_t tStop1 = 0;

		std::string sBegTime = TC.Run_at;
		if(!sBegTime.length()) return;

		std::string sEndTime = TC.Finish_at;
		if(!sEndTime.length()) sEndTime = TC.End_at;
		if(!sEndTime.length()) sEndTime = GetStringDataTime();

		//if(!sEndTime.length())
		//{
		//	std::string sTempTime = TC.End_at;
		//
		//	//if(!sTempTime.length())sTempTime = TC.Error_at;
		//	//if(!sTempTime.length())
		//	{
		//		//����� ����� ��������
		//		std::stringstream sdd;
		//		sdd << "SELECT min(create_at) FROM todos WHERE create_at > '" << sBegTime << "' AND (id_name = " << app.ProcEnd->ID << ") AND content = 'true';"; // OR id_name = " << app.ProcFault->ID << "
		//		std::string command = sdd.str();
		//		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		//		PGresult* res = conn->PGexec(command);
		//		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		//		{
		//			if(PQntuples(res))
		//				sTempTime = conn->PGgetvalue(res, 0, 0);
		//		}
		//		else
		//			LOG_ERR_SQL(SQLLogger, res, command);
		//		PQclear(res);
		//		if(!sTempTime.length())
		//			return;
		//
		//		//sdd = std::stringstream("");
		//		//sdd << "UPDATE cassette SET "
		//	}
		//
		//	DataTimeOfString(sTempTime, TM_Temp);
		//	TM_Temp.tm_year -= 1900;
		//	TM_Temp.tm_mon -= 1;
		//
		//	tStop1 = mktime(&TM_Temp) + (60 * 15); //15 �����
		//	localtime_s(&TM_Temp, &tStop1); 
		//
		//	std::stringstream sdw;
		//	sdw << boost::format("%|04|-") % (TM_Temp.tm_year + 1900);
		//	sdw << boost::format("%|02|-") % (TM_Temp.tm_mon + 1);
		//	sdw << boost::format("%|02| ") % TM_Temp.tm_mday;
		//	sdw << boost::format("%|02|:") % TM_Temp.tm_hour;
		//	sdw << boost::format("%|02|:") % TM_Temp.tm_min;
		//	sdw << boost::format("%|02|") % TM_Temp.tm_sec;
		//	std::string sEndTime = sdw.str();
		//
		//}
		//else
		{
			DataTimeOfString(sEndTime, TM_Temp);
			TM_Temp.tm_year -= 1900;
			TM_Temp.tm_mon -= 1;
			tStop1 = mktime(&TM_Temp);
		}


		//sdw << "+05";
		//std::string sEndTime = sdw.str();
		//
		//
		//sdw.str(std::string());
		//sdw << boost::format("%|04|-") % (TM_Beg.tm_year + 1900);
		//sdw << boost::format("%|02|-") % (TM_Beg.tm_mon + 1);
		//sdw << boost::format("%|02| ") % TM_Beg.tm_mday;
		//sdw << boost::format("%|02|:") % TM_Beg.tm_hour;
		//sdw << boost::format("%|02|:") % TM_Beg.tm_min;
		//sdw << boost::format("%|02|") % TM_Beg.tm_sec;
		//sdw << "+05";
		//std::string sBegTime = sdw.str();

		std::stringstream sde;
		//sde << "SELECT max(create_at) FROM todos WHERE id_name = " << val->ID;
		sde << "SELECT create_at FROM todos WHERE id_name = " << val->ID;
		sde << " AND create_at <= '";
		sde << sBegTime;
		sde << "' ORDER BY create_at ASC LIMIT 1;";
		std::string sBegTime2 = sBegTime;
		std::string command = sde.str();
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		PGresult* res = conn->PGexec(command);
		//LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			if(PQntuples(res))
				sBegTime2 = conn->PGgetvalue(res, 0, 0);
		}
		else
			LOG_ERR_SQL(SQLLogger, res, command);

		PQclear(res);


		std::stringstream sdt;
		sdt << "SELECT create_at, content FROM todos WHERE id_name = " << val->ID;
		if(sBegTime2.length())	sdt << " AND create_at >= '" << sBegTime2 << "'";
		if(sEndTime.length())	sdt << " AND create_at <= '" << sEndTime << "'";

		sdt << " ORDER BY create_at ASC ;";

		command = sdt.str();
		if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		res = conn->PGexec(command);
		//LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			int line = PQntuples(res);
			if(line)
			{
				int i = 0;
				int64_t t = 0;

				float f = Stof(conn->PGgetvalue(res, 0, 1));
				DataTimeOfString(sBegTime, TM_Temp);
				TM_Temp.tm_year -= 1900;
				TM_Temp.tm_mon -= 1;


				st[sBegTime] = std::pair(mktime(&TM_Temp), f);


				for(int l = 0; l < line; l++)
				{
					std::string sData = conn->PGgetvalue(res, l, 0);

					if(sBegTime <= sData)
					{
						std::string sTemp = conn->PGgetvalue(res, l, 1);
						DataTimeOfString(sData, TM_Temp);
						TM_Temp.tm_year -= 1900;
						TM_Temp.tm_mon -= 1;

						f = Stof(sTemp.c_str());
						if(f != 0 && f < 2000)
							st[sData] = std::pair(mktime(&TM_Temp), f);
					}
				}
				st[sEndTime] = std::pair(tStop1, f);
			}
		}
		else
			LOG_ERR_SQL(SQLLogger, res, command);

		PQclear(res);
	}
	CATCH(Graphics, "");
}

void GetGrTempFURN0(Graff& gr, T_ForBase_RelFurn& app, TCassette& TC)
{
	try
	{
		SqlTempFURN0(gr.conn, gr.TempAct, app.TempAct, app, TC);
		SqlTempFURN0(gr.conn, gr.TempRef, app.TempRef, app, TC);
		InvalidateRect(gr.gHwnd, NULL, false);
		gr.full = true;
	}
	CATCH(Graphics, "");
}

//std::shared_ptr<spdlog::logger>LogInit(std::string LoggerOut);
//typedef std::shared_ptr<spdlog::logger>(std::string) loginit;


void SetProc(HWND hWnd, std::string& p)
{
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)&p);
	if(!OldSubProc)OldSubProc = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)SubProc);
	else SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)SubProc);
}

void InitGrafWindow(HWND hWnd)
{
	InitLogger(Graphics);

	try
	{
		GetGdiplusEncoderClsid(L"image/bmp", &guidBmp);
		GetGdiplusEncoderClsid(L"image/jpeg", &guidJpeg);
		GetGdiplusEncoderClsid(L"image/gif", &guidGif);
		GetGdiplusEncoderClsid(L"image/tiff", &guidTiff);
		GetGdiplusEncoderClsid(L"image/png", &guidPng);

	//	GRAGG::RegisterClassGraf();
	//
	//	GraffKPVL.AddTime = std::to_string(SecCount2 / 60);
	//
	//	GraffKPVL.gHwnd = CreateWindowEx(0, "GrafClass", NULL, WS_CLIPCHILDREN | WS_BORDER | WS_CHILD | WS_VISIBLE, 1170, 170, Xsize, Ysize1, hWnd, (HMENU)5103, hInstance, 0);
	//	GraffKPVL.bHwnd = CreateWindowEx(0, "Edit", GraffKPVL.StartKPVL.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_CENTER, 1350, 150, 140, 20, hWnd, (HMENU)5104, hInstance, 0);
	//	GraffKPVL.tHwnd = CreateWindowEx(0, "Edit", GraffKPVL.AddTime.c_str(),   WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_CENTER, 1490, 150,  30, 20, hWnd, (HMENU)5105, hInstance, 0);
	//
	//	//GtHwnd = GraffKPVL.tHwnd;
	//	SetProc(GraffKPVL.bHwnd, GraffKPVL.StartKPVL);
	//	SetProc(GraffKPVL.tHwnd, GraffKPVL.AddTime);
	//
	//	GraffFurn1.gHwnd = CreateWindowEx(0, "GrafClass", NULL, WS_CLIPCHILDREN | WS_BORDER | WS_CHILD | WS_VISIBLE, 505, 25, Xsize, Ysize2, winmap(hGroup200), (HMENU)5103, hInstance, 0);
	//	GraffFurn2.gHwnd = CreateWindowEx(0, "GrafClass", NULL, WS_CLIPCHILDREN | WS_BORDER | WS_CHILD | WS_VISIBLE, 505, 25, Xsize, Ysize2, winmap(hGroup300), (HMENU)5103, hInstance, 0);
	//
	//	GraffFurn.gHwnd = CreateWindowEx(0, "GrafClass", NULL, WS_CLIPCHILDREN | WS_BORDER | WS_CHILD | WS_VISIBLE, 1424, 530, 469, 160, hWnd, (HMENU)5104, hInstance, 0);
	//
	//	SetWindowLongPtr(GraffKPVL.gHwnd, GWLP_USERDATA, (LONG_PTR)&GraffKPVL);
	//	SetWindowLongPtr(GraffFurn1.gHwnd, GWLP_USERDATA, (LONG_PTR)&GraffFurn1);
	//	SetWindowLongPtr(GraffFurn2.gHwnd, GWLP_USERDATA, (LONG_PTR)&GraffFurn2);
	//	SetWindowLongPtr(GraffFurn.gHwnd, GWLP_USERDATA, (LONG_PTR)&GraffFurn);
	//
	//	GraffKPVL.conn = &connKPVL;
	//	GraffFurn1.conn = &connFurn1;
	//	GraffFurn2.conn = &connFurn2;
	//	GraffFurn.conn = &connFurn;
	//
	//
	//	UpdateWindow(GraffKPVL.gHwnd);
	//	UpdateWindow(GraffFurn1.gHwnd);
	//	UpdateWindow(GraffFurn2.gHwnd);
	//
	//	UpdateWindow(GraffFurn.gHwnd);
	//	//Open_GRAFF_FURN();
	//
	////#ifdef TESTGRAFF
	//	hGGraff1 = CreateThread(0, 0, Open_GRAFF_KPVL, (LPVOID)0, 0, 0);
	//	hGGraff2 = CreateThread(0, 0, Open_GRAFF_FURN1, (LPVOID)0, 0, 0);
	//	hGGraff3 = CreateThread(0, 0, Open_GRAFF_FURN2, (LPVOID)0, 0, 0);
	////#endif
	}
	CATCH(Graphics, "");
	int t = 0;
}

void Open_GRAFF_FURN(TCassette& TC)
{
	InitLogger(Graphics);

	try
	{
		//CONNECTION2(GraffFurn.conn, Graphics);
		//
		//GraffFurn.MaxSecCount = 0;// SecCount1;
		//
		//if(atoi(TC.Peth.c_str()) == 1)
		//{
		//	GetGrTempFURN0(GraffFurn, AppFurn1, TC);
		//}
		//if(atoi(TC.Peth.c_str()) == 2)
		//{
		//	GetGrTempFURN0(GraffFurn, AppFurn2, TC);
		//}
		
	}
	CATCH(Graphics, "");
}


void StopGraff()
{
	//WaitCloseTheread(hGGraff1, "hGGraff1");
	//WaitCloseTheread(hGGraff2, "hGGraff1");
	//WaitCloseTheread(hGGraff3, "hGGraff1");
}