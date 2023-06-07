#include "hooks.h"

decltype(&Hooks::Cheat::VirtualAlloc) g_pOriginalVirtualAlloc = NULL;

LPVOID WINAPI Hooks::Cheat::VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
	LPVOID pReturnAddress = _ReturnAddress();
	ModuleInfo* pMi;

	if (dwSize > 0x400000 && flProtect == PAGE_EXECUTE_READWRITE && !Modules::GetAddressOwner((ULONG_PTR)pReturnAddress, &pMi))
	{
		LPVOID pAllocatedPage = g_pOriginalVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
		g_cheatInfo.ImageBase = (ULONG_PTR)pAllocatedPage;
		g_cheatInfo.ImageSize = dwSize;
		
		// shutdown hook
		if (!::Cheat::ShutdownVirtualAllocHook())
		{
			ReportErrorAndExit("A fatal errror occured while shutdown VirtualAlloc hook!", "USF");
		}

		// init copy trap
		if (!::Cheat::InitCopyTrap())
		{
			ReportErrorAndExit("An errror occured while initializing copy trap!", "USF");
		}

		// bypass virtual query (because we will set the last byte to PAGE_GUARD, the VirtualQuery will return the wrong result
		if (!::Cheat::InitVirtualQueryHook())
		{
			ReportErrorAndExit("An errror occured while initializing VirtualQuery hook!", "USF");
		}

		return pAllocatedPage;
	}

	return g_pOriginalVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}


