#include "main/precomp.h"
#include "engine/engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLineArgs, int nShowCmd)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(cmdLineArgs);
	UNREFERENCED_PARAMETER(nShowCmd);

#ifdef KH_FINAL
    const char szUniqueNamedMutex[] = "com_mycompany_apps_appname";
    HANDLE hHandle = CreateMutex(NULL, TRUE, szUniqueNamedMutex);
    if (ERROR_ALREADY_EXISTS == GetLastError())
    {
        MessageBox(NULL, "Only one instance of the game is allowed to run.", "Error!", MB_ICONERROR);
        return 1;
    }
#endif // KH_FINAL

    Khan::Engine::Run();

#ifdef KH_FINAL
    ReleaseMutex(hHandle);
    CloseHandle(hHandle);
#endif // KH_FINAL

	return 0;
}
