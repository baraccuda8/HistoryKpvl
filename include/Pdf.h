#pragma once

#include "term.h"
#include "hard.h"
#include "KPVL.h"
#include "Furn.h"


typedef std::map <int, T_Todos> MapRunn;
typedef std::map<int, TCassette> TCASS;

namespace PDF
{
	class GetPdf
	{
		std::string DateStart = "";
		TCASS P0;

	public:

		void TodosColumn(PGresult* res);
		OpcUa::Variant GetVarVariant(OpcUa::VariantType Type, std::string value);
		std::string GetVal(PGConnection& conn, int ID, std::string Run_at, std::string End_at);
		void GetVal(PGConnection& conn, TCassette& P, T_ForBase_RelFurn* Furn);
		void EndCassette(PGConnection& conn, TCassette& P, int Petch, std::fstream& s1);
		void GetCassetteData(PGConnection& conn, std::string id, TCassette& ct, TCassette& it);
		void SaveFileSdg(MapRunn& CasseteTodos);
		void SaveDataBaseForId(PGConnection& conn, TCassette& ct, TCassette& it);
		void SaveDataBaseNotId(PGConnection& conn, TCassette& ct, TCassette& it);
		void SaveDataBase(PGConnection& conn, TCassette& ct, TCassette& it);
		void SaveFileCass();
		void GetCassete(PGConnection& conn, MapRunn& CasseteTodos, int Petch);
		void GetCassete(PGConnection& conn);
		void CorrectSQL(PGConnection& conn);

		GetPdf(PGConnection& conn);

	};

	//�������������� �������� �� �������
	void PrintPdfAuto(TCassette& TC);

	//����������� �� ����� �� ����
	void PrintPdf(TSheet& Sheet);

	//����� �������������� �������������
	DWORD WINAPI RunAllPdf(LPVOID);

};

