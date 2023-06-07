#pragma once
#include "usf.h"

namespace Hooks
{
	namespace Cheat
	{
		LPVOID WINAPI VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
		SIZE_T WINAPI VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
	}
	
	namespace Surface
	{
		bool __fastcall SetFontGlyphSet(LPVOID pEcx, LPVOID pEdx, ULONG uFont, CONST CHAR* pFontName, INT iTall, INT iWeight, INT iBlur, INT iScanlines, INT iFlags, INT iRangeMin, INT iRangeMax);
	}
}
