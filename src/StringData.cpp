#include "pch.h"
#include "main.h"
#include "win.h"
#include "StringData.h"
#include "file.h"
#include "exel.h"
#include "win.h"
#include "Graff.h"

#include "SQL.h"
#include "Event.h"

bool TestDataTimeOfString(std::string str)
{
    bool b = false;
    try
    {
        //2024-10-19
        //19-10-2024 01:01:01
        //boost::regex xRegEx("^((0?[1-9]|[12][0-9]|3[01])[\\/\\-\\.](0?[1-9]|1[012])[ \\/\\.\\-])");
        //((([0]?[1-9]|1[0-2])(:|\\.)[0-5][0-9]((:|\\.)[0-5][0-9])?( )?(AM|am|aM|Am|PM|pm|pM|Pm))|(([0]?[0-9]|1[0-9]|2[0-3])(:|\\.)[0-5][0-9]((:|\\.)[0-5][0-9])?))$
        //boost::regex xRegEx("^
        //                    (?:(?:31(\\/|-|\\.)(?:0?[13578]|1[02]))\\1|(?:(?:29|30)(\\/|-|\\.)(?:0?[1,3-9]|1[0-2])\\2))
        //                    (?:(?:1[6-9]|[2-9]\\d)?\\d{2})$
        //                    |^(?:29(\\/|-|\\.)0?2\\3(?:(?:(?:1[6-9]|[2-9]\\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$
        //                    |^(?:0?[1-9]|1\\d|2[0-8])
        //                    (\\/|-|\\.)(?:(?:0?[1-9])|(?:1[0-2]))\\4(?:(?:1[6-9]|[2-9]\\d)?\\d{2})
        //                    ");
        // 12/01/2002 12:32:10
        // ^(([0-2]\\d|[3][0-1])[\\/-]
        // ([0]\\d|[1][0-2])[\\/-]
        // [2][0]\\d{2}\\s
        // ([0-1]\\d|[2][0-3])\\:[0-5]\\d\\:[0-5]\\d)$
        //
        std::string Spl = "[\\/-]";
        std::string Day = "([0-2]\\d|[3][0-1])";
        std::string Mon = "([0]\\d|[1][0-2])";
        std::string Yar = "[2][0]\\d{2}";
        std::string Tim = " (([0-1]\\d|[2][0-3])\\:[0-5]\\d\\:[0-5]\\d)$";
        boost::regex xRegEx("(^(" + Day + Spl + Mon + Spl + Yar + ")|" + "^(" + Yar + Spl + Mon + Spl + Day + "))" + Tim);
        b = boost::regex_match(str, xRegEx);
        if(b)
        {
            int tt = 0;
        }
    }
    catch(std::invalid_argument& exc) { 
        SetWindowText(hWndDebug, (FUNCTION_LINE_NAME + ": " + exc.what()).c_str());
    }
    catch(std::filesystem::filesystem_error& exc) { 
        SetWindowText(hWndDebug, (FUNCTION_LINE_NAME + ": " + exc.what()).c_str());
    }
    catch(std::runtime_error& exc) { 
        SetWindowText(hWndDebug, (FUNCTION_LINE_NAME + ": " + exc.what()).c_str());
    }
    catch(std::exception& exc) {
        SetWindowText(hWndDebug, (FUNCTION_LINE_NAME + ": " + exc.what()).c_str());
    }
    catch(...) { 
        SetWindowText(hWndDebug, (FUNCTION_LINE_NAME + ": " + "Unknown error").c_str());
    }
    return b;
}

std::string GetDataTimeString(std::time_t& st)
{
    st = time(NULL);
    tm curr_tm;
    localtime_s(&curr_tm, &st);

    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (curr_tm.tm_year + 1900);
    sdt << boost::format("%|02|-") % (curr_tm.tm_mon + 1);
    sdt << boost::format("%|02| ") % curr_tm.tm_mday;
    sdt << boost::format("%|02|:") % curr_tm.tm_hour;
    sdt << boost::format("%|02|:") % curr_tm.tm_min;
    sdt << boost::format("%|02|") % curr_tm.tm_sec;
    return sdt.str();
}

std::string GetDataTimeString(std::tm& st)
{
    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (st.tm_year + 1900);
    sdt << boost::format("%|02|-") % (st.tm_mon + 1);
    sdt << boost::format("%|02| ") % st.tm_mday;
    sdt << boost::format("%|02|:") % st.tm_hour;
    sdt << boost::format("%|02|:") % st.tm_min;
    sdt << boost::format("%|02|") % st.tm_sec;
    return sdt.str();
}

std::string GetDataTimeString(std::time_t* st)
{
    tm curr_tm;
    localtime_s(&curr_tm, st);

    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (curr_tm.tm_year + 1900);
    sdt << boost::format("%|02|-") % (curr_tm.tm_mon + 1);
    sdt << boost::format("%|02| ") % curr_tm.tm_mday;
    sdt << boost::format("%|02|:") % curr_tm.tm_hour;
    sdt << boost::format("%|02|:") % curr_tm.tm_min;
    sdt << boost::format("%|02|") % curr_tm.tm_sec;
    return sdt.str();
}

std::wstring GetData(std::wstring str)
{
    std::wstring::const_iterator start = str.begin();
    std::wstring::const_iterator end = str.end();
    boost::wregex xRegEx(L".* (\\d{1,2}:\\d{1,2}):\\d{1,2}.*");
    boost::match_results<std::wstring::const_iterator> what;

    boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size();
    return what[1].str();
}


std::string GetDataTimeString()
{
    std::time_t st = time(NULL);
    tm curr_tm;
    localtime_s(&curr_tm, &st);

    std::stringstream sdt;
    sdt << boost::format("%|04|-") % (curr_tm.tm_year + 1900);
    sdt << boost::format("%|02|-") % (curr_tm.tm_mon + 1);
    sdt << boost::format("%|02| ") % curr_tm.tm_mday;
    sdt << boost::format("%|02|:") % curr_tm.tm_hour;
    sdt << boost::format("%|02|:") % curr_tm.tm_min;
    sdt << boost::format("%|02|") % curr_tm.tm_sec;
    return sdt.str();
}

std::string GetDataTimeStringMS()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    std::stringstream sdt;
    sdt << boost::format("%|04|-") % st.wYear;
    sdt << boost::format("%|02|-") % st.wMonth;
    sdt << boost::format("%|02| ") % st.wDay;
    sdt << boost::format("%|02|:") % st.wHour;
    sdt << boost::format("%|02|:") % st.wMinute;
    sdt << boost::format("%|02|.") % st.wSecond;
    sdt << boost::format("%|02|") % st.wMilliseconds;

    return sdt.str();
}


time_t DataTimeOfString(std::string str, int& d1, int& d2, int& d3, int& d4, int& d5, int& d6)
{
    try
    {
        std::tm tm;
        d1 = 0; d2 = 0; d3 = 0; d4 = 0; d5 = 0; d6 = 0;
        std::string::const_iterator start = str.begin();
        std::string::const_iterator end = str.end();
        boost::match_results<std::string::const_iterator> what;
        boost::regex xRegEx;

        xRegEx = FORMATTIME1;
        if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
        {
            d1 = Stoi(what[1]);
            d2 = Stoi(what[2]);
            d3 = Stoi(what[3]);
            d4 = Stoi(what[4]);
            d5 = Stoi(what[5]);
            d6 = Stoi(what[6]);
            tm.tm_year = d1 - 1900;
            tm.tm_mon = d2 - 1;
            tm.tm_mday = d3;
            tm.tm_hour = d4;
            tm.tm_min = d5;
            tm.tm_sec = d6;
            return mktime(&tm);
        }
        else
        {
            xRegEx = FORMATTIME2;
            if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
            {
                d3 = Stoi(what[1]);
                d2 = Stoi(what[2]);
                d1 = Stoi(what[3]);
                d4 = Stoi(what[4]);
                d5 = Stoi(what[5]);
                d6 = Stoi(what[6]);

                tm.tm_year = d1 - 1900;
                tm.tm_mon = d2 - 1;
                tm.tm_mday = d3;
                tm.tm_hour = d4;
                tm.tm_min = d5;
                tm.tm_sec = d6;
                return mktime(&tm);
            }
        }
    }
    catch(...) {}
    return 0;
}


time_t DataTimeOfString(std::string str, std::tm& TM)
{
    try
    {
        TM.tm_year = 0; TM.tm_mon = 0; TM.tm_mday = 0; TM.tm_hour = 0; TM.tm_min = 0; TM.tm_sec = 0;
        std::string::const_iterator start = str.begin();
        std::string::const_iterator end = str.end();
        boost::match_results<std::string::const_iterator> what;
        boost::regex xRegEx;

        xRegEx = FORMATTIME1;
        if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
        {
            TM.tm_year = Stoi(what[1]);
            TM.tm_mon = Stoi(what[2]);
            TM.tm_mday = Stoi(what[3]);
            TM.tm_hour = Stoi(what[4]);
            TM.tm_min = Stoi(what[5]);
            TM.tm_sec = Stoi(what[6]);

            std::tm tm = TM;
            tm.tm_year -= 1900;
            tm.tm_mon -= 1;
            return mktime(&tm);
        }
        else
        {
            xRegEx = FORMATTIME2;
            if(boost::regex_search(start, end, what, xRegEx, boost::match_default))
            {
                TM.tm_mday = Stoi(what[1]);
                TM.tm_mon = Stoi(what[2]);
                TM.tm_year = Stoi(what[3]);
                TM.tm_hour = Stoi(what[4]);
                TM.tm_min = Stoi(what[5]);
                TM.tm_sec = Stoi(what[6]);

                std::tm tm = TM;
                tm.tm_year -= 1900;
                tm.tm_mon -= 1;
                return mktime(&tm);
            }
        }
    }
    catch(...) {}

    return 0;
}

std::string DataYarDaySwap(std::string str)
{
    std::string::const_iterator start = str.begin();
    std::string::const_iterator end = str.end();
    boost::match_results<std::string::const_iterator> what;
    boost::regex xRegEx;
    xRegEx = FORMATTIME1;
    if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
    {
        std::stringstream sdt;
        sdt << boost::format("%|04|-") % Stoi(what[1]);
        sdt << boost::format("%|02|-") % Stoi(what[2]);
        sdt << boost::format("%|02| ") % Stoi(what[3]);
        sdt << boost::format("%|02|:") % Stoi(what[4]);
        sdt << boost::format("%|02|:") % Stoi(what[5]);
        sdt << boost::format("%|02|")  % Stoi(what[6]);
        return sdt.str();
    }
    else
    {
        xRegEx = FORMATTIME2;
        if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
        {
            std::stringstream sdt;
            sdt << boost::format("%|04|-") % Stoi(what[3]);
            sdt << boost::format("%|02|-") % Stoi(what[2]);
            sdt << boost::format("%|02| ") % Stoi(what[1]);
            sdt << boost::format("%|02|:") % Stoi(what[4]);
            sdt << boost::format("%|02|:") % Stoi(what[5]);
            sdt << boost::format("%|02|")  % Stoi(what[6]);
            return sdt.str();
        }
    }
    return "";
}

time_t DataTimeOfString(std::string str)
{
    try
    {
        std::string::const_iterator start = str.begin();
        std::string::const_iterator end = str.end();
        boost::match_results<std::string::const_iterator> what;
        boost::regex xRegEx;

        xRegEx = FORMATTIME1;
        if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
        {
            std::tm TM;
            TM.tm_year = Stoi(what[1]) - 1900;
            TM.tm_mon = Stoi(what[2]) - 1;
            TM.tm_mday = Stoi(what[3]);
            TM.tm_hour = Stoi(what[4]);
            TM.tm_min = Stoi(what[5]);
            TM.tm_sec = Stoi(what[6]);
            return mktime(&TM);
        }
        else
        {
            xRegEx = FORMATTIME2;
            if(boost::regex_search(start, end, what, xRegEx, boost::match_default) && what.size() > 6)
            {
                std::tm TM;
                TM.tm_mday = Stoi(what[1]);
                TM.tm_mon = Stoi(what[2]) - 1;
                TM.tm_year = Stoi(what[3]) - 1900;
                TM.tm_hour = Stoi(what[4]);
                TM.tm_min = Stoi(what[5]);
                TM.tm_sec = Stoi(what[6]);
                return mktime(&TM);
            }
        }
    }
    catch(...) {}

    return 0;
}
time_t DataTimeDiff(std::string str1, std::string str2)
{
    std::tm TM;
    time_t tm1 = DataTimeOfString(str1, TM);
    time_t tm2 = DataTimeOfString(str2, TM);
    return (time_t)difftime(tm1, tm2);
}

std::string GetStringData(std::string d)
{
    if(!d.size())return "";
    std::string ss = d;
    std::vector <std::string>split1;
    boost::split(split1, d, boost::is_any_of("."), boost::token_compress_on);
    if(split1.size())
        d = split1[0];

    std::vector <std::string>split2;
    boost::split(split2, d, boost::is_any_of("+"), boost::token_compress_on);
    if(split2.size())
        d = split2[0];

    return d;
}


std::string GetDataTimeStr(std::string str, std::string& outDate, std::string& outTime)
{
    outDate = "";
    outTime = "";
    boost::regex xRegEx("^(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}:\\d{2}:\\d{2}).*");
    boost::match_results<std::string::const_iterator>what;
    if(boost::regex_search(str, what, xRegEx, boost::match_default) && what.size() > 4)
    {
        outTime = what[4];
        outDate = what[3] + "-" + what[2] + "-" + what[1];
        return outDate + " " + outTime;
    }
    return "";
}

std::string GetDataTimeStr(std::string str)
{
    std::string outDate = "";
    std::string outTime = "";

    boost::regex xRegEx("^(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}:\\d{2}:\\d{2})*");
    boost::match_results<std::string::const_iterator>what;
    if(boost::regex_search(str, what, xRegEx, boost::match_default) && what.size() > 4)
    {
        outTime = what[4];
        outDate = what[3] + "-" + what[2] + "-" + what[1];
        return outDate + " " + outTime;
    }

    //boost::regex_search(str, what, xRegEx, boost::match_default) && what.size();
    //if(what.size() > 4)
    //{
    //    std::string year = what[1].str();
    //    std::string month = what[2].str();
    //    std::string day = what[3].str();
    //    if(what[4].length())
    //        outTime = what[4].str();
    //    if(day.length() && month.length() && year.length())
    //        outDate = day + "-" + month + "-" + year;
    //}
    //if(outDate.length() && outTime.length())
    //    return outDate + " " + outTime;
    //else
    //{
    //    if(outDate.length())
    //    {
    //        return outDate;
    //    }
    //    else
    //    {
    //        if(outTime.length())
    //        {
    //            outTime;
    //        }
    //    }
    //}
    return "";
}