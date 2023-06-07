#include "usf.h"
#include "loader_system.h"

HMODULE g_hUSFModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
        AllocConsole();
        SetConsoleTitle(TEXT("USF"));
        freopen("CONOUT$", "w", stdout);
#endif
        g_hUSFModule = hModule;
        AddVectoredExceptionHandler(0, Cheat::ExceptionHandler);
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)USF::InitFramework, NULL, 0, NULL);
        return TRUE;

    case DLL_PROCESS_DETACH:
        if (ConfigSystem::GeneratingConfig())
        {
            if (!ConfigSystem::OutputConfig())
            {
                
            }
        }

        if (!LoaderSystem::ShutdownLoader())
        {
            ReportErrorAndExit("A fatal error occured while shuting down the loader!", "USF");
        }

        if (!USF::ShutdownSurfaceHooks())
        {
            ReportErrorAndExit("A fatal error occured while shuting down surface hooks!", "USF");
        }
                        
        return TRUE;
    }
    return TRUE;
}

