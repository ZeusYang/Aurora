#include "ArReporter.h"

#ifdef AURORA_WINDOWS_OS
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#endif 

#include <algorithm>
#include "ArParallel.h"

namespace Aurora
{
	static int terminalWidth();

	// ProgressReporter Method Definitions
	AReporter::AReporter(int64_t totalWork, const std::string &title)
		: m_totalWork(std::max((int64_t)1, totalWork)),
		m_title(title),
		m_startTime(std::chrono::system_clock::now()) 
	{
		m_workDone = 0;
		m_exitThread = false;
		// Launch thread to periodically update progress bar
		// We need to temporarily disable the profiler before launching
		// the update thread here, through the time the thread calls
		// ProfilerWorkerThreadInit(). Otherwise, there's a potential
		// deadlock if the profiler interrupt fires in the progress
		// reporter's thread and we try to access the thread-local
		// ProfilerState variable in the signal handler for the first
		// time. (Which in turn calls malloc, which isn't allowed in a
		// signal handler.)
		//SuspendProfiler();
		//std::shared_ptr<ABarrier> barrier = std::make_shared<ABarrier>(0);
		m_updateThread = std::thread([this/*, barrier*/]() 
		{
			//ProfilerWorkerThreadInit();
			//ProfilerState = 0;
			//barrier->wait();
			printBar();
		});
		// Wait for the thread to get past the ProfilerWorkerThreadInit()
		// call.
		//barrier->wait();
		//ResumeProfiler();
	}

	AReporter::~AReporter() 
	{
		m_workDone = m_totalWork;
		m_exitThread = true;
		m_updateThread.join();
		printf("\n");
	}

	void AReporter::printBar() 
	{
		int barLength = terminalWidth() - 28;
		int totalPlusses = std::max(2, barLength - (int)m_title.size());
		int plussesPrinted = 0;

		// Initialize progress string
		const int bufLen = m_title.size() + totalPlusses + 64;
		std::unique_ptr<char[]> buf(new char[bufLen]);
		snprintf(buf.get(), bufLen, "\r%s: [", m_title.c_str());
		char *curSpace = buf.get() + strlen(buf.get());
		char *s = curSpace;
		for (int i = 0; i < totalPlusses; ++i) 
			*s++ = ' ';
		*s++ = ']';
		*s++ = ' ';
		*s++ = '\0';
		fputs(buf.get(), stdout);
		fflush(stdout);

		std::chrono::milliseconds sleepDuration(250);
		int iterCount = 0;
		while (!m_exitThread) 
		{
			std::this_thread::sleep_for(sleepDuration);

			// Periodically increase sleepDuration to reduce overhead of
			// updates.
			++iterCount;
			if (iterCount == 10)
				// Up to 0.5s after ~2.5s elapsed
				sleepDuration *= 2;
			else if (iterCount == 70)
				// Up to 1s after an additional ~30s have elapsed.
				sleepDuration *= 2;
			else if (iterCount == 520)
				// After 15m, jump up to 5s intervals
				sleepDuration *= 5;

			Float percentDone = Float(m_workDone) / Float(m_totalWork);
			int plussesNeeded = std::round(totalPlusses * percentDone);
			while (plussesPrinted < plussesNeeded) 
			{
				*curSpace++ = '+';
				++plussesPrinted;
			}
			fputs(buf.get(), stdout);

			// Update elapsed time and estimated time to completion
			Float seconds = elapsedMS() / 1000.f;
			Float estRemaining = seconds / percentDone - seconds;
			if (percentDone == 1.f)
				printf(" (%.1fs)       ", seconds);
			else if (!std::isinf(estRemaining))
				printf(" (%.1fs|%.1fs)  ", seconds, std::max((Float)0., estRemaining));
			else
				printf(" (%.1fs|?s)  ", seconds);
			fflush(stdout);
		}
	}

	void AReporter::done() 
	{
		m_workDone = m_totalWork;
	}

	static int terminalWidth() 
	{
#ifdef AURORA_WINDOWS_OS
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (h == INVALID_HANDLE_VALUE || !h) 
		{
			fprintf(stderr, "GetStdHandle() call failed");
			return 80;
		}
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo = { 0 };
		GetConsoleScreenBufferInfo(h, &bufferInfo);
		return bufferInfo.dwSize.X;
#else
		struct winsize w;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0) 
		{
			// ENOTTY is fine and expected, e.g. if output is being piped to a file.
			if (errno != ENOTTY) 
			{
				static bool warned = false;
				if (!warned) 
				{
					warned = true;
					fprintf(stderr, "Error in ioctl() in TerminalWidth(): %d\n", errno);
				}
			}
			return 80;
		}
		return w.ws_col;
#endif
	}
}