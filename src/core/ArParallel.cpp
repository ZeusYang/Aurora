#include "ArParallel.h"

namespace Aurora
{
	void ABarrier::wait() 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (--m_count == 0)
		{
			// This is the last thread to reach the barrier; wake up all of the
			// other ones before exiting.
			m_cv.notify_all();
		}
		else
		{
			// Otherwise there are still threads that haven't reached it. Give
			// up the lock and wait to be notified.
			m_cv.wait(lock, [this] { return m_count == 0; });
		}
	}
}