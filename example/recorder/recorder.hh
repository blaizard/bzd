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

template <class Context>
class Hello
{
public:
	constexpr explicit Hello(Context& context) noexcept : context_{context} {}

	bzd::Async<> run()
	{
		{
			auto maybeScope = context_.io.send.set();
			if (maybeScope)
			{
				maybeScope.valueMutable() = 12.4;
			}
		}

		/*co_await !bzd::print("Hello {}"_csv, bzd::Data<"example.Hello.send">::id);

		bzd::Recorder recorder;
		recorder.publish<"example.Hello.send">(12);*/

		co_return {};
	}

	// Getter for the latest input value.
	// input = const Integer;
	// bzd::Async<Object<bzd::Int32>> input() { return context_.comms.input.get(); }
	// bzd::Async<> input(const bzd::Int32 value) { return config_.comms<"example.hello.input">.set(value); }

	// Object is a RAII object that is kept alive until the value is needed.
	/*
	context_ = {
		template <class T0>
		comms {
			bzd::io::Writer<bzd::Int32> input = io_example_hello_input.makeWriter();
			bzd::threadsafe::interface::RingBuffer<bzd::Int32>& input = makeCommsExecutor().comms_example_hello_input;
		}
	}
	*/

private:
	Context& context_;
};

class World
{
public:
	template <class Context>
	constexpr explicit World(Context&) noexcept
	{
	}
};

} // namespace example
