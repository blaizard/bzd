#pragma once

#include "cc/bzd.hh"
#include "cc/bzd/core/serialization/serialization.hh"

template <class T>
class Object
{
public:
	Object(bzd::Span<T> range) : range_{range} {}

	const T& get() const;

	~Object()
	{
		// release.
	}

private:
	bzd::Span<T> range_;
};

namespace example {

class Hello
{
public:
	template <class Config>
	constexpr explicit Hello(const Config&) noexcept
	{
	}

	bzd::Async<> run();

	// Getter for input:
	// input = const Integer;
	bzd::Async<Object<bzd::Int32>> input();

	// Object is a RAII object that is kept alive until the value is needed.
};

} // namespace example

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
