#include "pch.h"
#include "main.h"
#include "win.h"
#include "StringData.h"
#include "ValueTag.h"
#include "term.h"
#include "hard.h"
#include "Graff.h"
#include "KPVL.h"
#include "Furn.h"
#include "Pdf.h"

#include "ClCodeSys.h"
#include "file.h"
#include "ValueTag.h"

//#include "hard.h"
//#include "SQL.h"

//Соединение с SQL базой
int DEB = 0;
PGConnection conn_spis;
PGConnection conn_spic;
PGConnection conn_kpvl;
PGConnection conn_kpvl2;
PGConnection conn_dops;
PGConnection conn_temp;


std::shared_ptr<spdlog::logger> SQLLogger = NULL;

//Таблица толщин листов
//std::map <int, std::map<int, std::string>>MapThicknessCode;

std::string m_dbhost = "localhost";
std::string m_dbport = "5432";
std::string m_dbname = "";
std::string m_dbuser = "";
std::string m_dbpass = "";

extern std::deque<Value*> AllTagKpvl;
extern std::deque<Value*> AllTagPeth;
extern std::map<std::string, std::string> NamePos;
extern std::map<int, std::string> GenSeq1;
extern std::map<int, std::string> GenSeq2;
extern std::map<int, std::string> GenSeq3;
extern std::map<int, std::string> EventCassette;

namespace KPVL {
    extern uint32_t NextID;
};


namespace LoginDlg
{
#define SQLFileName "PostgreSQL.dat"

    std::string pKey = "хабраbarracudabarracudaхабра";
    char sCommand[0xFFFF];
    char sCommand2[0xFFFF];


    void encode(char* pText, size_t len)
    {
        for(size_t i = 0; i < len; i++)
            pText[i] = (byte)(pText[i] ^ pKey[i % pKey.length()]);
    }

    void SaveConnect()
    {
        std::stringstream pass;
        pass << m_dbhost << std::endl
            << m_dbport << std::endl
            << m_dbname << std::endl
            << m_dbuser << std::endl
            << m_dbpass;

        std::string p = pass.str();
        memset(sCommand2, 0, 0xFFFF);
        strcpy_s(sCommand2, 255, p.c_str());;
        encode(sCommand2, p.length());

        std::ofstream s(SQLFileName, std::ios::binary | std::ios::out | std::ios::trunc);
        if(s.is_open())
        {
            s.write(sCommand2, p.length());
            s.close();
        }
    }

    bool LoadConnect()
    {
        memset(sCommand2, 0, 0xFFFF);
        std::ifstream s(SQLFileName, std::ios::binary | std::ios::in);
        if(s.is_open())
        {
            s.read(sCommand2, 1024);
            int len = (int)s.gcount();
            s.close();
            encode(sCommand2, len);
            std::vector <std::string>split;
            boost::split(split, sCommand2, boost::is_any_of("\n"));
            if(split.size() == 5)
            {
                m_dbhost = split[0];
                m_dbport = split[1];
                m_dbname = split[2];
                m_dbuser = split[3];
                m_dbpass = split[4];
                return TRUE;
            }
        }
        return FALSE;
    }

    INT_PTR InitDialog(HWND hWnd)
    {
        CenterWindow(hWnd, NULL);
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT1), m_dbhost.c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT2), m_dbport.c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT3), m_dbname.c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT4), m_dbuser.c_str());
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT5), m_dbpass.c_str());
        return 0;
    }

    INT_PTR CommandDialog(HWND hWnd, WPARAM wParam)
    {
        if(wParam == IDOK)
        {
            char ss[256];
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT1), ss, 256);    m_dbhost = ss;
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT2), ss, 256);    m_dbport = ss;
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT3), ss, 256);    m_dbname = ss;
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT4), ss, 256);    m_dbuser = ss;
            GetWindowText(GetDlgItem(hWnd, IDC_EDIT5), ss, 256);    m_dbpass = ss;
            CONNECTION1(conn_spis, SQLLogger);
            if(conn_spis.connections)
            {
                SaveConnect();
                EndDialog(hWnd, FALSE);
            }
            else
            {
                MessageBox(NULL, "Ошибка соединения с базой!", "Ошибка!", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
            }
        }
        if(wParam == IDCANCEL)
        {
            EndDialog(hWnd, FALSE);
            Quit();
        }
        return 0;
    }

    INT_PTR CALLBACK bagSave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if(message == WM_INITDIALOG)return InitDialog(hWnd);
        if(message == WM_COMMAND) return CommandDialog(hWnd, wParam);
        return (0);
    }
};


#pragma region class PGConnection

std::string PGgetvalue(PGresult* res, int l, int i)
{
    try
    {
        std::string ss = PQgetvalue(res, l, i);
        if(!ss.empty())
            return utf8_to_cp1251(ss);
    }
    CATCH(SQLLogger, "");

    return "";
}


PGConnection::PGConnection() :m_connection(NULL), connections(false)
{
};

PGConnection::~PGConnection()
{
    if(connections)::PQfinish(m_connection);
    connections = false;
};

bool PGConnection::Сonnection(std::string name){
    if(connections)return connections;

    m_connection = ::PQsetdbLogin(m_dbhost.c_str(), m_dbport.c_str(), nullptr, nullptr, m_dbname.c_str(), m_dbuser.c_str(), m_dbpass.c_str());

    ConnStatusType st = ::PQstatus(m_connection);
    if(st != CONNECTION_OK && ::PQsetnonblocking(m_connection, 1) != 0)
    {
        connections = false;
        throw std::runtime_error(::PQerrorMessage(m_connection));
    }

    PGresult* res = ::PQexec(m_connection, "set time zone 'Asia/Yekaterinburg'");
    ExecStatusType sd = ::PQresultStatus(res);
    if(sd != PGRES_COMMAND_OK)
    {
        std::string errc = utf8_to_cp1251(::PQresultErrorMessage(res));
        throw std::runtime_error(errc);
    }
    PQclear(res);

    connections = true;

    return connections;
}

int PGConnection::PQntuples(PGresult* res)
{
    if(!connections) return 0;
    return ::PQntuples(res);
}

PGresult* PGConnection::PGexec(std::string std){
    if(!connections) return NULL;
#ifdef NOSQL 
    std::string st = std;
    boost::to_upper(st);
    if(st.find("SELECT ") != 0)
        return NULL;
#endif
    return ::PQexec(m_connection, cp1251_to_utf8(std).c_str());

};

PGresult* PGConnection::PGexec(std::stringstream std){
    if(!connections) return NULL;
#ifdef NOSQL 
    std::string st = std.str();
    boost::to_upper(st);
    if(st.find("SELECT ") != 0)
        return NULL;
#endif
    return ::PQexec(m_connection, cp1251_to_utf8(std.str()).c_str());
};

std::string PGConnection::PGgetvalue(PGresult* res, int l, int i)
{
    if(!connections) return "";
    try
    {
        std::string ss = PQgetvalue(res, l, i);
        if(!ss.empty())
            return utf8_to_cp1251(ss);
    }
    CATCH(SQLLogger, "");
    return "";
}

#pragma endregion


bool cmpMaxMin(Value* first, Value* second)
{
    return first->ID< second->ID;
}


void SetValue(OpcUa::VariantType type, Value* val, std::string value)
{
    if(type == OpcUa::VariantType::BOOLEAN)        val->Val = (bool)(value == "true");      // atoi_t(bool, atoi, value);
    else if(type == OpcUa::VariantType::SBYTE)     val->Val = int8_t(int8_t(std::stoi(value)) / (int8_t)val->coeff);
    else if(type == OpcUa::VariantType::BYTE)      val->Val = uint8_t(uint8_t(std::stoi(value)) / (uint8_t)val->coeff);
    else if(type == OpcUa::VariantType::INT16)     val->Val = int16_t(int16_t(std::stoi(value)) / (int16_t)val->coeff);
    else if(type == OpcUa::VariantType::UINT16)    val->Val = uint16_t(uint16_t(std::stoi(value)) / (uint16_t)val->coeff);
    else if(type == OpcUa::VariantType::INT32)     val->Val = int32_t(int32_t(std::stoi(value)) / (int32_t)val->coeff);
    else if(type == OpcUa::VariantType::UINT32)    val->Val = uint32_t(uint32_t(std::stoll(value)) / (uint32_t)val->coeff);
    else if(type == OpcUa::VariantType::INT64)     val->Val = int64_t(int64_t(std::stoll(value)) / (int64_t)val->coeff);
    else if(type == OpcUa::VariantType::UINT64)    val->Val = uint64_t(uint64_t(std::stod(value)) / (uint64_t)val->coeff);
    else if(type == OpcUa::VariantType::FLOAT)     val->Val = float(float(std::stof(value)) / (float)val->coeff);
    else if(type == OpcUa::VariantType::DOUBLE)    val->Val = double(double(std::stof(value)) / (double)val->coeff);
    else if(type == OpcUa::VariantType::STRING)    val->Val =  cp1251_to_utf8(value); // utf8_to_cp1251(value); //cp1251_to_utf8
    val->GetString();
    val->OldVal = val->Val;
}

namespace CollTag{

    int Id = 0;
    int Name = 0;
    int Type = 0;
    int Arhive = 0;
    int Comment = 0;
    int Content = 0;
    int Coeff = 0;
    int Hist = 0;
    int Format = 0;
    int Idsec = 0;
}

namespace TODOS
{
    int create_at = 0;
    int id = 0;
    int id_name = 0;
    int content = 0;
    int type = 0;
    int name = 0;
};



std::string GetType(OpcUa::VariantType type)
{
    if(type == OpcUa::VariantType::BOOLEAN)        return "bool";
    else if(type == OpcUa::VariantType::SBYTE)     return "int8_t";
    else if(type == OpcUa::VariantType::BYTE)      return "uint8_t";
    else if(type == OpcUa::VariantType::INT16)     return "int16_t";
    else if(type == OpcUa::VariantType::UINT16)    return "uint16_t";
    else if(type == OpcUa::VariantType::INT32)     return "int32_t";
    else if(type == OpcUa::VariantType::UINT32)    return "uint32_t";
    else if(type == OpcUa::VariantType::INT64)     return "int64_t";
    else if(type == OpcUa::VariantType::UINT64)    return "uint64_t";
    else if(type == OpcUa::VariantType::FLOAT)     return "float";
    else if(type == OpcUa::VariantType::DOUBLE)    return "double";
    else if(type == OpcUa::VariantType::STRING)    return "string";
    return "";
}

void GetTagTable(std::deque<Value*>& All, std::string Patch, PGresult* res, int l, std::ofstream& ofs)
{
    for(auto& val : All)
    {
        if(val->Patch == Patch)
        {

            val->ID = atol(conn_spis.PGgetvalue(res, l, CollTag::Id).c_str());
            OpcUa::VariantType type =  static_cast<OpcUa::VariantType>(Stoi(conn_spis.PGgetvalue(res, l, CollTag::Type)));
            val->Arhive = conn_spis.PGgetvalue(res, l, CollTag::Arhive) == "t";
            val->Comment = conn_spis.PGgetvalue(res, l, CollTag::Comment);
            std::string value = conn_spis.PGgetvalue(res, l, CollTag::Content);
            val->coeff = static_cast<float>(atof(conn_spis.PGgetvalue(res, l, CollTag::Coeff).c_str()));
            val->hist = static_cast<float>(atof(conn_spis.PGgetvalue(res, l, CollTag::Hist).c_str()));
            val->format = conn_spis.PGgetvalue(res, l, CollTag::Format);
            if(!val->Sec) val->Sec = static_cast<MSSEC>(atoi(conn_spis.PGgetvalue(res, l, CollTag::Idsec).c_str()));
            if(!val->Sec) val->Sec = MSSEC::sec01000;

            SetValue(type, val, value);
            

            //boost::replace_all(value, ".", ",");
            if(!ofs.bad())
                ofs << val->ID << ";"
                << "\"\"\"" << val->Patch << "\"\"\";" 
                << GetType(type) << ";"
                << static_cast<int>(type) << ";"
                << val->Arhive << ";" 
                << "\"\"\"" << val->Comment << "\"\"\";" 
                << val->GetString() << ";" 
                << val->coeff << ";" 
                << val->hist << ";" 
                << "\"\"\"" << val->format << "\"\"\";" 
                << val->Sec << ";" 
                << std::endl;

            val->Update = true;
            //LOG_INFO(SQLLogger, "{:90}| ID = {}, coeff = {}, Val = {}, Patch = {}", FUNCTION_LINE_NAME, val->ID, val->coeff, val->GetString(), val->Patch);
            break;
        }
    }
}

//void GetPetch(S107::T_cass& tc, int p)
//{
//    std::string comand = "SELECT id FROM cassette2 WHERE peth = " + std::to_string(p) + " ORDER BY id ASC LIMIT 1";
//    PGresult* res = conn_spis.PGexec(comand);
//    if(PQresultStatus(res) == PGRES_TUPLES_OK)
//    {
//        int line = PQntuples(res);
//        if(line)
//        {
//            std::string sid = conn_spis.PGgetvalue(res, line - 1, 0);
//            if(sid.length())
//                tc.id = Stoi(sid);
//        }
//    }
//    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
//        LOG_ERR_SQL(SQLLogger, res, comand);
//    PQclear(res);
//
//    if(tc.id)
//    {
//        std::string comand = "SELECT day, month, year, cassetteno, run_at, error_at, end_at FROM cassette2 WHERE id = " + std::to_string(tc.id);
//        PGresult* res = conn_spis.PGexec(comand);
//        if(PQresultStatus(res) == PGRES_TUPLES_OK)
//        {
//            int line = PQntuples(res);
//            if(line)
//            {
//                tc.End_at = conn_spis.PGgetvalue(res, line - 1, 6);
//                if(tc.End_at.size())
//                    tc = S107::T_cass();
//                else
//                {
//                    tc.Day = Stoi(conn_spis.PGgetvalue(res, line - 1, 0));
//                    tc.Month = Stoi(conn_spis.PGgetvalue(res, line - 1, 1));
//                    tc.Year = Stoi(conn_spis.PGgetvalue(res, line - 1, 2));
//                    tc.CassetteNo = Stoi(conn_spis.PGgetvalue(res, line - 1, 3));
//                    tc.Run_at = conn_spis.PGgetvalue(res, line - 1, 4);
//                    tc.Err_at = conn_spis.PGgetvalue(res, line - 1, 5);
//                }
//            }
//        }
//
//        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
//            LOG_ERR_SQL(SQLLogger, res, comand);
//        PQclear(res);
//
//
//    }
//}


void InitCurentTag()
{
#pragma region SELECT id, name, type, arhive, comment, content, coeff, hist, format, idsec FROM tag ORDER BY id 
    std::ofstream ofs("all_tag.csv", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
    if(!ofs.bad())
        ofs << "id;name;type;type;arhive;comment;content;coeff;hist;format;idsec" << std::endl;

    std::string comand = "SELECT id, name, type, arhive, comment, content, coeff, hist, format, idsec FROM tag ORDER BY type, name;";

    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    PGresult* res = conn_spis.PGexec(comand);
    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        if(!CollTag::Idsec)
        {
            int nFields = PQnfields(res);
            for(int j = 0; j < nFields; j++)
            {
                std::string l =  utf8_to_cp1251(PQfname(res, j));
                if(l == "id") CollTag::Id = j;
                else if(l == "name") CollTag::Name = j;
                else if(l == "type") CollTag::Type = j;
                else if(l == "arhive") CollTag::Arhive = j;
                else if(l == "comment") CollTag::Comment = j;
                else if(l == "content") CollTag::Content = j;
                else if(l == "coeff") CollTag::Coeff = j;
                else if(l == "hist") CollTag::Hist = j;
                else if(l == "format") CollTag::Format = j;
                else if(l == "idsec") CollTag::Idsec = j;
            }
        }

        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
        {
            std::string Patch = conn_spis.PGgetvalue(res, l, CollTag::Name);
            if(Patch.find("PLC210 OPC-UA") != std::string::npos)
            {
                GetTagTable(AllTagKpvl, Patch, res, l, ofs);
            }
            else if(Patch.find("SPK107 (M01)") != std::string::npos)
            {
                GetTagTable(AllTagPeth, Patch, res, l, ofs);
            }
        }
    }

    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        LOG_ERR_SQL(SQLLogger, res, comand);
    PQclear(res);

    //for(auto& val : AllTagPeth)
    //{
    //    if(val->Patch.find("1.Data.ProcRun") != std::string::npos)
    //    {
    //        id1ProcRun = val->ID;
    //    }
    //    if(val->Patch.find("1.Data.ProcEnd") != std::string::npos)
    //    {
    //        id1ProcEnd = val->ID;
    //    }
    //    if(val->Patch.find("1.Data.ProcFault") != std::string::npos)
    //    {
    //        id1ProcError = val->ID;
    //    }
    //    if(val->Patch.find("2.Data.ProcRun") != std::string::npos)
    //    {
    //        id2ProcRun = val->ID;
    //    }
    //    if(val->Patch.find("2.Data.ProcEnd") != std::string::npos)
    //    {
    //        id2ProcEnd = val->ID;
    //    }
    //    if(val->Patch.find("2.Data.ProcFault") != std::string::npos)
    //    {
    //        id2ProcError = val->ID;
    //    }
    //}

    for(auto& val : AllTagKpvl)
    {
        val->UpdateVal();
    }
    for(auto& val : AllTagPeth)
    {
        val->UpdateVal();
    }

    //GetPetch(S107::Furn1::Petch, 1);
    //GetPetch(S107::Furn2::Petch, 2);


    if(!ofs.bad())
        ofs.close();
    int tt = 0;
#pragma endregion
}

void InitTag()
{
    InitCurentTag();
    
#pragma region SELECT id, content FROM possheet
    std::string comand = "SELECT id, content FROM possheet"; ///* WHERE name = '" + val->Patch + "'*/;";
    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    PGresult* res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            NamePos[conn_kpvl.PGgetvalue(res, l, 0)] = conn_kpvl.PGgetvalue(res, l, 1);
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        LOG_ERR_SQL(SQLLogger, res, comand);
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

            {"20", "Потерян На входе" },
            {"21", "Потерян 1-я часть печи"},
            {"22", "Потерян 2-я часть печи"},
            {"23", "Потерян Закалке"},
            {"24", "Потерян Охлаждении"},
            {"25", "Потерян Кантовке"},
            {"26", "Потерян Кантовке"},
            {"27", "Потерян В касете"},
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
        {
            ss << "INSERT INTO possheet (id, content) VALUES ('" << it->first << "', '" << it->second << "');\n";
            NamePos[it->first] = it->second;
        }
        comand = ss.str();
        if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);
    }
#pragma endregion

#pragma region SELECT id, content FROM EventCassette
    comand = "SELECT id, content FROM EventCassette"; ///* WHERE name = '" + val->Patch + "'*/;";
    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            EventCassette[(evCassete::EV)Stoi(conn_kpvl.PGgetvalue(res, l, 0))] = conn_kpvl.PGgetvalue(res, l, 1).c_str();
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        LOG_ERR_SQL(SQLLogger, res, comand);
    PQclear(res);

    if(!EventCassette.size())
    {
        std::map<int, std::string> eventcassette ={
            {evCassete::Nul,  "Неизвестно"},
            {evCassete::Fill, "Набирается"},
            {evCassete::Wait, "Ожидает"},
            {evCassete::Rel, "Отпуск"},
            {evCassete::Error, "Авария"},
            {evCassete::End, "Конец"},
            {evCassete::History, "Из базы"},
            {evCassete::Delete, "Удален"},
        };

        std::stringstream ss;
        for(std::map <int, std::string>::iterator it = eventcassette.begin(); it != eventcassette.end(); it++)
        {
            ss << "INSERT INTO EventCassette (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";
            EventCassette[it->first] = it->second;
        }

        comand = ss.str();
        if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);
    }
#pragma endregion

#pragma region SELECT id, content FROM genseq1
    comand = "SELECT id, content FROM genseq1"; ///* WHERE name = '" + val->Patch + "'*/;";
    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            GenSeq1[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        LOG_ERR_SQL(SQLLogger, res, comand);
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
        {
            ss << "INSERT INTO genseq1 (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";
            GenSeq1[it->first] = it->second;
        }

        comand = ss.str();
        if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);
    }
#pragma endregion

#pragma region SELECT id, content FROM genseq2
    comand = "SELECT id, content FROM genseq2"; ///* WHERE name = '" + val->Patch + "'*/;";
    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            GenSeq2[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        LOG_ERR_SQL(SQLLogger, res, comand);
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
        {
            ss << "INSERT INTO genseq2 (id, content) VALUES (" << it->first << ", '" << it->second << "');\n";
            GenSeq2[it->first] = it->second;
        }

        comand = ss.str();
        if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);
    }
#pragma endregion

#pragma region SELECT id, content FROM genseq3
    comand = "SELECT id, content FROM genseq3"; ///* WHERE name = '" + val->Patch + "'*/;";
    if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    res = conn_kpvl.PGexec(comand);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        for(int l = 0; l < line; l++)
            GenSeq3[atoi(conn_kpvl.PGgetvalue(res, l, 0).c_str())] = conn_kpvl.PGgetvalue(res, l, 1);
    }
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        LOG_ERR_SQL(SQLLogger, res, comand);
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
            GenSeq3[it->first] = it->second;
        }

        comand = ss.str();
        if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
        res = conn_kpvl.PGexec(comand);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, comand);
        PQclear(res);
    }
#pragma endregion

}


void ConnectSQL()
{
    CONNECTION1(conn_spis, SQLLogger);
    CONNECTION1(conn_kpvl, SQLLogger);
    CONNECTION1(conn_kpvl2, SQLLogger);
    CONNECTION1(conn_dops, SQLLogger);
    CONNECTION1(conn_temp, SQLLogger);
    CONNECTION1(conn_spic, SQLLogger);
}

bool InitSQL()
{
    try
    {
        InitLogger(SQLLogger);
        if(!LoginDlg::LoadConnect())
        {
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, LoginDlg::bagSave);

            ConnectSQL();
            LoginDlg::SaveConnect();
        }
        else
        {
            ConnectSQL();
        }
        InitTag();
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

