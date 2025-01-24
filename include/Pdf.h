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
		std::string Start1 = "";		//Дата, время загрузки листа в закалочную печь
		std::string Start2 = "";		//Дата, время загрузки листа во вторую зону
		std::string Start3 = "";		//Дата, время загрузки листа во вторую зону
		std::string Start5 = "";		//Дата, время загрузки листа на охлаждение

		std::string Nagrev = "";		//Дата, время начала нагрева
		std::string CloseInDor = "";	//Закрыть входную дверь
		
		std::string InCant = "";		//Дата, время загрузки листа на кантовку
		std::string Cant = "";			//Дата, время кантовки
		std::string DataTime_End = "";	//Дата, время выгрузки листа из печи 

		std::string TimeTest = "";		//Для тестов
		std::string DataTime = "";		//Старт нашрева
		std::string Correct = "";		//Корректировка

		int Melt = 0;					//Плавка
		int Pack = 0;					//Пачка
		int PartNo = 0;					//Партия
		int Sheet = 0;					//Номер листа
		int SubSheet = 0;				//Номер подлиста
		int Slab = 0;					//Сляб

		std::string Alloy = "нет";		//Марка стали
		std::string Thikness = "нет";	//Толщина листа, мм

		//float HeatTime_Z2 = 0.0f;		//время нагрева в зоне 2
		float TimeForPlateHeat = 0.0f;	//Задание Время нахождения листа в закалочной печи. мин
		float DataTime_All = 0.0f;		//Факт Время нахождения листа в закалочной печи. мин
		float PresToStartComp = 0.0f;	//Задание Давления воды

		//Фиксируем на начало входа в печь State_1 = 3;
		float Temper = 0.0f;			//Уставка температуры
		float Speed = 0.0f;				//Скорость выгрузки
		float SpeedTopSet = 0.0f;		//Клапан. Скоростная секция. Верх
		float SpeedBotSet = 0.0f;		//Клапан. Скоростная секция. Низ
		float Lam1PosClapanTop = 0.0f;	//Клапан. Ламинарная секция 1. Верх
		float Lam1PosClapanBot = 0.0f;	//Клапан. Ламинарная секция 1. Низ
		float Lam2PosClapanTop = 0.0f;	//Клапан. Ламинарная секция 2. Верх
		float Lam2PosClapanBot = 0.0f;	//Клапан. Ламинарная секция 2. Низ
		uint16_t Valve_1x = 0;			//Режим работы клапана 1
		uint16_t Valve_2x = 0;			//Режим работы клапана 2
		std::string Mask1 = "000000000";//Режим работы клапана 1
		std::string Mask2 = "000000000";//Режим работы клапана 2
		std::string Mask = "";			//Режим работы клапана

		//Фиксируем на выходе из печи State_2 = 5;
		float Temperature = 0.0f;		//Фактическая температура
		float LAM_TE1 = 0.0f;			//Температура воды в поддоне
		float Za_TE3 = 0.0f;			//Температура воды в баке
		float Za_PT3 = 0.0f;			//Давление воды в баке (фиксировать в момент команды " в закалку" там шаг меняется по биту)

		//Фиксируем на выходе из печи State_2 = 5 плюс 5 секунд;
		float LaminPressTop = 0.0f;		//Давление в верхнем коллекторе 
		float LaminPressBot = 0.0f;		//Давление в нижнем коллекторе

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

	//Поток автоматической корректировки
	DWORD WINAPI RunCassettelPdf(LPVOID);

	DWORD CorrectSheet(LPVOID);
	DWORD CorrectCassetteFinal(LPVOID);

#ifdef _DEBUG
	DWORD CorrectSheet2(LPVOID);
#endif
};

void Open_PDF();
void Close_PDF();

