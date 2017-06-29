#pragma once
#include <deque>

//full of platform specific stuff
namespace kms {

template<class T>
	class concurrent_queue
	{
		CRITICAL_SECTION	_cs;
		std::deque<T>		_queue;
		HANDLE				_sema;
	public:
		concurrent_queue()
			: _sema(NULL)
		{
			InitializeCriticalSection(&_cs);
			_sema = ::CreateSemaphore(NULL, 0, MAXINT, NULL);
		}

		~concurrent_queue()
		{
			DeleteCriticalSection(&_cs);
			CloseHandle(_sema);
		}

		void push(T val)
		{
			EnterCriticalSection(&_cs);
			_queue.push_front(val);
			LeaveCriticalSection(&_cs);
			ReleaseSemaphore(_sema, 1, NULL);
		}

		void pop(T* val)
		{
			if (WAIT_OBJECT_0 == ::WaitForSingleObject(_sema, INFINITE)) {
				EnterCriticalSection(&_cs);
				*val = _queue.back();
				_queue.pop_back();
				LeaveCriticalSection(&_cs);
			}
		}

		bool try_pop(T* val, long timeout = 50)
		{
			if (WAIT_OBJECT_0 == ::WaitForSingleObject(_sema, timeout)) {
				EnterCriticalSection(&_cs);
				*val = _queue.back();
				_queue.pop_back();
				LeaveCriticalSection(&_cs);
				return true;
			}
			return false;
		}
	};

}