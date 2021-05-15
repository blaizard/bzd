///////////////////////////////////////////////////////////////////////////
// Example source code for blog post:
// "C++ Coroutines: Understanding Symmetric-Transfer"
//
// Implementation of a naive 'task' coroutine type.

#include <coroutine>
#include <iostream>
#include <atomic>

using namespace std;

class task {
public:
  class promise_type {
  public:
    task get_return_object() noexcept {
      return task{coroutine_handle<promise_type>::from_promise(*this)};
    }

    suspend_always initial_suspend() noexcept {
      return {};
    }

    void return_void() noexcept {}

    void unhandled_exception() noexcept {
      std::terminate();
    }

    struct final_awaiter {
      bool await_ready() noexcept {
        return false;
      }
  /*    coroutine_handle<> await_suspend(coroutine_handle<promise_type> h) noexcept {
        return h.promise().continuation;
      }
*/
    void await_suspend(coroutine_handle<promise_type> h) noexcept {
      promise_type& promise = h.promise();
      if (promise.ready.exchange(true, memory_order_acq_rel)) {
        // The coroutine did not complete synchronously, resume it here.
        promise.continuation.resume();
      }
    }

      void await_resume() noexcept {}
    };

    final_awaiter final_suspend() noexcept {
      return {};
    }

    coroutine_handle<> continuation;
    atomic<bool> ready = false;
  };

  void start()
  {
    coro_.resume();
  }

  ~task() {
    if (coro_)
      coro_.destroy();
  }

  class awaiter {
  public:
    bool await_ready() noexcept {
      return false;
    }

/*
    coroutine_handle<> await_suspend(coroutine_handle<> continuation) noexcept {
      // Store the continuation in the task's promise so that the final_suspend()
      // knows to resume this coroutine when the task completes.
      coro_.promise().continuation = continuation;

      // Then we resume the task's coroutine, which is currently suspended
      // at the initial-suspend-point (ie. at the open curly brace).
      return coro_;
    }
*/
      bool await_suspend(coroutine_handle<> continuation) noexcept {
        coro_.promise().continuation = continuation;
        coro_.resume();
        return !coro_.promise().ready.exchange(true, std::memory_order_acq_rel);
      }

    void await_resume() noexcept {}
  private:
    friend task;
    explicit awaiter(coroutine_handle<promise_type> h) noexcept
    : coro_(h)
    {}

    coroutine_handle<promise_type> coro_;
  };

  awaiter operator co_await() && noexcept {
    return awaiter{coro_};
  }

private:
  explicit task(coroutine_handle<promise_type> h) noexcept
  : coro_(h)
  {}

  coroutine_handle<promise_type> coro_;
};


task completes_synchronously() {
    void* current = __builtin_frame_address(0);
    static void* base = current;
    (void)base;
    std::cout << base << " + " << std::hex << reinterpret_cast<std::intptr_t>(base) - reinterpret_cast<std::intptr_t>(current) << std::dec << std::endl;

    co_return;
}


task loop_synchronously(int count) {
    std::cout << "loop_synchronously(" << count << ")" << std::endl;
    for (int i = 0; i < count; ++i) {
        co_await completes_synchronously();
    }
    std::cout << "loop_synchronously(" << count << ") returning" << std::endl;
}

int main() {
    auto task = loop_synchronously(100);
    task.start();
    return 0;
}
