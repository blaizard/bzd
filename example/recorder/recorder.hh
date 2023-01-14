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

template <class T>
class Comms
{
public:
	// Try to get a slot, if not return an error.
	// bzd::Result<Object<T>> get();
	// Retries until a slot in the ring is available.
	// bzd::Async<Object<T>> get();
	// bzd::Result<ObjectWriter<T>> set();
	// bzd::Async<ObjectWriter<T>> set();

	// void fetch()
};

/*The code generation should generate an interface class for the interface part, as follow:

template <class Impl>
class HelloInterface
{
public:
	// method run();

	// send = Float;
	ObjectWriter<bzd::Int32> send() { return context_.comms<"example.Hello.send">.set(); }
};
*/

class Hello
{
public:
	template <class Config>
	constexpr explicit Hello(const Config&) noexcept
	{
	}

	bzd::Async<> run();

	// Getter for the latest input value.
	// input = const Integer;
	// bzd::Async<Object<bzd::Int32>> input() { return config_.comms<"example.Hello.input">.get(); }
	// bzd::Async<> input(const bzd::Int32 value) { return config_.comms<"example.Hello.input">.set(value); }

	// Object is a RAII object that is kept alive until the value is needed.
};

class World
{
public:
	template <class Config>
	constexpr explicit World(const Config&) noexcept
	{
	}
};

} // namespace example

namespace bzd {

template <bzd::meta::StringLiteral name>
struct Data;

class Recorder
{
public:
	template <bzd::meta::StringLiteral name>
	void publish(typename bzd::Data<name>::Type&& value) noexcept
	{
		const auto id = bzd::Data<name>::id;
		bzd::serialize(string_, id);
		bzd::serialize(string_, value);
	}

private:
	bzd::String<128u> string_;
};

} // namespace bzd
