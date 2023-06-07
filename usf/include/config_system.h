#pragma once
#include <Windows.h>

struct FontInfo
{
	CHAR FontName[64];
	INT  FontSize;
	INT  GlyphRanges; // -1 == not set, 0 == ALL, 1 == English, 2 == Chinese
};

namespace ConfigSystem
{
	BOOL FetchConfigFromServer();
	BOOL LoadConfig();

	BOOL& GeneratingConfig();

	FontInfo* Fonts();
	VOID AddFont(const FontInfo& fontInfo);
	VOID AddTranslation(ULONG32 uHash, const char* string);
	VOID AddTextResizing(ULONG32 uHash, const char* string);
	CONST CHAR* GetTranslation(ULONG32 uHash);
	CONST CHAR* GetTextResizing(ULONG32 uHash);

	VOID HotKeyRoutine();
	BOOL OutputConfig();

	ULONG32 GetRHash();
	PBYTE ReadWholeFile(CONST CHAR* pFilePath, SIZE_T* pSize);
}
