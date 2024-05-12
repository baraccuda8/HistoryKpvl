// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows
#include <windows.h>
#include <windowsx.h>
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
//#include <time.h>

#include <conio.h>
#include <stdio.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <commdlg.h>

#include <iostream>
#include <fstream>
#include  <io.h>

//#include <mutex>
#include <string>
#include <vector>

#include <set>
#include <exception>
#include <iostream>

#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#ifdef _WIN64
#pragma comment(lib, "libxl64.lib")
//#define SIZE_MAX 0xffffffffffffffff
#define DLLRESULT LRESULT
#else
#pragma comment(lib, "libxl32.lib")
//#define SIZE_MAX 0xffffffff
#define DLLRESULT INT_PTR
#endif

