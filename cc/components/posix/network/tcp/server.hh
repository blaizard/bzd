#pragma once

namespace bzd::platform::posix::network::tcp {
template <class Handler>
class Server
{
public:
	constexpr Server(const StringView endpoint, Handler& handler) {}

	auto init() { socket_ = endpoint.createSocket(endpoint_); }

private:
	void onNewConnection()
	{
		connection = accept();
		Stream stream{connection};
		handler.handle(bzd::move(stream));
	}
};
} // namespace bzd::platform::posix::network::tcp

/*
Usage in bdl:
proactor = Proactor();
webserver = Webserver();
server = Server(endpoint = "0.0.0.0:8888", proactor = proactor, handler = webserver);

Usage in C++:
class Webserver
{
public:
	void onNewConnection(auto connection)
	{
		co_await connection.read();
		co_await connection.write();
	}
};
*/
