#if __has_include(<coroutine>)
#include <coroutine>
using namespace std;
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
using namespace std::experimental;
#else
static_assert(false, "Compiler is missing support for couroutines.");
#endif

#include <deque>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

static std::deque<coroutine_handle<>> queue;

void register_coro(std::string, coroutine_handle<>& handle)
{
	//std::cout << "Registering '" << str << "': " << handle.address() << std::endl;
	queue.push_back(handle);
}

void register_delay_coro(std::string, coroutine_handle<>& handle)
{
  static std::deque<coroutine_handle<>> tempQueue;
  static std::thread thr{[]() {
    while (true)
    {
      std::this_thread::sleep_for(20ms);
      if (!tempQueue.empty())
      {
        auto coro = tempQueue.front();
        tempQueue.pop_front();
        register_coro("nested", coro);
      }
    }
  }};

  tempQueue.push_back(handle);
}

coroutine_handle<>&& get_next()
{
  // Wait until there is something in the queue
  while (queue.empty())
  {
    //std::cout << "wait get_next()" << std::endl;
    std::this_thread::sleep_for(500ms);
  }
	auto coro = queue.front();
	//std::cout << "Executing " << coro.address() << ", count: " << queue.size() << std::endl;
	queue.pop_front();
  return std::move(coro);
}

void scheduler(void* /*address*/ = nullptr)
{
  while (true)
  {
    while (!queue.empty())
    {
      auto coro = get_next();
      coro.resume();
    }
    //std::cout << "wait scheduler()" << std::endl;
    std::this_thread::sleep_for(500ms);
  }
  //std::cout << "end!" << std::endl;
}

struct Async
{
	struct final_awaiter;
  struct Promise
	{
    Async get_return_object()
		{
      return coroutine_handle<Promise>::from_promise(*this);
    }
    suspend_always initial_suspend() { return {}; }

    struct final_awaiter {
      bool await_ready() noexcept {
        return false;
      }
      coroutine_handle<> await_suspend(coroutine_handle<Promise> h) noexcept {
        if (h.promise().caller)
        {
          return h.promise().caller;
        }
        return noop_coroutine();
      }

      void await_resume() noexcept {}
    };

    final_awaiter final_suspend() { return {}; }
    void return_value(int )
    {
      //std::cout << "got " << value << "\n";
    }
    void unhandled_exception() {}
		coroutine_handle<> caller;
  };

  using promise_type = Promise;
  Async(coroutine_handle<Promise> h) : handle(h)
  {
    //std::cout << "Async " << handle.address() << std::endl;
  }

  ~Async()
  {
    //std::cout << "~Async " << handle.address() << std::endl;
    if (handle)
    {
      handle.destroy();
    }
  }

	bool await_ready()
	{
		return false; //handle.done();
	}

	auto await_suspend(coroutine_handle<> caller)
	{
		//std::cout << "await_suspend caller " << caller.address() << ", handle " << handle.address() << std::endl;
		handle.promise().caller = caller;

    // Must be valid as there is at least a single entry
    // The coroutine should be added to the list when it resolves
    register_delay_coro("nested", handle);

		return get_next();
	}

	auto await_resume()
	{
		// std::cout << "Await value is returned: " << r << std::endl;
		return 23;
	}

  coroutine_handle<Promise> handle;
};

Async nop()
{
	co_return 42;
}

Async count(int id, int n)
{
  int sum = 0;
	//std::cout << id << ": count(" << n << ")" << std::endl;
	for (int i=0; i<n; ++i)
	{
		std::cout << id << ": " << i << std::endl;
    if (n > 5)
       sum += co_await count(id * 10, n - 1);
    sum += co_await nop();
  }

	co_return sum;
}

// https://lewissbaker.github.io/2020/05/11/understanding_symmetric_transfer
// Another https://stackoverflow.com/questions/41413489/c1z-coroutine-threading-context-and-coroutine-scheduling
int main()
{
	auto mycoro1 = count(1, 10);
	auto mycoro2 = count(2, 8);

	register_coro("mycoro1", mycoro1.handle);
	register_coro("mycoro2", mycoro2.handle);

	scheduler();

	return 0;
}
