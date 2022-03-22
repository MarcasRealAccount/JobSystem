#pragma once

#include <Fibers/Fiber.h>

#include <set>

namespace JobSystem
{
	struct Job
	{
	public:
		Job(Fibers::Fiber&& fiber, const std::set<std::uint32_t>& threadIndices);
		Job(Fibers::Fiber&& fiber, std::set<std::uint32_t>&& threadIndices = {});
		Job(Job&& move) noexcept;
		Job& operator=(Job&& move) noexcept;

		bool resume();

		void setAssociatedJobSystem(class JobSystem* jobSystem);

		auto  getAssociatedJobSystem() const { return m_JobSystem; }
		auto& getFiber() const { return m_Fiber; }
		auto& getThreadIndices() const { return m_ThreadIndices; }

	private:
		class JobSystem* m_JobSystem;
		Fibers::Fiber    m_Fiber;

		std::set<std::uint32_t> m_ThreadIndices;
	};
} // namespace JobSystem