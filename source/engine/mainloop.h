#pragma once
#include "system/timer.hpp"

namespace Khan
{
	class MainLoop
	{
	public:
		void Run();

	private:
		Timer m_UpdateTimer;
		bool m_Running;
	};
}