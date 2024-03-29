#include "example/terminal/composition.hh"
#include "example/terminal/reader.hh"

namespace example {

bzd::Async<> run(bzd::OStream& out, bzd::IStream& in)
{
	bzd::Array<bzd::Byte, 2> separators{bzd::Byte{' '}, bzd::Byte{'\t'}};
	bzd::Reader<16> reader{in, out, separators.asSpan()};

	bzd::Map<bzd::StringView, bzd::Int32, 12> keywords{
		{"info"_sv, 0},
		{"test"_sv, 1},
		{"hallo"_sv, 2},
		{"hello"_sv, 3},
		{"hell"_sv, 4},
		{"--help"_sv, 5},
	};

	co_await !bzd::print(out, "Please type something...\n"_csv);

	{
		const auto result = co_await reader.readAnyOf(keywords);
		if (result)
		{
			co_await !bzd::print(out, "Matched {}\n"_csv, result.value());
		}
		else
		{
			// co_await !bzd::log::error(result.error());
			reader.clear();
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		const auto result = co_await reader.readUntil(bzd::Byte{'\n'});
		if (result)
		{
			co_await !out.write(result.value());
		}
		else
		{
			// co_await !bzd::log::error(result.error());
			reader.clear();
		}
	}

	co_return {};
}

} // namespace example
