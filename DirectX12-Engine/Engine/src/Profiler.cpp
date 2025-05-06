#include "Profiler.h"

#define PROFILER

Engine::Profiler::Profiler() : m_task(Task())
{
}

void Engine::Profiler::NewTask(std::string name)
{
#ifdef PROFILER
	m_task.name = name;
	m_task.mStart = std::chrono::high_resolution_clock::now();

#endif // PROFILER

}

void Engine::Profiler::EndTask(float normalTime)
{
#ifdef PROFILER
	auto endTime = std::chrono::high_resolution_clock::now();
	auto durationNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - m_task.mStart).count();
	double durationMs = durationNs / 1e6;

	const char* colorCode = (durationMs < normalTime) ? "\033[32m" : "\033[31m";
	const char* resetCode = "\033[0m";

	printf("[PROFILER] %sYour task : %s took %lld ns | %.3f ms%s\n",
		colorCode, m_task.name.c_str(), durationNs, durationMs, resetCode);
#endif // PROFILER
}
