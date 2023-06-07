#include "hooks.h"

decltype(&Hooks::Cheat::VirtualQuery) g_pOriginalVirtualQuery = NULL;

SIZE_T WINAPI Hooks::Cheat::VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength)
{
	SIZE_T uResult = g_pOriginalVirtualQuery(lpAddress, lpBuffer, dwLength);

	if (lpAddress == (LPVOID)g_cheatInfo.ImageBase)
	{
		lpBuffer->RegionSize = g_cheatInfo.ImageSize;
		lpBuffer->Protect = PAGE_EXECUTE_READWRITE;

		// shutdown hook
		if (!::Cheat::ShutdownVirtualQueryHook())
		{
			ReportErrorAndExit("A fatal errror occured while shutdown VirtualQuery hook!", "USF");
		}
	}
	return uResult;
}