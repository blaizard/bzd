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
#include <atomic>

using namespace std::chrono_literals;

static std::deque<coroutine_handle<>> queue;
static std::atomic<bool> isEnd{false};

void register_coro(coroutine_handle<>& handle)
{
	//std::cout << "Registering '" << str << "': " << handle.address() << std::endl;
	queue.push_back(handle);
}

class RAIIThread
{
public:
  template <class T>
  RAIIThread(T&& fct) : t_{fct} {}

  ~RAIIThread()
  {
    t_.join();
  }

private:
  std::thread t_;
};

void register_delay_coro(coroutine_handle<>& handle)
{
  static std::deque<coroutine_handle<>> tempQueue;
  static RAIIThread thr{[]() {
    while (!isEnd)
    {
      std::this_thread::sleep_for(20ms);
      if (!tempQueue.empty())
      {
        auto coro = tempQueue.front();
        tempQueue.pop_front();
        register_coro(coro);
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
    std::this_thread::sleep_for(10ms);
  }
	auto coro = queue.front();
	//std::cout << "Executing " << coro.address() << ", count: " << queue.size() << std::endl;
	queue.pop_front();
  return std::move(coro);
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
    register_delay_coro(handle);

		return get_next();
	}

	auto await_resume()
	{
		// std::cout << "Await value is returned: " << r << std::endl;
		return 23;
	}

  coroutine_handle<Promise> handle;
};

void promiseAnd(coroutine_handle<> a, coroutine_handle<> b)
{
  register_coro(a);
  register_coro(b);
  while (!a.done() || !b.done())
  {
    auto coro = get_next();
    coro.resume();
    std::this_thread::sleep_for(10ms);
  }
  std::cout << "end promiseAnd" << std::endl;
}

void promiseOr(coroutine_handle<> a, coroutine_handle<> b)
{
  register_coro(a);
  register_coro(b);
  while (!a.done() && !b.done())
  {
    auto coro = get_next();
    coro.resume();
    std::this_thread::sleep_for(10ms);
  }
  // Remove a and b from list
  // note, sub coroutines also needs to be deleted, like the ones in
  // register_delay_coro for example.
  std::cout << "end promiseOr" << std::endl;
}

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
    if (n-i > 5)
       sum += co_await count(id * 10, n - 1);
    sum += co_await nop();
  }

	co_return sum;
}

// https://lewissbaker.github.io/2020/05/11/understanding_symmetric_transfer
// Another https://stackoverflow.com/questions/41413489/c1z-coroutine-threading-context-and-coroutine-scheduling
int main()
{
	auto mycoro1 = count(1, 3);
	auto mycoro2 = count(2, 6);

  promiseAnd(mycoro1.handle, mycoro2.handle);

  isEnd.store(true);
	return 0;
}
