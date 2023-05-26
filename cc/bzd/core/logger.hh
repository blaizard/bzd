#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/pattern/to_stream.hh"
#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/source_location.hh"

namespace bzd {
template <>
struct ToStream<bzd::Error>
{
	// Specialization for bzd::Error type.
	static bzd::Async<Size> process(bzd::OStream& stream, const bzd::Error& e)
	{
		co_return co_await bzd::toStream(stream,
										 "[origin:{}:{}] [{}] {}"_csv,
										 e.getSource(),
										 e.getLine(),
										 e.getTypeAsString(),
										 e.getMessage());
	}
};
} // namespace bzd

namespace bzd::log {
enum class Level
{
	error = 0,
	warning = 1,
	info = 2,
	debug = 3
};
}

namespace bzd {
class Logger
{
public:
	constexpr explicit Logger(bzd::OStream& stream) noexcept : stream_{stream} {}

public:
	/// Set an error log entry.
	template <class A>
	Async<> error(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::error, location, bzd::forward<A>(a));
		co_return {};
	}
	template <class A, class B>
	Async<> error(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::error, location, bzd::forward<A>(a), bzd::forward<B>(b));
		co_return {};
	}
	template <class A, class B, class C>
	Async<> error(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::error, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c));
		co_return {};
	}
	template <class A, class B, class C, class D>
	Async<> error(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::error, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d));
		co_return {};
	}
	template <class A, class B, class C, class D, class E>
	Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::error,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e));
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::error,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f));
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::error,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f),
						bzd::forward<G>(g));
		co_return {};
	}

	/// Set a warning log entry.
	template <class A>
	Async<> warning(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::warning, location, bzd::forward<A>(a));
		co_return {};
	}
	template <class A, class B>
	Async<> warning(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::warning, location, bzd::forward<A>(a), bzd::forward<B>(b));
		co_return {};
	}
	template <class A, class B, class C>
	Async<> warning(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::warning, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c));
		co_return {};
	}
	template <class A, class B, class C, class D>
	Async<> warning(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::warning, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d));
		co_return {};
	}
	template <class A, class B, class C, class D, class E>
	Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::warning,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e));
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::warning,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f));
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::warning,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f),
						bzd::forward<G>(g));
		co_return {};
	}

	/// Set an informative log entry.
	template <class A>
	Async<> info(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::info, location, bzd::forward<A>(a));
		co_return {};
	}
	template <class A, class B>
	Async<> info(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::info, location, bzd::forward<A>(a), bzd::forward<B>(b));
		co_return {};
	}
	template <class A, class B, class C>
	Async<> info(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::info, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c));
		co_return {};
	}
	template <class A, class B, class C, class D>
	Async<> info(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::info, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d));
		co_return {};
	}
	template <class A, class B, class C, class D, class E>
	Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::info,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e));
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::info,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f));
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::info,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f),
						bzd::forward<G>(g));
		co_return {};
	}

	/// Set a debug log entry.
	template <class A>
	Async<> debug(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::debug, location, bzd::forward<A>(a));
		co_return {};
	}
	template <class A, class B>
	Async<> debug(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::debug, location, bzd::forward<A>(a), bzd::forward<B>(b));
		co_return {};
	}
	template <class A, class B, class C>
	Async<> debug(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::debug, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c));
		co_return {};
	}
	template <class A, class B, class C, class D>
	Async<> debug(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::debug, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d));
		co_return {};
	}
	template <class A, class B, class C, class D, class E>
	Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::debug,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e));
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::debug,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f));
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await !print(bzd::log::Level::debug,
						location,
						bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f),
						bzd::forward<G>(g));
		co_return {};
	}

	/// Set the minimum logging level to be displayed.
	void setMinimumLevel(const bzd::log::Level level) noexcept;

	/// Get the default Logger.
	[[nodiscard]] static Logger& getDefault() noexcept;

private:
	Async<> printHeader(const bzd::log::Level level, const SourceLocation location) noexcept;

	template <class... Args>
	Async<> print(const bzd::log::Level level, const SourceLocation location, Args&&... args) noexcept
	{
		if (level <= minLevel_)
		{
			auto scope = co_await stream_.getLock();
			co_await !printHeader(level, location);
			co_await !toStream(stream_, bzd::forward<Args>(args)...);
			co_await !stream_.write("\n"_sv.asBytes());
		}
		co_return {};
	}

protected:
	bzd::OStream& stream_;
	bzd::log::Level minLevel_{bzd::log::Level::info};
};

} // namespace bzd
