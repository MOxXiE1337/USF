#include <stdio.h>
#include <sys/stat.h>

#include "usf.h"
#include "loader_system.h"
#include "signature_system.h"

LoaderInfo g_loaderInfo = { 0 };

BOOL LoaderSystem::FetchLoaderFromServer(ULONG64 uHash)
{
	CHAR aFilePath[64] = { 0 };
	sprintf_s(aFilePath, "usf/loaders/%016llX.dll", g_cheatInfo.Hash);

	struct stat bStat = { 0 };
	if (stat(aFilePath, &bStat) == 0)
	{
		return TRUE;
	}
	else
	{
		CHAR aUrl[512] = { 0 };
		sprintf_s(aUrl, "https://ghproxy.com/https://github.com/MOxXiE1337/MOxXiE1337.github.io/blob/main/files/loaders/%016llX.dll", uHash);

		HRESULT result = URLDownloadToFileA(NULL,
			aUrl,
			aFilePath,
			0, NULL);

		return (result == S_OK);
	}

	return TRUE;
}

BOOL LoaderSystem::LoadLoader(ULONG64 uHash)
{
	CHAR aFilePath[64] = { 0 };
	sprintf_s(aFilePath, "usf/loaders/%016llX.dll", g_cheatInfo.Hash);

	HMODULE hLoader = LoadLibraryA(aFilePath);

	if (hLoader == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	g_loaderInfo.Module = hLoader;

	LoaderInitFn LoaderInit = (LoaderInitFn)GetProcAddress(hLoader, "LoaderInit");
	FARPROC ImFontAtlas__AddFontProxy = GetProcAddress(hLoader, "ImFontAtlas__AddFontProxy");

	
	if (LoaderInit == NULL 
		|| ImFontAtlas__AddFontProxy == NULL)
	{
		return FALSE;
	}
	
	if (!LoaderInit(&g_loaderInfo))
	{
		return FALSE;
	}

	g_loaderInfo.ImFont__RenderText = (LPVOID)SignatureSystem::ScanRawSignature("__CHEAT__", g_loaderInfo.ImFont__RenderTextSignature);
	g_loaderInfo.ImFont__CalcTextSizeA = (LPVOID)SignatureSystem::ScanRawSignature("__CHEAT__", g_loaderInfo.ImFont__CalcTextSizeASignature);
	g_loaderInfo.ImFontAtlas__AddFont = (LPVOID)SignatureSystem::ScanRawSignature("__CHEAT__", g_loaderInfo.ImFontAtlas__AddFontSignature);

	if (g_loaderInfo.ImFont__RenderText == NULL
		|| g_loaderInfo.ImFont__CalcTextSizeA == NULL
		|| g_loaderInfo.ImFontAtlas__AddFont == NULL)
	{
		return FALSE;
	}

	// setup imgui hooks
	if (MH_CreateHook(g_loaderInfo.ImFontAtlas__AddFont, ImFontAtlas__AddFontProxy, &g_loaderInfo.OriginalImFontAtlas__AddFont))
	{
		return FALSE;
	}
	if (MH_EnableHook(MH_ALL_HOOKS))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL LoaderSystem::ShutdownLoader()
{
	if (g_loaderInfo.Module)
	{
		LoaderShutdownFn LoaderShutdown = (LoaderInitFn)GetProcAddress(g_loaderInfo.Module, "LoaderShutdown");

		if (LoaderShutdown == NULL)
		{
			return FALSE;
		}

		if (!LoaderShutdown(&g_loaderInfo))
		{
			return FALSE;
		}

		// shutdown imgui hooks
		if (MH_RemoveHook(g_loaderInfo.ImFontAtlas__AddFont))
		{
			return FALSE;
		}
	}

	return TRUE;
}
