#pragma once

//Открывается по клику на лист
void PrintPdf(TSheet& Sheet);

//Автоматическое создание по листам
void PrintPdfAuto(TSheet& Sheet, bool view = false);

//Автоматическое создание по кассете
void PrintPdfAuto(TCassette& TC);
