#pragma once

#include "Job.h"

#include <shared_mutex>
#include <thread>
#include <vector>

namespace JobSystem
{
	class JobSystem
	{
	public:
		JobSystem(std::size_t maxThreadCount);
		JobSystem(JobSystem&& move) noexcept;
		JobSystem& operator=(JobSystem&& move) noexcept;

		std::uint64_t addJob(Job&& job);

		bool scheduleJobs();
		void runLoop();

		auto isAlive() const { return m_Alive; }

	private:
		void runScheduledJobs(std::uint32_t threadIndex);
		void threadFunc(std::uint32_t threadIndex);

	private:
		bool m_Alive;

		std::vector<std::thread>       m_Threads;
		std::vector<bool>              m_ThreadsDone;
		std::size_t                    m_TotalThreadsDone;
		std::vector<std::vector<Job*>> m_ScheduledJobs;
		std::vector<Job>               m_Jobs;

		std::shared_mutex m_Mutex;
	};
} // namespace JobSystem