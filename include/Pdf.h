#pragma once

#include "term.h"
#include "hard.h"
#include "KPVL.h"
#include "Furn.h"


//typedef std::map <int, T_Todos> MapRunn;
typedef std::map<int, TCassette> TCASS;


namespace PDF
{
	typedef std::vector<T_Todos> V_Todos;
	typedef std::vector<T_Todos> MapTodos;

	typedef struct T_IdSheet{
		int id = 0;
		std::string Start1 = "";		//����, ����� �������� ����� � ���������� ����
		std::string Start2 = "";		//����, ����� �������� ����� �� ������ ����
		std::string Start3 = "";		//����, ����� �������� ����� �� ������ ����
		std::string Start5 = "";		//����, ����� �������� ����� �� ����������

		std::string Nagrev = "";		//����, ����� ������ �������
		std::string CloseInDor = "";	//������� ������� �����
		
		std::string InCant = "";		//����, ����� �������� ����� �� ��������
		std::string Cant = "";			//����, ����� ��������
		std::string DataTime_End = "";	//����, ����� �������� ����� �� ���� 

		std::string TimeTest = "";		//��� ������
		std::string DataTime = "";		//����� �������
		std::string Correct = "";		//�������������

		int Melt = 0;					//������
		int Pack = 0;					//�����
		int PartNo = 0;					//������
		int Sheet = 0;					//����� �����
		int SubSheet = 0;				//����� ��������
		int Slab = 0;					//����

		std::string Alloy = "���";		//����� �����
		std::string Thikness = "���";	//������� �����, ��

		//float HeatTime_Z2 = 0.0f;		//����� ������� � ���� 2
		float TimeForPlateHeat = 0.0f;	//������� ����� ���������� ����� � ���������� ����. ���
		float DataTime_All = 0.0f;		//���� ����� ���������� ����� � ���������� ����. ���
		float PresToStartComp = 0.0f;	//������� �������� ����

		//��������� �� ������ ����� � ���� State_1 = 3;
		float Temper = 0.0f;			//������� �����������
		float Speed = 0.0f;				//�������� ��������
		float SpeedTopSet = 0.0f;		//������. ���������� ������. ����
		float SpeedBotSet = 0.0f;		//������. ���������� ������. ���
		float Lam1PosClapanTop = 0.0f;	//������. ���������� ������ 1. ����
		float Lam1PosClapanBot = 0.0f;	//������. ���������� ������ 1. ���
		float Lam2PosClapanTop = 0.0f;	//������. ���������� ������ 2. ����
		float Lam2PosClapanBot = 0.0f;	//������. ���������� ������ 2. ���
		uint16_t Valve_1x = 0;			//����� ������ ������� 1
		uint16_t Valve_2x = 0;			//����� ������ ������� 2
		std::string Mask1 = "000000000";//����� ������ ������� 1
		std::string Mask2 = "000000000";//����� ������ ������� 2
		std::string Mask = "";			//����� ������ �������

		//��������� �� ������ �� ���� State_2 = 5;
		float Temperature = 0.0f;		//����������� �����������
		float LAM_TE1 = 0.0f;			//����������� ���� � �������
		float Za_TE3 = 0.0f;			//����������� ���� � ����
		float Za_PT3 = 0.0f;			//�������� ���� � ���� (����������� � ������ ������� " � �������" ��� ��� �������� �� ����)

		//��������� �� ������ �� ���� State_2 = 5 ���� 5 ������;
		float LaminPressTop = 0.0f;		//�������� � ������� ���������� 
		float LaminPressBot = 0.0f;		//�������� � ������ ����������

		int Pos = 0;

		int hour = 0;
		int day = 0;
		int month = 0;
		int year = 0;
		int cassetteno = 0;
		int sheetincassette = 0;
		int news = 0;
		int cassette = 0;
	}T_IdSheet;

	typedef std::vector<T_IdSheet>V_IdSheet;
	typedef struct T_fTemp{
		float t0 = 0;
		float t1 = 0;
		float t2 = 0;
		float t3 = 0;
		float t4 = 0;
	}T_fTemp;


	OpcUa::Variant GetVarVariant(OpcUa::VariantType Type, std::string value);
	void TodosColumn(PGresult* res);

	namespace PASSPORT
	{
		typedef struct PFS{
			int sec = 0;
			float temper = 0.f;
			std::string data = "";
			int count = 0;
			int oldStep = 0;
		}PFS;

		typedef std::vector<PFS> VPFS;
	};

	namespace SHEET
	{
		DWORD WINAPI RunAllPdf(LPVOID);
	};

	//����� �������������� �������������
	DWORD WINAPI RunCassettelPdf(LPVOID);

	DWORD CorrectSheet(LPVOID);
	DWORD CorrectCassetteFinal(LPVOID);

#ifdef _DEBUG
	DWORD CorrectSheet2(LPVOID);
#endif
};

void Open_PDF();
void Close_PDF();

