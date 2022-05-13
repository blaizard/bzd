#include "cc/bzd.hh"
#include "example/terminal/reader.hh"

namespace Example {

bzd::Async<> run()
{
	bzd::Array<bzd::ByteType, 2> separators{bzd::ByteType{' '}, bzd::ByteType{'\t'}};
	bzd::Reader<16> reader{bzd::platform::in(), bzd::platform::out(), separators.asSpan()};

	bzd::Map<bzd::StringView, int, 12> keywords{
		{"info"_sv, 0},
		{"test"_sv, 1},
		{"hallo"_sv, 2},
		{"hello"_sv, 3},
		{"hell"_sv, 4},
		{"--help"_sv, 5},
	};

	co_await bzd::print("Please type something...\n"_csv).assert();

	{
		const auto result = co_await reader.readAnyOf(keywords);
		if (result)
		{
			co_await bzd::print("Matched {}\n"_csv, result.value()).assert();
		}
		else
		{
			co_await bzd::log::error(result.error()).assert();
			reader.clear();
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		const auto result = co_await reader.readUntil(bzd::ByteType{'\n'});
		if (result)
		{
			co_await bzd::platform::out().write(result.value()).assert();
		}
		else
		{
			co_await bzd::log::error(result.error()).assert();
			reader.clear();
		}
	}

	co_return {};
}

} // namespace Example
