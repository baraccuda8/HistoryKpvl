#include "pch.h"
#include "main.h"
#include "win.h"
#include "ClCodeSys.h"
#include "file.h"
#include "ValueTag.h"

//#include "hard.h"
//#include "SQL.h"

//Соединение с SQL базой
PGConnection conn_spis;
PGConnection conn_spic;
PGConnection conn_kpvl;
PGConnection conn_dops;
PGConnection conn_temp;


std::shared_ptr<spdlog::logger> SQLLogger = NULL;

//Таблица толщин листов
//std::map <int, std::map<int, std::string>>MapThicknessCode;

std::string m_dbhost = "192.168.9.63";
std::string m_dbport = "5432";
std::string m_dbname = "kpvl";
std::string m_dbuser = "user";
std::string m_dbpass = "TutonHamon8*";

extern std::deque<Value*> AllTagKpvl;
extern std::deque<Value*> AllTagPeth;
extern std::map<std::string, std::string> NamePos;
extern std::map<int, std::string> GenSeq1;
extern std::map<int, std::string> GenSeq2;
extern std::map<int, std::string> GenSeq3;

namespace KPVL {
    extern uint32_t NextID;
};

//Закрытие программы
LRESULT Quit()
{
    PostQuitMessage(0);
    return 0;
}


DLLRESULT InitDialog(HWND hWnd)
{
    CenterWindow(hWnd, NULL);
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT1), m_dbhost.c_str());
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT2), m_dbport.c_str());
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT3), m_dbname.c_str());
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT4), m_dbuser.c_str());
    SetWindowText(GetDlgItem(hWnd, IDC_EDIT5), m_dbpass.c_str());
    return 0;
}

DLLRESULT CommandDialog(HWND hWnd, WPARAM wParam)
{
    if(wParam == IDOK)
    {
        char ss[256];
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT1), ss, 256);    m_dbhost = ss;
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT2), ss, 256);    m_dbport = ss;
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT3), ss, 256);    m_dbname = ss;
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT4), ss, 256);    m_dbuser = ss;
        GetWindowText(GetDlgItem(hWnd, IDC_EDIT5), ss, 256);    m_dbpass = ss;

        conn_kpvl.connection();
        if(conn_kpvl.connections)
        {
            //SaveConnect();
            EndDialog(hWnd, FALSE);
        }
    }
    if(wParam == IDCANCEL)
    {
        EndDialog(hWnd, FALSE);
        Quit();
    }
    return 0;
}



DLLRESULT CALLBACK bagSave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_INITDIALOG)return InitDialog(hWnd);
    if(message == WM_COMMAND) return CommandDialog(hWnd, wParam);
    return (0);
}


void SetValue(OpcUa::VariantType type, Value* val, std::string value)
{
    if(type == OpcUa::VariantType::BOOLEAN)        val->Val = atoi_t(bool, atoi, value);
    else if(type == OpcUa::VariantType::SBYTE)     val->Val = atoi_t(int8_t, atoi, value) / (int8_t)val->coff;
    else if(type == OpcUa::VariantType::BYTE)      val->Val = atoi_t(uint8_t, atoi, value) / (uint8_t)val->coff;
    else if(type == OpcUa::VariantType::INT16)     val->Val = atoi_t(int16_t, atoi, value) / (int16_t)val->coff;
    else if(type == OpcUa::VariantType::UINT16)    val->Val = atoi_t(uint16_t, atoi, value) / (uint16_t)val->coff;
    else if(type == OpcUa::VariantType::INT32)     val->Val = atoi_t(int32_t, atol, value) / (int32_t)val->coff;
    else if(type == OpcUa::VariantType::UINT32)    val->Val = atoi_t(uint32_t, atol, value) / (uint32_t)val->coff;
    else if(type == OpcUa::VariantType::INT64)     val->Val = atoi_t(int64_t, atoll, value) / (int64_t)val->coff;
    else if(type == OpcUa::VariantType::UINT64)    val->Val = atoi_t(uint64_t, atoll, value) / (uint64_t)val->coff;
    else if(type == OpcUa::VariantType::FLOAT)     val->Val = atoi_t(float, atof, value) / (float)val->coff;
    else if(type == OpcUa::VariantType::DOUBLE)    val->Val = atoi_t(double, atof, value) / (double)val->coff;
    else if(type == OpcUa::VariantType::STRING)    val->Val =  cp1251_to_utf8(value);
    val->OldVal = val->Val;
}


void InitCurentTag()
{
    int ColId = 0;
    int ColContent = 1;
    int ColName = 2;
    int ColArhive = 3;
    int ColType = 4;
    int ColCoff = 5;
    int ColGist = 6;
    int ColFormat = 7;
    int ColIdSec = 8;

#pragma region tag
    std::string comand = "SELECT id, content, name, arhive, type, cof, gist, format, idsec FROM tag ORDER BY id;"; ///* WHERE name = '" + val->Patch + "'*/;";

    PGresult* res = conn_spis.PGexec(comand);
    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
        {
            std::string Patch = conn_spis.PGgetvalue(res, l, ColName);
            if(Patch.find("PLC210 OPC-UA") != SIZE_MAX)
            {
                for(auto& val : AllTagKpvl)
                {
                    if(val->Patch == Patch)
                    {
                        val->ID = atol(conn_spis.PGgetvalue(res, l, ColId).c_str());
                        std::string value = conn_spis.PGgetvalue(res, l, ColContent);
                        val->Arhive = conn_spis.PGgetvalue(res, l, ColArhive) == "t";
                        val->coff = (float)atof(conn_spis.PGgetvalue(res, l, ColCoff).c_str());
                        val->gist = (float)atof(conn_spis.PGgetvalue(res, l, ColGist).c_str());
                        val->format = conn_spis.PGgetvalue(res, l, ColFormat);
                        if(!val->Sec)
                            val->Sec = (MSSEC)atoi(conn_spis.PGgetvalue(res, l, ColIdSec).c_str());
                        if(!val->Sec)
                            val->Sec = MSSEC::sec01000;

                        OpcUa::VariantType type = (OpcUa::VariantType)atoi(conn_spis.PGgetvalue(res, l, ColType).c_str());
                        SetValue(type, val, value);
                        LOG_INFO(SQLLogger, "{:90}| ID = {}, coff = {}, Val = {}, Patch = {}", FUNCTION_LINE_NAME, val->ID, val->coff, val->GetString(), val->Patch);
                        break;
                    }
                }
            }
            else if(Patch.find("SPK107 (M01)") != SIZE_MAX)
            {
                for(auto& val : AllTagPeth)
                {
                    if(val->Patch == Patch)
                    {
                        val->ID = atol(conn_spis.PGgetvalue(res, l, ColId).c_str());
                        std::string value = conn_spis.PGgetvalue(res, l, ColContent);
                        val->Arhive = conn_spis.PGgetvalue(res, l, ColArhive) == "t";
                        val->coff = (float)atof(conn_spis.PGgetvalue(res, l, ColCoff).c_str());
                        val->gist = (float)atof(conn_spis.PGgetvalue(res, l, ColGist).c_str());
                        val->format = conn_spis.PGgetvalue(res, l, ColFormat);
                        if(!val->Sec)
                            val->Sec = (MSSEC)atoi(conn_spis.PGgetvalue(res, l, ColIdSec).c_str());
                        if(!val->Sec)
                            val->Sec = MSSEC::sec01000;

                        OpcUa::VariantType type = (OpcUa::VariantType)atoi(conn_spis.PGgetvalue(res, l, ColType).c_str());
                        SetValue(type, val, value);
                        LOG_INFO(SQLLogger, "{:90}| ID = {}, coff = {}, Val = {}, Patch = {}", FUNCTION_LINE_NAME, val->ID, val->coff, val->GetString(), val->Patch);
                        break;
                    }
                }
            }
        }
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    for(auto& val : AllTagKpvl)
        if(val->ID)val->UpdateVal();
    for(auto& val : AllTagPeth)
        if(val->ID)val->UpdateVal();

#pragma endregion
}

void InitTag()
{
    InitCurentTag();
    
#pragma region possheet
    std::string comand = "SELECT id, content FROM possheet"; ///* WHERE name = '" + val->Patch + "'*/;";
    PGresult* res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            NamePos[conn_kpvl.PGgetvalue(res, l, 0)] = conn_kpvl.PGgetvalue(res, l, 1);
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    if(!NamePos.size())
    {
        NamePos = {
            {"0", "На входе" },
            {"1", "1-я часть печи"},
            {"2", "2-я часть печи"},
            {"3", "Закалка"},
            {"4", "Охлаждение"},
            {"5", "Кантовка"},
            {"6", "Кантовка"},
            {"7", "В касете"},

            {"10", "Потерян На входе" },
            {"11", "Потерян 1-я часть печи"},
            {"12", "Потерян 2-я часть печи"},
            {"13", "Потерян Закалке"},
            {"14", "Потерян Охлаждении"},
            {"15", "Потерян Кантовке"},
            {"16", "Потерян Кантовке"},
            {"17", "Потерян В касете"},
        };


        //NamePos["0"] = "На входе";
        //NamePos["1"] = "1-я часть печи";
        //NamePos["2"] = "2-я часть печи";
        //NamePos["3"] = "Закалка";
        //NamePos["4"] = "Охлаждение";
        //NamePos["5"] = "Кантовка";
        //NamePos["6"] = "Кантовка";
        //NamePos["7"] = "В касете";
        //
        //NamePos["10"] = "Потерян На входе";
        //NamePos["11"] = "Потерян 1-я часть печи";
        //NamePos["12"] = "Потерян 2-я часть печи";
        //NamePos["13"] = "Потерян Закалке";
        //NamePos["14"] = "Потерян Охлаждении";
        //NamePos["15"] = "Потерян Кантовке";
        //NamePos["16"] = "Потерян Кантовке";
        //NamePos["17"] = "Потерян В касете";

        std::stringstream ss;
        for(std::map <std::string, std::string>::iterator it = NamePos.begin(); it != NamePos.end(); it++)
            ss << "INSERT INTO possheet (id, content) VALUES ('" << it->first << "', '" << it->second << "');\n";
        comand = ss.str();
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        }
        PQclear(res);
    }
#pragma endregion

#pragma region genseq1
    comand = "SELECT id, content FROM genseq1"; ///* WHERE name = '" + val->Patch + "'*/;";
    res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            GenSeq1[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    if(!GenSeq1.size())
    {
        GenSeq1 = {
            {0, "Отключено"},
            {1, "Подготовка"},
            {2, "Прогрев"},
            {3, "Открыть входную дверь"},
            {4, "Загрузка в печь"},
            {5, "Закрыть входную дверь"},
            {6, "Нагрев листа"},
            {7, "Передача на 2 рольганг"},
            {8, "Передача на 2-й рольганг печи"},
        };

        std::stringstream ss;
        for(std::map <int, std::string>::iterator it = GenSeq1.begin(); it != GenSeq1.end(); it++)
            ss << "INSERT INTO genseq1 (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";

        comand = ss.str();
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        }
        PQclear(res);
    }
#pragma endregion

#pragma region genseq2
    comand = "SELECT id, content FROM genseq2"; ///* WHERE name = '" + val->Patch + "'*/;";
    res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            GenSeq2[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    if(!GenSeq2.size())
    {
        GenSeq2 = {
            {0, "Отключено"},
            {1, "Подготовка"},
            {2, "Прогрев"},
            {3, "Прием заготовки с 1-го рольганга печи"},
            {4, "Осциляция. Нагрев Листа"},
            {5, "Открыть выходную дверь"},
            {6, "Выдача в линию закалки"},
            {7, "Закрыть выходную дверь"},
        };

        std::stringstream ss;
        for(std::map <int, std::string>::iterator it = GenSeq2.begin(); it != GenSeq2.end(); it++)
            ss << "INSERT INTO genseq2 (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";

        comand = ss.str();
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        }
        PQclear(res);
    }
#pragma endregion

#pragma region genseq3
    comand = "SELECT id, content FROM genseq3"; ///* WHERE name = '" + val->Patch + "'*/;";
    res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            GenSeq3[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    if(!GenSeq3.size())
    {
        GenSeq3 = {
            {0, "Отключено"},
            {1, "Ожидание листа"},
            {2, "Осциляция. Охл.листа."},
            {3, "Выдача заготовки"},
            {4, "Окончание цикла обработки"},
        };

        std::stringstream ss;
        for(std::map <int, std::string>::iterator it = GenSeq3.begin(); it != GenSeq3.end(); it++)
        {
            ss << "INSERT INTO genseq3 (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";
        }

        comand = ss.str();
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        }
        PQclear(res);
    }
#pragma endregion



}

bool InitSQL()
{
    try
    {
        SQLLogger = InitLogger("BASE_SQL");
        if(!LoadConnect())
        {
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, bagSave);
            //if(!conn_kpvl.connection()) throw std::exception(__FUN(std::string("Error SQL conn_kpvl connection")));

            if(!conn_dops.connection()) throw std::exception(__FUN(std::string("Error SQL conn_dops connection")));
            if(!conn_temp.connection()) throw std::exception(__FUN(std::string("Error SQL conn_temp connection")));
            if(!conn_spis.connection()) throw std::exception(__FUN(std::string("Error SQL conn_spis connection")));
            if(!conn_spic.connection()) throw std::exception(__FUN(std::string("Error SQL conn_spic connection")));
            

            SaveConnect();
        }
        else
        {
            if(!conn_kpvl.connection()) throw std::exception(__FUN(std::string("Error SQL conn_kpvl connection")));
            if(!conn_dops.connection()) throw std::exception(__FUN(std::string("Error SQL conn_dops connection")));
            if(!conn_temp.connection()) throw std::exception(__FUN(std::string("Error SQL conn_temp connection")));
            if(!conn_spis.connection()) throw std::exception(__FUN(std::string("Error SQL conn_spis connection")));
            if(!conn_spic.connection()) throw std::exception(__FUN(std::string("Error SQL conn_spic connection")));


        }
        LOG_INFO(AllLogger, "{:90}|", FUNCTION_LINE_NAME);

//#ifndef TESTWIN
        InitTag();
//#endif
        LOG_INFO(AllLogger, "{:90}|", FUNCTION_LINE_NAME);


    }
    catch(std::exception& exc)
    {
        WinErrorExit(NULL, exc.what());
    }
    catch(...)
    {
        WinErrorExit(NULL, "Unknown error.");
    }


    return conn_kpvl.connections;
}

