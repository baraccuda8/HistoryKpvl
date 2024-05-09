#pragma once

namespace S107
{
    typedef struct T_cass{
        int id = 0;
        std::string Run_at = "";
        std::string End_at = "";
        std::string Err_at = "";
        int Day = 0;
        int Month = 0;
        int Year = 0;
        int CassetteNo = 0;
    };

    extern std::string URI;
    extern std::string ServerDataTime;

    namespace Coll{
        extern int Create_at;
        extern int Id;
        extern int Event;
        extern int Day;
        extern int Month;
        extern int Year;
        extern int CassetteNo;
        extern int SheetInCassette;
        extern int Close_at;
        extern int Peth;
        extern int Run_at;
        extern int Error_at;
        extern int End_at;
        extern int Delete_at;
        extern int TempRef;           //�������� �������� �����������
        extern int PointTime_1;       //����� �������
        extern int PointRef_1;        //������� �����������
        extern int TimeProcSet;       //������ ����� �������� (�������), ���
        extern int PointDTime_2;      //����� ��������
        extern int f_temper;          //���� ����������� �� 5 ����� �� ����� �������
        extern int Finish_a;
    };

    void GetColl(PGresult* res);
    void GetCassette(PGresult* res, TCassette& cassette, int line);


    namespace SQL{
        void FURN_SQL(PGConnection& conn, std::deque<TCassette>& allCassette);
        void GetIsPos(PGConnection& conn, TCassette& CD);
        bool GetCountSheet(PGConnection& conn, TCassette& CD);
    };


#pragma region ������� � ��������� � ����
    bool IsCassete(TCassette& CD);
    bool IsCassete1(T_cassette& CD);
    bool IsCassete2(T_cassette& CD);

    void UpdateCassetteProcRun(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth);
    void UpdateCassetteProcEnd(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth);
    void UpdateCassetteProcError(PGConnection& conn, T_ForBase_RelFurn& Furn, int Peth);
    void SetTemperCassette(PGConnection& conn, T_cassette& CD, std::string teper);;
    void CloseAllCassette2(PGConnection& conn, T_cassette& CD, int Peth);
#pragma endregion

    //��� ������� #1
    namespace Furn1{
        extern T_cass Petch;

        DWORD Data_WDG_toBase(Value* value);

        //BOOL ������
        DWORD ProcRun(Value* value);

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value);

        //BOOL ������ ��������
        DWORD ProcError(Value* value);

        //��������� ����������� ��� ���������
        //DWORD SetNull_Temper(Value* value);

        DWORD Day(Value* value);
        DWORD Month(Value* value);
        DWORD Year(Value* value);
        DWORD No(Value* value);


        DWORD TimeHeatAcc(Value* value);
        DWORD TimeHeatWait(Value* value);
        DWORD TimeTotal(Value* value);

        ////REAL ����� �� ��������� ��������, ���
        DWORD TimeToProcEnd(Value* value);

        ////REAL ����� �������
        //DWORD PointTime_1(Value* value);
        //
        ////REAL ������� �����������
        //DWORD PointRef_1(Value* value);
        //
        ////REAL ����� ��������
        //DWORD PointDTime_2(Value* value);
        //
        ////REAL ������ ����� �������� (�������), ���
        //DWORD TimeProcSet(Value* value);
        //
        ////REAL ����� ��������, ��� (0..XX)
        //DWORD ProcTimeMin(Value* value);
        //
        ////REAL �������� �������� �����������
        //DWORD TempRef(Value* value);
        //
        ////REAL ����������� �������� �����������
        //DWORD TempAct(Value* value);
        //
        ////REAL ��������� 1
        //DWORD T1(Value* value);
        //
        ////REAL ��������� 2
        //DWORD T2(Value* value);
        //
        ////ID ������
        //DWORD IDCassette(Value* value);
        //
        ////ID ������ �����
        //DWORD Month(Value* value);
        //
        ////ID ������ ����
        //DWORD Day(Value* value);
        //
        ////ID ������ �����
        //DWORD CassetteNo(Value* value);
        //
        ////�����
        //DWORD SelectedCassete(Value* value);

    }


    //��� ������� #2
    namespace Furn2{
        extern T_cass Petch;

        DWORD Data_WDG_toBase(Value* value);

        //BOOL ������
        DWORD ProcRun(Value* value);

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value);

        //BOOL ������ ��������
        DWORD ProcError(Value* value);

        //��������� ����������� ��� ���������
        //DWORD SetNull_Temper(Value* value);

        DWORD Day(Value* value);
        DWORD Month(Value* value);
        DWORD Year(Value* value);
        DWORD No(Value* value);



        DWORD TimeHeatAcc(Value* value);
        DWORD TimeHeatWait(Value* value);
        DWORD TimeTotal(Value* value);

        ////REAL ����� �� ��������� ��������, ���
        DWORD TimeToProcEnd(Value* value);

        ////REAL ����� �������
        //DWORD PointTime_1(Value* value);
        //
        ////REAL ������� �����������
        //DWORD PointRef_1(Value* value);
        //
        ////REAL ����� ��������
        //DWORD PointDTime_2(Value* value);
        //
        ////REAL ������ ����� �������� (�������), ���
        //DWORD TimeProcSet(Value* value);
        //
        ////REAL ����� ��������, ��� (0..XX)
        //DWORD ProcTimeMin(Value* value);
        //
        ////REAL �������� �������� �����������
        //DWORD TempRef(Value* value);
        //
        ////REAL ����������� �������� �����������
        //DWORD TempAct(Value* value);
        //
        ////REAL ��������� 1
        //DWORD T1(Value* value);
        //
        ////REAL ��������� 2
        //DWORD T2(Value* value);
        //
        ////ID ������ ���
        //DWORD Year(Value* value);
        //
        ////ID ������ �����
        //DWORD Month(Value* value);
        //
        ////ID ������ ����
        //DWORD Day(Value* value);
        //
        ////ID ������ �����
        //DWORD CassetteNo(Value* value);
        //
        ////�����
        //DWORD SelectedCassete(Value* value);

    };

    //DWORD SelectedCassete(Value* value);
}


void SetUpdateCassete(PGConnection& conn, TCassette& cassette, std::string update, std::string where);

