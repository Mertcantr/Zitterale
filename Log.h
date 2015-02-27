#pragma once

#ifdef DO_LOG
void DbgLog( char *pszBuf, ... );
#endif

#ifdef DO_LOG
#define Log(x) DbgLog x
#else
#define Log(x)
#endif
