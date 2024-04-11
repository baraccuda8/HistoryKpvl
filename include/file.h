#pragma once

namespace std
{
    typedef std::vector<boost::filesystem::path> Pathc;
};

std::string cp1251_to_utf8(std::string str);
std::string utf8_to_cp1251(std::string str);
bool LoadConnect();
void SaveConnect();

void Log(std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl, std::string msg);

void SaveWindowPos(HWND hWnd, const char* name);
WINDOWPLACEMENT LoadWindowPos(HWND hWnd, const char* name);
WINDOWPLACEMENT LoadWindowPos(const char* name);

std::string GetShortTimes();

//void SendDebug(std::string, std::string ss = "");

std::Pathc getDirContents (const std::string& dirName);
BOOL CenterWindow(HWND hwndChild, HWND hwndParent);

void GetRessusce(int ID, const char* DATE, LPVOID* pLockedResource, DWORD* dwResourceSize);

//std::string GetStringData(std::string d);
