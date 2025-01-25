#pragma once

namespace S107
{
    //typedef struct T_cass{
    //    int id = 0;
    //    std::string Run_at = "";
    //    std::string End_at = "";
    //    std::string Err_at = "";
    //    int Hour = 0;
    //    int Day = 0;
    //    int Month = 0;
    //    int Year = 0;
    //    int CassetteNo = 0;
    //};

    extern std::string URI;
    extern std::string ServerDataTime;

    namespace Coll{
        extern int Create_at;
        extern int Id;
        extern int Event;
        extern int Hour;
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
        extern int PointTime_2;      //����� ��������
        extern int facttemper;          //���� ����������� �� 5 ����� �� ����� �������
        extern int Finish_a;
    };

#pragma region ������� � ��������� � ����
    //void GetColl(PGresult* res);
    void GetCassette(PGConnection& conn, PGresult* res, TCassette& cassette, int line);

    bool GetFinishCassete(PGConnection& conn, LOGGER Logger, TCassette& it);
    //bool IsCassette(TCassette& CD);
	template <class T>
	bool IsCassette(T& CD);
	Tcass& GetIgCassetteFurn(int Peth);
	T_ForBase_RelFurn& GetBaseRelFurn(int Peth);


    namespace SQL{

        void FURN_SQL(PGConnection& conn, std::deque<TCassette>& allCassette, LOGGER Logger);
        //bool isCasseteCant(T_CassetteData& Cassette, TCassette& CD);
        //bool isCasseteFurn(T_Fcassette& Cassette, TCassette& CD);
        //void GetIsPos(PGConnection& conn, TCassette& CD);
        //void SearthEnd_at(PGConnection& conn, TCassette& CD, int id);
        //bool GetCountSheet(PGConnection& conn, TCassette& CD);
    };


#pragma endregion

    //��� ������� #1
    namespace Furn1{
        extern const int nPetch;
		extern Tcass Petch;
        extern int CassetteId;
		extern T_ForBase_RelFurn& Furn;
		extern LOGGER Logger;
        DWORD Data_WDG_toBase(Value* value);

        //BOOL ������
        DWORD ProcRun(Value* value);

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value);

        //BOOL ������ ��������
        DWORD ProcError(Value* value);

        //BOOL ������� ������ � ������
        DWORD ReturnCassetteCmd(Value* value);

        //��������� ����������� ��� ���������
        //DWORD SetNull_Temper(Value* value);

        DWORD Hour(Value* value);
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
        //DWORD PointTime_2(Value* value);
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
        extern const int nPetch;
		extern Tcass Petch;
        extern int CassetteId;
		extern T_ForBase_RelFurn& Furn;
		extern LOGGER Logger;


        DWORD Data_WDG_toBase(Value* value);

        //BOOL ������
        DWORD ProcRun(Value* value);

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value);

        //BOOL ������ ��������
        DWORD ProcError(Value* value);

        //BOOL ������� ������ � ������
        DWORD ReturnCassetteCmd(Value* value);

        //��������� ����������� ��� ���������
        //DWORD SetNull_Temper(Value* value);

        DWORD Hour(Value* value);
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
        //DWORD PointTime_2(Value* value);
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

