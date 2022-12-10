#pragma once

#include "cc/bzd.hh"
#include "cc/bzd/core/serialization/serialization.hh"

namespace example {

bzd::Async<> run();

}

namespace bzd {

template <bzd::meta::StringLiteral name>
struct Record;

template <>
struct Record<"hello">
{
	using Type = char;
	static UInt32 id;
};

class Recorder
{
public:
	template <bzd::meta::StringLiteral name>
	void publish(typename Record<name>::Type&& value) noexcept
	{
		const auto id = Record<name>::id;
		bzd::serialize(string_, id);
		bzd::serialize(string_, value);
	}

private:
	bzd::String<128u> string_;
};

} // namespace bzd
