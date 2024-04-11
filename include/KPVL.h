#pragma once

//����� ������� � ������� ������
#pragma region //����� ������� � ������� ������
extern const int Col_Sheet_id;
extern const int Col_Sheet_create_at;
extern const int Col_Sheet_start_at;
extern const int Col_Sheet_datatime_end;
extern const int Col_Sheet_pos;
extern const int Col_Sheet_datatime_all;
extern const int Col_Sheet_alloy;
extern const int Col_Sheet_thikness;
extern const int Col_Sheet_melt;
extern const int Col_Sheet_slab;
extern const int Col_Sheet_partno;
extern const int Col_Sheet_pack;
extern const int Col_Sheet_sheet;
extern const int Col_Sheet_subsheet;
extern const int Col_Sheet_temper;
extern const int Col_Sheet_speed;
extern const int Col_Sheet_za_pt3;
extern const int Col_Sheet_za_te3;
extern const int Col_Sheet_lampresstop;
extern const int Col_Sheet_lampressbot;
extern const int Col_Sheet_posclapantop;
extern const int Col_Sheet_posclapanbot;
extern const int Col_Sheet_mask;
extern const int Col_Sheet_lam1posclapantop;
extern const int Col_Sheet_lam1posclapanbot;
extern const int Col_Sheet_lam2posclapantop;
extern const int Col_Sheet_lam2posclapanbot;
extern const int Col_Sheet_lam_te1;
extern const int Col_Sheet_news;
extern const int Col_Sheet_top1;
extern const int Col_Sheet_top2;
extern const int Col_Sheet_top3;
extern const int Col_Sheet_top4;
extern const int Col_Sheet_top5;
extern const int Col_Sheet_top6;
extern const int Col_Sheet_top7;
extern const int Col_Sheet_top8;
extern const int Col_Sheet_bot1;
extern const int Col_Sheet_bot2;
extern const int Col_Sheet_bot3;
extern const int Col_Sheet_bot4;
extern const int Col_Sheet_bot5;
extern const int Col_Sheet_bot6;
extern const int Col_Sheet_bot7;
extern const int Col_Sheet_bot8;
extern const int Col_Sheet_day;
extern const int Col_Sheet_month;
extern const int Col_Sheet_year;
extern const int Col_Sheet_cassetteno;
extern const int Col_Sheet_sheetincassette;
extern const int Col_Sheet_timeforplateheat;
extern const int Col_Sheet_prestostartcomp;
//};
#pragma endregion


//���� �������
namespace KPVL {

    namespace SQL
    {
        void KPVL_SQL();
    };

    extern std::string ServerDataTime;

    bool IsCassete(T_CassetteData& CD);
     //�������� ID ������� �� ����� �� ������
    std::string GetIdCassette(T_CassetteData& CD);
    int GetCountSheetInCassete(T_CassetteData& CD);
     //��������� ��� �� �������� ������ ����� ������� �� ��������
    void CloseCassete(T_CassetteData& CD);
    bool IsSheet(T_PlateData& PD);
    //�������� ID �����
    std::string GetIdSheet(T_PlateData& PD);
    void InsertSheet(T_PlateData& PD, int pos);
    //���������� � ���� ������ �� �����
    void UpdateSheetPos(T_PlateData& PD, std::string id, int pos);
    bool SetUpdateSheet(PGConnection* con, T_PlateData& PD, std::string update, std::string where);
    bool SetUpdateSheet2(PGConnection* con, T_PlateData& PD, std::string update, std::string where);
    bool SetUpdateSheet3(PGConnection* con, T_PlateData* PD, std::string update, std::string where);
    void SheetPos(T_PlateData& PD, int pos);
    void InsertCassette(T_CassetteData& CD);
    void UpdateCassette(T_CassetteData& CD, std::string id);
    void CassettePos(T_CassetteData& CD);
    bool TestIDSheet(T_PlateData& PD, int& Melt, int& Pack, int& PartNo, int& Sheet);
    DWORD WINAPI ThreadState2(LPVOID);
        //�������� ������ �� 2 ������ ����� 5 ������ ����� State_2 = 5
    void InsertSheetPos0(T_PlateData& PD);
    void UpdateSheetPos0(T_PlateData& PD);
    void UpdateSheet0();
        //void InsertPos(int pos, std::string name);
    void OutTime(T_PlateData& PD, HWNDCLIENT chwnd);
    DWORD DataPosBool(Value* value);

    //���� 0 �� ����� � ����
        namespace  Z0{
            DWORD DataAlloyThikn(Value* value);
            //DWORD DataThikn(Value* value);
            DWORD Data(Value* value);
        }

        //���� 1 ���� ����
        namespace  Z1{
            DWORD DataAlloyThikn(Value* value);
            //DWORD DataThikn(Value* value);

            DWORD Data(Value* value);
        };

        //���� 2 ���� ����
        namespace Z2{
            DWORD DataAlloyThikn(Value* value);
            //DWORD DataThikn(Value* value);

            DWORD Data(Value* value);
        }

        //���� 3 �������
        namespace Z3{
            DWORD DataAlloyThikn(Value* value);
            //DWORD DataThikn(Value* value);

            DWORD Data(Value* value);
        }

        //���� 4 ����������
        namespace Z4{
            DWORD DataAlloyThikn(Value* value);
            //DWORD DataThikn(Value* value);

            DWORD Data(Value* value);
        }

        //���� 5 �����
        namespace Z5{
            DWORD DataAlloyThikn(Value* value);
            //DWORD DataThikn(Value* value);

            DWORD Data(Value* value);
        }

        //���� 6 (��������)
        namespace Z6{
            DWORD DataTime(Value* value);

            DWORD DataAlloyThikn(Value* value);
            //DWORD DataThikn(Value* value);

            DWORD Data(Value* value);

            //����� ����� ������� �� ����
            DWORD CassetteNo(Value* value);

            //����� ���� ID �����
            DWORD InitCassette(Value* value);

            //����� ����� ����� � ������
            DWORD Sheet_InCassette(Value* value);

            //����� ���� � ������, ������� �����������
            DWORD NewSheetData(Value* value);

            DWORD StartNewCassette(Value* value);

            DWORD CassetteIsFill(Value* value);

            void SetSaveDone();

        }

    //�������� � �����
    namespace ZState{
        //�������� � 1 ����
        DWORD DataPosState_1(Value* value);

        //�������� � 2 ����
        DWORD DataPosState_2(Value* value);

        //�������� � 3 ����
        DWORD DataPosState_3(Value* value);
    }

    //������ � ����� � ������ WDG
    //������� ������


    ////������ ����� �������
    //DWORD CassetteIsFill(Value* value);

    //�������
    namespace An{
    //������� �����������
        DWORD TempSet1(Value* value);

        //�������� ��������
        DWORD UnloadSpeed(Value* value);
        //����� ������������ ��������� �������, ���
        DWORD fTimeForPlateHeat(Value* value);
        //������� �������� ��� ������� �����������
        DWORD fPresToStartComp(Value* value);


        //��������� �� �������
        //state2=5
            //�������� ���� ������� ��������� ���� 
            //�������� � ������� ���������� 
            //����� 5 ��� ����� state2=5
        //DWORD LaminPressTop(Value* value);

        //�������� ���� ������� ��������� ���
        //�������� ������ ����������
        //����� 5 ��� ����� state2=5
        //DWORD LaminPressBot(Value* value);

        //����������� ���� � �������
        //state2=5
        //DWORD LAM_TE1(Value* value);

        //����������� ���� � ����
        //state2=5
        //DWORD Za_TE3(Value* value);

        //�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)
        //state2=5
        //DWORD Za_PT3(Value* value);
    };

    ////����������� ����
    //namespace Htr{
    ////����������� � ���� 1.1
    //    DWORD Htr_11(Value* value);
    //    //����������� � ���� 1.2
    //    DWORD Htr_12(Value* value);
    //    //����������� � ���� 1.3
    //    DWORD Htr_13(Value* value);
    //    //����������� � ���� 1.4
    //    DWORD Htr_14(Value* value);
    //    //����������� � ���� 2.1
    //    DWORD Htr_21(Value* value);
    //    //����������� � ���� 2.2
    //    DWORD Htr_22(Value* value);
    //    //����������� � ���� 2.3
    //    DWORD Htr_23(Value* value);
    //    //����������� � ���� 2.4
    //    DWORD Htr_24(Value* value);
    //    //��������
    //    DWORD Za_TE4(Value* value);
    //}

    //����� ������ �������
    //State_1 = 3 
    //����� ��������
    namespace Mask{
        DWORD DataMaskKlapan1(Value* value);
        DWORD DataMaskKlapan2(Value* value);
    }
    //���������� ������
    namespace Speed{
        //����
        DWORD SpeedSectionTop(Value* value);
        //���
        DWORD SpeedSectionBot(Value* value);
    }
    //���������� ������ 1
    namespace Lam1{
        //����
        DWORD LaminarSection1Top(Value* value);
        //���
        DWORD LaminarSection1Bot(Value* value);
    };
    //���������� ������ 2
    namespace Lam2{
        //����
        DWORD LaminarSection2Top(Value* value);
        //���
        DWORD LaminarSection2Bot(Value* value);
    };
//���������� ����� �� ��������
    namespace Side{
        DWORD SheetTop1(Value* value);
        DWORD SheetTop2(Value* value);
        DWORD SheetTop3(Value* value);
        DWORD SheetTop4(Value* value);
        DWORD SheetTop5(Value* value);
        DWORD SheetTop6(Value* value);
        DWORD SheetTop7(Value* value);
        DWORD SheetTop8(Value* value);

        DWORD SheetBot1(Value* value);
        DWORD SheetBot2(Value* value);
        DWORD SheetBot3(Value* value);
        DWORD SheetBot4(Value* value);
        DWORD SheetBot5(Value* value);
        DWORD SheetBot6(Value* value);
        DWORD SheetBot7(Value* value);
        DWORD SheetBot8(Value* value);
    }

    //namespace TimePos{
    //    DWORD TimePosState_1(Value* value);
    //    DWORD TimePosState_2(Value* value);
    //    DWORD TimePosState_3(Value* value);
    //}

    namespace WDG{
        DWORD SheetData_WDG_toBase(Value* value);
    }

}


extern std::deque<TSheet>AllSheet;