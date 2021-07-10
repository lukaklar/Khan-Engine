#include "system/precomp.h"

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
		std::queue<std::function<void()>> m_JobQueue;
		std::mutex m_QueueMutex;
		std::condition_variable m_Condition;
	};
	
	class ThreadPool
	{
	public:
		ThreadPool(uint32_t threadCount);

		// Sets the number of threads to be allocated in this pool
		void SetThreadCount(uint32_t count);

		void AddJob(std::function<void()> job);

		// Wait until all threads have finished their work items
		void Wait();

	private:
		std::vector<std::unique_ptr<Thread>> m_Threads;
		uint32_t m_NextWorkerThread = 0;
	};
}