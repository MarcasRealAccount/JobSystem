#include "JobSystem/JobSystem.h"

namespace JobSystem
{
	JobSystem::JobSystem(std::size_t maxThreadCount)
	    : m_Alive(true), m_TotalThreadsDone(0)
	{
		maxThreadCount = maxThreadCount > 1 ? maxThreadCount : 1;
		m_Threads.resize(maxThreadCount - 1);
		for (std::size_t thread = 0; thread < maxThreadCount - 1; ++thread)
			m_Threads[thread] = std::thread(&JobSystem::threadFunc, this, thread + 1);
	}

	JobSystem::JobSystem(JobSystem&& move) noexcept
	    : m_Alive(move.m_Alive),
	      m_Threads(std::move(move.m_Threads)),
	      m_ThreadsDone(std::move(move.m_ThreadsDone)),
	      m_TotalThreadsDone(move.m_TotalThreadsDone),
	      m_ScheduledJobs(std::move(move.m_ScheduledJobs)),
	      m_Jobs(std::move(move.m_Jobs))
	{
		move.m_Alive = false;
	}

	JobSystem& JobSystem::operator=(JobSystem&& move) noexcept
	{
		m_Alive            = move.m_Alive;
		m_Threads          = std::move(move.m_Threads);
		m_ThreadsDone      = std::move(move.m_ThreadsDone);
		m_TotalThreadsDone = move.m_TotalThreadsDone;
		m_ScheduledJobs    = std::move(move.m_ScheduledJobs);
		m_Jobs             = std::move(move.m_Jobs);
		move.m_Alive       = false;
		return *this;
	}

	std::uint64_t JobSystem::addJob(Job&& job)
	{
		std::unique_lock lock { m_Mutex };
		job.setAssociatedJobSystem(this);
		return m_Jobs.emplace_back(std::move(job)).getFiber().getID();
	}

	bool JobSystem::scheduleJobs()
	{
		if (!m_Alive)
			return false;

		std::unique_lock lock { m_Mutex };
		for (auto& threadJobs : m_ScheduledJobs)
			threadJobs.clear();

		auto end = std::remove_if(
		    m_Jobs.begin(), m_Jobs.end(),
		    [](Job& job)
		    {
			    return job.getFiber().isFinished();
		    });
		m_Jobs.erase(end, m_Jobs.end());
		if (m_Jobs.empty())
			return false;

		for (auto& job : m_Jobs)
		{
			std::size_t bestThread = 0;
			if (job.getThreadIndices().empty())
			{
				for (std::size_t thread = 0; thread < m_Threads.size(); ++thread)
					if (m_ScheduledJobs[thread].size() < m_ScheduledJobs[bestThread].size())
						bestThread = thread;
			}
			else
			{
				for (auto thread : job.getThreadIndices())
					if (thread < m_Threads.size())
						if (m_ScheduledJobs[thread].size() < m_ScheduledJobs[bestThread].size())
							bestThread = thread;
			}
			m_ScheduledJobs[bestThread].push_back(&job);
		}

		return true;
	}

	void JobSystem::runLoop()
	{
		std::fill(m_ThreadsDone.begin(), m_ThreadsDone.end(), false);

		runScheduledJobs(0);

		// Wait until all other threads are done
		while (true)
		{
			{
				std::shared_lock lock { m_Mutex };
				if (m_TotalThreadsDone > m_Threads.size())
					break;
			}

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
		}
	}

	void JobSystem::runScheduledJobs(std::uint32_t threadIndex)
	{
		auto scheduledJobs = m_ScheduledJobs[threadIndex];
		for (auto job : scheduledJobs)
			job->resume();

		std::unique_lock lock { m_Mutex };
		m_ThreadsDone[threadIndex] = true;
		++m_TotalThreadsDone;
	}

	void JobSystem::threadFunc(std::uint32_t threadIndex)
	{
		while (isAlive())
		{
			// Wait until job system loops.
			while (true)
			{
				{
					std::shared_lock lock { m_Mutex };
					if (threadIndex >= m_ThreadsDone.size() || !m_ThreadsDone[threadIndex])
						break;
				}

				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1ms);
			}

			runScheduledJobs(threadIndex);
		}
	}
} // namespace JobSystem