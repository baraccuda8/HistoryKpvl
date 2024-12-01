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

#define CONNECTION1(_n, _l) {try{if(!_n.connections  && !_n.Ñonnection(#_n) )throw std::exception((std::string("Error SQL ") + #_n + " connection").c_str());}CATCH(_l, "");}
#define CONNECTION2(_n, _l) {try{if(!_n->connections && !_n->Ñonnection(#_n))throw std::exception((std::string("Error SQL ") + #_n + " connection").c_str());}CATCH(_l, "");}

#define SETUPDATESQL(_l, _c, _s){\
    std::string _comand = _s.str(); \
    if(DEB)LOG_INFO(_l, "{:90}| {}", FUNCTION_LINE_NAME, _comand); \
    PGresult* _res = _c.PGexec(_comand); \
    if(PQresultStatus(_res) == PGRES_FATAL_ERROR)\
        LOG_ERR_SQL(_l, _res, _comand); \
    PQclear(_res);\
}

std::string PGgetvalue(PGresult * res, int l, int i);

class PGConnection
{
public:
    PGconn* m_connection = NULL;
    bool connections = false;
    PGConnection();
    ~PGConnection();
    bool Ñonnection(std::string name);

    PGresult* PGexec(std::string std);
    PGresult* PGexec(std::stringstream std);

    std::string PGgetvalue(PGresult* res, int l, int i);

    int PQntuples(PGresult* res);
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


