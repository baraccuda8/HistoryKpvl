#include <windows.h>

#include <string>
#include <iostream>
#include <strsafe.h>

#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include "Source.h"

#include "pgconnection.h"


std::string m_dbhost = "localhost";
std::string m_dbport = "5432";
std::string m_dbname = "";
std::string m_dbuser = "";
std::string m_dbpass = "";

PGConnection conn;

//std::string m_dbname = "Test";
//std::string m_dbuser = "user";
//std::string m_dbpass = "TutonHamon8*";


PGConnection::~PGConnection()
{
    PQfinish(m_connection);
}


bool PGConnection::connection()
{
    m_connection = PQsetdbLogin(
        m_dbhost.c_str(), 
        m_dbport.c_str(), 
        nullptr, nullptr, 
        m_dbname.c_str(), 
        m_dbuser.c_str(), 
        m_dbpass.c_str());
    try
    {
        if(PQstatus(m_connection) != CONNECTION_OK && PQsetnonblocking(m_connection, 1) != 0)
        {
            connections = false;
            throw std::runtime_error(PQerrorMessage(m_connection));
        }
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

PGresult* PGConnection::PGexec(std::string std)
{
    return PQexec(m_connection, std.c_str());
}




void testConnection(std::string exec)
{
    SetWindowText(ExecuteOut, "");
    std::string s= cp1251_to_utf8(exec);
    PGresult* res = conn.PGexec(s);

    if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res))
    {
        std::stringstream streams;
        int nLiness = PQntuples(res);
        streams << "Найдено " << nLiness << " записей\r\n\r\n";
        int nFields = PQnfields(res);
        for(int i = 0; i < nFields; i++)
            streams << std::setw(10) << utf8_to_cp1251(PQfname(res, i));
        streams << "\r\n\r\n";

        for(int i = 0; i < nLiness; i++)
        {
            for(int j = 0; j < nFields; j++)
                streams << std::setw(10) << utf8_to_cp1251(PQgetvalue(res, i, j));
            streams << "\r\n";
        }
        SetWindowText(ExecuteOut, streams.str().c_str());
    }

    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        std::string errc = utf8_to_cp1251(PQresultErrorMessage(res));
        boost::replace_all(errc, "\n", "\r\n");
        SetWindowText(ExecuteOut, errc.c_str());
    }
    PQclear(res);
}
