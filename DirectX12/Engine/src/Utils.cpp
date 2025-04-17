#include "pch.h"
#include "Utils.h"

const wchar_t* Engine::Utils::GetWchar(const Int8* c)
{
    size_t charSize = strlen((const char*)c) + 1;
    wchar_t* wc = new wchar_t[charSize];

    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wc, charSize, (const char*)c, _TRUNCATE);

    return wc;
}

