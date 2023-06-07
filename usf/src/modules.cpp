#include <list>

#include "modules.h"

std::list<ModuleInfo> g_listModules = {};

BOOL Modules::InitModules()
{
	g_listModules.clear();

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

	if (hSnapshot == INVALID_HANDLE_VALUE)
		return FALSE;

	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	
	// scan every module
	BOOL bRet = Module32First(hSnapshot, &me);
	while (bRet)
	{
		g_listModules.push_back({ me.szModule,(ULONG_PTR)me.modBaseAddr,me.modBaseSize });
		bRet = Module32Next(hSnapshot, &me);
	}

	return TRUE;
}

BOOL Modules::GetAddressOwner(ULONG_PTR uAddress, ModuleInfo** pModuleInfo)
{
	for (auto& i : g_listModules)
	{
		if (uAddress >= i.ImageBase && uAddress < i.ImageBase + i.ImageSize)
		{
			*pModuleInfo = &i;
			return TRUE;
		}
	}

	return FALSE;
}
