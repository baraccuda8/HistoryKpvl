#include "pch.h"
#include "main.h"
#include "file.h"
#include "win.h"
#include "ClCodeSys.h"
#include "SQL.h"
#include "ValueTag.h"
#include "Graff.h"

//extern PGConnection conn;

extern std::shared_ptr<spdlog::logger> SQLLogger;

void Value::InsertVal()
{
    char comand[1024];
//    comand += "'" + Patch + "', " + std::to_string((int)GetType()) + ", " + (Arhive ? "true" : "false") + ", now(), ";
    if(GetType() == OpcUa::VariantType::BOOLEAN)
        sprintf_s(comand, 1023, "INSERT INTO tag (name, type, arhive, content_at, content, coeff, hist) VALUES(\'%s\', %u, %s, now(), %s, %f, %f);", &Patch[0], GetType(), (Arhive ? "true" : "false"), Val.ToString().c_str(), coeff, hist);
        //comand += Val.ToString() + ");";
    else if(GetType() == OpcUa::VariantType::STRING)
        sprintf_s(comand, 1023, "INSERT INTO tag (name, type, arhive, content_at, content, coeff, hist) VALUES(\'%s\', %u, %s, now(), '%s', %f, %f);", &Patch[0], GetType(), (Arhive ? "true" : "false"), GetString().c_str(), coeff, hist);
        //comand += "'" + GetString() + "');";
    else
        sprintf_s(comand, 1023, "INSERT INTO tag (name, type, arhive, content_at, content, coeff, hist) VALUES(\'%s\', %u, %s, now(), %s, %f, %f);", &Patch[0], GetType(), (Arhive ? "true" : "false"), GetString().c_str(), coeff, hist);

        //comand += GetString() + ");";

    PGresult* res = Conn->PGexec(comand);
    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        //char* s = PQresultErrorMessage(res);
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);

    GetIdValSQL();
}

void Value::UpdateVal()
{
    if(Update)return;
    //char comand[1024];
    //sprintf_s(comand, 1023, "UPDATE tag SET comment = '%s' WHERE id = %u;", Comment.c_str(), ID);
    //PGresult* res = Conn->PGexec(comand);
    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    //{
    //    LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
    //    LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    //}
    //PQclear(res);

    Update = true;
}

void Value::InsertValue()
{
    char comand[1024];
    if(!ID)
        GetIdValSQL();
    if(!ID)
    {
        LOG_INFO(SQLLogger, "{:90}| Error ID {}", FUNCTION_LINE_NAME, Patch);
        return;
    }

    sprintf_s(comand, 1023, "INSERT INTO todos (id_name, content) VALUES( %u, '%s');", ID, GetString().c_str());

    //std::string comand = "INSERT INTO todos (id_name, content) VALUES( (SELECT id FROM tag WHERE tag.name = '" + Patch + "')";
    ////comand += std::to_string(handle);
    //comand += ", '";
    //comand += GetString();
    //comand += "');";
    PGresult* res = Conn->PGexec(comand);
    //PGresult* res = PQexec(Conn.m_connection, comand);
    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    if(PQresultStatus(res) == PGRES_FATAL_ERROR)
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
    }
    PQclear(res);
}

void Value::GetIdValSQL()
{
        //sprintf_s(Test, 1023, "SELECT id FROM tag WHERE name = '%s';", Patch.c_str());
    char Test[1024];
    sprintf_s(Test, 1023, "SELECT id FROM tag WHERE name = '%s';", Patch.c_str());
    PGresult* res = Conn->PGexec(Test);
    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, Test);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        if(line)
            ID = atol(conn_kpvl.PGgetvalue(res, 0, 0).c_str());
        else
        {
            LOG_ERROR(SQLLogger, "{:90}| {}, PQntuples = {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)), line);
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, Test);
        }
    }
    else
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, Test);
    }
    PQclear(res);
}

void Value::TestValSQL()
{
    char Test[1024];
    sprintf_s(Test, 1023, "SELECT id FROM tag WHERE name = '%s';", Patch.c_str());
    PGresult* res = Conn->PGexec(Test);
    //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, Test);
    if(PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int line = PQntuples(res);
        if(!line)
        {
            PQclear(res);
            InsertVal();
        }
        else
        {
            ID = atol(conn_kpvl.PGgetvalue(res, 0, 0).c_str());
            PQclear(res);
            //UpdateVal();
        }
    }
    else
    {
        LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
        LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, Test);
        PQclear(res);
        InsertVal();
    }
    GetIdValSQL();

}

void Value::SetGraff()
{
    //if(Patch == StrFurn1 + "TempRef")
    //    GraffFurn1.SqlTemp(GraffFurn1.TempRef, this);
    //if(Patch == StrFurn1 + "TempAct")
    //    GraffFurn1.SqlTemp(GraffFurn1.TempAct, this);
    //
    //if(Patch == StrFurn2 + "TempRef")
    //    GraffFurn2.SqlTemp(GraffFurn2.TempRef, this);
    //if(Patch == StrFurn2 + "TempAct")
    //    GraffFurn2.SqlTemp(GraffFurn2.TempAct, this);
}

void Value::SaveSQL()
{
    //char ss[1024];
    //LOG_INFO(SQLLogger, "{:90}| {} {}", FUNCTION_LINE_NAME, Arhive, Patch);
    if(!Arhive) return;

    //std::string update ="UPDATE tag SET content_at = now(), content = ";

    //std::string comand = "INSERT INTO todos (id_name, content) VALUES(";
    bool ifval = OldVal.IsNul();
    if(GetType() == OpcUa::VariantType::FLOAT)
    {
        float f1 = Val.As<float>() * coeff;
        float f2 = float(ifval ? 0 : (OldVal.As<float>() * coeff));

        float f3 = std::abs(f1 - f2);
        if(f3 > hist || ifval)
        {
            if(!ID)
                TestValSQL();
            //comand += ", '";
            //update += GetString();
            //comand += "');";
            //update += "'" + std::to_string(f1) + "'";
            std::stringstream sd;
            sd << "UPDATE tag SET content_at = now(), content = ";
            sd << GetString();
            sd << " WHERE name = '" << Patch.c_str() << "';";

            //sprintf_s(ss, 1023, " WHERE name = '%s';", Patch.c_str());
            //update += sd.str();
            PGresult* res = Conn->PGexec(sd.str());
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, update);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
            }
            PQclear(res);
            InsertValue();
            OldVal = Val;
            //SetGraff();
        }
        else
        {
            float f1 = Val.As<float>() * coeff;
            float f2 = float(ifval ? 0 : (OldVal.As<float>() * coeff));
            //LOG_INFO(SQLLogger, "{:90}| f1: {}, f2: {}, Patch: {}", FUNCTION_LINE_NAME, f1, f2, Patch);
        }
    }
    else if(GetType() == OpcUa::VariantType::DOUBLE)
    {
        double f1 = Val.As<double>() * coeff;
        //double f2 = OldVal.As<double>() * coeff;
        double f2 = double(ifval ? 0 : (OldVal.As<double>() * coeff));

        double f3 = std::abs(f1 - f2);
        if(f3 > hist || ifval)
        {
            if(!ID)
                TestValSQL();

            //comand += std::to_string(handle);
            //comand += ", '";
            std::stringstream sd;
            sd << "UPDATE tag SET content_at = now(), content = ";
            sd << GetString();
            sd << " WHERE name = '" << Patch.c_str() << "';";
            PGresult* res = Conn->PGexec(sd.str());
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, update);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
            {
                LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
                LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
            }
            PQclear(res);
            InsertValue();
            OldVal = Val;
        }
    }
    else
    {
        //update += std::to_string(handle);
        //comand += ", '";
        if(!ID)
            TestValSQL();
        std::stringstream sd;
        sd << "UPDATE tag SET content_at = now(), content = ";
        if(GetType() == OpcUa::VariantType::STRING)
            sd << "'" << GetString() << "'";
        else
            sd << GetString();
        sd << " WHERE name = '" << Patch.c_str() << "';";
        //sprintf_s(ss, 1023, " WHERE name = '%s';", Patch.c_str());
        //update += sd.str(); // " WHERE name = '" + Patch + "';";

        PGresult* res = Conn->PGexec(sd.str());
        //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, update);
        if(PQresultStatus(res) == PGRES_FATAL_ERROR)
        {
            LOG_ERROR(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, utf8_to_cp1251(PQresultErrorMessage(res)));
            LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, sd.str());
        }
        PQclear(res);
        InsertValue();
        OldVal = Val;
    }
    
}

void MySetWindowText(HWND h, const char* ss)
{
    if(h)
    {
        SetWindowText(h, ss);
        InvalidateRect(h, NULL, false);
    }
}

Value::Value(const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn, MSSEC s) :
    Patch(n), strVal(""), oldstrVal(""), winId(hc), FunctionCallbak(fn), Conn(conn), Sec(s)
{
    if(!Patch.length())
        throw std::runtime_error(std::string(FUNCTION_LINE_NAME + std::string("; Error Patch = NULL")).c_str());
    if(Patch.find("WDG") != SIZE_MAX)
    {
        Arhive = false;
    }
};

Value::Value(const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn):
    Patch(n), strVal(""), oldstrVal(""), winId(hc), FunctionCallbak(fn), Conn(conn)
{
    if(!Patch.length())
        throw std::runtime_error(std::string(FUNCTION_LINE_NAME + std::string("; Error Patch = NULL")).c_str());
    if(Patch.find("WDG") != SIZE_MAX)
    {
        Arhive = false;
    }
};


void Value::InitNodeId(Client* cds)
{
    if(!cds)
        throw std::runtime_error(std::string(FUNCTION_LINE_NAME + std::string("; Error Codesys = NULL")).c_str());

    Codesys = cds;
    NodeId = OpcUa::NodeId(Patch, cds->NamespaceIndex); 
}

OpcUa::Node Value::GetNode()
{
    if(!Codesys)
        throw std::runtime_error(std::string(FUNCTION_LINE_NAME + std::string("; Error Codesys = NULL")).c_str());

    return Codesys->GetNode(Patch);
}

//void Value::Subscribe()
//{
//    if(!isRun)
//        throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());
//
//    if(Sec ==  sec00000) return;
//
//    try
//    {
//        if(Node.IsValid())
//        {
//            Log(Codesys ? Codesys->Logger : AllLogger, spdlog::level::level_enum::info, std::string("Подключение переменной ") + Patch);
//            SetWindowText(hWndDebug, (std::string("Подключение переменной ") + Patch).c_str());
//            Codesys->Subscribe(Sec, Node);
//            Log(Codesys ? Codesys->Logger : AllLogger, spdlog::level::level_enum::info, std::string("Подключена переменная ") + Patch);
//            SetWindowText(hWndDebug, (std::string("Подключена переменная ") + Patch).c_str());
//        }
//    }
//    VARCATCH(!"Node.IsValid ");
//
//}

OpcUa::VariantType Value::GetType()
{
    return Val.Type();
}

OpcUa::Variant Value::GetValue()
{
    if(isRun)
    {

        try
        {
            if(Codesys)
            {
                if(!Node.IsValid())
                {
                    Node = Codesys->client->GetNode(OpcUa::NodeId(Patch, Codesys->NamespaceIndex));

                    if(!Node.IsValid())
                        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Error Patch = ") + Patch).c_str());
                }

                Val = Node.GetValue();
                if(Val.IsNul())
                    throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Error Type Variant is NULL; Patch = ") + Patch).c_str());

                GetString();
            }
        }
        catch(std::runtime_error& exc)
        {
            LOG_ERROR(AllLogger, "{:90}| Error {}, Patch = {}", FUNCTION_LINE_NAME, exc.what(), Patch);
        }
        catch(...)
        {
            LOG_ERROR(AllLogger, "{:90}| Unknown error, Patch = {}", FUNCTION_LINE_NAME, Patch);
        }
    }
    return Val;
};

void Value::Set_Value(OpcUa::Variant var)
{
    if(isRun)
    {
        try
        {
            if(Codesys)
            {
                if(!Node.IsValid())
                {
                    Node = Codesys->client->GetNode(OpcUa::NodeId(Patch, Codesys->NamespaceIndex));

                    if(!Node.IsValid())
                        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Error Patch = ") + Patch).c_str());
                }

                Val = var;
                Node.SetValue(Val);
            }
        }
        catch(std::runtime_error& exc)
        {
            //isNode++;
            LOG_ERROR(AllLogger, "{:90}| Error {}, Patch = {}", FUNCTION_LINE_NAME, exc.what(), Patch);
        }
        catch(...)
        {
            //isNode++;
            LOG_ERROR(AllLogger, "{:90}| Unknown error, Patch = {}", FUNCTION_LINE_NAME, Patch);
        }
    }
}
void Value::Set_Value(){
    if(isRun)
    {
        try
        {
            if(Codesys)
            {
                if(!Node.IsValid())
                {
                    Node = Codesys->client->GetNode(OpcUa::NodeId(Patch, Codesys->NamespaceIndex));

                    if(!Node.IsValid())
                        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Error Patch = ") + Patch).c_str());
                }

                Node.SetValue(Val);
            }
        }
        catch(std::runtime_error& exc)
        {
            LOG_ERROR(AllLogger, "{:90}| Error {}, Patch = {}", FUNCTION_LINE_NAME, exc.what(), Patch);
        }
        catch(...)
        {
            LOG_ERROR(AllLogger, "{:90}| Unknown error, Patch = {}", FUNCTION_LINE_NAME, Patch);
        }
    }
}

std::string Value::GetString(std::string patch)
{
    if(isRun)
    {
        if(Val.IsNul()) return "";

        OpcUa::VariantType type = Val.Type();
        if(format.length())
        {
            char ss[256] = "";


            //HMISheetData.Sheet.WDG

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


std::string Value::GetString()
{
    if(isRun)
    {
        std::string pp = Patch;
        if(Val.IsNul()) return "0";

        OpcUa::VariantType type = Val.Type();
        if(format.length())
        {
            char ss[256] = "0";


            //HMISheetData.Sheet.WDG

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
            if(type == OpcUa::VariantType::STRING)
                strVal = utf8_to_cp1251(Val.ToString());
            else
                strVal = Val.ToString();
        }
    }
    return strVal;
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

//Ошибка	LNK2019	ссылка на неразрешенный внешний символ "public: short __cdecl Value::GetVal<short>(void)" (??$GetVal@F@Value@@QEAAFXZ) в функции "unsigned long __cdecl KPVL::DataMaskKlapan1(class Value *)" (?DataMaskKlapan1@KPVL@@YAKPEAVValue@@@Z).	HistoryKpvl	D:\YandexDisk\Project\HistoryKpvl\hard.obj	1		


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
        Val = v;
        if(FunctionCallbak)
        {
            GetString(Patch);
            FunctionCallbak(this);
        }
        else
        {
            SetVariant(Patch);
        }
        SaveSQL();
    }
    return true;
}

bool Value::TestNode(Client* cds)
{
    if(isRun)
    {

        if(!Patch.length())return false;
            //throw std::exception(std::string(FUNCTION_LINE_NAME, std::string("; Error Patch = NULL")).c_str());

        if(cds)
        {
            Codesys = cds;

            //Log(cds->Logger, spdlog::level::level_enum::info, "Проверка Patch = " + Patch);
            SetWindowText(hWndDebug, (std::string("Проверка Patch = ") + Patch).c_str());

            Node = cds->client->GetNode(OpcUa::NodeId(Patch, cds->NamespaceIndex));

            if(!Node.IsValid()) 
                return false;

                //throw std::exception(std::string(FUNCTION_LINE_NAME, std::string("; Error Patch = ") + Patch).c_str());
            return true;
        }
    }
    return false;
}

