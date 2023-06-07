#include <locale>
#include <codecvt>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <unordered_map>

#include "json/json.h"

#include "usf.h"
#include "config_system.h"

BOOL ConfigSystem::FetchConfigFromServer()
{
	CHAR aFilePath[64] = { 0 };
	sprintf_s(aFilePath, "usf/configs/%016llX.json", g_cheatInfo.Hash);

	struct stat bStat = { 0 };
	if (stat(aFilePath, &bStat) == 0)
	{
		return TRUE;
	}
	else
	{
		CHAR aUrl[512] = { 0 };
		sprintf_s(aUrl, "https://ghproxy.com/https://github.com/MOxXiE1337/MOxXiE1337.github.io/blob/main/files/configs/%016llX.json", g_cheatInfo.Hash);

		HRESULT result = URLDownloadToFileA(NULL,
			aUrl,
			aFilePath,
			0, NULL);

		return (result == S_OK);
	}

	return TRUE;
}

BOOL ConfigSystem::LoadConfig()
{
	CHAR aPath[64] = { 0 };
	sprintf_s(aPath, "usf/configs/%016llX.json", g_cheatInfo.Hash);
	
	std::ifstream stream;
	stream.open(aPath, std::ios::in | std::ios::binary);
	stream.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

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

	// process fonts
	for (auto& i : data["fonts"])
	{
		FontInfo fi;
		strcpy_s(fi.FontName, 64, i["name"].asCString());
		fi.FontSize = i["size"].asInt();
		fi.GlyphRanges = i["glyph_ranges"].asInt();
		AddFont(fi);
	}

	Json::Value::Members mem = data["translations"].getMemberNames();
	for (auto& i : mem)
	{
		ULONG32 uHash = std::stoul(i, nullptr, 16);
		AddTranslation(uHash, data["translations"][i].asCString());
	}

	mem = data["text_resizing"].getMemberNames();
	for (auto& i : mem)
	{
		ULONG32 uHash = std::stoul(i, nullptr, 16);
		AddTextResizing(uHash, data["text_resizing"][i].asCString());
	}
	
	return TRUE;
}

BOOL& ConfigSystem::GeneratingConfig()
{
	static BOOL bGeneratingConfig = FALSE;
	return bGeneratingConfig;
}

std::vector<FontInfo> g_vecFonts = {};

FontInfo* ConfigSystem::Fonts()
{
	return &g_vecFonts[0];
}

VOID ConfigSystem::AddFont(const FontInfo& fontInfo)
{
	g_vecFonts.push_back(fontInfo);
}

std::unordered_map<ULONG32, std::string> g_mapTranslations;
std::unordered_map<ULONG32, std::string> g_mapTextResizings;

VOID ConfigSystem::AddTranslation(ULONG32 uHash, const char* string)
{
	if (g_mapTranslations.count(uHash) > 0)
		return;

	g_mapTranslations[uHash] = string;
}

VOID ConfigSystem::AddTextResizing(ULONG32 uHash, const char* string)
{
	if (g_mapTextResizings.count(uHash) > 0)
		return;

	g_mapTextResizings[uHash] = string;
}

CONST CHAR* ConfigSystem::GetTranslation(ULONG32 uHash)
{
	if (g_mapTranslations.count(uHash) == 0)
		return NULL;

	return g_mapTranslations[uHash].c_str();
}

CONST CHAR* ConfigSystem::GetTextResizing(ULONG32 uHash)
{
	if (g_mapTextResizings.count(uHash) == 0)
		return NULL;

	return g_mapTextResizings[uHash].c_str();
}

VOID ConfigSystem::HotKeyRoutine()
{
	while (true)
	{
		if (GetAsyncKeyState(VK_F11) & 0x8000)
		{
			if (!OutputConfig())
			{
				ReportErrorAndExit("A fatal error occured while generating the config!", "USF");
			}

			CHAR aContent[64] = { 0 };
			sprintf_s(aContent, "Config has been written to \"usf/configs/%016llX.json\"!", g_cheatInfo.Hash);

			MessageBoxA(FindWindow(TEXT("Vavle001"), NULL), aContent, "USF", MB_OK | MB_ICONINFORMATION);
			TerminateProcess(GetCurrentProcess(), 0);
		}
	}
}

BOOL ConfigSystem::OutputConfig()
{
	CHAR aPath[64] = { 0 };
	sprintf_s(aPath, "usf/configs/%016llX.json", g_cheatInfo.Hash);

	Json::Value data = {};

	Json::Value global = {};

	global["message"] = "When you load the config, this message will automatically pop up!";

	data["global"] = global;

	// process fonts

	for (auto& i : g_vecFonts)
	{
		Json::Value font = {};
		font["name"] = i.FontName;
		font["size"] = i.FontSize;
		font["glyph_ranges"] = i.GlyphRanges;
		data["fonts"].append(font);
	}

	CHAR aHash[16] = { 0 };

	// process translations
	data["translations"] = {};
	for (auto& i : g_mapTranslations)
	{
		sprintf_s(aHash, "%08X", i.first);
		data["translations"][aHash] = i.second;
	}

	// process text_resizings
	data["text_resizings"] = {};
	for (auto& i : g_mapTextResizings)
	{
		sprintf_s(aHash, "%08X", i.first);
		data["text_resizings"][aHash] = i.second;
	}

	Json::StyledWriter sw = {};

	std::ofstream stream;
	stream.open(aPath, std::ios::out);

	if (!stream.is_open())
		return FALSE;

	stream << sw.write(data) << std::endl;
	stream.close();


	return TRUE;
}

ULONG32 ConfigSystem::GetRHash()
{
	ULONG32 uReturnAddress0, uReturnAddress1, uReturnAddress2, uReturnAddress3;

	__asm
	{
		push ecx
		mov eax, ebp; get old ebp
		mov eax, [eax]
		mov eax, [eax]
		mov ecx, [eax + 4]
		mov uReturnAddress0, ecx
		mov eax, [eax]
		mov ecx, [eax + 4]
		mov uReturnAddress1, ecx
		mov eax, [eax]
		mov ecx, [eax + 4]
		mov uReturnAddress2, ecx
		mov eax, [eax]
		mov ecx, [eax + 4]
		mov uReturnAddress3, ecx
		pop ecx
	}
	
	if (uReturnAddress0 >= g_cheatInfo.ImageBase && uReturnAddress0 < g_cheatInfo.ImageBase + g_cheatInfo.ImageSize)
		uReturnAddress0 -= g_cheatInfo.ImageBase;
	else
		uReturnAddress0 = 0x10000000;

	if (uReturnAddress1 >= g_cheatInfo.ImageBase && uReturnAddress1 < g_cheatInfo.ImageBase + g_cheatInfo.ImageSize)
		uReturnAddress1 -= g_cheatInfo.ImageBase;
	else
		uReturnAddress1 = 0x20000000;
		
	if (uReturnAddress2 >= g_cheatInfo.ImageBase && uReturnAddress2 < g_cheatInfo.ImageBase + g_cheatInfo.ImageSize)
		uReturnAddress2 -= g_cheatInfo.ImageBase;
	else
		uReturnAddress2 = 0x20000000;

	if (uReturnAddress3 >= g_cheatInfo.ImageBase && uReturnAddress3 < g_cheatInfo.ImageBase + g_cheatInfo.ImageSize)
		uReturnAddress3 -= g_cheatInfo.ImageBase;
	else
		uReturnAddress3 = 0x20000000;

	return LOWORD(uReturnAddress0 * uReturnAddress1) | (LOWORD(uReturnAddress2 * uReturnAddress3) << 16);
}

PBYTE ConfigSystem::ReadWholeFile(CONST CHAR* pFilePath, SIZE_T* pSize)
{
	FILE* pFile;
	PBYTE pData;

	pFile = fopen(pFilePath, "rb");
	if (pFile == NULL)
	{
		return NULL;
	}
	fseek(pFile, 0, SEEK_END);
	*pSize = ftell(pFile);
	pData = (PBYTE)malloc((*pSize + 1) * sizeof(char));

	rewind(pFile);
	*pSize = fread(pData, 1, *pSize, pFile);
	pData[*pSize] = '\0';
	fclose(pFile);
	return pData;
}
