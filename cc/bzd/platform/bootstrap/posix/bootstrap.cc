// std
#include "bzd/container/array.h"
#include "bzd/container/optional.h"
#include "bzd/container/string_channel.h"
#include "bzd/container/string_view.h"
#include "bzd/core/channel.h"
#include "bzd/platform/types.h"
#include "bzd/utility/format/format.h"
#include "bzd/utility/ignore.h"
#include "bzd/utility/singleton.h"

#include <array>
#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <iomanip>
#include <iostream>
#include <link.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace {
class AsyncSignalSafeChannel : public bzd::OChannel
{
public:
	bzd::Result<bzd::SizeType> write(const bzd::Span<const bzd::ByteType>& data) noexcept final
	{
		constexpr int fd = STDERR_FILENO;
		return ::write(fd, data.data(), data.size());
	}
};

constexpr const char* getSignalName(int sig) noexcept
{
	switch (sig)
	{
	case SIGINT:
		return "SIGINT";
	case SIGILL:
		return "SIGILL";
	case SIGABRT:
		return "SIGABRT";
	case SIGFPE:
		return "SIGFPE";
	case SIGSEGV:
		return "SIGSEGV";
	case SIGTERM:
		return "SIGTERM";
	case SIGHUP:
		return "SIGHUP";
	case SIGQUIT:
		return "SIGQUIT";
	case SIGTRAP:
		return "SIGTRAP";
	case SIGKILL:
		return "SIGKILL";
	case SIGBUS:
		return "SIGBUS";
	case SIGSYS:
		return "SIGSYS";
	case SIGPIPE:
		return "SIGPIPE";
	case SIGALRM:
		return "SIGALRM";
	case SIGURG:
		return "SIGURG";
	case SIGSTOP:
		return "SIGSTOP";
	case SIGTSTP:
		return "SIGTSTP";
	case SIGCONT:
		return "SIGCONT";
	case SIGCHLD:
		return "SIGCHLD";
	case SIGTTIN:
		return "SIGTTIN";
	case SIGTTOU:
		return "SIGTTOU";
	case SIGPOLL:
		return "SIGPOLL";
	case SIGXCPU:
		return "SIGXCPU";
	case SIGXFSZ:
		return "SIGXFSZ";
	case SIGVTALRM:
		return "SIGVTALRM";
	case SIGPROF:
		return "SIGPROF";
	case SIGUSR1:
		return "SIGUSR1";
	case SIGUSR2:
		return "SIGUSR2";
	default:
		return "<unknown>";
	}
}

bzd::StringView exec(const char* cmd)
{
	constexpr std::size_t maxSize = 1024;
	static char result[maxSize + 1];
	std::memset(result, 0, sizeof(result));

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe)
	{
		return nullptr;
	}

	std::size_t index = 0;
	while (index < maxSize && fgets(&result[index], maxSize - index, pipe.get()) != nullptr)
	{
		index = strlen(result);
	}

	result[sizeof(result) - 1] = '\0';
	return bzd::StringView{result};
}

bzd::StringView readLine(bzd::StringView str, bzd::SizeType& start)
{
	const auto end = str.find('\n', start);
	if (end != bzd::StringView::npos)
	{
		const auto line = str.subStr(start, end - start);
		// Needs to be modified
		const_cast<char&>(str[end]) = '\0';
		start = end + 1;
		return line;
	}
	return "";
}

struct DemangledBuffer
{
	static constexpr bzd::SizeType initialSize = 1024;

	DemangledBuffer() noexcept
	{
		data = static_cast<char*>(::malloc(initialSize));
		bzd::assert::isTrue(data);
		size = initialSize;
	}

	~DemangledBuffer()
	{
		if (data)
		{
			::free(data);
		}
	}

	char* data{nullptr};
	bzd::SizeType size{0};
};
// Pre-allocate data for demangling
DemangledBuffer demangleBuffer{};

bzd::Optional<bzd::StringView> tryDemangle(const char* const pSymbol) noexcept
{
	int status;
	auto data = abi::__cxa_demangle(pSymbol, demangleBuffer.data, &demangleBuffer.size, &status);
	if (data)
	{
		demangleBuffer.data = data;
	}
	if (status != 0)
	{
		return bzd::nullopt;
	}

	return bzd::StringView{demangleBuffer.data};
}

struct SymbolicInfo
{
	bzd::StringView path{};
	bzd::StringView symbol{};
	void* address{};
	bzd::IntPtrType offset{};
};

static void* getSelfBaseAddress()
{
	Dl_info info{};
	const auto result = ::dladdr(reinterpret_cast<const void*>(getSelfBaseAddress), &info);
	return (result) ? info.dli_fbase : nullptr;
}

SymbolicInfo makeSymbolicInfo(void* address)
{
	static auto selfBaseAddress = getSelfBaseAddress();
	SymbolicInfo symbolicInfo{};
	Dl_info info{};
	void* extraInfo{nullptr};
	const auto result = ::dladdr1(address, &info, &extraInfo, RTLD_DL_LINKMAP);

	if (result)
	{
		if (info.dli_saddr)
		{
			symbolicInfo.offset = reinterpret_cast<bzd::IntPtrType>(address) - reinterpret_cast<bzd::IntPtrType>(info.dli_saddr);
		}
		else if (extraInfo)
		{
			const ::link_map& linkMap = reinterpret_cast<const ::link_map&>(extraInfo);
			symbolicInfo.offset = reinterpret_cast<bzd::IntPtrType>(address) - reinterpret_cast<bzd::IntPtrType>(linkMap.l_ld);
		}
	}

	symbolicInfo.address = (info.dli_fbase == selfBaseAddress) ? address : reinterpret_cast<void*>(symbolicInfo.offset);
	symbolicInfo.path = (info.dli_fname) ? bzd::StringView{info.dli_fname} : bzd::StringView{""};
	symbolicInfo.symbol = (info.dli_sname) ? bzd::StringView{info.dli_sname} : bzd::StringView{""};

	return symbolicInfo;
}

void callStack() noexcept
{
	constexpr size_t MAX_STACK_LEVEL = 32;
	static void* addresses[MAX_STACK_LEVEL];

	const int nbLevels = ::backtrace(addresses, MAX_STACK_LEVEL);
	AsyncSignalSafeChannel channel;

	for (int level = 0; level < nbLevels; ++level)
	{
		// Do not print the last stack trace, but ellipsis instead.
		if (level == MAX_STACK_LEVEL - 1)
		{
			bzd::format::toString(channel, CSTR("...\n"));
			return;
		}

		auto address = addresses[level];
		const auto info = makeSymbolicInfo(address);
		bzd::StringView path{info.path};
		bzd::StringView symbol{info.symbol};

		// Look for improved function/source names with addr2line
		{
			bzd::StringChannel<1024> command;
			bzd::format::toString(command,
								  CSTR("addr2line -f -e \"{}\" {:#x} {:#x}"),
								  info.path.data(),
								  reinterpret_cast<bzd::IntPtrType>(info.address),
								  info.offset);

			{
				const auto result = exec(command.str().data());
				bzd::SizeType start = 0;
				const auto line1 = readLine(result, start);
				const auto line2 = readLine(result, start);
				const auto line3 = readLine(result, start);
				const auto line4 = readLine(result, start);
				symbol = (line1.size() && line1.at(0) != '?') ? line1 : symbol;
				symbol = (line3.size() && line3.at(0) != '?') ? line3 : symbol;
				path = (line2.size() && line2.at(0) != '?') ? line2 : path;
				path = (line4.size() && line4.at(0) != '?') ? line4 : path;
			}
		}

		if (symbol.data())
		{
			const auto maybeDemangled = tryDemangle(symbol.data());
			if (maybeDemangled)
			{
				symbol = maybeDemangled.value();
			}
		}

		// Print stack trace number and memory address
		bzd::format::toString(channel,
							  CSTR("#{:d} {:#x} in {}+{:#x} {}\n"),
							  level,
							  reinterpret_cast<uint64_t>(address),
							  symbol.data(),
							  info.offset,
							  path.data());
	}
}

void sigHandler(const int sig)
{
	// Ensure only a single instance is running at a time
	static volatile std::atomic_flag sigHandlerInProgress = ATOMIC_FLAG_INIT;
	if (sigHandlerInProgress.test_and_set())
	{
		return;
	}

	AsyncSignalSafeChannel channel;
	bzd::format::toString(channel, CSTR("\nCaught signal: {} ({:d})\n"), getSignalName(sig), sig);

	callStack();
	std::exit(sig);
}
} // namespace

bool installBootstrap()
{
	// Specify that the signal handler will be allocated onto the alternate signal stack.
	static bzd::Array<bzd::UInt8Type, SIGSTKSZ * 10> stack;

	stack_t signalStack{};
	signalStack.ss_size = stack.size();
	signalStack.ss_sp = stack.data();
	bzd::assert::isTrue(sigaltstack(&signalStack, 0) != -1);

	struct ::sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sigHandler;
	sa.sa_flags = SA_ONSTACK;

	for (const auto& signal : {SIGSEGV, SIGFPE, SIGILL, SIGSYS, SIGABRT, SIGBUS, SIGTERM, SIGINT, SIGHUP})
	{
		::sigaction(signal, &sa, nullptr);
	}

	return true;
}

static bool isBootstrapInstalled = installBootstrap();
