#include "hooks.h"

decltype(&Hooks::Surface::SetFontGlyphSet) g_pOriginalSetFontGlyphSet = NULL;

bool __fastcall Hooks::Surface::SetFontGlyphSet(LPVOID pEcx, LPVOID pEdx, ULONG uFont, const CHAR* pFontName, INT iTall, INT iWeight, INT iBlur, INT iScanlines, INT iFlags, INT iRangeMin, INT iRangeMax)
{
	ULONG_PTR uReturnAddress = (ULONG_PTR)_ReturnAddress();

	if (uReturnAddress >= g_cheatInfo.ImageBase && uReturnAddress < g_cheatInfo.ImageBase + g_cheatInfo.ImageSize)
	{
		DebugPrint("[USF] FontCreation:\n\tFontName: %s\n\tTall: %d\n\tWeight: %d\n", pFontName, iTall, iWeight);
	}

	return g_pOriginalSetFontGlyphSet(pEcx, pEdx, uFont, pFontName, iTall, iWeight, iBlur, iScanlines, iFlags, iRangeMin, iRangeMax);
}