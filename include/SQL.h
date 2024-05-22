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

#define SETUPDATESQL(_l, _c, _s) \
{\
    std::string comand = _s.str(); \
    if(DEB)LOG_INFO(_l, "{:90}| {}", FUNCTION_LINE_NAME, comand); \
    PGresult* res = _c.PGexec(comand); \
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)\
        LOG_ERR_SQL(_l, res, comand); \
    PQclear(res);\
}


class PGConnection
{
public:
    PGconn* m_connection;
    bool connections = false;
    std::string Name = "";
    PGConnection() { };
    //void PGDisConnection() 
    //{ 
    //    if(connections)PQfinish(m_connection); 
    //    connections = false; 
    //};
    ~PGConnection()
    { 
        if(connections)PQfinish(m_connection); 
        connections = false; 
    };
    bool connection(){
        try
        {
            if(connections)return connections;

            m_connection = PQsetdbLogin(m_dbhost.c_str(), m_dbport.c_str(), nullptr, nullptr, m_dbname.c_str(), m_dbuser.c_str(), m_dbpass.c_str());

            //if(PQstatus(m_connection) == CONNECTION_OK && PQsetnonblocking(m_connection, 1) != 0)
            if(PQstatus(m_connection) != CONNECTION_OK && PQsetnonblocking(m_connection, 1) != 0)
            {
                connections = false;
                throw std::runtime_error(PQerrorMessage(m_connection));
                //LOG_INFO(AllLogger, "{:90}| conn_kpvl {}", FUNCTION_LINE_NAME, errc);
            }
            PGresult* res = PGexec("set time zone 'Asia/Yekaterinburg';");
            if(PQresultStatus(res) != ExecStatusType::PGRES_TUPLES_OK)
            {
                std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
                LOG_INFO(AllLogger, "{:90}| conn_kpvl {}", FUNCTION_LINE_NAME, errc);
            }
            PQclear(res);

            connections = true;
        }
        catch(std::runtime_error rt)
        {
            LOG_INFO(AllLogger, "{:90}| error conn_kpvl {} {}", FUNCTION_LINE_NAME, rt.what());
            //MessageBox(NULL, rt.what(), "Error", 0);
        }
        catch(...)
        {
            LOG_ERROR(AllLogger, "{:90}| Unknown error conn_kpvl {}", FUNCTION_LINE_NAME);
            //MessageBox(NULL, "Неизвестная ошибка", "Error", 0);
        }
        return connections;
    }
    
    int PQntuples(PGresult* res)
    {
        if(!connections) return 0;
        return ::PQntuples(res);
    }

    PGresult* PGexec(std::string std){ 
        if(!connections) return 0;
        return 
            ::PQexec(m_connection, cp1251_to_utf8(std).c_str());
    };

    PGresult* MyPQexec(std::string std){
        if(!connections) return 0;
        return
            ::PQexec(m_connection, std.c_str());
    };

    std::string PGgetvalue(PGresult* res, int l, int i)
    {
        if(!connections) return "";
        std::string ss = PQgetvalue(res, l, i);
        if(!ss.empty())
            return utf8_to_cp1251(ss);
        else return "";
    }

};


namespace CollTag{
    extern int Id;
    extern int Name;
    extern int Type;
    extern int Arhive;
    extern int Comment;
    extern int Content;
    extern int Coeff;
    extern int Hist;
    extern int Format;
    extern int Idsec;
}

namespace TODOS
{
    extern int create_at;
    extern int id;
    extern int id_name;
    extern int content;
    extern int type;
    extern int name;
};

typedef struct T_Todos{
    std::string create_at = "";
    std::string id_name_at = "";
    std::string value = "";
    int id = 0;
    int id_name = 0;
    int type = 0;
    int Petch = 0;
    OpcUa::Variant content;
}T_Todos;



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

