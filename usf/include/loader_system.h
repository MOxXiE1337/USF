#pragma once
#include <Windows.h>

struct LoaderProcs
{

};

namespace LoaderSystem
{
	BOOL FetchLoaderFromServer(ULONG64 uHash);
	BOOL LoadLoader(ULONG64 uHash);
}
