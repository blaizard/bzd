#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/logger/backend/logger.hh"
#include "cc/bzd/core/logger/minimal.hh"
#include "cc/bzd/utility/format/stream.hh"
#include "cc/bzd/utility/source_location.hh"

namespace bzd::log {
enum class Level
{
	ERROR = 0,
	WARNING = 1,
	INFO = 2,
	DEBUG = 3
};
}

namespace bzd {
class Logger
{
public:
	/// Set an error log entry.
	template <class A>
	Async<void> error(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR, location, bzd::forward<A>(a));
	}
	template <class A, class B>
	Async<void> error(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR, location, bzd::forward<A>(a), bzd::forward<B>(b));
	}
	template <class A, class B, class C>
	Async<void> error(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c));
	}
	template <class A, class B, class C, class D>
	Async<void> error(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d));
	}
	template <class A, class B, class C, class D, class E>
	Async<void> error(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e));
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<void> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f));
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<void> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f),
					   bzd::forward<G>(g));
	}

	/// Set a warning log entry.
	template <class A>
	Async<void> warning(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING, location, bzd::forward<A>(a));
	}
	template <class A, class B>
	Async<void> warning(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING, location, bzd::forward<A>(a), bzd::forward<B>(b));
	}
	template <class A, class B, class C>
	Async<void> warning(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c));
	}
	template <class A, class B, class C, class D>
	Async<void> warning(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d));
	}
	template <class A, class B, class C, class D, class E>
	Async<void> warning(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e));
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<void> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f));
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<void> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f),
					   bzd::forward<G>(g));
	}

	/// Set an informative log entry.
	template <class A>
	Async<void> info(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO, location, bzd::forward<A>(a));
	}
	template <class A, class B>
	Async<void> info(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO, location, bzd::forward<A>(a), bzd::forward<B>(b));
	}
	template <class A, class B, class C>
	Async<void> info(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c));
	}
	template <class A, class B, class C, class D>
	Async<void> info(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d));
	}
	template <class A, class B, class C, class D, class E>
	Async<void> info(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e));
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<void> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f));
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<void> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f),
					   bzd::forward<G>(g));
	}

	/// Set a debug log entry.
	template <class A>
	Async<void> debug(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG, location, bzd::forward<A>(a));
	}
	template <class A, class B>
	Async<void> debug(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG, location, bzd::forward<A>(a), bzd::forward<B>(b));
	}
	template <class A, class B, class C>
	Async<void> debug(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c));
	}
	template <class A, class B, class C, class D>
	Async<void> debug(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d));
	}
	template <class A, class B, class C, class D, class E>
	Async<void> debug(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e));
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<void> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f));
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<void> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f),
					   bzd::forward<G>(g));
	}

	/// Set the minimum logging level to be displayed.
	void setMinimumLevel(const bzd::log::Level level) noexcept;

	/// Get the default Logger.
	[[nodiscard]] static Logger& getDefault() noexcept;

private:
	Async<void> printHeader(const bzd::log::Level level, const SourceLocation location) noexcept;

	template <class... Args>
	Async<void> print(const bzd::log::Level level, const SourceLocation location, Args&&... args) noexcept;

protected:
	bzd::log::Level minLevel_{bzd::log::Level::INFO};
};

} // namespace bzd

namespace bzd::log {

/// Set an error log entry using the default logger.
template <class A>
Async<void> error(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a), location);
}
template <class A, class B>
Async<void> error(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a), bzd::forward<B>(b), location);
}
template <class A, class B, class C>
Async<void> error(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location);
}
template <class A, class B, class C, class D>
Async<void> error(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location);
}
template <class A, class B, class C, class D, class E>
Async<void> error(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location);
}
template <class A, class B, class C, class D, class E, class F>
Async<void> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a),
											 bzd::forward<B>(b),
											 bzd::forward<C>(c),
											 bzd::forward<D>(d),
											 bzd::forward<E>(e),
											 bzd::forward<F>(f),
											 location);
}
template <class A, class B, class C, class D, class E, class F, class G>
Async<void> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a),
											 bzd::forward<B>(b),
											 bzd::forward<C>(c),
											 bzd::forward<D>(d),
											 bzd::forward<E>(e),
											 bzd::forward<F>(f),
											 bzd::forward<G>(g),
											 location);
}

/// Set a warning log entry using the default logger.
template <class A>
Async<void> warning(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a), location);
}
template <class A, class B>
Async<void> warning(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a), bzd::forward<B>(b), location);
}
template <class A, class B, class C>
Async<void> warning(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location);
}
template <class A, class B, class C, class D>
Async<void> warning(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location);
}
template <class A, class B, class C, class D, class E>
Async<void> warning(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.warning(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location);
}
template <class A, class B, class C, class D, class E, class F>
Async<void> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a),
											   bzd::forward<B>(b),
											   bzd::forward<C>(c),
											   bzd::forward<D>(d),
											   bzd::forward<E>(e),
											   bzd::forward<F>(f),
											   location);
}
template <class A, class B, class C, class D, class E, class F, class G>
Async<void> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a),
											   bzd::forward<B>(b),
											   bzd::forward<C>(c),
											   bzd::forward<D>(d),
											   bzd::forward<E>(e),
											   bzd::forward<F>(f),
											   bzd::forward<G>(g),
											   location);
}

/// Set an informative log entry using the default logger.
template <class A>
Async<void> info(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a), location);
}
template <class A, class B>
Async<void> info(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a), bzd::forward<B>(b), location);
}
template <class A, class B, class C>
Async<void> info(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location);
}
template <class A, class B, class C, class D>
Async<void> info(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location);
}
template <class A, class B, class C, class D, class E>
Async<void> info(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.info(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location);
}
template <class A, class B, class C, class D, class E, class F>
Async<void> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a),
											bzd::forward<B>(b),
											bzd::forward<C>(c),
											bzd::forward<D>(d),
											bzd::forward<E>(e),
											bzd::forward<F>(f),
											location);
}
template <class A, class B, class C, class D, class E, class F, class G>
Async<void> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a),
											bzd::forward<B>(b),
											bzd::forward<C>(c),
											bzd::forward<D>(d),
											bzd::forward<E>(e),
											bzd::forward<F>(f),
											bzd::forward<G>(g),
											location);
}

/// Set a debug log entry using the default logger.
template <class A>
Async<void> debug(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a), location);
}
template <class A, class B>
Async<void> debug(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a), bzd::forward<B>(b), location);
}
template <class A, class B, class C>
Async<void> debug(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location);
}
template <class A, class B, class C, class D>
Async<void> debug(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location);
}
template <class A, class B, class C, class D, class E>
Async<void> debug(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.debug(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location);
}
template <class A, class B, class C, class D, class E, class F>
Async<void> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a),
											 bzd::forward<B>(b),
											 bzd::forward<C>(c),
											 bzd::forward<D>(d),
											 bzd::forward<E>(e),
											 bzd::forward<F>(f),
											 location);
}
template <class A, class B, class C, class D, class E, class F, class G>
Async<void> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a),
											 bzd::forward<B>(b),
											 bzd::forward<C>(c),
											 bzd::forward<D>(d),
											 bzd::forward<E>(e),
											 bzd::forward<F>(f),
											 bzd::forward<G>(g),
											 location);
}

} // namespace bzd::log

// ----------------------------------------------------------------------------
// Implementation

template <class... Args>
bzd::Async<void> bzd::Logger::print(const bzd::log::Level level, const SourceLocation location, Args&&... args) noexcept
{
	if (level <= minLevel_)
	{
		auto& backend = bzd::backend::Logger::getDefault();
		auto scope = co_await backend.getLock();
		co_await printHeader(level, location);
		co_await bzd::format::toStream(backend, bzd::forward<Args>(args)...);
		co_await backend.write("\n"_sv.asBytes());
	}
}
