#pragma once

#include "term.h"
#include "hard.h"
#include "KPVL.h"
#include "Furn.h"


typedef std::map <int, T_Todos> MapRunn;
typedef std::map<int, TCassette> TCASS;

namespace PDF
{
	OpcUa::Variant GetVarVariant(OpcUa::VariantType Type, std::string value);
	void TodosColumn(PGresult* res);

	namespace Cassette

	{
		class GetPdf
		{
			std::string DateStart = "";
			TCASS P0;

		public:

			std::string GetVal(PGConnection& conn, int ID, std::string Run_at, std::string End_at);
			void GetVal(PGConnection& conn, TCassette& P, T_ForBase_RelFurn* Furn);
			void EndCassette(PGConnection& conn, TCassette& P, int Petch, std::fstream& s1);
			void GetCassetteData(PGConnection& conn, std::string id, TCassette& ct, TCassette& it);
			void SaveFileSdg(MapRunn& CassetteTodos);
			void SaveDataBaseForId(PGConnection& conn, TCassette& ct, TCassette& it);
			void SaveDataBaseNotId(PGConnection& conn, TCassette& ct, TCassette& it);
			void SaveDataBase(PGConnection& conn, TCassette& ct, TCassette& it);
			void SaveFileCass();
			void GetCassette(PGConnection& conn, MapRunn& CassetteTodos, int Petch);
			void GetCassette(PGConnection& conn);
			void CorrectSQL(PGConnection& conn);
			GetPdf(PGConnection& conn);
		};

		//Автоматическое создание по кассете
		void PrintCassettePdfAuto(TCassette& TC);

		//Открывается по клику на лист
		void PrintCassettePdfAuto(TSheet& Sheet);

		//Поток автоматической корректировки
		DWORD WINAPI RunCassettelPdf(LPVOID);
	};
	namespace SHEET
	{
		DWORD WINAPI RunAllPdf(LPVOID);
		void GetRawSheet(PGConnection& conn);
	};

};

