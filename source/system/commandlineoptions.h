#pragma once

#include "core/primitivetypes.h"

namespace Khan
{
	struct CommandLineOptions
	{
		uint32 m_WindowWidth;
		uint32 m_WindowHeight;
		bool m_Fullscreen;
		bool m_VSyncEnabled;
		bool m_GPUDebugEnabled;
		//bool m_ImGuiEnabled;
		//bool m_PostFXEnabled;
	};

	extern CommandLineOptions g_CmdLineOptions;

	void ParseCommandLineArguments();
}