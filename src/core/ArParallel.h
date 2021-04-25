#ifndef ARPARALLEL_H
#define ARPARALLEL_H

#include "ArAurora.h"

#include <mutex>
#include <atomic>
#include <functional>
#include <condition_variable>

#include "oneapi/tbb/spin_mutex.h"
#include "oneapi/tbb/parallel_for.h"

namespace Aurora
{
	class AAtomicFloat 
	{
	public:

		explicit AAtomicFloat(Float v = 0) { bits = floatToBits(v); }

		operator Float() const { return bitsToFloat(bits); }
		
		Float operator=(Float v) 
		{
			bits = floatToBits(v);
			return v;
		}

		void add(Float v) 
		{
#ifdef AURORA_DOUBLE_AS_FLOAT
			uint64_t oldBits = bits, newBits;
#else
			uint32_t oldBits = bits, newBits;
#endif
			do 
			{
				newBits = floatToBits(bitsToFloat(oldBits) + v);
			} while (!bits.compare_exchange_weak(oldBits, newBits));
		}

	private:
		// AtomicFloat Private Data
#ifdef AURORA_DOUBLE_AS_FLOAT
		std::atomic<uint64_t> bits;
#else
		std::atomic<uint32_t> bits;
#endif
	};

	// Simple one-use barrier; ensures that multiple threads all reach a
	// particular point of execution before allowing any of them to proceed
	// past it.
	//
	// Note: this should be heap allocated and managed with a shared_ptr, where
	// all threads that use it are passed the shared_ptr. This ensures that
	// memory for the Barrier won't be freed until all threads have
	// successfully cleared it.
	class ABarrier 
	{
	public:
		ABarrier(int count) : m_count(count) { }
		~ABarrier() { }
		void wait();

	private:
		std::mutex m_mutex;
		std::condition_variable m_cv;
		int m_count;
	};

	using AFilmMutexType = tbb::spin_mutex;

	//Execution policy tag.
	enum class AExecutionPolicy { ASERIAL, APARALLEL };

	//parallel for loop with automic chunking
	template <typename Function>
	void parallelFor(size_t beginIndex, size_t endIndex,
		const Function& function, AExecutionPolicy policy = AExecutionPolicy::APARALLEL);

	//parallel for loop with manual chunking
	template <typename Function>
	void parallelFor(size_t beginIndex, size_t endIndex, size_t grainSize,
		const Function& function, AExecutionPolicy policy = AExecutionPolicy::APARALLEL);

	template <typename Function>
	void parallelFor(size_t start, size_t end, const Function& func, AExecutionPolicy policy)
	{
		if (start > end)
			return;
		if (policy == AExecutionPolicy::APARALLEL)
		{
			tbb::parallel_for(start, end, func);
		}
		else
		{
			for (auto i = start; i < end; ++i)
				func(i);
		}
	}

	template <typename Function>
	void parallelFor(size_t start, size_t end, size_t grainSize, const Function& func, AExecutionPolicy policy)
	{
		if (start > end)
			return;
		if (policy == AExecutionPolicy::APARALLEL)
		{
			tbb::parallel_for(tbb::blocked_range<size_t>(start, end, grainSize), 
				func, tbb::simple_partitioner());
		}
		else
		{
			tbb::blocked_range<size_t> range(start, end, grainSize);
			func(range);
		}
	}
}

#endif