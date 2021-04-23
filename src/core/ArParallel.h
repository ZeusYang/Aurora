#ifndef ARPARALLEL_H
#define ARPARALLEL_H

#include "ArAurora.h"

#include <mutex>
#include <atomic>
#include <functional>
#include <condition_variable>

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
}

#endif