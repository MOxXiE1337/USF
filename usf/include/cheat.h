#pragma once
#include <Windows.h>

struct CheatInfo
{
	ULONG64   Hash;
	ULONG_PTR ImageBase;
	ULONG_PTR ImageSize;

	ULONG64 CalculateHash();
};

namespace Cheat
{
	BOOL LocateByPageScanning(CheatInfo* pCheatInfo);

	BOOL InitVirtualAllocHook();
	BOOL ShutdownVirtualAllocHook();

	BOOL InitCopyTrap();
	BOOL ShutdownCopyTrap();

	BOOL InitVirtualQueryHook();
	BOOL ShutdownVirtualQueryHook();

	LONG NTAPI ExceptionHandler(_EXCEPTION_POINTERS* pExceptionPointers);
}

extern CheatInfo g_cheatInfo;
