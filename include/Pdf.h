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
		//�������������� �������� �� �������
		//void PrintCassettePdfAuto(TCassette& TC);

		//����������� �� ����� �� ����
		//void PrintCassettePdfAuto(TSheet& Sheet);

	};
	namespace SHEET
	{
		DWORD WINAPI RunAllPdf(LPVOID);
		//void GetRawSheet(PGConnection& conn);
	};

	//����� �������������� �������������
	DWORD WINAPI RunCassettelPdf(LPVOID);

	//DWORD CorrectCassette(LPVOID);
	DWORD CorrectSheet(LPVOID);
#ifdef _DEBUG
	DWORD CorrectSheet2(LPVOID);
#endif
};

