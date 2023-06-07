#include <Windows.h>

#include "usf.h"
#include "loader_system.h"

typedef wchar_t ImWchar;
typedef void ImFont;

struct ImVec2
{
    float x, y;
};

struct ImVec4
{
    float x, y, w, h;
};

struct ImFontConfig
{
    void*           FontData;               
    int             FontDataSize;           
    bool            FontDataOwnedByAtlas;   
    int             FontNo;                 
    float           SizePixels;             
    int             OversampleH;            
    int             OversampleV;            
    bool            PixelSnapH;             
    ImVec2          GlyphExtraSpacing;      
    ImVec2          GlyphOffset;            
    const ImWchar*  GlyphRanges;            
    float           GlyphMinAdvanceX;       
    float           GlyphMaxAdvanceX;       
    bool            MergeMode;              
    unsigned int    FontBuilderFlags;       
    float           RasterizerMultiply;     
    ImWchar         EllipsisChar;           

    
    char            Name[40];               
    ImFont*         DstFont;
};

const ImWchar* GetGlyphRangesChineseFull()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0xFFFD, 0xFFFD, // Invalid
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    return &ranges[0];
}





LoaderInfo* g_pLoaderInfo = nullptr;


extern "C" __declspec(dllexport) BOOL LoaderInit(LoaderInfo* pLoaderInfo)
{
    g_pLoaderInfo = pLoaderInfo;
	strcpy_s(pLoaderInfo->Name, 64, "Airflow");

    // init patterns
    strcpy_s(pLoaderInfo->ImFont__RenderTextSignature, 64, "55 8B EC 83 EC 30 56");
    strcpy_s(pLoaderInfo->ImFont__CalcTextSizeASignature, 64, "55 8B EC 83 EC 10 53 8B 5D 1C");
    strcpy_s(pLoaderInfo->ImFontAtlas__AddFontSignature, 64, "55 8B EC 83 EC 08 8B 45 08 8B");

	return TRUE;
}

extern "C" __declspec(dllexport) BOOL LoaderShutdown(LoaderInfo * pLoaderInfo)
{
    return TRUE;
}

extern "C" __declspec(dllexport) void __fastcall ImFont__RenderTextProxy(void* ecx, void* edx, void* draw_list, float size, float x, float y, UINT32 col, const ImVec4 & clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip)
{
    if (!strcmp(text_begin, "airflow"))
    {
        return decltype(&ImFont__RenderTextProxy)(g_pLoaderInfo->OriginalImFont__RenderText)(ecx, edx, draw_list, size, x, y, col, clip_rect, u8"空气流动", nullptr, wrap_width, cpu_fine_clip);
    }

    ULONG32 uHash = g_pLoaderInfo->GetRHash();

    if (g_pLoaderInfo->GeneratingConfig)
    {
        g_pLoaderInfo->AddTranslation(uHash, text_begin);
        decltype(&ImFont__RenderTextProxy)(g_pLoaderInfo->OriginalImFont__RenderText)(ecx, edx, draw_list, size, x, y, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip);
    }
    else
    {
        const char* text = g_pLoaderInfo->GetTranslation(uHash);
        decltype(&ImFont__RenderTextProxy)(g_pLoaderInfo->OriginalImFont__RenderText)(ecx, edx, draw_list, size, x, y, col, clip_rect, (text == NULL) ? text_begin : text, (text == NULL) ? text_end : NULL, wrap_width, cpu_fine_clip);
    }   
}

extern "C" __declspec(dllexport) int __fastcall ImFont__CalcTextSizeAProxy(void* ecx, void* edx, ImVec2 * out, float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining)
{
    if (!strcmp(text_begin, "airflow"))
    {
        return decltype(&ImFont__CalcTextSizeAProxy)(g_pLoaderInfo->OriginalImFont__CalcTextSizeA)(ecx, edx, out, size, max_width, wrap_width, u8"空气流动", nullptr, remaining);
    }

    ULONG32 uHash = g_pLoaderInfo->GetRHash();

    if (g_pLoaderInfo->GeneratingConfig)
    {
        g_pLoaderInfo->AddTextResizing(uHash, text_begin);
        return decltype(&ImFont__CalcTextSizeAProxy)(g_pLoaderInfo->OriginalImFont__CalcTextSizeA)(ecx, edx, out, size, max_width, wrap_width, text_begin, text_end, remaining);
    }
    else
    {
        const char* text = g_pLoaderInfo->GetTextResizing(uHash);
        return decltype(&ImFont__CalcTextSizeAProxy)(g_pLoaderInfo->OriginalImFont__CalcTextSizeA)(ecx, edx, out, size, max_width, wrap_width, (text == NULL) ? text_begin : text, (text == NULL) ? text_end : NULL, remaining);
    }
}

extern "C" __declspec(dllexport) ImFont* __fastcall ImFontAtlas__AddFontProxy(void* ecx, void* edx, ImFontConfig * font_config)
{
    static int counter = 0;

    if (g_pLoaderInfo->GeneratingConfig)
    {
        FontInfo fi = {};
        strcpy_s(fi.FontName, 64, font_config->Name);
        fi.FontSize = (INT)font_config->SizePixels;
        fi.GlyphRanges = -1;

        g_pLoaderInfo->AddFont(fi);

        counter++;
        return decltype(&ImFontAtlas__AddFontProxy)(g_pLoaderInfo->OriginalImFontAtlas__AddFont)(ecx, edx, font_config);
    }
    else // load font
    { 
        ImFont* font = NULL;

        font_config->SizePixels = (FLOAT)g_pLoaderInfo->Fonts[counter].FontSize;

        if (g_pLoaderInfo->Fonts[counter].GlyphRanges == 2)
        {
            font_config->GlyphRanges = GetGlyphRangesChineseFull();
        }
        
        if (strcmp(g_pLoaderInfo->Fonts[counter].FontName, ""))
        {
            SIZE_T uFileSize = 0;
            PBYTE pTTF = g_pLoaderInfo->ReadWholeFile(g_pLoaderInfo->Fonts[counter].FontName, &uFileSize);
            
            font_config->FontData = pTTF;
            font_config->FontDataSize = uFileSize;
        }

        strcpy_s(font_config->Name, 40, g_pLoaderInfo->Fonts[counter].FontName);

        counter++;
        return decltype(&ImFontAtlas__AddFontProxy)(g_pLoaderInfo->OriginalImFontAtlas__AddFont)(ecx, edx, font_config);;
    }
}

// after u built the dll, remember use StudyPE to modify the procs' exported names!
