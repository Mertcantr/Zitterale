#include "stdafx.h"
#include <stdio.h>

#pragma warning(disable:4996)


#ifdef DO_LOG
void DbgLog( char *pszBuf, ... )
{
	char buffer[1024];
	char outstring[1128];

	va_list va_alist;
	va_start(va_alist, pszBuf);
	vsnprintf(buffer, sizeof(buffer), pszBuf, va_alist);
	va_end(va_alist);

	strcpy(outstring, XStr( /*ENIGMA: */ 0x02, 0x08, 0x00, 0x454F4B44, 0x49443C27 ).c());
	strcat(outstring, buffer);

	OutputDebugStringA(outstring);

	FILE *file = fopen(XStr( /*C:\\Users\\Public\\g32.log*/ 0x07, 0x1A, 0x00, 0x433B5E5F, 0x51766375, 0x7B55565B, 0x796F6266, 0x734D4E74, 0x2727387B, 0x777E0000 ).c(), XStr( /*a+*/ 0x01, 0x02, 0x00, 0x612A0000 ).c());
	if( file )
	{
		fprintf(file, XStr( /*%s\n*/ 0x01, 0x04, 0x00, 0x25725E6D ).c(), outstring);
		fclose(file);
	}
}
#endif
