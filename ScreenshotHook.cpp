#include "stdafx.h"
#include "ScreenshotHook.h"
#include "Client.h"
#include "SecureEngine.h"
#include <gl/Gl.h>
#pragma comment(lib, "opengl32.lib")

/*
glReadPixels
73E02228 >  64:8B15 18000000                       mov     edx, dword ptr fs:[18] ; Get TEB address
73E0222F    833D 2C10EA73 40                       cmp     dword ptr ds:[73EA102C], 40 ; Check which graphic we're using
73E02236    73 10                                  jnb     short opengl32.73E02248
73E02238    8BC2                                   mov     eax, edx
73E0223A    0315 0C10EA73                          add     edx, dword ptr ds:[73EA100C] ; Add value in 73EA100C to edx which is TEB pointer, which will then point to the GL table in the TEB
73E02240    8B12                                   mov     edx, dword ptr ds:[edx] ; Move the address in that entry to edx
73E02242    FFA2 00040000                          jmp     dword ptr ds:[edx+400] ; jmp to edx + 0x400
73E02248    FF35 2C10EA73                          push    dword ptr ds:[73EA102C] : Push TLS index
73E0224E    FF15 E810E073                          call    dword ptr ds:[<&KERNEL32.TlsGetValue>]                   ; kernel32.TlsGetValue
73E02254    92                                     xchg    eax, edx ; Load address we obtained from TlsGetValue into edx
73E02255    FFA2 00040000                          jmp     dword ptr ds:[edx+400] ; jmp to edx + 0x400

We will exchange the address that is saved in the pointer which is calculated here (edx+400).
 */

DWORD l_origReadPixels = 0;
PBYTE l_pScreenData = NULL;
size_t l_nScreenSize = 0;
GLenum l_uFlag = GL_RGB;
GLenum l_uDataFlag = GL_UNSIGNED_BYTE;

__declspec(naked) void _stdcall myGlReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
	_asm jmp l_origReadPixels
}

DWORD dwDataAddr =  0;
__declspec(naked) void hooked_glReadPixels( void )
{
	_asm pushad; // push registers
	_asm mov eax, dword ptr ds:[esp+0x34]
	_asm mov l_uFlag, eax
	_asm mov eax, dword ptr ds:[esp+0x38]
	_asm mov l_uDataFlag, eax
	_asm mov eax, dword ptr ds:[esp+0x3C] // get parameter "GLVoid *pixels"
	_asm mov dwDataAddr, eax // mov address from eax into dwData
	if( l_pScreenData && l_nScreenSize )
		memcpy((PVOID)dwDataAddr, (PVOID)l_pScreenData, l_nScreenSize); // copy screendata into that address
	_asm popad; // pop registers
	_asm ret 0x1C; // ret and clean up stack
	//_asm jmp l_origReadPixels;
}

bool TakeScreenshot( void )
{
	SCREENINFO sScreen;
	sScreen.iSize = sizeof(SCREENINFO);
	g_cSecureEngine.Get()->pfnGetScreenInfo( &sScreen );

	int nComponents = 3; // RGB
	if( l_uFlag == GL_RGBA || l_uFlag == GL_BGRA_EXT )
		nComponents = 4;

	int nSize = sScreen.iHeight * sScreen.iWidth * nComponents; // Height * Width and for every pixel three or four channels (RGB or RGBA)

	if( l_pScreenData != NULL )
	{
		delete[] l_pScreenData; // free already taken shot
		l_pScreenData = NULL;
		l_nScreenSize = 0;
	}

	Log(("Taking screenshot nSize %d nComponents %d Flag 0x%04x DataFlag 0x%04x", nSize, nComponents, l_uFlag, l_uDataFlag));
	l_pScreenData = new BYTE[nSize];
	if( !l_pScreenData )
		return false;

	myGlReadPixels(0, 0, sScreen.iWidth, sScreen.iHeight, l_uFlag, l_uDataFlag, (GLvoid*)l_pScreenData);

	l_nScreenSize = nSize;
	return true;
}

DWORD GetReadPixelAddr( void )
{
	return (DWORD)GetProcAddress(GetModuleHandle(XStr( /*opengl32.dll*/ 0x03, 0x0C, 0x00, 0x6F71676D, 0x63693535, 0x266D6667 ).c()), XStr( /*glReadPixels*/ 0x03, 0x0C, 0x00, 0x676D5066, 0x6561566E, 0x706C6678 ).c());
}

bool HookReadPixelsByTLS( void )
{
	DWORD dwGlReadPixels = GetReadPixelAddr();
	if( !dwGlReadPixels )
		return false;

	__try
	{
		Log(("glReadPixels at %08x", dwGlReadPixels));
		DWORD dwPointerTls = 0;

		// Calculate the pointer
		DWORD dwCmpIndexAddr = *(DWORD*)(dwGlReadPixels+0x9); // cmp |dwCmpIndexAddr|, 40 and push |dwCmpIndexAddr| before TlsGetValue
		DWORD dwAddAddr = *(DWORD*)(dwGlReadPixels+0x14); // add edx, |dwAddAddr|

		Log(("dwCmpIndexAddr %08x dwAddAddr %08x", dwCmpIndexAddr, dwAddAddr));

		DWORD dwTIB = __readfsdword(0x18);
		Log(("dwTIB at %08x", dwTIB));
		if( !dwTIB )
			return false;

		if( (*(DWORD*)dwCmpIndexAddr) <= 40 ) // cmp     dword ptr ds:[73EA102C], 40
		{
			dwTIB += *(DWORD*)dwAddAddr; // add     edx, dword ptr ds:[73EA100C]
			dwPointerTls = *(DWORD*)dwTIB; //  mov     edx, dword ptr ds:[edx]
			Log(("dwTIB after add %08x", dwTIB));
		}
		else
		{
			DWORD dwIndex = *(DWORD*)dwCmpIndexAddr;
			Log(("dwIndex %08x", dwIndex));
			dwPointerTls = (DWORD)TlsGetValue(dwIndex);
		}

		dwPointerTls += 0x400;
		Log(("dwPointerTls %08x", dwPointerTls));
		if( !dwPointerTls )
			return false;

		l_origReadPixels = *(DWORD*)dwPointerTls;
		Log(("l_origReadPixels %08x", l_origReadPixels));
		*(DWORD*)dwPointerTls = (DWORD)hooked_glReadPixels;
		Log(("New dwPointerTls %08x", (DWORD)hooked_glReadPixels));

		return true;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}
