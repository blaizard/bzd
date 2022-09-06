#pragma once

#include "cc/bzd.hh"
#include "cc/bzd/utility/synchronization/lock_guard.hh"
#include "cc/bzd/utility/synchronization/mutex.hh"

namespace bzd::http {

struct Header
{
	StringView key;
	StringView value;

	static constexpr Header host(const StringView value) noexcept { return {"Host"_sv, value}; }
	static constexpr Header userAgent(const StringView value) noexcept { return {"User-Agent"_sv, value}; }
	static constexpr Header accept(const StringView value) noexcept { return {"Accept"_sv, value}; }
};

template <Size capacity>
class BufferedStream : public bzd::OStream
{
public:
	constexpr BufferedStream(bzd::OStream& stream) noexcept : stream_{stream} {}

	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept override
	{
		auto dataLeft = data;
		while (dataLeft.size())
		{
			const auto sizeLeft = buffer_.capacity() - buffer_.size();
			if (sizeLeft < dataLeft.size())
			{
				buffer_.insertBack(dataLeft.first(sizeLeft));
				dataLeft = dataLeft.subSpan(sizeLeft);
				co_await !flush();
			}
			else
			{
				buffer_.insertBack(dataLeft);
			}
		}
		co_return {};
	}

	bzd::Async<> flush() noexcept
	{
		co_await !stream_.write(buffer_.asBytes());
		buffer_.clear();
		co_return {};
	}

private:
	bzd::OStream& stream_;
	Vector<Byte, capacity> buffer_;
};

template <class Client, Size capacityHeaders = 1u>
class Request
{
public:
	constexpr Request(Client& client, const StringView path) noexcept : client_{client}, path_{path} {}

private:
	template <Size oldCapacityHeaders>
	constexpr Request(Request<Client, oldCapacityHeaders>&& other) noexcept :
		client_{other.client_}, path_{other.path_}, headers_{bzd::move(other.headers_)}
	{
	}

public:
	/// Add an additional header to the request.
	constexpr auto header(Header&& header) &&
	{
		Request<Client, capacityHeaders + 1u> request{bzd::move(*this)};
		request.headers_.emplaceBack(bzd::move(header));
		return request;
	}

	template <Size bufferCapacity = 100>
	Async<> send() &&
	{
		auto scope = co_await !client_.connectIfNeeded();

		BufferedStream<bufferCapacity> buffer{client_.stream_.valueMutable()};
		co_await !::toStream(buffer, "GET {} HTTP/1.1\r\n"_csv, path_);
		for (const auto& header : headers_)
		{
			co_await !::toStream(buffer, "{}: {}\r\n"_csv, header.key, header.value);
		}
		co_await !::toStream(buffer, "\r\n"_csv);
		co_await !buffer.flush();

		co_return {};
	}

private:
	template <class, Size>
	friend class Request;

	Client& client_;
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
	constexpr Client(Network& network, const StringView hostname, const UInt32 port) noexcept :
		network_{network}, hostname_{hostname}, port_{port}
	{
	}

	constexpr auto request(const StringView path = "/"_sv) { return Request{*this, path}; }

	Async<> get(const StringView path, const interface::Map<StringView, StringView>) noexcept
	{
		stream_ = co_await !network_.connect(hostname_, port_);
		co_await !toStream(stream_.valueMutable(),
						   "GET {} HTTP/1.1\r\n"
						   "Host: {}\r\n"
						   "User-Agent: bzd\r\n"
						   "Accept: */*\r\n\r\n"_csv,
						   path,
						   hostname_);

		co_return {};
	}

private:
	template <class, Size>
	friend class Request;

	Async<lockGuard::Type<Mutex>> connectIfNeeded()
	{
		auto scope = co_await !makeLockGuard(mutex_);
		stream_ = co_await !network_.connect(hostname_, port_);
		co_return bzd::move(scope);
	}

	template <class... Args>
	Async<> toStream(interface::String& buffer, Args&&... args)
	{
		co_await !::toStream(stream_.valueMutable(), bzd::forward<Args>(args)...);
		co_return {};
	}

private:
	Network& network_;
	Optional<typename Network::Stream> stream_;
	StringView hostname_;
	UInt32 port_;
	Mutex mutex_;
};

} // namespace bzd::http
