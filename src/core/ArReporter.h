#ifndef ARREPORTER_H
#define ARREPORTER_H

#include "ArAurora.h"

#include <atomic>
#include <chrono>
#include <thread>

namespace Aurora
{
	class AReporter 
	{
	public:
		// ProgressReporter Public Methods
		AReporter(int64_t totalWork, const std::string &title);
		~AReporter();
		
		void update(int64_t num = 1) 
		{
			if (num == 0) 
				return;
			m_workDone += num;
		}

		Float elapsedMS() const 
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			int64_t elapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
			return (Float)elapsedMS;
		}

		void done();

	private:
		// ProgressReporter Private Methods
		void printBar();

		// ProgressReporter Private Data
		const int64_t m_totalWork;
		const std::string m_title;
		const std::chrono::system_clock::time_point m_startTime;
		std::atomic<int64_t> m_workDone;
		std::atomic<bool> m_exitThread;
		std::thread m_updateThread;
	};
}

#endif