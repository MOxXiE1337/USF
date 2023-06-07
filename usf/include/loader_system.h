#pragma once
#include <Windows.h>
#include "config_system.h"

typedef VOID(*AddFontFn)(const FontInfo&);
typedef ULONG32(*GetRHashFn)();
typedef VOID(*AddTranslationFn)(ULONG32, CONST CHAR*);
typedef VOID(*AddTextResizingFn)(ULONG32, CONST CHAR*);
typedef CONST CHAR* (*GetTranslationFn)(ULONG32);
typedef CONST CHAR* (*GetTextResizingFn)(ULONG32);
typedef PBYTE(*ReadWholeFileFn)(CONST CHAR* pFilePath, SIZE_T* pSize);

struct LoaderInfo
{
	HMODULE Module;
	CHAR Name[64];

	// imgui hooking 
	LPVOID ImFont__RenderText;
	LPVOID ImFont__CalcTextSizeA;
	LPVOID ImFontAtlas__AddFont;

	CHAR   ImFont__RenderTextSignature[64];
	CHAR   ImFont__CalcTextSizeASignature[64];
	CHAR   ImFontAtlas__AddFontSignature[64];

	LPVOID OriginalImFont__RenderText;
	LPVOID OriginalImFont__CalcTextSizeA;
	LPVOID OriginalImFontAtlas__AddFont;

	// translating
	FontInfo* Fonts;

	BOOL GeneratingConfig;

	AddFontFn AddFont;
	GetRHashFn GetRHash;
	AddTranslationFn AddTranslation;
	AddTextResizingFn AddTextResizing;
	GetTranslationFn GetTranslation;
	GetTextResizingFn GetTextResizing;
	ReadWholeFileFn ReadWholeFile;
};

typedef BOOL(*LoaderInitFn)(LoaderInfo* pLoaderInfo);
typedef BOOL(*LoaderShutdownFn)(LoaderInfo* pLoaderInfo);

namespace LoaderSystem
{
	BOOL FetchLoaderFromServer();
	BOOL LoadLoader();
	BOOL ShutdownLoader();
}

extern LoaderInfo g_loaderInfo;
