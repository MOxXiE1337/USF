
#include "usf.h"
#include "hooks.h"
#include "loader_system.h"
#include "signature_system.h"

CheatInfo g_cheatInfo = {};

VOID USF::InitFramework()
{
	// create folders
	_wmkdir(L"usf");
	_wmkdir(L"usf/loaders");
	_wmkdir(L"usf/configs");

	// init module system
	if (!Modules::InitModules())
	{
		ReportErrorAndExit("An error occured while initalizing module system!", "USF");
	}

	// init minhook
	if (MH_Initialize())
	{
		ReportErrorAndExit("An errror occured while initializing minhook!", "USF");
	}

	if (!SignatureSystem::FetchSignaturesFromServer())
	{
		ReportErrorAndExit("An errror occured while fetching signautres online!", "USF");
	}
	
	BOOL bDynamicMapped = !Cheat::LocateByPageScanning(&g_cheatInfo);

	// locate cheat by page scanning first to identify if the cheat is static mapped
	if (bDynamicMapped)
	{
		// setup the virtual alloc hook to locate the cheat dynamicly
		if (!Cheat::InitVirtualAllocHook())
		{
			ReportErrorAndExit("An errror occured while initializing VirtualAlloc hook!", "USF");
		}
	}
	else
	{
		// init copy trap
		if (!Cheat::InitCopyTrap())
		{
			ReportErrorAndExit("An errror occured while initializing copy trap!", "USF");
		}

		// bypass virtual query (because we will set the last byte to PAGE_GUARD, the VirtualQuery will return the wrong result
		if (!Cheat::InitVirtualQueryHook())
		{
			ReportErrorAndExit("An errror occured while initializing VirtualQuery hook!", "USF");
		}
	}

	MessageBox(FindWindow(TEXT("Vavle001"), NULL), TEXT("The framework has been initialized successfully, please inject your cheat."), TEXT("USF"), MB_OK | MB_ICONINFORMATION);

	return;
}

VOID USF::LoadFramework()
{
	// calculate hash
	g_cheatInfo.CalculateHash();

	ULONG_PTR pDrawPrintText = SignatureSystem::ScanSignature("ISurface::DrawPrintText");
	ULONG_PTR pGetTextSize = SignatureSystem::ScanSignature("ISurface::GetTextSize");
	ULONG_PTR pSetFontGlyphSet = SignatureSystem::ScanSignature("ISurface::SetFontGlyphSet");

	DebugPrint("[USF] Hash: 0x%016llX\n", g_cheatInfo.Hash);
	DebugPrint("[USF] ImageBase: 0x%08X\n", g_cheatInfo.ImageBase);
	DebugPrint("[USF] ImageSize: 0x%08X\n", g_cheatInfo.ImageSize);
	DebugPrint("[USF] ISurface::DrawPrintText: 0x%08X\n", pDrawPrintText);
	DebugPrint("[USF] ISurface::GetTextSize: 0x%08X\n", pGetTextSize);
	DebugPrint("[USF] ISurface::SetFontGlyphSet: 0x%08X\n", pSetFontGlyphSet);

	CHAR aFilePath[64] = { 0 };
	sprintf_s(aFilePath, "usf/loaders/%016llX.dll", g_cheatInfo.Hash);

	// load loader
	if (LoaderSystem::FetchLoaderFromServer(g_cheatInfo.Hash))
	{
		DebugPrint("[USF] Loading loader usf/loaders/%016llX.dll\n", g_cheatInfo.Hash);

		if (!LoaderSystem::LoadLoader(g_cheatInfo.Hash))
		{
			ReportErrorAndExit("An errror occured while loading the loader!", "USF");
		}
	}
	else // use surface hook
	{
		DebugPrint("[USF] Using surface hook to translate the cheat...\n");
		if (!InitSurfaceHooks())
		{
			ReportErrorAndExit("An errror occured while initializing surface hooks!", "USF");
		}
	}

	return;
}


BOOL g_bUseSurfaceHook = FALSE;
extern decltype(&Hooks::Surface::SetFontGlyphSet) g_pOriginalSetFontGlyphSet;

BOOL USF::InitSurfaceHooks()
{
	// hook at ISurface::SetFontGlyphSet
	if (MH_CreateHook((LPVOID)SignatureSystem::ScanSignature("ISurface::SetFontGlyphSet"), Hooks::Surface::SetFontGlyphSet, (LPVOID*)&g_pOriginalSetFontGlyphSet))
	{
		return FALSE;
	}
	if (MH_EnableHook(MH_ALL_HOOKS))
	{
		return FALSE;
	}

	g_bUseSurfaceHook = TRUE;
	return TRUE;
}

BOOL USF::ShutdownSurfaceHooks()
{
	if (!g_bUseSurfaceHook)
		return TRUE;

	if (MH_RemoveHook((LPVOID)SignatureSystem::ScanSignature("ISurface::SetFontGlyphSet")))
	{
		return FALSE;
	}
	return TRUE;
}

