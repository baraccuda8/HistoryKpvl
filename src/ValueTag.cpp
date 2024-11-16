#include "pch.h"
#include "main.h"
#include "StringData.h"
#include "file.h"
#include "win.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "Graff.h"

//extern PGConnection conn;


extern std::shared_ptr<spdlog::logger> SQLLogger;

std::string ToString(const OpcUa::DateTime& t)
{
    
    try
    {
        std::time_t st = OpcUa::DateTime::ToTimeT(t);
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
    CATCH(AllLogger, "");
    return "";
}

time_t ToTimeT(OpcUa::DateTime dateTime)
{
    static int64_t secsFrom1970 = 0;
    try
    {
        const int64_t daysBetween1601And1970 = 134774;
        const int64_t secsFrom1601To1970 = daysBetween1601And1970 * 24 * 3600LL;

        if(dateTime.Value < secsFrom1601To1970)
        {
            std::stringstream stream;
            stream << "Дата и время не могут быть меньше " << secsFrom1601To1970;
            throw std::invalid_argument(stream.str());
        }
        secsFrom1970 = dateTime.Value / 10000000LL - secsFrom1601To1970;
    }
    CATCH(AllLogger, "");

    return secsFrom1970;
}

std::string GetValString(OpcUa::Variant& Val, float coeff, std::string format = "")
{
    try
    {
        std::string strVal = "";
        if(isRun)
        {
            if(Val.IsNul()) return "";

            OpcUa::VariantType type = Val.Type();
            if(format.length())
            {
                char ss[256] = "";

                if(type == OpcUa::VariantType::BOOLEAN)        sprintf_s(ss, 255, format.c_str(), Val.As<bool>());
                else if(type == OpcUa::VariantType::SBYTE)     sprintf_s(ss, 255, format.c_str(), Val.As<int8_t>() * (int8_t)coeff);
                else if(type == OpcUa::VariantType::BYTE)      sprintf_s(ss, 255, format.c_str(), Val.As<uint8_t>() * (uint8_t)coeff);
                else if(type == OpcUa::VariantType::INT16)     sprintf_s(ss, 255, format.c_str(), Val.As<int16_t>() * (int16_t)coeff);
                else if(type == OpcUa::VariantType::UINT16)    sprintf_s(ss, 255, format.c_str(), Val.As<uint16_t>() * (uint16_t)coeff);
                else if(type == OpcUa::VariantType::INT32)     sprintf_s(ss, 255, format.c_str(), Val.As<int32_t>() * (int32_t)coeff);
                else if(type == OpcUa::VariantType::UINT32)    sprintf_s(ss, 255, format.c_str(), Val.As<uint32_t>() * (uint32_t)coeff);
                else if(type == OpcUa::VariantType::INT64)     sprintf_s(ss, 255, format.c_str(), Val.As<int64_t>() * (int64_t)coeff);
                else if(type == OpcUa::VariantType::UINT64)    sprintf_s(ss, 255, format.c_str(), Val.As<uint64_t>() * (uint64_t)coeff);
                else if(type == OpcUa::VariantType::FLOAT)     sprintf_s(ss, 255, format.c_str(), Val.As<float>() * (float)coeff);
                else if(type == OpcUa::VariantType::DOUBLE)    sprintf_s(ss, 255, format.c_str(), Val.As<double>() * (double)coeff);
                else if(type == OpcUa::VariantType::STRING)    sprintf_s(ss, 255, format.c_str(), utf8_to_cp1251(Val.ToString()));
                strVal = ss;
            }
            else
            {
                if(type == OpcUa::VariantType::STRING)  strVal = utf8_to_cp1251(Val.ToString());
                else                                    strVal = Val.ToString();
            }
        }
        return strVal;
    }
    CATCH(AllLogger, "");
    return "";
}


void Value::InsertVal()
{
    try
    {
        char command[1024];
        if(GetType() == OpcUa::VariantType::BOOLEAN)
            sprintf_s(command, 1023, "INSERT INTO tag (name, type, arhive, content_at, content, coeff, hist, idsec) VALUES(\'%s\', %u, %s, now(), %s, %f, %f, %u);", &Patch[0], GetType(), (Arhive ? "true" : "false"), Val.ToString().c_str(), coeff, hist, Sec);
        else if(GetType() == OpcUa::VariantType::STRING)
            sprintf_s(command, 1023, "INSERT INTO tag (name, type, arhive, content_at, content, coeff, hist, idsec) VALUES(\'%s\', %u, %s, now(), '%s', %f, %f, %u);", &Patch[0], GetType(), (Arhive ? "true" : "false"), GetString().c_str(), coeff, hist, Sec);
        else
            sprintf_s(command, 1023, "INSERT INTO tag (name, type, arhive, content_at, content, coeff, hist, idsec) VALUES(\'%s\', %u, %s, now(), %s, %f, %f, %u);", &Patch[0], GetType(), (Arhive ? "true" : "false"), GetString().c_str(), coeff, hist, Sec);

        PGresult* res = Conn->PGexec(command);
        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, command);
        PQclear(res);

        GetIdValSQL();
    }
    CATCH(AllLogger, Patch);
}

//Временная функция для обновления таблици tag
void Value::UpdateVal()
{
    try
    {
        if(Update)return;

        int type = (int)Val.Type();

        std::stringstream st;
        st << "INSERT INTO tag (content, name, comment, arhive, type, coeff, hist, format, idsec) VALUES (";
        st << "'" << GetString() << "', ";
        st << "'" << Patch << "', ";
        st << "'" << Comment << "', ";
        st << (Arhive ? "true" : "false") << ", ";
        st << type << ", ";
        st << coeff << ", ";
        st << hist << ", ";
        st << "'" << format << "', ";
        st << Sec << ");";
        std::string command =st.str();
        PGresult* res = Conn->PGexec(command);
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            LOG_ERR_SQL(SQLLogger, res, command);
        PQclear(res);
        GetIdValSQL();
        Update = true;
    }
    CATCH(AllLogger, Patch);
}

void Value::InsertValue(std::string& Create_at)
{
    try
    {
        if(!ID)
            GetIdValSQL();
        if(!ID)
        {
            LOG_INFO(SQLLogger, "{:90}| Error ID {}", FUNCTION_LINE_NAME, Patch);
            return;
        }

        std::stringstream ssd;
        if(GetType() == OpcUa::VariantType::STRING)
            ssd << "INSERT INTO todos (create_at, id_name, content) VALUES('" << Create_at << "', " << ID << ", '" << GetString() << "');";
        else
            ssd << "INSERT INTO todos (create_at, id_name, content) VALUES('" << Create_at << "', " << ID << ", " << GetString() << ");";

        SETUPDATESQL(SQLLogger, (*Conn), ssd);

    }
    CATCH(AllLogger, Patch);
}

void Value::GetIdValSQL()
{
    try
    {
        char command[1024];
        sprintf_s(command, 1023, "SELECT id FROM tag WHERE name = '%s';", Patch.c_str());
        PGresult* res = Conn->PGexec(command);
        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            int line = PQntuples(res);
            if(line)
                ID = atol(conn_kpvl.PGgetvalue(res, 0, 0).c_str());
            else
                LOG_ERR_SQL(SQLLogger, res, command);
        }
        else
            LOG_ERR_SQL(SQLLogger, res, command);

        PQclear(res);
    }
    CATCH(AllLogger, Patch);
}

void Value::TestValSQL()
{
    try
    {
        char command[1024];
        sprintf_s(command, 1023, "SELECT id FROM tag WHERE name = '%s';", Patch.c_str());
        PGresult* res = Conn->PGexec(command);
        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
        if(PQresultStatus(res) == PGRES_TUPLES_OK)
        {
            if(PQntuples(res))
            {
                ID = atol(conn_kpvl.PGgetvalue(res, 0, 0).c_str());
                PQclear(res);
                return;
            }
            else
            {
                PQclear(res);
                InsertVal();
                GetIdValSQL();
            }
        }
        else
        {
            LOG_ERR_SQL(SQLLogger, res, command);
            PQclear(res);
            InsertVal();
            GetIdValSQL();
        }
    }
    CATCH(AllLogger, Patch);
}

void Value::SetGraff()
{
    //try
    //{
    //if(Patch == StrFurn1 + "TempRef")
    //    GraffFurn1.SqlTemp(GraffFurn1.TempRef, this);
    //if(Patch == StrFurn1 + "TempAct")
    //    GraffFurn1.SqlTemp(GraffFurn1.TempAct, this);
    //
    //if(Patch == StrFurn2 + "TempRef")
    //    GraffFurn2.SqlTemp(GraffFurn2.TempRef, this);
    //if(Patch == StrFurn2 + "TempAct")
    //    GraffFurn2.SqlTemp(GraffFurn2.TempAct, this);
    //}
    //CATCH(AllLogger, Patch);
}

void Value::SaveSQL()
{
    try
    {
        if(!Arhive) return;
        if(!ID) TestValSQL();

        bool ifval = OldVal.IsNul();
        if(GetType() == OpcUa::VariantType::FLOAT)
        {
            float f1 = GetFloat() * coeff;
            std::string s = OldVal.ToString();
            float f2 = float(ifval ? 0 : (Stof(s) * coeff));
            float f3 = std::abs(f1 - f2);
            if(f3 > hist || ifval)
            {
                std::string Create_at = GetStringMSDataTime();
                std::stringstream sd;
                sd << "UPDATE tag SET content_at = '"<< Create_at << "', content = ";
                sd << GetString();
                sd << " WHERE id = " << ID;
                //sd << " WHERE name = '" << Patch.c_str() << "';";
                SETUPDATESQL(SQLLogger, (*Conn), sd);

                //std::string command = sd.str();
                //PGresult* res = Conn->PGexec(command);
                ////LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                //    LOG_ERR_SQL(SQLLogger, res, command);
                //PQclear(res);
                InsertValue(Create_at);
                OldVal = Val;
            }
        }
        else if(GetType() == OpcUa::VariantType::DOUBLE)
        {
            double f1 = GetDouble() * coeff;
            std::string s = OldVal.ToString();
            double f2 = double(ifval ? 0 : (Stod(s) * coeff));
            double f3 = std::abs(f1 - f2);
            if(f3 > hist || ifval)
            {
                std::string Create_at = GetStringMSDataTime();
                std::stringstream sd;
                sd << "UPDATE tag SET content_at = '" << Create_at << "', content = ";
                sd << GetString();
                sd << " WHERE id = " << ID;
                //sd << " WHERE name = '" << Patch.c_str() << "';";
                SETUPDATESQL(SQLLogger, (*Conn), sd);

                //std::string command = sd.str();
                //PGresult* res = Conn->PGexec(command);
                ////LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, command);
                //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                //    LOG_ERR_SQL(SQLLogger, res, command);
                //PQclear(res);
                InsertValue(Create_at);
                OldVal = Val;
            }
        }
        else if(GetType() == OpcUa::VariantType::STRING)
        {
            //std::string s1 = GetString();
            std::string s1 = GetValString(Val, coeff, format);
            std::string s2 = GetValString(OldVal, coeff, format);
            if(ifval || s1 != s2)
            {
                std::string Create_at = GetStringMSDataTime();
                std::stringstream sd;
                sd << "UPDATE tag SET content_at = '" << Create_at << "', content = ";
                sd << "'" << s1 << "'";
                sd << " WHERE id = " << ID;
                SETUPDATESQL(SQLLogger, (*Conn), sd);
                InsertValue(Create_at);
                OldVal = Val;
            }
        }
        //else if(GetType() == OpcUa::VariantType::BOOLEAN)
        //{
        //    std::string s1 = GetValString(Val, coeff, format);
        //    std::string s2 = GetValString(OldVal, coeff, format);
        //    if(ifval && s1 != s2)
        //    {
        //        std::stringstream sd;
        //        sd << "UPDATE tag SET content_at = now(), content = ";
        //        sd << s1;
        //        sd << " WHERE id = " << ID;
        //        SETUPDATESQL(SQLLogger, (*Conn), sd);
        //        InsertValue();
        //        OldVal = Val;
        //    }
        //}
        else if(ifval || OldVal != Val)
        {
            std::string Create_at = GetStringMSDataTime();
            std::stringstream sd;
            sd << "UPDATE tag SET content_at = '" << Create_at << "', content = ";
            sd << GetString();
            sd << " WHERE id = " << ID;
            SETUPDATESQL(SQLLogger, (*Conn), sd);
            InsertValue(Create_at);
            OldVal = Val;
        }
    }
    CATCH(AllLogger, Patch);
}

Value::Value(const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn, MSSEC s) :
    Patch(n), strVal(""), oldstrVal(""), winId(hc), FunctionCallbak(fn), Conn(conn), Sec(s)
{
    try
    {
        if(!Patch.length())
            throw std::runtime_error(std::string(FUNCTION_LINE_NAME + std::string("; Error Patch = NULL")).c_str());
        if(Patch.find("WDG") != std::string::npos)
        {
            Arhive = false;
        }
    }
    CATCH(AllLogger, Patch);
};

Value::Value(const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn):
    Patch(n), strVal(""), oldstrVal(""), winId(hc), FunctionCallbak(fn), Conn(conn)
{
    try
    {
        if(!Patch.length())
            throw std::runtime_error(std::string(FUNCTION_LINE_NAME + std::string("; Error Patch = NULL")).c_str());
        if(Patch.find("WDG") != std::string::npos)
        {
            Arhive = false;
        }
    }
    CATCH(AllLogger, Patch);
};

Value::Value (const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn, bool ar, float co, float hi, MSSEC sec, std::string fo, OpcUa::Variant v, std::string com):
    Patch(n), strVal(""), oldstrVal(""), winId(hc), FunctionCallbak(fn), Conn(conn), Arhive(ar), format(fo), Sec(sec), Val(v), OldVal(v), Comment(com)
{
    try
    {
        if(!Patch.length())
            throw std::runtime_error(std::string(FUNCTION_LINE_NAME + std::string("; Error Patch = NULL")).c_str());

        if(Patch.find("WDG") != std::string::npos)
        {
            Arhive = false;
        }
    }
    CATCH(AllLogger, Patch);
}

int ouu = 0;
void vvv (std::string ss)
{
    ouu = 1;
}


void Value::InitNodeId(Client* cds)
{
    try
    {
        if(!cds)
            throw std::runtime_error("Error Codesys = NULL");

        if(Patch.find("ReturnCassetteCmd") != std::string::npos)
        {
            vvv (Patch);
        }
        Codesys = cds;
        NodeId = OpcUa::NodeId(Patch, cds->NamespaceIndex);
    }
    CATCH(AllLogger, Patch);
}

OpcUa::Node Value::GetNode()
{
    try
    {
        if(!Codesys)
            throw std::runtime_error("Error Codesys = NULL");
        return Codesys->GetNode(Patch);
    }
    CATCH(AllLogger, Patch);
    return OpcUa::Node();
}

OpcUa::VariantType Value::GetType()
{
    try
    {
        return Val.Type();
    }
    CATCH(AllLogger, Patch);
    return OpcUa::VariantType::NUL;
}

OpcUa::Variant Value::GetValuew()
{
    if(isRun)
    {
        try
        {
            Val = Node.GetValue();
            if(Val.IsNul())
                throw std::runtime_error("Error Type Variant is NULL");
            GetString();
        }
        CATCH(AllLogger, Patch);
    }
    return Val;
}

OpcUa::Variant Value::GetValue()
{
    if(isRun)
    {
        //try
        {
            if(!Codesys)
                throw std::runtime_error("Error Codesys = NULL");

            if(!Node.IsValid())
            {
                Node = Codesys->client->GetNode(OpcUa::NodeId(Patch, Codesys->NamespaceIndex));
                if(!Node.IsValid())
                    throw std::runtime_error("Error Node Is Not Valid");
            }

            Val = Node.GetValue();
            if(Val.IsNul())
                throw std::runtime_error("Error Type Variant is NULL");
            GetString();
        }
        //CATCH(AllLogger, Patch);
    }
    return Val;
};

void Value::Set_Value(OpcUa::Variant var)
{
    if(isRun)
    {
        try
        {
            if(!Codesys)
                throw std::runtime_error("Error Codesys = NULL");
            
            if(!Node.IsValid())
            {
                Node = Codesys->client->GetNode(OpcUa::NodeId(Patch, Codesys->NamespaceIndex));

                if(!Node.IsValid())
                    throw std::runtime_error("Error Node Is Not Valid");
            }

            Val = var;

            if(Val.IsNul())
                throw std::runtime_error("Error Type Variant is NULL");
#ifndef NOSQL
            Node.SetValue(Val);
#endif
            if(Arhive)
            //if(Patch.find(".WDG_fromBase") == std::string::npos)
                LOG_INFO(SQLLogger, "{:89}| {} {}", FUNCTION_LINE_NAME, Patch, GetString());
        }
        CATCH(AllLogger, Patch);
    }
}
void Value::Set_Value(){
    if(isRun)
    {
        try
        {
            if(!Codesys)
                throw std::runtime_error("Error Codesys = NULL");

            if(!Node.IsValid())
            {
                Node = Codesys->client->GetNode(OpcUa::NodeId(Patch, Codesys->NamespaceIndex));

                if(!Node.IsValid())
                    throw std::runtime_error("Error Node Is Not Valid");
            }

            if(Val.IsNul())
                throw std::runtime_error("Error Type Variant is NULL");

#ifndef NOSQL
            Node.SetValue(Val);
#endif
            if(Arhive)
            ///if(Patch.find(".WDG_fromBase") == std::string::npos)
                LOG_INFO(SQLLogger, "{:89}| {} {}", FUNCTION_LINE_NAME, Patch, GetString());
        }
        CATCH(AllLogger, Patch);
    }
}

std::string Value::GetString(std::string patch)
{
    strVal = GetValString(Val, coeff, format);
    return strVal;
}


std::string Value::GetString()
{
    strVal = GetValString(Val, coeff, format);
    return strVal;
}


bool Value::GetBool()
{
    try
    {
        return Val.As<bool>();
    }
    CATCH(AllLogger, Patch);
    return false;
}

int Value::GetInt()
{
    return Stoi(GetString());
}
float Value::GetFloat()
{
    return Stof(GetString());
}
double Value::GetDouble()
{
    return Stod(GetString());
}




void Value::SetVariant(HWNDCLIENT id)
{
    if(isRun)
    {
        if(!FunctionCallbak)
        {
            GetString();

            if(id != HWNDCLIENT::hNull)  SetWindowText(winmap(id), strVal.c_str());
            if(winId != HWNDCLIENT::hNull)  SetWindowText(winmap(winId), strVal.c_str());
        }
    }
}

void Value::SetVariant(std::string patch, HWNDCLIENT id)
{
    if(isRun)
    {
        if(!FunctionCallbak)
        {
            GetString(patch);

            if(id != HWNDCLIENT::hNull)  SetWindowText(winmap(id), strVal.c_str());
            if(winId != HWNDCLIENT::hNull)  SetWindowText(winmap(winId), strVal.c_str());
        }
    }
}

bool Value::Find(const uint32_t h, const std::string p, const OpcUa::Variant& v, OpcUa::AttributeId a)
{
    if(isRun)
    {
        if(p.length() && p == Patch)
        {
            attr = a;
            handle = h;
            return Find(h, v);
        }
    }
    return false;
}

bool Value::Find(const uint32_t h, const OpcUa::Variant& v)
{
    if(isRun)
    {
        if(Val.Type() != v.Type()) //Val.Type() != v.Type())
        {
            LOG_WARN(AllLogger, "{:90}| Ошибка типа: SQL:{} != OPC:{} Tag: {}", FUNCTION_LINE_NAME, Val.Type(), v.Type(), Patch);
        }
        Val = v;
        SaveSQL();
        if(FunctionCallbak)
        {
            GetString(Patch);
            FunctionCallbak(this);
        }
        else
        {
            SetVariant(Patch);
        }
    }
    return true;
}

bool Value::TestNode(Client* cds)
{
    if(isRun)
    {

        if(!Patch.length())return false;

        if(cds)
        {
            Codesys = cds;

            SetWindowText(hWndDebug, (std::string("Проверка Patch = ") + Patch).c_str());

            Node = cds->client->GetNode(OpcUa::NodeId(Patch, cds->NamespaceIndex));

            if(!Node.IsValid()) 
                return false;

            return true;
        }
    }
    return false;
}

void MySetWindowText(HWND h, const char* ss)
{
    if(h)
    {
        SetWindowText(h, ss);
        InvalidateRect(h, NULL, false);
    }
}

void MySetWindowText(HWND h, std::string ss)
{
    if(h)
    {
        SetWindowText(h, ss.c_str());
        InvalidateRect(h, NULL, false);
    }
}

void MySetWindowText(Value* value)
{
    HWND h = winmap(value->winId);
    if(value && h)
    {
        SetWindowText(h, value->GetString().c_str());
        InvalidateRect(h, NULL, false);
    }
}
