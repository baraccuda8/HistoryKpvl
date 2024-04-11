#pragma once


class CGdiPlusInit
{
public:
    CGdiPlusInit();
    virtual ~CGdiPlusInit();
    bool Good() { return present; }
private:
    bool present;
    ULONG_PTR token;
};
