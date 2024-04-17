#include "pch.h"

#include "win.h"
#include "main.h"
#include "file.h"
#include "SQL.h"
#include "ValueTag.h"
#include "term.h"
#include "hard.h"
#include "Graff.h"
#include "Furn.h"

extern std::shared_ptr<spdlog::logger> PethLogger;


namespace S107
{
#pragma region Номера колонок в кассете
    int Col_Create_at = 0;
    int Col_Id = 1;
    int Col_Event = 2;
    int Col_Day = 3;
    int Col_Month = 4;
    int Col_Year = 5;
    int Col_CassetteNo = 6;
    int Col_SheetInCassette = 7;
    int Col_Close_at = 8;
    int Col_Peth = 9;
    int Col_Run_at = 10;
    int Col_Error_at = 11;
    int Col_End_at = 12;
    int Col_Delete_at = 13;
    int Col_TempRef = 14;           //Заданное значение температуры
    int Col_PointTime_1 = 15;       //Время разгона
    int Col_PointRef_1 = 16;        //Уставка температуры
    int Col_TimeProcSet = 17;       //Полное время процесса (уставка), мин
    int Col_PointDTime_2 = 18;      //Время выдержки
    int Col_f_temper = 19;          //Факт температуры за 5 минут до конца отпуска
    int Col_Finish_at = 20;
#pragma endregion

    std::string URI = "opc.tcp://192.168.9.40:4840";
    std::string ServerDataTime = "";

    namespace SQL{
        void FURN_SQL()
        {
            std::tm TM_End ={0};
            std::tm TM_Beg ={0};

            time_t tTemp = 60 * 60 * 24 * 7; //7 суток
            time_t tStop = time(NULL);
            time_t tStart = (time_t)difftime(tStop, tTemp);

            localtime_s(&TM_Beg, &tStart);

            std::stringstream ss;
            ss << "SELECT * FROM cassette WHERE create_at > '";
            ss << boost::format("%|04|-") % (TM_Beg.tm_year + 1900);
            ss << boost::format("%|02|-") % (TM_Beg.tm_mon + 1);
            ss << boost::format("%|02| ") % TM_Beg.tm_mday;
            ss << boost::format("%|02|:") % TM_Beg.tm_hour;
            ss << boost::format("%|02|:") % TM_Beg.tm_min;
            ss << boost::format("%|02|'") % TM_Beg.tm_sec;
            ss << " ORDER BY event ASC, create_at DESC;"; //DESC
            std::string comand = ss.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn_spic.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| sMaxId = {}", FUNCTION_LINE_NAME, sMaxId);
            if(PQresultStatus(res) == PGRES_TUPLES_OK)
            {
                int nFields = PQnfields(res);
                for(int j = 0; j < nFields; j++)
                {
                    std::string l =  utf8_to_cp1251(PQfname(res, j));
                    if(l == "create_at") Col_Create_at = j;
                    else if(l == "id") Col_Id = j;
                    else if(l == "event") Col_Event = j;
                    else if(l == "day") Col_Day = j;
                    else if(l == "month") Col_Month = j;
                    else if(l == "year") Col_Year = j;
                    else if(l == "cassetteno") Col_CassetteNo = j;
                    else if(l == "sheetincassette") Col_SheetInCassette = j;
                    else if(l == "close_at") Col_Close_at = j;
                    else if(l == "peth") Col_Peth = j;
                    else if(l == "run_at") Col_Run_at = j;
                    else if(l == "error_at") Col_Error_at = j;
                    else if(l == "end_at") Col_End_at = j;
                    else if(l == "delete_at") Col_Delete_at = j;
                    else if(l == "tempref") Col_TempRef = j;
                    else if(l == "pointtime_1") Col_PointTime_1 = j;
                    else if(l == "pointref_1") Col_PointRef_1 = j;
                    else if(l == "timeprocset") Col_TimeProcSet = j;
                    else if(l == "pointdtime_2") Col_PointDTime_2 = j;
                    else if(l == "facttemper") Col_f_temper = j;
                    else if(l == "finish_at") Col_Finish_at = j;
                    
                }
                
                AllCassette.erase(AllCassette.begin(), AllCassette.end());
                int line =  PQntuples(res);
                for(int l = 0; l < line; l++)
                {
                    TCassette cassette;
                    cassette.Create_at = GetStringData(conn_spis.PGgetvalue(res, l, Col_Create_at));
                    cassette.Id = conn_spis.PGgetvalue(res, l, Col_Id);
                    cassette.Event = conn_spis.PGgetvalue(res, l, Col_Event);
                    cassette.Day = conn_spis.PGgetvalue(res, l, Col_Day);
                    cassette.Month = conn_spis.PGgetvalue(res, l, Col_Month);
                    cassette.Year = conn_spis.PGgetvalue(res, l, Col_Year);
                    cassette.CassetteNo = conn_spis.PGgetvalue(res, l, Col_CassetteNo);
                    cassette.SheetInCassette = conn_spis.PGgetvalue(res, l, Col_SheetInCassette);
                    cassette.Close_at = GetStringData(conn_spis.PGgetvalue(res, l, Col_Close_at));
                    cassette.Peth = conn_spis.PGgetvalue(res, l, Col_Peth);
                    cassette.Run_at = GetStringData(conn_spis.PGgetvalue(res, l, Col_Run_at));
                    cassette.Error_at = GetStringData(conn_spis.PGgetvalue(res, l, Col_Error_at));
                    cassette.End_at = GetStringData(conn_spis.PGgetvalue(res, l, Col_End_at));
                    cassette.Delete_at = GetStringData(conn_spis.PGgetvalue(res, l, Col_Delete_at));
                    cassette.TempRef = conn_spis.PGgetvalue(res, l, Col_TempRef);           //Заданное значение температуры
                    cassette.PointTime_1 = conn_spis.PGgetvalue(res, l, Col_PointTime_1);       //Время разгона
                    cassette.PointRef_1 = conn_spis.PGgetvalue(res, l, Col_PointRef_1);        //Уставка температуры
                    cassette.TimeProcSet = conn_spis.PGgetvalue(res, l, Col_TimeProcSet);       //Полное время процесса (уставка), мин
                    cassette.PointDTime_2 = conn_spis.PGgetvalue(res, l, Col_PointDTime_2);      //Время выдержки
                    cassette.f_temper = conn_spis.PGgetvalue(res, l, Col_f_temper);          //Факт температуры за 5 минут до конца отпуска
                    cassette.Finish_at = GetStringData(conn_spis.PGgetvalue(res, l, Col_Finish_at)); //Завершение процесса + 15 минут
                    
                    AllCassette.push_back(cassette);
                }
            }
            else
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);

            int tt = 0;

        }
    };

#pragma region Функции с кассетами в базе

    bool IsCassete1(T_cassette& CD)
    {
        int32_t Day = GetVal<int32_t>(CD.Day);
        int32_t Month = GetVal<int32_t>(CD.Month);
        int32_t Year = GetVal<int32_t>(CD.Year);
        int32_t CassetteNo = GetVal<int32_t>(CD.CassetteNo);
        return Day && Month && Year && CassetteNo;
    }
    bool IsCassete2(T_cassette& CD)
    {
        int32_t Day = GetVal<int32_t>(CD.Day);
        int32_t Month = GetVal<int32_t>(CD.Month);
        int32_t Year = GetVal<int32_t>(CD.Year);
        int32_t CassetteNo = GetVal<int32_t>(CD.CassetteNo);
        return (Day && Month && Year && CassetteNo) || (!Day && !Month && !Year && !CassetteNo);
    }

    void UpdateCassetteProcRun(T_ForBase_RelFurn& Furn, int Peth)
    {
        //T_cassette& CD = Furn.Cassette;
        if(IsCassete1(Furn.Cassette))
        {
            //CloseAllCassette(CD);
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "peth = " << Peth;
            sd << ", run_at = now()";

            sd << ", event = 3";
            sd << ", tempref = " << Furn.TempRef->Val.As<float>(); //GetString();
            sd << ", pointtime_1 = " << Furn.PointTime_1->Val.As<float>(); //GetString();
            sd << ", pointref_1 = " << Furn.PointRef_1->Val.As<float>(); //GetString();
            sd << ", timeprocset = " << Furn.TimeProcSet->Val.As<float>(); //GetString();
            sd << ", pointdtime_2 = " << Furn.PointDTime_2->Val.As<float>(); //GetString();

            sd << " WHERE";
            sd << " run_at IS NULL";
            sd << " AND day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";

            std::string comand = sd.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn_temp.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| Peth={}, {}", FUNCTION_LINE_NAME, Peth, comand);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);

            //std::stringstream ss;
            
            sd = std::stringstream("");
            sd << "UPDATE cassette SET ";
            sd << "error_at = DEFAULT, ";
            sd << "end_at = DEFAULT";
            sd << " WHERE";
            sd << " day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";

            comand = sd.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            res = conn_temp.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| Peth={}, {}", FUNCTION_LINE_NAME, Peth, comand);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);

        }
    }
    void UpdateCassetteProcEnd(T_ForBase_RelFurn& Furn, int Peth)
    {
        if(IsCassete1(Furn.Cassette))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "end_at = now()";
            sd << ", event = 5";
            sd << " WHERE end_at IS NULL";
            sd << " AND peth = " << Peth;
            sd << " AND day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";

            std::string comand = sd.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn_temp.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| Peth={}, {}", FUNCTION_LINE_NAME, Peth, comand);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);
        }
    }
    void UpdateCassetteProcError(T_ForBase_RelFurn& Furn, int Peth)
    {
        if(IsCassete1(Furn.Cassette))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << "run_at = DEFAULT";
            sd << "error_at = now()";
            sd << ", event = 4";
            sd << " WHERE error_at IS NULL";
            sd << " AND peth = " << Peth;
            sd << " AND day = " << Furn.Cassette.Day->Val.As<int32_t>(); //GetString();
            sd << " AND month = " << Furn.Cassette.Month->Val.As<int32_t>(); //GetString();
            sd << " AND year = " << Furn.Cassette.Year->Val.As<int32_t>(); //GetString();
            sd << " AND cassetteno = " << Furn.Cassette.CassetteNo->Val.As<int32_t>(); //GetString();
            sd << ";";

            std::string comand = sd.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn_temp.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| Peth={}, {}", FUNCTION_LINE_NAME, Peth, comand);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);
        }
    }

    void SetTemperCassette(T_cassette& CD, std::string teper)
    {
        if(teper != CD.f_temper)
            if(IsCassete1(CD))
            {
                CD.f_temper = teper;
                std::string comand = "UPDATE cassette SET";
                comand += " facttemper = " + teper;
                comand += " WHERE ";
                //comand += " facttemper = 0.0 AND";
                comand += " day = " + CD.Day->Val.As<int32_t>();
                comand += " AND month = " + CD.Month->Val.As<int32_t>();
                comand += " AND year = " + CD.Year->Val.As<int32_t>();
                comand += " AND cassetteno = " + CD.CassetteNo->Val.As<int32_t>();
                comand += ";";

                if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
                PGresult* res = conn_temp.PGexec(comand);
                //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
                if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                    LOG_ERR_SQL(SQLLogger, res, comand);
                PQclear(res);
            }
    }

    void CloseAllCassette2(T_cassette& CD, int Peth)
    {
        if(IsCassete2(CD))
        {
            std::stringstream sd;
            sd << "UPDATE cassette SET ";
            sd << " event = 5,";
            sd << " end_at = now()";
            sd << " WHERE end_at IS NULL";
            sd << " AND event = 3";
            sd << " AND peth = " << Peth;
            if(IsCassete1(CD))
            {
                sd << " AND day <> " << CD.Day->Val.As<int32_t>(); //GetString();
                sd << " AND month <> " << CD.Month->Val.As<int32_t>(); //GetString();
                sd << " AND year <> " << CD.Year->Val.As<int32_t>(); //GetString();
                sd << " AND cassetteno <> " << CD.CassetteNo->Val.As<int32_t>(); //GetString();
            }
            sd << ";";

            std::string comand = sd.str();
            if(DEB)LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME, comand);
            PGresult* res = conn_temp.PGexec(comand);
            //LOG_INFO(SQLLogger, "{:90}| {}", FUNCTION_LINE_NAME,  comand);
            if(PQresultStatus(res) == PGRES_FATAL_ERROR)
                LOG_ERR_SQL(SQLLogger, res, comand);
            PQclear(res);
        }
    }

#pragma endregion

    //Печ отпуска #1
    namespace Furn1{
        const int Peth = 1;
        DWORD Data_WDG_toBase(Value* value)
        {
            if(value->Val.As<bool>())
            {
                AppFurn1.Furn_old_dt = time(NULL);
                try
                {
                    //if(!MyServer)
                    {
                        //AppFurn1.WDG_fromBase->Set_Value(true);
                        //TAG_PLC_SPK1.Application.ForBase_RelFurn_1.Data;
                    }
                }
                catch(std::exception& exc)
                {
                    LOG_ERROR(PethLogger, "{:90}| Ошибка передачи данных ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value", FUNCTION_LINE_NAME, exc.what());
                }
                catch(...)
                {
                    LOG_ERROR(PethLogger, "{:90}| Ошибка передачи данных ForBase_RelFurn_1.Data.WDG_fromBase.Set_Value", FUNCTION_LINE_NAME);
                };


                struct tm TM;
                localtime_s(&TM, &AppFurn1.Furn_old_dt);

                SetWindowText(winmap(value->winId), string_time(&TM).c_str());
            }

            //MySetWindowText(winmap(value->winId), value->GetString().c_str());
            return 0;
        }

        //BOOL Работа
        DWORD ProcRun(Value* value)
        {
            std::string out = "Простой";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcRun(AppFurn1, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value)
        {
            std::string out = "";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcEnd(AppFurn1, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL Авария процесса
        DWORD ProcError(Value* value)
        {
            std::string out = "";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcError(AppFurn1, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //Обнуление температуры для сравнения
        DWORD SetNull_Temper(Value* value)
        {
            AppFurn1.Cassette.f_temper = "0";
            MySetWindowText(value);
            return 0;
        }


        //REAL Время до окончания процесса, мин
        DWORD TimeToProcEnd(Value* value)
        {
            MySetWindowText(value);
            float time = GetVal<float>(value);
            if(time <= 5.0 && time >= 4.9)
            {
                MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.TempAct->GetString().c_str());
                SetTemperCassette(AppFurn1.Cassette, AppFurn1.TempAct->GetString());
            }
            else if(GetVal<float>(value) >= GetVal<float>(AppFurn1.TimeProcSet))
            {
                AppFurn1.Cassette.f_temper = "0";
                MySetWindowText(winmap(RelF1_Edit_Proc1), AppFurn1.Cassette.f_temper.c_str());
            }
            return 0;
        }
    }

    //Печ отпуска #2
    namespace Furn2{
        const int Peth = 2;
        DWORD Data_WDG_toBase(Value* value)
        {
            if(value->Val.As<bool>())
            {
                AppFurn2.Furn_old_dt = time(NULL);
                try
                {
                    //if(!MyServer)
                    {
                        //AppFurn2.WDG_fromBase->Set_Value(true);
                    }
                }
                catch(std::exception& exc)
                {
                    LOG_ERROR(PethLogger, "{:90}| Ошибка передачи данных ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value {}", FUNCTION_LINE_NAME, exc.what());
                }
                catch(...)
                {
                    LOG_ERROR(PethLogger, "Unknown Ошибка передачи данных ForBase_RelFurn_2.Data.WDG_fromBase.Set_Value ", FUNCTION_LINE_NAME);
                };


                struct tm TM;
                localtime_s(&TM, &AppFurn2.Furn_old_dt);

                SetWindowText(winmap(value->winId), string_time(&TM).c_str());
            }

            //MySetWindowText(winmap(value->winId), value->GetString().c_str());
            return 0;
        }

        //BOOL Работа
        DWORD ProcRun(Value* value)
        {
            std::string out = "Простой";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcRun(AppFurn2, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL Окончание процесса
        DWORD ProcEnd(Value* value)
        {
            std::string out = "";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcEnd(AppFurn2, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //BOOL Авария процесса
        DWORD ProcError(Value* value)
        {
            std::string out = "";
            if(value->Val.As<bool>())
            {
                out = GetShortTimes();
                UpdateCassetteProcError(AppFurn2, Peth);
            }
            MySetWindowText(winmap(value->winId), out.c_str());
            return 0;
        }

        //Обнуление температуры для сравнения
        DWORD SetNull_Temper(Value* value)
        {
            AppFurn2.Cassette.f_temper = "0";
            MySetWindowText(value);
            return 0;
        }



        //REAL Время до окончания процесса, мин
        DWORD TimeToProcEnd(Value* value)
        {
            MySetWindowText(value);
            float time = GetVal<float>(value);
            if(time <= 5.0 && time >= 4.9)
            {
                MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.TempAct->GetString().c_str());
                SetTemperCassette(AppFurn2.Cassette, AppFurn2.TempAct->GetString());
            }
            else if(GetVal<float>(value) >= GetVal<float>(AppFurn2.TimeProcSet))
            {
                AppFurn2.Cassette.f_temper = "0";
                MySetWindowText(winmap(RelF2_Edit_Proc1), AppFurn2.Cassette.f_temper.c_str());
            }
            return 0;
        }
    };

}
