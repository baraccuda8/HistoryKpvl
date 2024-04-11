#include "pch.h"




#include "GdiPlusInit.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")


CGdiPlusInit::CGdiPlusInit()
{
    present = true;
    Gdiplus::GdiplusStartupInput input;
    __try
    {
        Gdiplus::GdiplusStartup(&token, &input, 0);
    }
    __except(1)
    {
        present = false;
    }
}
CGdiPlusInit::~CGdiPlusInit()
{
    if(present) Gdiplus::GdiplusShutdown(token);
}

CGdiPlusInit init;
