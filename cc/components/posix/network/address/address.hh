#pragma once

#include "cc/bzd/algorithm/copy_n.hh"
#include "cc/bzd/container/variant.hh"
#include "cc/bzd/utility/iterators/forward.hh"
#include "cc/components/posix/network/address/family.hh"
#include "cc/components/posix/network/address/protocol.hh"
#include "cc/components/posix/network/address/socket_type.hh"
#include "interfaces/network.hh"

#include <netdb.h>

namespace bzd::components::posix::network {

// Address family abstraction.
class Address
{
public:
	/// Create an address object from an IP v4 address and a port number.
	[[nodiscard]] static bzd::Result<Address, bzd::Error> fromIpV4(const Protocol protocol,
																   const StringView string,
																   const bzd::network::PortType port) noexcept;

	/// Create an address object from an IP v6 address and a port number.
	[[nodiscard]] static bzd::Result<Address, bzd::Error> fromIpV6(const Protocol protocol,
																   const StringView string,
																   const bzd::network::PortType port) noexcept;

	/// Create an address object from an IP address and a port number.
	[[nodiscard]] static bzd::Result<Address, bzd::Error> fromIp(const Protocol protocol,
																 const StringView string,
																 const bzd::network::PortType port) noexcept;

	constexpr AddressFamily family() const noexcept { return AddressFamily{storage_.ss_family}; }
	constexpr const ::sockaddr* native() const noexcept { return &storageErased_; }
	constexpr ::socklen_t size() const noexcept { return size_; }
	constexpr SocketType socketType() const noexcept { return type_; }
	constexpr Protocol::Number protocol() const noexcept { return protocol_; }

	/// Create an address object from an addrinfo structure.
	explicit Address(const ::addrinfo& addr) noexcept : size_{addr.ai_addrlen}, type_{addr.ai_socktype}, protocol_{addr.ai_protocol}
	{
		bzd::algorithm::copyN(reinterpret_cast<bzd::Byte*>(addr.ai_addr), size_, reinterpret_cast<bzd::Byte*>(&storageErased_));
	}

private:
	constexpr Address(const Size size, const Protocol protocol) noexcept :
		size_{size}, type_{protocol.getSocketType()}, protocol_{protocol.getProtocolNumber()}
	{
	}

private:
	// Use an union here to avoid using reinterpret_cast, this will prevent the usage of constexpr.
	union {
		::sockaddr storageErased_;
		::sockaddr_storage storage_;
	};
	Size size_;
	SocketType type_;
	Protocol::Number protocol_;
};

// List of addresses.
class Addresses
{
private:
	struct IteratorPolicies : iterator::impl::DefaultPolicies<::addrinfo>
	{
		static constexpr void increment(::addrinfo*& data) noexcept { data = data->ai_next; }
	};

public:
	class Iterator : public iterator::Forward<::addrinfo, Iterator, IteratorPolicies>
	{
	public:
		using iterator::Forward<::addrinfo, Iterator, IteratorPolicies>::Forward;

		[[nodiscard]] constexpr Address& operator*() const noexcept
		{
			address_.emplace(*this->data_);
			return address_.valueMutable();
		}
		[[nodiscard]] constexpr Address* operator->() const noexcept
		{
			address_.emplace(*this->data_);
			return &address_.valueMutable();
		}

	private:
		mutable bzd::Optional<Address> address_{};
	};

public:
	/// Create an address object from a hostname and a port number.
	[[nodiscard]] static bzd::Result<Addresses, bzd::Error> fromHostname(const Protocol protocol,
																		 const StringView hostname,
																		 const bzd::network::PortType port) noexcept;

public: // Copy/move constructors/assignments
	Addresses(const Addresses&) = delete;
	Addresses& operator=(const Addresses&) = delete;
	constexpr Addresses(Addresses&& other) noexcept : addr_{other.addr_} { other.addr_ = nullptr; }
	Addresses& operator=(Addresses&& other) noexcept
	{
		reset();
		addr_ = other.addr_;
		other.addr_ = nullptr;
		return *this;
	}
	~Addresses() noexcept;

	constexpr Iterator begin() const noexcept { return Iterator{addr_}; }
	constexpr Iterator end() const noexcept { return Iterator{nullptr}; }

private:
	Addresses() = default;

	void reset() noexcept;

private:
	::addrinfo* addr_{nullptr};
};

} // namespace bzd::components::posix::network
