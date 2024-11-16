#pragma once

//����� ������� � ������� ������
#pragma region //����� ������� � ������� ������
extern int Col_Sheet_id;
extern int Col_Sheet_create_at;
extern int Col_Sheet_start_at;
extern int Col_Sheet_datatime_end;
extern int Col_Sheet_pos;
extern int Col_Sheet_datatime_all;
extern int Col_Sheet_alloy;
extern int Col_Sheet_thikness;
extern int Col_Sheet_melt;
extern int Col_Sheet_slab;
extern int Col_Sheet_partno;
extern int Col_Sheet_pack;
extern int Col_Sheet_sheet;
extern int Col_Sheet_subsheet;
extern int Col_Sheet_temper;
extern int Col_Sheet_speed;
extern int Col_Sheet_za_pt3;
extern int Col_Sheet_za_te3;
extern int Col_Sheet_lampresstop;
extern int Col_Sheet_lampressbot;
extern int Col_Sheet_posclapantop;
extern int Col_Sheet_posclapanbot;
extern int Col_Sheet_mask;
extern int Col_Sheet_lam1posclapantop;
extern int Col_Sheet_lam1posclapanbot;
extern int Col_Sheet_lam2posclapantop;
extern int Col_Sheet_lam2posclapanbot;
extern int Col_Sheet_lam_te1;
extern int Col_Sheet_news;
extern int Col_Sheet_top1;
extern int Col_Sheet_top2;
extern int Col_Sheet_top3;
extern int Col_Sheet_top4;
extern int Col_Sheet_top5;
extern int Col_Sheet_top6;
extern int Col_Sheet_top7;
extern int Col_Sheet_top8;
extern int Col_Sheet_bot1;
extern int Col_Sheet_bot2;
extern int Col_Sheet_bot3;
extern int Col_Sheet_bot4;
extern int Col_Sheet_bot5;
extern int Col_Sheet_bot6;
extern int Col_Sheet_bot7;
extern int Col_Sheet_bot8;
extern int Col_Sheet_day;
extern int Col_Sheet_month;
extern int Col_Sheet_year;
extern int Col_Sheet_cassetteno;
extern int Col_Sheet_sheetincassette;
extern int Col_Sheet_timeforplateheat;
extern int Col_Sheet_prestostartcomp;
extern int Col_Sheet_correct;
//};
#pragma endregion


//���� �������
namespace KPVL {
    extern std::string ServerDataTime;

    void UpdateCountSheet(PGConnection& conn, int id);

    namespace SQL
    {
        //�������� ������ ������� � ������� sheet
        void GetCollumn(PGresult* res);
        //������ ������
        void GetSheet(PGConnection& conn, PGresult* res, std::deque<TSheet>& Sheet);


        //�������� ������ ������ �� ����
        //void KPVL_SQL(PGConnection& conn);
        void KPVL_SQL(PGConnection& conn, std::deque<TSheet>& Sheet);
        void GetDataTime_All(PGConnection& conn, TSheet& TS);
    };

    namespace Sheet{
        //�������� ID �����
        std::string GetIdSheet(PGConnection& conn, std::string sMelt, std::string sPack, std::string sPartNo, std::string sSheet, std::string sSubSheet, std::string sSlab);
        std::string GetIdSheet(PGConnection& conn, int32_t Melt, int32_t Pack, int32_t PartNo, int32_t Sheet, int32_t SubSheet, int32_t Slab);

        ////��������� � ���� ������ �� �����
        void SetUpdateSheet(PGConnection& conn, T_PlateData& PD, std::string update, std::string where);
        void SetUpdateSheet(PGConnection& conn, TSheet& TS, std::string update, std::string where);

        //���� 0 �� ����� � ����
        namespace  Z0{
            extern const int Pos;
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //���� 1 ���� ����
        namespace  Z1{
            extern const int Pos;
            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        };

        //���� 2 ���� ����
        namespace Z2{
            extern const int Pos;

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //���� 3 �������
        namespace Z3{
            extern const int Pos;

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //���� 4 ����������
        namespace Z4{
            extern const int Pos;

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //���� 5 �����
        namespace Z5{
            extern const int Pos;

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);
        }

        //���� 6 (��������)
        namespace Z6{
            extern const int Pos;

            DWORD DataTime(Value* value);

            DWORD DataAlloyThikn(Value* value);

            DWORD Data(Value* value);
            //DWORD DataSlab(Value* value);

            //����� ���� � ������, ������� �����������
            DWORD NewSheetData(Value* value);

            void SetSaveDone(PGConnection& conn);

        }
    }

    namespace Cassette{

        //�������� �� ������� �������
        bool IsCassette(T_CassetteData& CD);

        //�������� ID �������
        int32_t GetIdCassette(PGConnection& conn, T_CassetteData& CD);

        //��������� ������ �� ������� ���� ������� ���� ��� ��������� �����
        int32_t CassettePos(PGConnection& conn, T_CassetteData& CD);

        //����� ����� ����� � ������
        DWORD Sheet_InCassette(Value* value);

        //����� ����� ������� �� ����
        DWORD CassetteNo(Value* value);

        //����� ��� ID �����
        DWORD CassetteHour(Value* value);

        //����� ���� ID �����
        DWORD CassetteDay(Value* value);

        //����� ����� ID �����
        DWORD CassetteMonth(Value* value);

        //����� ��� ID �����
        DWORD CassetteYear(Value* value);

        //������� �����������
        DWORD CasseteIsFill(Value* value);
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
        //
        //�������� ���� ������� ��������� ���
        //�������� ������ ����������
        //����� 5 ��� ����� state2=5
        //DWORD LaminPressBot(Value* value);
        //
        //����������� ���� � �������
        //state2=5
        //DWORD LAM_TE1(Value* value);
        //
        //����������� ���� � ����
        //state2=5
        //DWORD Za_TE3(Value* value);
        //
        //�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)
        //state2=5
        //DWORD Za_PT3(Value* value);
    };

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
    DWORD Side(Value* value);

    DWORD Temperature(Value* value);

    //������� ������
    DWORD SheetData_WDG_toBase(Value* value);

}


extern std::deque<TSheet>AllSheet;


