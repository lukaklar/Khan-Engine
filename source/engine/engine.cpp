#include "engine/precomp.h"
#include "engine/engine.h"
#include "engine/mainloop.h"
#include "core/ecs/world.hpp"
#include "data/datamanager.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/shadermanager.hpp"
#include "system/commandlineoptions.h"
#include "system/input/inputmanager.hpp"
#include "system/splashscreen.hpp"
#include "system/window.hpp"

namespace Khan
{
	namespace Engine
	{
		static MainLoop s_MainLoop;
		static std::thread g_MainWindowThread;
		volatile bool g_Running;

		Renderer* g_Renderer;

		//	bool InitWindowAndGraphics()
		//	{
		//		// creates window and initializes graphics in parallel
		//		// after those two are initialized it creates the swapchain before exiting the function
		//		//g_MainWindowThread = std::thread(&W)
		//		return false;
		//	}
		//
		//	bool InitSystems()
		//	{
		//#ifndef KH_FINAL
		//		ParseCommandLineArguments();
		//#endif // KH_FINAL
		//
		//		bool success = true;
		//		std::future<bool> futures[1];
		//		futures[0] = std::async(std::launch::async, &InitWindowAndGraphics);
		//		// init physics, sound etc.
		//
		//		for (int32_t i = 0; i < 1; ++i)
		//		{
		//			futures[i].wait();
		//			success = success && futures[i].get();
		//		}
		//
		//		return success;
		//	}
		//
		//	// Thread 1 (splash screen thread)
		//	static bool InitAsync()
		//	{
		//		SplashScreen splashScreen;
		//
		//		std::future<bool> future = std::async(std::launch::async, &InitSystems);
		//
		//		std::future_status status;
		//		do {
		//			// handle splash screen messages
		//			splashScreen.Update();
		//			using namespace std::chrono_literals;
		//			status = future.wait_for(0ms);
		//		} while (status != std::future_status::ready);
		//
		//		return future.get();
		//	}
		//
		//	// Thread 0 (main thread)
		//	inline static bool Initialize()
		//	{
		//		return std::async(&InitAsync).get();
		//	}
		//
		//	inline static void Shutdown()
		//	{
		//
		//	}

		void Run()
		{
			//if (!Initialize()) return;
			InputManager::CreateSingleton();
			Window::Initialize("Khan Engine", 1280u, 720u);
			RenderBackend::Initialize(true);
			ShaderManager::CreateSingleton();
			g_Renderer = new Renderer();
			DataManager::CreateSingleton();
			World* world = DataManager::Get()->LoadWorldFromFile("sponza.obj");
			World::SetCurrentWorld(world);
			s_MainLoop.Run();
			World::SetCurrentWorld(nullptr);
			DataManager::DestroySingleton();
			delete g_Renderer;
			ShaderManager::DestroySingleton();
			RenderBackend::Shutdown();
			Window::Shutdown();
			InputManager::DestroySingleton();
			//Shutdown();
		}
	}
}