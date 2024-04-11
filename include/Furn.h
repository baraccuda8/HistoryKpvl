#pragma once


namespace S107
{
    extern std::string URI;
    extern std::string ServerDataTime;

    namespace SQL{
        void FURN_SQL();
    };


#pragma region ������� � ��������� � ����
    bool IsCassete1(T_cassette& CD);
    bool IsCassete2(T_cassette& CD);

    void UpdateCassetteProcRun(T_ForBase_RelFurn& Furn, int Peth);
    void UpdateCassetteProcEnd(T_ForBase_RelFurn& Furn, int Peth);
    void UpdateCassetteProcError(T_ForBase_RelFurn& Furn, int Peth);
    void SetTemperCassette(T_cassette& CD, std::string teper);;

    void CloseAllCassette2(T_cassette& CD, int Peth);
#pragma endregion

    //��� ������� #1
    namespace Furn1{
        DWORD Data_WDG_toBase(Value* value);

        //BOOL ������
        DWORD ProcRun(Value* value);

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value);

        //BOOL ������ ��������
        DWORD ProcError(Value* value);

        //��������� ����������� ��� ���������
        DWORD SetNull_Temper(Value* value);

        ////REAL ����� �� ��������� ��������, ���
        //DWORD TimeToProcEnd(Value* value);

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
        DWORD Data_WDG_toBase(Value* value);

        //BOOL ������
        DWORD ProcRun(Value* value);

        //BOOL ��������� ��������
        DWORD ProcEnd(Value* value);

        //BOOL ������ ��������
        DWORD ProcError(Value* value);

        //��������� ����������� ��� ���������
        DWORD SetNull_Temper(Value* value);


        ////REAL ����� �� ��������� ��������, ���
        //DWORD TimeToProcEnd(Value* value);

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
