#pragma once

#include <sys/socket.h>

namespace bzd::platform::posix::network {
// Socket address information.
class IPv4
{
public:
    static constexpr int domain{AF_INET};

private:
    IPv4() = default;

public:
    static bzd::Result<IPv4, bzd::Error> fromString(const StringView string, const int port) noexcept
    {
        IPv4 ip{};
        const auto result = ::inet_pton(domain, string.data(), &(ip.address_.sin_addr));
        if (result == 0)
        {
            return bzd::error(ErrorType::failure, "Not a valid IPv4 {}."_csv, string);
        }
        else if (result < 0)
        {
            return bzd::error(ErrorType::failure, "inet_pton."_csv);
        }
        ip.address_.sin_family = domain;
        ip.address_.sin_port = ::htons(port);

        return ip;
    }

    struct ::sockaddr* address() noexcept
    {
        return &address_;
    }

    constexpr ::socklen_t size() const noexcept
    {
        return sizeof(address_);
    }

private:
    ::sockaddr_in address_{};
};
}
