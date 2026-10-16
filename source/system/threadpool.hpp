#include "system/precomp.h"
#include "core/containers.h"
#include "core/function.h"
#include "core/memory.h"
#include "core/primitivetypes.h"

namespace Khan
{
	class Thread
	{
	public:
		Thread();
		~Thread();

		void AddJob(std::function<void()> function);
		void Wait();

	private:
		void QueueLoop();

		bool m_Destroying = false;
		std::thread m_Worker;
		TQueue<TFunction<void()>> m_JobQueue;
		std::mutex m_QueueMutex;
		std::condition_variable m_Condition;
	};
	
	class ThreadPool
	{
	public:
		ThreadPool(uint32 threadCount);

		// Sets the number of threads to be allocated in this pool
		void SetThreadCount(uint32 count);

		void AddJob(TFunction<void()> job);

		// Wait until all threads have finished their work items
		void Wait();

	private:
		TArray<TUniquePtr<Thread>> m_Threads;
		uint32 m_NextWorkerThread = 0;
	};
}