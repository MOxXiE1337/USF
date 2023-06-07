#pragma once
#include <Windows.h>
#include <TlHelp32.h>

struct ModuleInfo
{
	CONST WCHAR* ImageName;
	ULONG_PTR ImageBase;
	ULONG_PTR ImageSize;
};

namespace Modules
{
	// note that this function only scan once, that means the after loaded modules won't be linked automaticly
	BOOL InitModules();
	// check if the address belongs to any module
	BOOL GetAddressOwner(ULONG_PTR uAddress, ModuleInfo** pModuleInfo);
}
