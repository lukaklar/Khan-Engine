#pragma once

namespace Khan
{
	struct CommandLineOptions
	{
		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;
		bool m_Fullscreen;
		bool m_VSyncEnabled;
		bool m_GPUDebugEnabled;
		//bool m_ImGuiEnabled;
		//bool m_PostFXEnabled;
	};

	extern CommandLineOptions g_CmdLineOptions;

	void ParseCommandLineArguments();
}