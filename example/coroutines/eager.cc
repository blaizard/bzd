// eagerFutureWithComments.cpp

#if __has_include(<coroutine>)
#include <coroutine>
using namespace std;
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
using namespace std::experimental;
#else
static_assert(false, "Compiler is missing support for couroutines.");
#endif

#include <iostream>
#include <memory>

template <typename T>
struct MyFuture
{
	std::shared_ptr<T> value;
	MyFuture(std::shared_ptr<T> p) : value(p)
	{ // (3)
		std::cout << "    MyFuture::MyFuture" << '\n';
	}
	~MyFuture() { std::cout << "    MyFuture::~MyFuture" << '\n'; }
	T get()
	{
		std::cout << "    MyFuture::get" << '\n';
		return *value;
	}

	struct promise_type
	{													// (4)
		std::shared_ptr<T> ptr = std::make_shared<T>(); // (11)
		promise_type() { std::cout << "        promise_type::promise_type" << '\n'; }
		~promise_type() { std::cout << "        promise_type::~promise_type" << '\n'; }
		MyFuture<T> get_return_object()
		{
			std::cout << "        promise_type::get_return_object" << '\n';
			return ptr;
		}
		void return_value(T v)
		{
			std::cout << "        promise_type::return_value" << '\n';
			*ptr = v;
		}
		suspend_never initial_suspend()
		{ // (6)
			std::cout << "        promise_type::initial_suspend" << '\n';
			return {};
		}
		suspend_never final_suspend() noexcept
		{ // (7)
			std::cout << "        promise_type::final_suspend" << '\n';
			return {};
		}
		void return_void() {}
		void unhandled_exception() { std::exit(1); }
	}; // (5)
};

MyFuture<int> createFuture1()
{ // (2)
	std::cout << "createFuture 1" << '\n';
	co_return 1;
}

MyFuture<int> createFuture2()
{ // (2)
	std::cout << "createFuture 2" << '\n';
	co_await createFuture1();
	co_return 2;
}

int main()
{
	std::cout << '\n';

	//   auto fut1 = createFuture1();                                         // (1)
	auto fut2 = createFuture2(); // (1)
								 // auto res1 = fut1.get();                                              // (8)
	// std::cout << "res1: " << res1 << '\n';
	auto res2 = fut2.get(); // (8)
	std::cout << "res2: " << res2 << '\n';

	std::cout << '\n';

} // (12)
