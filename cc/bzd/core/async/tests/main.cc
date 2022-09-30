#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"

bzd::Generator<bzd::Size> generator(bzd::Size count)
{
	for (bzd::Size i = 0; i < count; ++i)
	{
		co_yield i;
	}
}

namespace bzd::platform {
void panic()
{
	std::cout << "panic!" << std::endl;
	std::exit(1);
}

class Out : public bzd::OStream
{
public:
	bzd::Async<> write(const bzd::Span<const bzd::Byte>) noexcept final
	{
		std::cout << "writing log!" << std::endl;
		std::exit(1);
		co_return {};
	}
};

Out& out()
{
	static Out o;
	return o;
}
} // namespace bzd::platform

bzd::Async<> noop() { co_return {}; }

bzd::Async<> executor()
{
	std::cout << "START!" << std::endl;
	auto n = noop();
	std::cout << "before noop" << std::endl;
	[[maybe_unused]] auto a = co_await n;
	std::cout << "after noop" << std::endl;

	/*
		auto async = generator(10);

		{
			const auto result = co_await async;
			if (!result)
			{
				std::cout << "1st result false" << std::endl;
				co_return {};
			}
			std::cout << "1st value " << result.value() << std::endl;
		}

		{
			const auto result = co_await async;
			if (!result)
			{
				std::cout << "2nd result false" << std::endl;
				co_return {};
			}
			std::cout << "2nd value " << result.value() << std::endl;
		}
	*/
	co_return {};
}

int main()
{
	executor().sync();
	std::cout << "SUCCESS!!" << std::endl;
	return 0;
}
