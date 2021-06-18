#include "system/precomp.h"
#include "system/commandlineoptions.h"
#include <thread>

namespace Khan
{
    CommandLineOptions g_CmdLineOptions;

    void ParseCommandLineArguments()
    {
        std::ifstream cmdArgFile("khan.ini");

        while (cmdArgFile.good())
        {
            char c;
            cmdArgFile.read(&c, 1);
        }

        int argc;
        wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10s);
        for (size_t i = 0; i < argc; ++i)
        {
            /* if (::wcscmp(argv[i], L"-w") == 0 || ::wcscmp(argv[i], L"--width") == 0)
             {
                 g_ClientWidth = ::wcstol(argv[++i], nullptr, 10);
             }
             if (::wcscmp(argv[i], L"-h") == 0 || ::wcscmp(argv[i], L"--height") == 0)
             {
                 g_ClientHeight = ::wcstol(argv[++i], nullptr, 10);
             }
             if (::wcscmp(argv[i], L"-warp") == 0 || ::wcscmp(argv[i], L"--warp") == 0)
             {
                 g_UseWarp = true;
             }*/
        }

        // Free memory allocated by CommandLineToArgvW
        ::LocalFree(argv);
    }
}