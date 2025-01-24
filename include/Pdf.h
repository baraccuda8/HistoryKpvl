#pragma once

#include "term.h"
#include "hard.h"
#include "KPVL.h"
#include "Furn.h"


//typedef std::map <int, T_Todos> MapRunn;
typedef std::map<int, TCassette> TCASS;


namespace PDF
{
	OpcUa::Variant GetVarVariant(OpcUa::VariantType Type, std::string value);
	void TodosColumn(PGresult* res);

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

