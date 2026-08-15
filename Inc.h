#pragma once

#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1

#include "ntdll.h"

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#define CWA(dll, api) ::api

#include <windows.h>