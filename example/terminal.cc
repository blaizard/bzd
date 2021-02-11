#include "example/terminal.h"

#include "bzd/utility/singleton.h"

#include <poll.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

namespace impl {
class Terminal : public bzd::Singleton<Terminal>
{
public:
	Terminal()
	{
		tcgetattr(0, &old_);
		current_ = old_;
		// Disable buffered i/o
		current_.c_lflag &= ~ICANON;
		// Set no echo mode
		current_.c_lflag &= ~ECHO;
		tcsetattr(0, TCSANOW, &current_);
	}

	~Terminal() { tcsetattr(0, TCSANOW, &old_); }

private:
	struct termios old_, current_;
};
} // namespace impl

bzd::Result<bzd::SizeType> Terminal::write(const bzd::Span<const bzd::ByteType>& /*data*/)
{
	return 0;
}

bzd::Result<bzd::SizeType> Terminal::read(const bzd::Span<bzd::ByteType>& data)
{
	bzd::ignore = impl::Terminal::getInstance();
	::pollfd fd{};
	fd.fd = STDIN_FILENO;
	fd.events = POLLIN;
	const auto ret = ::poll(&fd, 1, 0);
	if (ret > 0 && ((fd.revents & POLLIN) != 0))
	{
		const auto size = ::read(STDIN_FILENO, data.data(), data.size());
		return size;
	}
	return bzd::makeError();
}
/*
static bzd::Promise<bzd::SizeType>::ReturnType getChar()
{
	::pollfd fd{};
	fd.fd = STDIN_FILENO;
	fd.events = POLLIN;
	const auto ret = ::poll(&fd, 1, 0);
	if (ret > 0 && ((fd.revents & POLLIN) != 0))
	{
		const auto size = ::read(STDIN_FILENO, data.data(), data.size());
		return size;
	}
	return bzd::makeError();
}
*/
/*bzd::Promise<bzd::SizeType> Terminal::read2(const bzd::Span<bzd::ByteType>& data)
{
	bzd::ignore = impl::Terminal::getInstance();

	// Set mutex here

	return makePromise([&data]() {
		
	});
}
*/