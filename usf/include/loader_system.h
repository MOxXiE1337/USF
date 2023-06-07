#pragma once
#include <Windows.h>

struct LoaderInfo
{
	HMODULE Module;
	CHAR Name[64];

	LPVOID ImFont__RenderText;
	LPVOID ImFont__CalcTextSizeA;
	LPVOID ImFontAtlas__AddFont;

	CHAR   ImFont__RenderTextSignature[64];
	CHAR   ImFont__CalcTextSizeASignature[64];
	CHAR   ImFontAtlas__AddFontSignature[64];

	LPVOID OriginalImFont__RenderText;
	LPVOID OriginalImFont__CalcTextSizeA;
	LPVOID OriginalImFontAtlas__AddFont;
};

typedef BOOL(*LoaderInitFn)(LoaderInfo* pLoaderInfo);
typedef BOOL(*LoaderShutdownFn)(LoaderInfo* pLoaderInfo);

namespace LoaderSystem
{
	BOOL FetchLoaderFromServer(ULONG64 uHash);
	BOOL LoadLoader(ULONG64 uHash);
	BOOL ShutdownLoader();
}

extern LoaderInfo g_loaderInfo;
