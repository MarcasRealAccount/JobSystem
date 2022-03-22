#include "JobSystem/Job.h"
#include "JobSystem/JobSystem.h"

#include <utility>

namespace JobSystem
{
	Job::Job(Fibers::Fiber&& fiber, const std::set<std::uint32_t>& threadIndices)
	    : m_Fiber(std::move(fiber)), m_ThreadIndices(threadIndices) {}

	Job::Job(Fibers::Fiber&& fiber, std::set<std::uint32_t>&& threadIndices)
	    : m_Fiber(std::move(fiber)), m_ThreadIndices(std::move(threadIndices)) {}

	Job::Job(Job&& move) noexcept
	    : m_Fiber(std::move(move.m_Fiber)) {}

	Job& Job::operator=(Job&& move) noexcept
	{
		m_Fiber = std::move(move.m_Fiber);
		return *this;
	}

	bool Job::resume()
	{
		m_Fiber.resume();
	}

	void Job::setAssociatedJobSystem(class JobSystem* jobSystem)
	{
		m_JobSystem = jobSystem;
	}
} // namespace JobSystem