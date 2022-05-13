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
	Async<> error(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR, location, bzd::forward<A>(a)).assert();
		co_return {};
	}
	template <class A, class B>
	Async<> error(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR, location, bzd::forward<A>(a), bzd::forward<B>(b)).assert();
		co_return {};
	}
	template <class A, class B, class C>
	Async<> error(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c)).assert();
		co_return {};
	}
	template <class A, class B, class C, class D>
	Async<> error(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E>
	Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::ERROR,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f),
					   bzd::forward<G>(g))
			.assert();
		co_return {};
	}

	/// Set a warning log entry.
	template <class A>
	Async<> warning(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING, location, bzd::forward<A>(a)).assert();
		co_return {};
	}
	template <class A, class B>
	Async<> warning(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING, location, bzd::forward<A>(a), bzd::forward<B>(b)).assert();
		co_return {};
	}
	template <class A, class B, class C>
	Async<> warning(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c)).assert();
		co_return {};
	}
	template <class A, class B, class C, class D>
	Async<> warning(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E>
	Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::WARNING,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f),
					   bzd::forward<G>(g))
			.assert();
		co_return {};
	}

	/// Set an informative log entry.
	template <class A>
	Async<> info(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO, location, bzd::forward<A>(a)).assert();
		co_return {};
	}
	template <class A, class B>
	Async<> info(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO, location, bzd::forward<A>(a), bzd::forward<B>(b)).assert();
		co_return {};
	}
	template <class A, class B, class C>
	Async<> info(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c)).assert();
		co_return {};
	}
	template <class A, class B, class C, class D>
	Async<> info(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E>
	Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::INFO,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f),
					   bzd::forward<G>(g))
			.assert();
		co_return {};
	}

	/// Set a debug log entry.
	template <class A>
	Async<> debug(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG, location, bzd::forward<A>(a)).assert();
		co_return {};
	}
	template <class A, class B>
	Async<> debug(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG, location, bzd::forward<A>(a), bzd::forward<B>(b)).assert();
		co_return {};
	}
	template <class A, class B, class C>
	Async<> debug(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c)).assert();
		co_return {};
	}
	template <class A, class B, class C, class D>
	Async<> debug(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG, location, bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E>
	Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F>
	Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f))
			.assert();
		co_return {};
	}
	template <class A, class B, class C, class D, class E, class F, class G>
	Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
	{
		co_await print(bzd::log::Level::DEBUG,
					   location,
					   bzd::forward<A>(a),
					   bzd::forward<B>(b),
					   bzd::forward<C>(c),
					   bzd::forward<D>(d),
					   bzd::forward<E>(e),
					   bzd::forward<F>(f),
					   bzd::forward<G>(g))
			.assert();
		co_return {};
	}

	/// Set the minimum logging level to be displayed.
	void setMinimumLevel(const bzd::log::Level level) noexcept;

	/// Get the default Logger.
	[[nodiscard]] static Logger& getDefault() noexcept;

private:
	Async<> printHeader(const bzd::log::Level level, const SourceLocation location) noexcept;

	template <class... Args>
	Async<> print(const bzd::log::Level level, const SourceLocation location, Args&&... args) noexcept;

protected:
	bzd::log::Level minLevel_{bzd::log::Level::INFO};
};

} // namespace bzd

namespace bzd::log {

/// Set an error log entry using the default logger.
template <class A>
Async<> error(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a), location).assert();
	co_return {};
}
template <class A, class B>
Async<> error(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a), bzd::forward<B>(b), location).assert();
	co_return {};
}
template <class A, class B, class C>
Async<> error(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location).assert();
	co_return {};
}
template <class A, class B, class C, class D>
Async<> error(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E>
Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E, class F>
Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.error(bzd::forward<A>(a),
			   bzd::forward<B>(b),
			   bzd::forward<C>(c),
			   bzd::forward<D>(d),
			   bzd::forward<E>(e),
			   bzd::forward<F>(f),
			   location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E, class F, class G>
Async<> error(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.error(bzd::forward<A>(a),
			   bzd::forward<B>(b),
			   bzd::forward<C>(c),
			   bzd::forward<D>(d),
			   bzd::forward<E>(e),
			   bzd::forward<F>(f),
			   bzd::forward<G>(g),
			   location)
		.assert();
	co_return {};
}

/// Set a warning log entry using the default logger.
template <class A>
Async<> warning(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a), location).assert();
	co_return {};
}
template <class A, class B>
Async<> warning(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a), bzd::forward<B>(b), location).assert();
	co_return {};
}
template <class A, class B, class C>
Async<> warning(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().warning(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location).assert();
	co_return {};
}
template <class A, class B, class C, class D>
Async<> warning(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.warning(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E>
Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.warning(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E, class F>
Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.warning(bzd::forward<A>(a),
				 bzd::forward<B>(b),
				 bzd::forward<C>(c),
				 bzd::forward<D>(d),
				 bzd::forward<E>(e),
				 bzd::forward<F>(f),
				 location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E, class F, class G>
Async<> warning(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.warning(bzd::forward<A>(a),
				 bzd::forward<B>(b),
				 bzd::forward<C>(c),
				 bzd::forward<D>(d),
				 bzd::forward<E>(e),
				 bzd::forward<F>(f),
				 bzd::forward<G>(g),
				 location)
		.assert();
	co_return {};
}

/// Set an informative log entry using the default logger.
template <class A>
Async<> info(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a), location).assert();
	co_return {};
}
template <class A, class B>
Async<> info(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a), bzd::forward<B>(b), location).assert();
	co_return {};
}
template <class A, class B, class C>
Async<> info(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().info(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location).assert();
	co_return {};
}
template <class A, class B, class C, class D>
Async<> info(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.info(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E>
Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.info(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E, class F>
Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.info(bzd::forward<A>(a),
			  bzd::forward<B>(b),
			  bzd::forward<C>(c),
			  bzd::forward<D>(d),
			  bzd::forward<E>(e),
			  bzd::forward<F>(f),
			  location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E, class F, class G>
Async<> info(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.info(bzd::forward<A>(a),
			  bzd::forward<B>(b),
			  bzd::forward<C>(c),
			  bzd::forward<D>(d),
			  bzd::forward<E>(e),
			  bzd::forward<F>(f),
			  bzd::forward<G>(g),
			  location)
		.assert();
	co_return {};
}

/// Set a debug log entry using the default logger.
template <class A>
Async<> debug(A&& a, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a), location).assert();
	co_return {};
}
template <class A, class B>
Async<> debug(A&& a, B&& b, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a), bzd::forward<B>(b), location).assert();
	co_return {};
}
template <class A, class B, class C>
Async<> debug(A&& a, B&& b, C&& c, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault().debug(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location).assert();
	co_return {};
}
template <class A, class B, class C, class D>
Async<> debug(A&& a, B&& b, C&& c, D&& d, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.debug(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E>
Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.debug(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E, class F>
Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.debug(bzd::forward<A>(a),
			   bzd::forward<B>(b),
			   bzd::forward<C>(c),
			   bzd::forward<D>(d),
			   bzd::forward<E>(e),
			   bzd::forward<F>(f),
			   location)
		.assert();
	co_return {};
}
template <class A, class B, class C, class D, class E, class F, class G>
Async<> debug(A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const SourceLocation location = SourceLocation::current()) noexcept
{
	co_await bzd::Logger::getDefault()
		.debug(bzd::forward<A>(a),
			   bzd::forward<B>(b),
			   bzd::forward<C>(c),
			   bzd::forward<D>(d),
			   bzd::forward<E>(e),
			   bzd::forward<F>(f),
			   bzd::forward<G>(g),
			   location)
		.assert();
	co_return {};
}

} // namespace bzd::log

// Specialization for bzd::Error type.
inline bzd::Async<> toStream(bzd::OStream& stream, const bzd::Error& e)
{
	co_await toStream(stream, "[origin:{}:{}] [{}] {}"_csv, e.getSource(), e.getLine(), e.getTypeAsString(), e.getMessage())
		.assert()
		.assert();
	co_return {};
}

// ----------------------------------------------------------------------------
// Implementation

template <class... Args>
bzd::Async<> bzd::Logger::print(const bzd::log::Level level, const SourceLocation location, Args&&... args) noexcept
{
	if (level <= minLevel_)
	{
		auto& backend = bzd::backend::Logger::getDefault();
		auto scope = co_await backend.getLock();
		co_await printHeader(level, location).assert();
		co_await toStream(backend, bzd::forward<Args>(args)...).assert();
		co_await backend.write("\n"_sv.asBytes()).assert();
	}
	co_return {};
}
