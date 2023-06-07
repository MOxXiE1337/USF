#pragma once
#include <string>
#include <Windows.h>
#include <Psapi.h>

namespace SignatureSystem
{
	BOOL FetchSignaturesFromServer();
	ULONG_PTR ScanSignature(const std::string& strLable);
	ULONG_PTR ScanFuzzySignature(const std::string& strLable);
	ULONG_PTR ScanRawSignature(ULONG_PTR pStartAddress, SIZE_T nSize, const std::string& strSignature);
	ULONG_PTR ScanRawSignature(HMODULE hModule, const std::string& strSignature);
	ULONG_PTR ScanRawSignature(const std::string& strModuleName, const std::string& strSignature);
}
