#pragma once


#include <libpq-fe.h>
#include <file.h>

#pragma comment(lib, "libpq.lib")
extern int DEB;
extern std::shared_ptr<spdlog::logger> SQLLogger;

extern std::string m_dbhost;
extern std::string m_dbport;
extern std::string m_dbname;
extern std::string m_dbuser;
extern std::string m_dbpass;


class PGConnection
{
public:
    PGconn* m_connection;
    bool connections = false;
    PGConnection() { };
    void PGDisConnection() { if(connections)PQfinish(m_connection); connections = false; };
    ~PGConnection(){ PGDisConnection(); };
    bool connection(){
        try
        {
            m_connection = PQsetdbLogin(m_dbhost.c_str(), m_dbport.c_str(), nullptr, nullptr, m_dbname.c_str(), m_dbuser.c_str(), m_dbpass.c_str());

            if(PQstatus(m_connection) != CONNECTION_OK && PQsetnonblocking(m_connection, 1) != 0)
            {
                connections = false;
                throw std::runtime_error(PQerrorMessage(m_connection));
            }
            PGresult* res = PGexec("set time zone 'Asia/Yekaterinburg';");
            if(PQresultStatus(res) == ExecStatusType::PGRES_TUPLES_OK)
            {
                std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                LOG_INFO(AllLogger, "{:90}| conn_kpvl {}", FUNCTION_LINE_NAME, errc);
            }
            PQclear(res);

            connections = true;
        }
        catch(std::runtime_error rt)
        {
            MessageBox(NULL, rt.what(), "Error", 0);
        }
        catch(...)
        {
            MessageBox(NULL, "Неизвестная ошибка", "Error", 0);
        }
        return connections;
    }
    
    PGresult* PGexec(std::string std){ 
        //SendDebug("<SQL> " + std);
        return 
            PQexec(m_connection, cp1251_to_utf8(std).c_str());
    };

    PGresult* MyPQexec(std::string std){
    //SendDebug("<SQL> " + std);
        return
            PQexec(m_connection, std.c_str());
    };

    std::string PGgetvalue(PGresult* res, int l, int i)
    {
        std::string ss = PQgetvalue(res, l, i);
        if(!ss.empty())
            return utf8_to_cp1251(ss);
        else return "";
    }

};


void InitCurentTag();
bool InitSQL();

extern PGConnection conn_kpvl;
extern PGConnection conn_kpvl2;

extern PGConnection conn_dops;
extern PGConnection conn_temp;
extern PGConnection conn_spis;
extern PGConnection conn_spic;

extern PGConnection conn_grqff1;
extern PGConnection conn_grqff2;
extern PGConnection conn_grqff3;

