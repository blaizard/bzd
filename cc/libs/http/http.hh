#pragma once

#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/container/map.hh"
#include "cc/bzd/container/ostream_buffered.hh"
#include "cc/bzd/container/string_stream.hh"
#include "cc/bzd/utility/pattern/from_stream.hh"
#include "cc/bzd/utility/pattern/to_stream.hh"
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

template <class Client, Size bufferCapacity = 128u>
class Response : public bzd::IStream
{
public:
	constexpr Response(Client& client) noexcept :
		client_{client}, connectCounter_{client.connectCounter_}, stream_{client_.stream_.valueMutable()}
	{
	}

	bzd::Async<bzd::Span<const bzd::Byte>> read(bzd::Span<bzd::Byte>&& data) noexcept override
	{
		// A new connection has been sent, the response object is invalid.
		if (client_.connectCounter_ != connectCounter_)
		{
			co_return bzd::error::Failure("Session expired.");
		}

		auto reader = stream_.reader();

		// Transfer-Encoding
		const auto [version, code] = co_await !readStatus(reader);

		switch (version)
		{
		case "1.0"_s:
			[[fallthrough]];
		case "1.1"_s:
			break;
		default:
			co_return bzd::error::Failure("Version '{}' not supported."_csv, version);
		}

		if (code != 200)
		{
			co_await !error(reader, "Code {}: {}"_csv, code);
		}

		::std::cout << "[-> " << code << " <-]" << std::endl;

		const auto read = co_await !stream_.read(bzd::move(data));
		co_return read;
	}

private:
	struct Status
	{
		bzd::String<3> version{};
		UInt16 code{};
	};

	/// Read the first line that should look like this.
	/// HTTP/1.1 200 OK
	bzd::Async<Status> readStatus(auto& reader) noexcept
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

	template <class... Args>
	bzd::Async<> error(auto& reader, Args&&... args) noexcept
	{
		bzd::StringStream<80u> buffer;
		auto fillBuffer = bzd::toStream(buffer, "[...]{:.64}[...]"_csv, reader);
		bzd::ignore = co_await bzd::async::any(bzd::move(fillBuffer), client_.timer_.timeout(1_s));
		co_return bzd::error::Failure(bzd::forward<Args>(args)..., buffer.str());
	}

private:
	Client& client_;
	Size connectCounter_;
	bzd::IStreamBuffered<bufferCapacity> stream_;
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
	Async<Response<Client, bufferCapacity>> send() &&
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

		co_return Response<Client, bufferCapacity>{client_};
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

	template <class, Size>
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
	Optional<typename Network::Stream> stream_;
	StringView hostname_;
	UInt32 port_;
	/// The number of times a new connection was established.
	Size connectCounter_{0};
	Mutex mutex_{};
};

} // namespace bzd::http
