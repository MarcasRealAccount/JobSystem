#include <Fibers/Fiber.h>
#include <JobSystem/Job.h>
#include <JobSystem/JobSystem.h>

#include <iostream>

int main()
{
	JobSystem::JobSystem jobSystem { 1 };

	auto job2Id = jobSystem.addJob(JobSystem::Job {
	    Fibers::Fiber {
	        Fibers::ECallingConvention::Native,
	        65536,
	        []()
	        {
		        std::cout << "Job 2 started!\n";

		        std::cout << "Job 2 finished!\n";
	        } } });

	jobSystem.addJob(JobSystem::Job {
	    Fibers::Fiber {
	        Fibers::ECallingConvention::Native,
	        65536,
	        [job2Id]()
	        {
		        std::cout << "Job 1 started!\n";

		        Fibers::WaitFor(*Fibers::GetFiber(job2Id));

		        std::cout << "Job 1 finished!\n";
	        } } });

	while (jobSystem.scheduleJobs())
		jobSystem.runLoop();

	std::cout << "Done\n";
}