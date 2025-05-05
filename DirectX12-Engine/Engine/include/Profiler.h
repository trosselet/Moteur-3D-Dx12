#ifndef PROFILER__H
#define PROFILER__H

#include <chrono>
#include <string>

struct Task
{
	std::string name;
	std::chrono::high_resolution_clock::time_point mStart;
};

namespace Engine
{
	class Profiler
	{
	public:
		Profiler();
		void NewTask(std::string name);
		void EndTask(float normalTime = 5.0f);

	private:
		Task m_task;
	};
}


#endif // !PROFILER__H