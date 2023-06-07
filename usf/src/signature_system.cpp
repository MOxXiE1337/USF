#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

#include "signature_system.h"

#include "usf.h"
#include "json/json.h"

std::unordered_map<std::string, std::pair<std::pair<std::string, std::string>, ULONG_PTR>> g_labeledSignatures = {};

BOOL SignatureSystem::FetchSignaturesFromServer()
{
	// delete old file
	remove("usf/signatures.json");

	// download the lastest signatures
	HRESULT result = URLDownloadToFileA(NULL,
		"https://ghproxy.com/https://raw.githubusercontent.com/MOxXiE1337/MOxXiE1337.github.io/main/files/signatures.json",
		"usf/signatures.json", 0, NULL);

	if (result == S_OK)
	{
		std::ifstream stream;
		stream.open("usf/signatures.json");

		if (!stream.is_open())
		{
			return FALSE;
		}

		Json::CharReaderBuilder readBuilder = {};
		readBuilder["emitUTF8"] = true;

		Json::Value data = {};

		std::string error = "";

		if (!Json::parseFromStream(readBuilder, stream, &data, &error))
		{
			return FALSE;
		}
		
		Json::Value::Members mem = data.getMemberNames();
		for (auto& i : mem)
		{
			g_labeledSignatures[i] = { { data[i]["module"].asString(), data[i]["signature"].asString() }, 0x0 };
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

ULONG_PTR SignatureSystem::ScanSignature(const std::string& strLable)
{
	if (g_labeledSignatures.count(strLable) == 0)
		return NULL;

	if (g_labeledSignatures[strLable].second != 0x0)
		return g_labeledSignatures[strLable].second;

	g_labeledSignatures[strLable].second = ScanRawSignature(g_labeledSignatures[strLable].first.first, g_labeledSignatures[strLable].first.second);

	return g_labeledSignatures[strLable].second;
}

ULONG_PTR SignatureSystem::ScanRawSignature(ULONG_PTR pStartAddress, SIZE_T nSize, const std::string& strSignature)
{
	PBYTE pScanStart, pScanEnd;
	std::stringstream stream{ strSignature };
	std::string c;
	std::vector<std::pair<BYTE, BOOL>> signature;

	if (!pStartAddress || !nSize || strSignature.empty())
		return NULL;

	while (stream >> c)
	{
		if (c[0] == '?')
			signature.push_back({ 0,TRUE });
		else if (isxdigit(c[0]) && isxdigit(c[1]))
			signature.push_back({ (BYTE)strtoul(c.c_str(), 0, 16), FALSE });
	}

	pScanStart = (PBYTE)pStartAddress;
	pScanEnd = pScanStart + nSize;

	PBYTE result = std::search(pScanStart, pScanEnd, signature.begin(), signature.end(),
		[](const BYTE b, const std::pair<BYTE, BOOL>& p) {
			return b == p.first || p.second;
		});

	if (result == pScanEnd)
		return NULL;

	return (ULONG_PTR)result;
}

ULONG_PTR SignatureSystem::ScanRawSignature(HMODULE hModule, const std::string& strSignature)
{
	MODULEINFO moduleInfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO));
	return ScanRawSignature((ULONG_PTR)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, strSignature);
}

ULONG_PTR SignatureSystem::ScanRawSignature(const std::string& strModuleName, const std::string& strSignature)
{
	if (strModuleName == "__CHEAT__")
		return ScanRawSignature(g_cheatInfo.ImageBase, g_cheatInfo.ImageSize, strSignature);
	return ScanRawSignature(GetModuleHandleA(strModuleName.c_str()), strSignature);
}
