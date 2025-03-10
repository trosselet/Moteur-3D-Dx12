#pragma once
#include "pch.h"

#include <iostream>
#include <sstream>

#define PRINT_CONSOLE_OUTPUT(s) { std::wostringstream os_; os_ << s; OutputDebugStringW(os_.str().c_str());};