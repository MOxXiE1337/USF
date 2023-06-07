#pragma once
#include <intrin.h>

#include "cheat.h"
#include "modules.h"

#include "minhook/minhook.h"

#define ReportErrorAndExit(text, title) \
MessageBox(FindWindow(TEXT("Vavle001"), NULL), TEXT(text), TEXT(title), MB_OK | MB_ICONERROR); \
TerminateProcess(GetCurrentProcess(), 0)

#ifdef _DEBUG
#include <stdio.h>
#define DebugPrint(text, ...) printf(text, __VA_ARGS__)
#else
#define DebugPrint(text, ...)
#endif

enum USF_STRATEGY
{
	UNKNOWN = -1,
	SURFACE,
	IMGUI
};

namespace USF
{
	VOID InitFramework();
	VOID LoadFramework();

	BOOL InitSurfaceHooks();
	BOOL ShutdownSurfaceHooks();
}

extern HMODULE g_hUSFModule;
