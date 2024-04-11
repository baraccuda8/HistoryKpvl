#pragma once

//extern DWORD ProgProcessId;

int FinwWindow();

int InstallService();

int RemoveService();

int StartServices();

int StoptServices();

int Services();

void CurrentDir();


BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lparam);
