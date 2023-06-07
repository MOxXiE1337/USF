#include <stdio.h>
#include <sys/stat.h>

#include "usf.h"
#include "loader_system.h"

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




	return 0;
}
