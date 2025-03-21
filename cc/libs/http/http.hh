#pragma once

#include "cc/bzd/container/map.hh"
#include "cc/bzd/container/ostream_buffered.hh"
#include "cc/bzd/container/string_stream.hh"
#include "cc/bzd/utility/pattern/from_stream.hh"
#include "cc/bzd/utility/pattern/to_stream.hh"
#include "cc/bzd/utility/ranges/views_async/join.hh"
#include "cc/bzd/utility/synchronization/lock_guard.hh"
#include "cc/bzd/utility/synchronization/mutex.hh"
#include "interfaces/timer.hh"

#include <iostream>

// TO READ: https://fasterthanli.me/articles/the-http-crash-course-nobody-asked-for

namespace bzd::http {

struct Header
{
	StringView key;
	StringView value;

	static constexpr Header host(const StringView value) noexcept { return {"Host"_sv, value}; }
	static constexpr Header userAgent(const StringView value) noexcept { return {"User-Agent"_sv, value}; }
	static constexpr Header accept(const StringView value) noexcept { return {"Accept"_sv, value}; }
};

template <class Client>
class Response : public bzd::IStream
{
public:
	constexpr Response(Client& client) noexcept : client_{client}, connectCounter_{client.connectCounter_} {}

	GeneratorIChannel reader(bzd::Span<Byte> data) noexcept override
	{
		auto reader = client_.stream_.valueMutable().reader(data);

		// Read the status.
		const auto [version, code] = co_await !readStatus(reader | bzd::ranges::join());

		switch (version)
		{
		case "1.0"_s:
			[[fallthrough]];
		case "1.1"_s:
			break;
		default:
			co_yield bzd::error::Failure("Version '{}' not supported."_csv, version);
		}
		if (code != 200)
		{
			co_await !error(reader, "Code {}: {}"_csv, code);
		}

		// Read the headers.
		co_await !readHeaders(reader | bzd::ranges::join(), options_);
		state_ = State::body;

		switch (options_.transferMode)
		{
		case Options::TransferMode::normal:
		{
			auto it = co_await !reader.begin();
			while (it != reader.end())
			{
				co_yield *it;
				co_await !++it;
			}
		}
		break;
		case Options::TransferMode::chunked:
			while (true)
			{
				// Read the size.
				Size contentLength{};
				co_await !bzd::fromStream(reader | bzd::ranges::join(), "\\s*{:x}[ \r\t]*\n"_csv, contentLength);
				if (contentLength == 0u)
				{
					break;
				}

				auto chunkReader = reader.first(contentLength);
				auto it = co_await !chunkReader.begin();
				while (it != chunkReader.end())
				{
					co_yield *it;
					co_await !++it;
				}
			}
		}
	}

private:
	struct Status
	{
		bzd::String<3> version{};
		UInt16 code{};
	};

	struct Options
	{
		/// The type of compression for the body.
		enum class Compression
		{
			none,
			lzw,
			zlib,
			lz77
		} compression{Compression::none};

		/// The mode of transfer for the body to be used.
		enum class TransferMode
		{
			normal,
			chunked,
		} transferMode{TransferMode::normal};

		/// The expected size of the body.
		Size contentLength{0u};
	};

	enum class State
	{
		init,
		body,
	};

	bzd::Async<void> readHeaderTransferEncoding(auto& reader, Options& options) noexcept
	{
		const bzd::Map<bzd::StringView, bzd::FunctionRef<void(Options&)>, 4> headers{
			{"chunked", [](Options& options) { options.transferMode = Options::TransferMode::chunked; }},
			{"compress", [](Options& options) { options.compression = Options::Compression::lzw; }},
			{"deflate", [](Options& options) { options.compression = Options::Compression::zlib; }},
			{"gzip", [](Options& options) { options.compression = Options::Compression::lz77; }}};
		bzd::ToSortedRangeOfRanges wrapper{headers};
		while (const auto maybeValue = co_await bzd::fromStream(reader, "[ \t,]*{}"_csv, wrapper))
		{
			wrapper.value()->second(options);
		}

		co_return {};
	}

	/// Read the first line that should look like this.
	/// HTTP/1.1 200 OK
	bzd::Async<Status> readStatus(auto&& reader) noexcept
	{
		Status status{};
		const auto maybeSuccess =
			co_await bzd::fromStream(reader, "HTTP/{:[0-9.]+}\\s+{}\\s+[^\n]*\n"_csv, status.version.assigner(), status.code);
		if (!maybeSuccess)
		{
			co_await !error(reader, "Malformed status: {}"_csv);
		}
		co_return status;
	}

	/// Read headers.
	bzd::Async<void> readHeaders(auto&& reader, Options& options) noexcept
	{
		// Note, when using lambda with capture by reference, it somehow fails.
		// I believe this is because the reference from the lambda is not valid anymore. <- This is still
		// strange, to be investigated further.
		// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rcoro-capture
		using HeaderReader = bzd::FunctionRef<bzd::Async<void>(decltype(reader)&, Options&)>;
		const bzd::Map<bzd::StringView, HeaderReader, 1> headers{
			{"Transfer-Encoding"_sv, HeaderReader::template toMember<Response, &Response::readHeaderTransferEncoding>(*this)}};
		// Content-Length
		// Content-Encoding

		// Read the headers of interest (only).
		while (true)
		{
			// If this is an empty line, break.
			if (const auto maybeEnd = co_await bzd::fromStream(reader, "[ \r\t]*\n"_csv))
			{
				break;
			}

			bzd::ToSortedRangeOfRanges wrapper{headers};
			const auto maybeResult = co_await bzd::fromStream(reader, "[ \t]*{}[ \t]*:"_csv, wrapper);
			if (maybeResult.hasValue())
			{
				const auto maybeResult = co_await wrapper.value()->second(reader, options);
				(void)maybeResult;
			}
			else
			{
				// Ignore header.
			}

			// Consume the rest of the line.
			co_await !bzd::fromStream(reader, "[^\n]*\n"_csv);
		}
		co_return {};
	}

	template <class... Args>
	bzd::Async<> error(auto&& reader, Args&&... args) noexcept
	{
		bzd::StringStream<80u> buffer;
		auto fillBuffer = bzd::toStream(buffer, "[...]{:.64}[...]"_csv, reader);
		bzd::ignore = co_await bzd::async::any(bzd::move(fillBuffer), client_.timer_.timeout(1_s));
		co_return bzd::error::Failure(bzd::forward<Args>(args)..., buffer.str());
	}

private:
	Client& client_;
	Size connectCounter_;
	Options options_{};
	State state_{State::init};
};

template <class Client, Size capacityHeaders = 1u>
class Request
{
private:
	template <Size otherCapacityHeaders>
	using RequestCompatible = Request<Client, otherCapacityHeaders>;

public:
	constexpr Request(Client& client, const StringView method, const StringView path) noexcept :
		client_{client}, method_{method}, path_{path}
	{
	}

private:
	template <Size oldCapacityHeaders>
	constexpr Request(RequestCompatible<oldCapacityHeaders>&& other) noexcept :
		client_{other.client_}, method_{other.method_}, path_{other.path_}, headers_{bzd::move(other.headers_)}
	{
	}

public:
	/// Add an additional header to the request.
	constexpr auto header(Header&& header) &&
	{
		RequestCompatible<capacityHeaders + 1u> request{bzd::move(*this)};
		request.headers_.emplaceBack(bzd::move(header));
		return request;
	}

	constexpr auto header(const StringView key, const StringView value) && { return bzd::move(*this).header(Header{key, value}); }

	template <Size bufferCapacity = 128u>
	Async<Response<Client>> send() &&
	{
		auto scope = co_await !client_.connectIfNeeded();

		OStreamBuffered<bufferCapacity> buffer{client_.stream_.valueMutable()};
		co_await !bzd::toStream(buffer, "{} {} HTTP/1.1\r\n"_csv, method_, path_);
		for (const auto& header : headers_)
		{
			co_await !bzd::toStream(buffer, "{}: {}\r\n"_csv, header.key, header.value);
		}
		co_await !bzd::toStream(buffer, "\r\n"_csv);
		co_await !buffer.flush();

		co_return Response<Client>{client_};
	}

private:
	template <class, Size>
	friend class Request;

	Client& client_;
	StringView method_;
	StringView path_;
	Vector<Header, capacityHeaders> headers_{};
};

// Support for HTTP/1.0 & HTTP/1.1
template <class Network>
class Client
{
public: // Traits.
	using Self = Client<Network>;
	using Stream = typename Network::Stream;

public:
	constexpr Client(Network& network, bzd::Timer& timer, const StringView hostname, const UInt32 port) noexcept :
		network_{network}, timer_{timer}, hostname_{hostname}, port_{port}
	{
	}

	constexpr auto request(const StringView method = "GET"_sv, const StringView path = "/"_sv) noexcept
	{
		return Request<Self>{*this, method, path};
	}
	constexpr auto get(const StringView path = "/"_sv) noexcept { return request("GET"_sv, path); }
	constexpr auto head(const StringView path = "/"_sv) noexcept { return request("HEAD"_sv, path); }
	constexpr auto post(const StringView path = "/"_sv) noexcept { return request("POST"_sv, path); }
	constexpr auto put(const StringView path = "/"_sv) noexcept { return request("PUT"_sv, path); }

private:
	template <class, Size>
	friend class Request;

	template <class>
	friend class Response;

	Async<lockGuard::Type<Mutex>> connectIfNeeded()
	{
		auto scope = co_await !makeLockGuard(mutex_);
		stream_ = co_await !network_.connect(hostname_, port_);
		++connectCounter_;
		co_return bzd::move(scope);
	}

private:
	Network& network_;
	bzd::Timer& timer_;
	Optional<Stream> stream_;
	StringView hostname_;
	UInt32 port_;
	/// The number of times a new connection was established.
	Size connectCounter_{0};
	Mutex mutex_{};
};

} // namespace bzd::http
