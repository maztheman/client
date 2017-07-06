#pragma once

namespace kms {

	class thread_base 
	{
	public:	

	protected:
		native_handle_type		m_handle;
	};


	struct true_type {};
	struct false_type {};

	struct _NoType
	{

	};

	template<class T>
	struct arg_trait
	{
		typedef false_type null_arg;
	};

	template<>
	struct arg_trait<_NoType>
	{
		typedef true_type null_arg;
	};

	template<class Function, class Arg1 = _NoType, class Arg2 = _NoType>
	class thread_t : public thread_base
	{
		Function				m_func;
		Arg1					m_arg1;
		Arg2					m_arg2;

		static DWORD WINAPI execute0(LPVOID param)
		{
			thread_t* const pThis = reinterpret_cast<thread_t* const>(param);
			pThis->m_func();
			return 0;
		}

		static DWORD WINAPI execute1(LPVOID param)
		{
			thread_t* const pThis = reinterpret_cast<thread_t* const>(param);
			pThis->m_func(pThis->m_arg1);
			return 0;
		}


		static DWORD WINAPI execute2(LPVOID param)
		{
			thread_t* const pThis = reinterpret_cast<thread_t* const>(param);
			pThis->m_func(pThis->m_arg1, pThis->m_arg2);
			return 0;
		}

		//no params
		void Create(true_type, true_type)
		{
			m_handle = ::CreateThread(NULL, 0, &execute0, this, 0, NULL);
		}

		//one param
		void Create(false_type, true_type)
		{
			m_handle = ::CreateThread(NULL, 0, &execute1, this, 0, NULL);
		}

		//two params
		void Create(false_type, false_type)
		{
			m_handle = ::CreateThread(NULL, 0, &execute2, this, 0, NULL);
		}
		
	public:
		thread_t(Function f)
			: m_func(f)
		{
			Create(arg_trait<Arg1>::null_arg(), arg_trait<Arg2>::null_arg());
		}

		thread_t(Function f, Arg1 arg1)
			: m_func(f)
			, m_arg1(arg1)
		{
			Create(arg_trait<Arg1>::null_arg(), arg_trait<Arg2>::null_arg());
		}

		thread_t(Function f, Arg1 arg1, Arg2 arg2)
			: m_func(f)
			, m_arg1(arg1)
			, m_arg2(arg2)
		{
			Create(arg_trait<Arg1>::null_arg(), arg_trait<Arg2>::null_arg());
		}
	};

	template<class Function>
	thread_base* make_thread(Function fn) {
		return new thread_t<Function>(fn);
	}
	
	template<class Function, class Arg1>
	thread_base* make_thread(Function fn, Arg1& arg1) {
		return new thread_t<Function, Arg1&>(fn, arg1);
	}

	template<class Function, class Arg1>
	thread_base* make_thread(Function fn, const Arg1& arg1) {
		return new thread_t<Function, const Arg1&>(fn, arg1);
	}


	template<class Function, class Arg1, class Arg2>
	thread_base* make_thread(Function fn, Arg1& arg1, Arg2& arg2) {
		return new thread_t<Function, Arg1&, Arg2&>(fn, arg1, arg2);
	}

	template<class Function, class Arg1, class Arg2>
	thread_base* make_thread(Function fn, const Arg1& arg1, const Arg2& arg2) {
		return new thread_t<Function, const Arg1&, const Arg2&>(fn, arg1, arg2);
	}
}
