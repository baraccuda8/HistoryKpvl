#include "pch.h"
#include "main.h"
#include "win.h"
//#include "file.h"

//#include "ClCodeSys.h"
//#include "SQL.h"

#include "ValueTag.h"

#include "hard.h"
#include "KPVL.h"
#include "term.h"
#include "Event.h"


HANDLE hEvent;
std::string NameEvent = "\\\\192.168.9.65\\private\\sqlflag\\LoggrNew.log";


DWORD WINAPI Event_Run(LPVOID pv)
{
#ifndef _DEBUG
	std::string hardWDG = KPVL::ServerDataTime;
	std::string termWDG = S107::ServerDataTime;
	int CounWait = 0;

	while(isRun)
	{
		std::string a1 = KPVL::ServerDataTime;
		std::string a2 = S107::ServerDataTime;
		if(hardWDG != a1 && termWDG != a2)
		{
			CounWait = 0;
			hardWDG = a1;
			termWDG = a2;
			tm curr_tm;
			std::time_t st = time(NULL);
			localtime_s(&curr_tm, &st);

			std::fstream s(NameEvent, std::fstream::binary | std::fstream::out | std::fstream::trunc);
			if(s.is_open())
			{
				s << boost::format("%|04|-") % (curr_tm.tm_year + 1900);
				s << boost::format("%|02|-") % (curr_tm.tm_mon + 1);
				s << boost::format("%|02| ") % curr_tm.tm_mday;
				s << boost::format("%|02|:") % curr_tm.tm_hour;
				s << boost::format("%|02|:") % curr_tm.tm_min;
				s << boost::format("%|02|") % curr_tm.tm_sec;
				s.close();
			}
			else
			{
				LOG_ERROR(AllLogger, "{:90}| Фаил '{}' недостурен", FUNCTION_LINE_NAME, NameEvent);
			}

		}
		else
		{
			if(++CounWait > 180) //Три минуты
				isRun = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
#endif
	return 0;
}


void Open_Event()
{
#ifndef _DEBUG
	hEvent = CreateThread(0, 0, Event_Run, 0, 0, 0);
#endif
}

void Close_Event()
{
#ifndef _DEBUG
	DWORD dwEvent = WaitForSingleObject(hEvent, INFINITE);
#endif
}