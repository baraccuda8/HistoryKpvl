#include "pch.h"
//#include "main.h"
//#include "win.h"
//#include "file.h"
//#include "exel.h"
//
//
//
//const char* NameKey = "ERICK RUBEN GONZALEZ BELTRAN";
//const char* PartKey = "windows-202c280c0ac0ec046db46a60a9obmek6";
//using namespace libxl;
//
//#define FSTART std::string(std::string("-->") + FUNCTION_LINE_NAME + " ")
//#define FSTOP std::string(std::string("<--") + FUNCTION_LINE_NAME + " ")
//#define FSTART std::string(std::string("-->") + FUNCTION_LINE_NAME + " ")
//#define TSTOP (FUNCTION_LINE_NAME + " ")
//
//std::mutex g_lock;
//extern std::string lpCassetteDir;
//
//
//std::vector<Format*> numFormat;
//
////Перечисление дирректорий. Возвращает все файлы
//std::Pathc getDirAll (const std::string& lpDir)
//{
//    return getDirContents(lpDir);
//}
//
//
//void CreateFormat(Book* book)
//{
//    Format* numFormat0 = book->addFormat();
//    numFormat0->setNumFormat(NUMFORMAT_GENERAL);
//    numFormat0->setAlignH(ALIGNH_CENTER);
//    numFormat0->setAlignV(ALIGNV_DISTRIBUTED);
//    numFormat0->setBorder(BORDERSTYLE_THIN);
//
//    Format* numFormat1 = book->addFormat();
//    numFormat1->setNumFormat(NUMFORMAT_NUMBER);
//    numFormat1->setAlignH(ALIGNH_CENTER);
//    numFormat1->setAlignV(ALIGNV_DISTRIBUTED);
//    numFormat1->setBorder(BORDERSTYLE_THIN);
//
//    int custom0 = book->addCustomNumFormat("0.0");
//    Format* numFormat2 = book->addFormat();
//    numFormat2->setNumFormat(custom0);
//    numFormat2->setAlignH(ALIGNH_CENTER);
//    numFormat2->setAlignV(ALIGNV_DISTRIBUTED);
//    numFormat2->setBorder(BORDERSTYLE_THIN);
//
//    int custom1 = book->addCustomNumFormat("[Magenta][<=0]0.0;[Blue]0.0");
//    Format* numFormat3 = book->addFormat();
//    numFormat3->setNumFormat(custom1);
//    numFormat3->setAlignH(ALIGNH_CENTER);
//    numFormat3->setAlignV(ALIGNV_DISTRIBUTED);
//    numFormat3->setBorder(BORDERSTYLE_THIN);
//
//
//    Format* numFormat4 = book->addFormat();
//    numFormat4->setNumFormat(custom1);
//    numFormat4->setAlignH(ALIGNH_CENTER);
//    numFormat4->setAlignV(ALIGNV_DISTRIBUTED);
//    numFormat4->setBorder(BORDERSTYLE_MEDIUM);
//
//    Format* numFormat5 = book->addFormat();
//    numFormat5->setNumFormat(book->addCustomNumFormat("000000"));
//    numFormat5->setAlignH(ALIGNH_CENTER);
//    numFormat5->setAlignV(ALIGNV_DISTRIBUTED);
//    numFormat5->setBorder(BORDERSTYLE_THIN);
//
//    Format* numFormat6 = book->addFormat();
//    numFormat6->setNumFormat(book->addCustomNumFormat("0000"));
//    numFormat6->setAlignH(ALIGNH_CENTER);
//    numFormat6->setAlignV(ALIGNV_DISTRIBUTED);
//    numFormat6->setBorder(BORDERSTYLE_THIN);
//
//    Format* numFormat7 = book->addFormat();
//    numFormat7->setNumFormat(book->addCustomNumFormat("000"));
//    numFormat7->setAlignH(ALIGNH_CENTER);
//    numFormat7->setAlignV(ALIGNV_DISTRIBUTED);
//    numFormat7->setBorder(BORDERSTYLE_THIN);
//
//    numFormat.push_back(numFormat0);
//    numFormat.push_back(numFormat1);
//    numFormat.push_back(numFormat2);
//    numFormat.push_back(numFormat3);
//    numFormat.push_back(numFormat4);
//    numFormat.push_back(numFormat5);
//    numFormat.push_back(numFormat6);
//    numFormat.push_back(numFormat7);
//}
//
//void ReleaseFormat()
//{
//    numFormat.erase(numFormat.begin(), numFormat.end());
//}
//
//void ReleaseBook(Book* book)
//{
//    //SendDebug(FSTART);
//
//    if(book)
//        book->release();
//
//    if(numFormat.size())
//        numFormat.erase(numFormat.begin(), numFormat.end());
//        //ReleaseFormat();
//    try
//    {
//            g_lock.unlock();
//    }
//    catch(...) {
//
//    }
//
//    //SendDebug(FSTOP);
//}
//
//
////Чтение Exel файла
//Book* LoadBook(std::string fname)
//{
//    //SendDebug(FSTART + fname);
//
//    g_lock.lock();
//    Book* book = NULL;
//    //try
//    //{
//        book = xlCreateXMLBook();
//
//        if(!book) book = xlCreateBook();
//        //if(!book)
//        //    throw std::exception(__FUN(std::string("book == NULL")));
//
//        if(!book->load(fname.c_str()))
//        {
//            book->release();
//            book = xlCreateBook();
//
//            //if(!book)
//            //    throw std::exception(__FUN(std::string("book == NULL")));
//
//            if(!book->load(fname.c_str()))
//            {
//                std::string s = book->errorMessage();
//                //throw std::exception(__FUN(s + " fname: \"" + fname + "\" "));
//            }
//        }
//        book->setKey(NameKey, PartKey);
//        CreateFormat(book);
//       // SendDebug(FSTOP + fname);
//        return book;
//    //}
//    //LOG_CATCH(TSTOP);
//
//    //if(book)
//    //    book->release();
//    //if(numFormat.size())
//    //    numFormat.erase(numFormat.begin(), numFormat.end());
//
//    ReleaseBook(book);
//    
//    //SendDebug(FSTOP + fname);
//
//    return NULL;
//
//}
//
//
////Создание новой книги Эксела
//Book* NewBook(std::string fname, int ID)
//{
//    SendDebug(FSTART + fname);
//
//    HGLOBAL hLoadedResource = NULL;
//    try
//    {
//        std::vector<boost::filesystem::path> P = getDirAll(lpCassetteDir);
//        for(auto p : P)
//        {
//            if(p.generic_string() == fname)
//            {
//                SendDebug(FSTOP);
//                return LoadBook(fname);
//            }
//        }
//        HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(ID), "DAT");
//        if(!hResource)
//            throw std::exception(__FUN("!FindResource"));
//
//        hLoadedResource = LoadResource(hInstance, hResource);
//        if(!hLoadedResource)
//            throw std::exception(__FUN("!hLoadedResource"));
//
//        LPVOID pLockedResource = LockResource(hLoadedResource);
//        if(!pLockedResource)
//            throw std::exception(__FUN("!pLockedResource"));
//
//        DWORD dwResourceSize = SizeofResource(hInstance, hResource);
//        if(!dwResourceSize)
//            throw std::exception(__FUN("!dwResourceSize"));
//
//        std::ofstream s(fname, std::ios::binary);
//        if(!s.is_open())
//            throw std::exception(__FUN("Not jpen new filename = \"" + fname + "\""));
//
//        s.write((char*)pLockedResource, dwResourceSize);
//        s.close();
//
//        if(hLoadedResource) FreeResource(hLoadedResource);
//
//        SendDebug(FSTOP + fname);
//        return LoadBook(fname);
//    }
//    LOG_CATCH(TSTOP);
//    if(hLoadedResource) GlobalFree(hLoadedResource);
//
//    SendDebug(FSTOP + fname);
//    return NULL;
//}
//
//
//
//#define AlloyCode "AlloyCode"
//#define Thickness_Alloy "Thickness_Alloy_"
//#define cNO "нет"
//
//std::string IN_NEW_XLS = "меткиIN_NEW.xls";
//
//
////Таблица марок листов
//std::map <int, std::string>MapAlloyCode2;
//
////Таблица толщин листов
//std::map <int, std::map<int, std::string>>MapThicknessCode2;
//
////Препарацыя Exel файла в таблицу марок и толщин листов
//void InitAlloyCode2()
//{
//    //SendDebug(FSTART + IN_NEW_XLS);
//
//    Book* book = NULL;
//    try
//    {
//        book = LoadBook(IN_NEW_XLS);
//        if(!book)
//            throw std::exception(__FUN(std::string("Не могу открыть файл: ") + IN_NEW_XLS));
//
//        Sheet* sheet = book->getSheet(0);
//        if(!sheet)
//        {
//            std::string s = book->errorMessage();
//            throw std::exception(__FUN(s + " fname: \"" + IN_NEW_XLS + "\" "));
//        }
//
//        int firstRow = sheet->firstRow();
//        int firstCol = sheet->firstCol();
//        int code = 0;
//        BOOL line = FALSE;
//        for(int row = firstRow; row < sheet->lastRow(); row++)
//        {
//            for(int col = firstCol; col < sheet->lastCol(); col++)
//            {
//                const char* s = sheet->readStr(row, col);
//                if(!s) continue;
//                std::string str = s;
//                if(col == firstCol)
//                {
//                    line = str == AlloyCode;
//                }
//                else
//                {
//                    if(line)
//                    {
//                        MapAlloyCode2[col - 1] = s;
//                        if(!memcmp(s, cNO, 3))
//                            MapAlloyCode2[col - 1] = "";
//                        else
//                            MapAlloyCode2[col - 1] = s;
//
//                        MapThicknessCode2[col - 1][0] = "";
//                    }
//                }
//            }
//        }
//
//        line = FALSE;
//        for(int row = firstRow; row < sheet->lastRow(); row++)
//        {
//            for(int col = firstCol; col < sheet->lastCol(); col++)
//            {
//                const char* s = sheet->readStr(row, col);
//                if(!s) continue;
//                std::string str = s;
//
//                if(col == firstCol)
//                {
//                    if(str.find(Thickness_Alloy) != SIZE_MAX)
//                    {
//                        for(auto a : MapAlloyCode2)
//                        {
//                            if(a.second == &s[16])
//                            {
//                                code = a.first;
//                                line = TRUE;
//                                break;
//                            }
//                        }
//                        continue;
//                    }
//                    else
//                    {
//                        code = 0;
//                        line = FALSE;
//                        col = sheet->lastCol();
//                        continue;
//                    }
//                }
//                else
//                {
//                    if(line)
//                    {
//                        if(!memcmp(s, cNO, 3))
//                            MapThicknessCode2[code][col - 1] = "";
//                        else
//                            MapThicknessCode2[code][col - 1] = s;
//                    }
//                }
//            }
//        }
//
//    }
//    LOG_CATCH(TSTOP);
//    ReleaseBook(book);
//    SendDebug(FSTOP + IN_NEW_XLS);
//
//}
//#undef AlloyCode
//#undef Thickness_Alloy
//#undef cNO
//
////Получить имя марки листа по кодам из PLC
//std::string GetAlloy(int32_t CodeAlloy)
//{
//    return MapAlloyCode[CodeAlloy];
//}
//
////Получить толщину листа по кодам из PLC
//std::string GetThicknes(int32_t CodeAlloy, int32_t CodeThicknes)
//{
//    return MapThicknessCode[CodeAlloy][CodeThicknes];
//}
//
//
//
//
//
//void OpenAllParam()
//{
//    SendDebug(FSTART);
//    try
//    {
//        InitAlloyCode();
//        //OpenSheet();
//        //OpenCassette();
//    }
//    LOG_CATCH(TSTOP);
//    SendDebug(FSTOP);
//    
//}