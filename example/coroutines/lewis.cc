///////////////////////////////////////////////////////////////////////////
// Example source code for blog post:
// "C++ Coroutines: Understanding Symmetric-Transfer"
//
// Implementation of a naive 'task' coroutine type.

#include <experimental/coroutine>
#include <iostream>

using namespace std::experimental;

class task
{
public:
	class promise_type
	{
	public:
		task get_return_object() noexcept { return task{coroutine_handle<promise_type>::from_promise(*this)}; }

		suspend_always initial_suspend() noexcept { return {}; }

		void return_void() noexcept {}

		void unhandled_exception() noexcept { std::terminate(); }

		struct final_awaiter
		{
			bool await_ready() noexcept { return false; }
			coroutine_handle<> await_suspend(coroutine_handle<promise_type> h) noexcept { return h.promise().caller; }
			void await_resume() noexcept {}
		};

		final_awaiter final_suspend() noexcept { return {}; }

		coroutine_handle<> caller;
	};

	task(task&& t) noexcept : coro_(std::exchange(t.coro_, {})) {}

	~task()
	{
		if (coro_) coro_.destroy();
	}

	class awaiter
	{
	public:
		bool await_ready() noexcept { return false; }

		coroutine_handle<> await_suspend(coroutine_handle<> caller) noexcept
		{
			// Store the continuation in the task's promise so that the final_suspend()
			// knows to resume this coroutine when the task completes.
			coro_.promise().caller = caller;

			// Then we resume the task's coroutine, which is currently suspended
			// at the initial-suspend-point (ie. at the open curly brace).
			return coro_;
		}

		void await_resume() noexcept {}

	private:
		friend task;
		explicit awaiter(coroutine_handle<promise_type> h) noexcept : coro_(h) {}

		coroutine_handle<promise_type> coro_;
	};

	awaiter operator co_await() && noexcept { return awaiter{coro_}; }

private:
	explicit task(coroutine_handle<promise_type> h) noexcept : coro_(h) {}

	coroutine_handle<promise_type> coro_;
};

struct sync_wait_task
{
	struct promise_type
	{
		sync_wait_task get_return_object() noexcept { return sync_wait_task{coroutine_handle<promise_type>::from_promise(*this)}; }

		suspend_never initial_suspend() noexcept { return {}; }

		suspend_always final_suspend() noexcept { return {}; }

		void return_void() noexcept {}

		void unhandled_exception() noexcept { std::terminate(); }
	};

	coroutine_handle<promise_type> coro_;

	explicit sync_wait_task(coroutine_handle<promise_type> h) noexcept : coro_(h) {}

	sync_wait_task(sync_wait_task&& t) noexcept : coro_(t.coro_) { t.coro_ = {}; }

	~sync_wait_task()
	{
		if (coro_)
		{
			coro_.destroy();
		}
	}

	static sync_wait_task start(task&& t) { co_await std::move(t); }

	bool done() { return coro_.done(); }
};

struct manual_executor
{
	struct schedule_op
	{
		manual_executor& executor_;
		schedule_op* next_ = nullptr;
		coroutine_handle<> continuation_;

		schedule_op(manual_executor& executor) : executor_(executor) {}

		bool await_ready() noexcept { return false; }

		void await_suspend(coroutine_handle<> continuation) noexcept
		{
			continuation_ = continuation;
			next_ = executor_.head_;
			executor_.head_ = this;
		}

		void await_resume() noexcept {}
	};

	schedule_op* head_ = nullptr;

	schedule_op schedule() noexcept { return schedule_op{*this}; }

	void drain()
	{
		while (head_ != nullptr)
		{
			auto* item = head_;
			head_ = item->next_;
			item->continuation_.resume();
		}
	}

	void sync_wait(task&& t)
	{
		auto t2 = sync_wait_task::start(std::move(t));
		while (!t2.done())
		{
			// std::cout << "sync_wait loop" << std::endl;
			drain();
		}
	}
};

task completes_synchronously()
{
	co_return;
}

task loop_synchronously2(int count)
{
	std::cout << "loop_synchronously2(" << count << ")" << std::endl;
	for (int i = 0; i < count; ++i)
	{
		co_await completes_synchronously();
	}
	std::cout << "loop_synchronously2(" << count << ") returning" << std::endl;
}

task loop_synchronously(int count)
{
	std::cout << "loop_synchronously(" << count << ")" << std::endl;
	for (int i = 0; i < count; ++i)
	{
		co_await loop_synchronously2(i);
	}
	std::cout << "loop_synchronously(" << count << ") returning" << std::endl;
}

task ret()
{
	co_return;
}

task count(int id, int n)
{
	std::cout << id << ": count(" << n << ")" << std::endl;
	for (int i = 0; i < n; ++i)
	{
		std::cout << id << ": " << i << std::endl;
		co_await ret();
		co_await suspend_always{};
	}
}

task nestedCount(int id, int n)
{
	std::cout << id << ": nested start" << std::endl;
	co_await count(id, n);
	std::cout << id << ": nested end" << std::endl;
}

int main()
{
	manual_executor ex;
	ex.sync_wait(loop_synchronously(10));
	ex.sync_wait(nestedCount(1, 10));

	return 0;
}
