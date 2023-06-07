#include <Windows.h>

#include "usf.h"
#include "loader_system.h"

typedef wchar_t ImWchar;
typedef void ImFont;

struct ImVec2
{
    float x, y;
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

extern "C" __declspec(dllexport) ImFont* __fastcall ImFontAtlas__AddFontProxy(void* ecx, void* edx, ImFontConfig * font_config)
{
    DebugPrint("[USF] FontCreation:\n");
    DebugPrint("\tName: \"%s\"\n", font_config->Name);
    DebugPrint("\tFontData: %p\n", font_config->FontData);
    DebugPrint("\tFontDataSize: %X\n", font_config->FontDataSize);

    ImFont* font = decltype(&ImFontAtlas__AddFontProxy)(g_pLoaderInfo->OriginalImFontAtlas__AddFont)(ecx, edx, font_config);
    return font;
}
