// stdafx.h : Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Selten verwendete Teile der Windows-Header nicht einbinden.
// Windows-Headerdateien:
#include <windows.h>



// TODO: Hier auf zus�tzliche Header, die das Programm erfordert, verweisen.
#include "Log.h"
#include "Sdk.h"
#include "Timer.h"
#include "XStr.h"

// TODO: Remove dependency
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

#pragma comment(lib, "BeaEngine\\Win32\\Lib\\BeaEngine.lib")

#define TRY(s) static char func_##s[128]; static bool b_##s = false; if(!b_##s){strcpy(func_##s, #s); b_##s = true;} __try {
#define EXCEPT(s) } __except(EXCEPTION_EXECUTE_HANDLER) { Log(("Exception %x at %s", GetExceptionCode(), func_##s)); }
