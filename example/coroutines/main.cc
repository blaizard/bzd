// infiniteDataStream.cpp

#if __has_include(<coroutine>)
#include <coroutine>
using namespace std;
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
using namespace std::experimental;
#else
static_assert(false, "No co_await support");
#endif

#include <memory>
#include <iostream>
#include<unistd.h>


#include "bzd.h"

template<typename T>
struct Generator {
    
    struct promise_type;
    using handle_type = std::experimental::coroutine_handle<promise_type>;
    
	void* operator new  ( std::size_t count );

    Generator(handle_type h): coro(h) {}                         // (3)
    handle_type coro;

    ~Generator() {
        if ( coro ) coro.destroy();
    }
    Generator(const Generator&) = delete;
    Generator& operator = (const Generator&) = delete;
    Generator(Generator&& oth) noexcept : coro(oth.coro) {
        oth.coro = nullptr;
    }
    Generator& operator = (Generator&& oth) noexcept {
        coro = oth.coro;
        oth.coro = nullptr;
        return *this;
    }
    T getValue() {
        return coro.promise().current_value;
    }
    bool next() {                                                // (5)
        coro.resume();
        return !coro.done();
    }
    struct promise_type {
        promise_type()  = default;                               // (1)
          
        ~promise_type() = default;
        
        auto initial_suspend() {                                 // (4)
            return std::experimental::suspend_always{};
        }
        auto final_suspend() {
            return std::experimental::suspend_always{};
        }
        auto get_return_object() {                               // (2)
            return Generator{handle_type::from_promise(*this)};
        }
        auto return_void() {
            return std::experimental::suspend_never{};
        }
      
        auto yield_value(const T value) {                        // (6) 
            current_value = value;
            return std::experimental::suspend_always{};
        }
        void unhandled_exception() {
            std::exit(1);
        }
        T current_value;
    };

};

Generator<int> getNext(int start = 0, int step = 1) noexcept {
    auto value = start;
    for (int i = 0;; ++i){
        co_yield value;
        value += step;
    }
}

// https://kirit.com/How%20C%2B%2B%20coroutines%20work/My%20first%20coroutine

template<typename T>
struct Async {

    struct promise_type;
    using handle_type = std::experimental::coroutine_handle<promise_type>;
    handle_type coro;

    Async(handle_type h)
    : coro(h) {
        std::cout << "Created an Async object" << std::endl;
    }

    Async(const Async &) = delete;
    Async(Async &&s)
    : coro(s.coro) {
        std::cout << "Sync moved leaving behind a husk" << std::endl;
        s.coro = nullptr;
    }

    Async &operator = (const Async &) = delete;
    Async &operator = (Async &&s) {
        coro = s.coro;
        s.coro = nullptr;
        return *this;
    }

    ~Async() {
        std::cout << "Async gone" << std::endl;
        if ( coro ) coro.destroy();
    }
    T&& get() {
        std::cout << "We got asked for the return value..." << std::endl;
        if (!this->coro.done()) {
            this->coro.resume();
        }
        return bzd::move(coro.promise().value);
    }

    auto operator co_await() {
        struct awaitable_type {
            handle_type coro;
            bool await_ready() {
                const auto ready = coro.done();
                std::cout << "Await " << (ready ? "is ready" : "isn't ready") << std::endl;
                return coro.done();
            }
            auto await_suspend(std::experimental::coroutine_handle<> awaiting) {
                std::cout << "Got to resume the lazy" << std::endl;
                coro.resume();
                std::cout << "Got to resume the awaiter" << std::endl;
                return awaiting;
            }
            auto await_resume() {
                const auto r = coro.promise().value;
                std::cout << "Await value is returned: " << r << std::endl;
                return r;
            }
        };
        return awaitable_type{coro};
    }

    struct promise_type {
        T value;
    
        promise_type() {
            std::cout << "Promise created" << std::endl;
        }
        ~promise_type() {
            std::cout << "Promise died" << std::endl;
        }
        auto get_return_object() {
            std::cout << "Send back an Async" << std::endl;
            return Async<T>{handle_type::from_promise(*this)};
        }
        auto initial_suspend() {
            std::cout << "Started the coroutine, don't stop now!" << std::endl;
            return std::experimental::suspend_always{};
        }
        auto return_value(T&& v) {
            std::cout << "Got an answer of " << v << std::endl;
            value = bzd::move(v);
            return std::experimental::suspend_never{};
        }
        auto final_suspend() {
            std::cout << "Finished the coro" << std::endl;
            return std::experimental::suspend_always{};
        }
        void unhandled_exception() {
            std::exit(1);
        }
    };
};

template <class T, class Fct>
auto promise(Fct fct) {
    struct Awaitable
    {
        Fct fct;
        T value{};

        bool await_ready() {
            std::cout << "Is it ready?" << std::endl;
            return false;
        }

        auto await_suspend(std::experimental::coroutine_handle<> awaiting) {
            std::cout << "Got to resume the await" << std::endl;
            fct([this, awaiting](T value) mutable {
                std::cout << "resolve callback" << std::endl;
                this->value = value;
                awaiting.resume();
            });
            //return awaiting;
            // schedule here
        }

        auto await_resume() {
            std::cout << "await_resume" << std::endl;
            return value;
        }
    };

    return Awaitable{fct};
}

/*
auto my_number() {
    std::cout << "Thinking deep thoughts..." << std::endl;
    return promise<int>([](auto resolve) {
        resolve(23);
    });
}
*/
Async<int> my_number() {
    std::cout << "Thinking deep thoughts..." << std::endl;
    co_return 34;
}

Async<int> answer() {
    std::cout << "Thinking deep thoghts..." << std::endl;

    auto a = co_await my_number();

    co_return bzd::move(a);
}

int main() {
    auto a = answer();
    std::cout << "Got a coroutine, let's get a value" << std::endl;
    auto v = a.get();
    std::cout << "And the coroutine value is: " << v << std::endl;
    return 0;
}

/*
int main() {
    std::cout << std::endl;
  
    std::cout << "getNext():";
    auto gen = getNext();
    for (int i = 0; i <= 10; ++i) {
        gen.next();
        std::cout << " " << gen.getValue();                      // (7)
    }
    
    std::cout << "\n\n";
    
    std::cout << "getNext(100, -10):";
    auto gen2 = getNext(100, -10);
    for (int i = 0; i <= 20; ++i) {
        gen2.next();
        std::cout << " " << gen2.getValue();
    }
    
    std::cout << std::endl;
    
}*/