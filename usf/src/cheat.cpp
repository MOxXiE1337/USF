#include "hooks.h"
#include "cheat.h"
#include "modules.h"

ULONG64 CheatInfo::CalculateHash()
{
	ULONG64 uHash = 0;

	// calculate from the middle of the cheat to avoid the 00 headers
	PBYTE pStart = PBYTE(ImageBase + ImageSize / 2);
		
	for (SIZE_T u = 0; u < 512; u++)
	{
		uHash = 0x1B39 * uHash + pStart[u];
	}
	
	Hash = uHash;

	return uHash;
}

BOOL Cheat::LocateByPageScanning(CheatInfo* pCheatInfo)
{
	// get system info
	SYSTEM_INFO sysInfo = { 0 };
	GetSystemInfo(&sysInfo);

	LPBYTE pStart = (LPBYTE)sysInfo.lpMinimumApplicationAddress;
	MEMORY_BASIC_INFORMATION mbi = { 0 };

	while (pStart < sysInfo.lpMaximumApplicationAddress)
	{
		SIZE_T uSize = VirtualQuery(
			pStart,
			&mbi,
			sizeof(MEMORY_BASIC_INFORMATION));

		if (uSize != sizeof(MEMORY_BASIC_INFORMATION))
			break;

		// check if the module belongs a module, size at least 0x400000, protect is PAGE_EXECUTE_READWRITE
		ModuleInfo* mi;
		if (!Modules::GetAddressOwner((ULONG_PTR)mbi.BaseAddress, &mi)
			&& mbi.RegionSize > 0x400000
			&& mbi.Protect == PAGE_EXECUTE_READWRITE
			&& mbi.AllocationProtect == PAGE_EXECUTE_READWRITE)
		{
			pCheatInfo->ImageBase = (ULONG_PTR)mbi.BaseAddress;
			pCheatInfo->ImageSize = mbi.RegionSize;
			return TRUE;
		}

		pStart += mbi.RegionSize;
	}
	
	return FALSE;
}

extern decltype(&Hooks::Cheat::VirtualAlloc) g_pOriginalVirtualAlloc;

BOOL Cheat::InitVirtualAllocHook()
{
	// hook at virtual alloc
	if (MH_CreateHook(VirtualAlloc, Hooks::Cheat::VirtualAlloc, (LPVOID*)&g_pOriginalVirtualAlloc))
	{
		return FALSE;
	}
	if (MH_EnableHook(MH_ALL_HOOKS))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Cheat::ShutdownVirtualAllocHook()
{
	if (MH_RemoveHook(VirtualAlloc))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL Cheat::InitCopyTrap()
{
	LPVOID pTargetAddress = (LPVOID)(g_cheatInfo.ImageBase + g_cheatInfo.ImageSize - 1);

	DWORD flOldProtect = 0;
	if (!VirtualProtect(pTargetAddress, 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &flOldProtect))
	{
		return FALSE;
	}

	MEMORY_BASIC_INFORMATION mbi = {};
	VirtualQuery((LPVOID)pTargetAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

	if (mbi.Protect != (PAGE_EXECUTE_READWRITE | PAGE_GUARD))
	{
		return FALSE;
	}

	return TRUE;
}


BOOL Cheat::ShutdownCopyTrap()
{
	LPVOID pTargetAddress = (LPVOID)(g_cheatInfo.ImageBase + g_cheatInfo.ImageSize - 1);

	DWORD flOldProtect = 0;
	if (!VirtualProtect(pTargetAddress, 1, PAGE_EXECUTE_READWRITE, &flOldProtect))
	{
		return FALSE;
	}

	MEMORY_BASIC_INFORMATION mbi = {};
	VirtualQuery((LPVOID)pTargetAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

	if (mbi.Protect != (PAGE_EXECUTE_READWRITE))
	{
		return FALSE;
	}

	return TRUE;
}

extern decltype(&Hooks::Cheat::VirtualQuery) g_pOriginalVirtualQuery;

BOOL Cheat::InitVirtualQueryHook()
{

	// hook at virtual alloc
	if (MH_CreateHook(VirtualQuery, Hooks::Cheat::VirtualQuery, (LPVOID*)&g_pOriginalVirtualQuery))
	{
		return FALSE;
	}
	if (MH_EnableHook(MH_ALL_HOOKS))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Cheat::ShutdownVirtualQueryHook()
{
	if (MH_RemoveHook(VirtualQuery))
	{
		return FALSE;
	}
	return TRUE;
}

LONG NTAPI Cheat::ExceptionHandler(_EXCEPTION_POINTERS* pExceptionPointers)
{
	// check if the exception is EXCEPTION_GUARD_PAGE
	if (pExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE)
	{
		// check if the target address is cheat's last byte
		ULONG uType = pExceptionPointers->ExceptionRecord->ExceptionInformation[0];	
		ULONG_PTR uTargetAddress = pExceptionPointers->ExceptionRecord->ExceptionInformation[1];
		
		if (uType != 0x00000001 || uTargetAddress <g_cheatInfo.ImageBase || uTargetAddress >= (g_cheatInfo.ImageBase + g_cheatInfo.ImageSize))
			return EXCEPTION_CONTINUE_SEARCH;

		// shutdown trap
		if (!Cheat::ShutdownCopyTrap())
		{
			ReportErrorAndExit("A fatal errror occured while shutdown copy trap!", "USF");
		}

		// load our framework!
		USF::LoadFramework();
		
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_EXECUTION;
}
