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
    extern std::string ServerDataTime;

    namespace SQL
    {
        //�������� ������ ������ �� ����
        void KPVL_SQL();
        void GetDataTime_All(TSheet& TS);
    };

    namespace Sheet{
        //�������� ID �����
        std::string GetIdSheet(std::string sMelt, std::string sPack, std::string sPartNo, std::string sSheet, std::string sSubSheet, std::string sSlab);
        std::string GetIdSheet(int32_t Melt, int32_t Pack, int32_t PartNo, int32_t Sheet, int32_t SubSheet, int32_t Slab);

    //�������� �� ������� �����
        //bool IsSheet(T_PlateData& PD);

        ////�������� ID �����
        //std::string GetIdSheet(T_PlateData& PD);

        ////���������� ����� � ����
        //void InsertSheet(T_PlateData& PD, int pos);

        ////��������� � ���� ������ �� �����
        //bool SetUpdateSheet(PGConnection* con, T_PlateData& PD, std::string update, std::string where);

        ////��������� ������� �����
        //void UpdateSheetPos(T_PlateData& PD, std::string id, int pos);

        ////��������� ������ �� ����� ���� ���� ���� ��� ��������� �����
        //void SheetPos(T_PlateData& PD, int pos);

        ////����� ����-�������
        //void OutTime(T_PlateData& PD, HWNDCLIENT chwnd);

        //DWORD DataPosBool(Value* value);

        //���� 0 �� ����� � ����
        namespace  Z0{
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //���� 1 ���� ����
        namespace  Z1{
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            DWORD DataSlab(Value* value);
        };

        //���� 2 ���� ����
        namespace Z2{
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            DWORD DataSlab(Value* value);
        }

        //���� 3 �������
        namespace Z3{
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            DWORD DataSlab(Value* value);
        }

        //���� 4 ����������
        namespace Z4{
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            DWORD DataSlab(Value* value);
        }

        //���� 5 �����
        namespace Z5{
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            DWORD DataSlab(Value* value);
        }

        //���� 6 (��������)
        namespace Z6{
            DWORD DataTime(Value* value);

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            DWORD DataSlab(Value* value);

            //����� ���� � ������, ������� �����������
            DWORD NewSheetData(Value* value);

            //DWORD StartNewCassette(Value* value);

            //DWORD CassetteIsFill(Value* value);

            void SetSaveDone();

        }
    }

    namespace Cassette{
        //��������� ������ �� ������� ���� ������� ���� ��� ��������� �����
        void CassettePos(T_CassetteData& CD);

        //����� ����� ����� � ������
        DWORD Sheet_InCassette(Value* value);

        //����� ����� ������� �� ����
        DWORD CassetteNo(Value* value);

        //����� ���� ID �����
        DWORD CassetteDay(Value* value);

        //����� ����� ID �����
        DWORD CassetteMonth(Value* value);

        //����� ��� ID �����
        DWORD CassetteYear(Value* value);

        //������� �����������
        DWORD CasseteIsFill(Value* value);

        ////������ ����� �������
        //DWORD StartNewCassette(Value* value);
        //
        ////������� ������
        //DWORD CassetteIsComplete(Value* value);


    }

    //�������� � �����
    namespace ZState{

        ////5-�� ��������� �������� �� ������ ������ �� ����� �� 2- ����
        //DWORD WINAPI ThreadState2(LPVOID);
        //�������� � 1 ����
        DWORD DataPosState_1(Value* value);

        //�������� � 2 ����
        DWORD DataPosState_2(Value* value);

        //�������� � 3 ����
        DWORD DataPosState_3(Value* value);
    }


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

    //������
    namespace WDG{
        //������� ������
        DWORD SheetData_WDG_toBase(Value* value);
    }

}


extern std::deque<TSheet>AllSheet;