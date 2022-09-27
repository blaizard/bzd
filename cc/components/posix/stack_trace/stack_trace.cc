// std
#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/pattern/formatter/to_stream.hh"
#include "cc/bzd/utility/singleton.hh"
#include "cc/components/posix/error.hh"

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
class AsyncSignalSafeStream : public bzd::OStream
{
public:
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept final
	{
		constexpr int fd = STDERR_FILENO;

		bzd::Size alreadyWritten = 0u;
		while (alreadyWritten < data.size())
		{
			const auto result = ::write(fd, data.data(), data.size());
			if (result < 0)
			{
				co_return bzd::error::Errno("write");
			}
			alreadyWritten += result;
		}
		co_return {};
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
	while (index < maxSize && fgets(&result[index], static_cast<int>(maxSize - index), pipe.get()) != nullptr)
	{
		index = strlen(result);
	}

	result[sizeof(result) - 1] = '\0';
	return bzd::StringView{result};
}

bzd::StringView readLine(bzd::StringView str, bzd::Size& start)
{
	const auto end = str.find('\n', start);
	if (end != bzd::npos)
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
	static constexpr bzd::Size initialSize = 1024;

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
	bzd::Size size{0};
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
	bzd::IntPointer offset{};
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
			symbolicInfo.offset = reinterpret_cast<bzd::IntPointer>(address) - reinterpret_cast<bzd::IntPointer>(info.dli_saddr);
		}
		else if (extraInfo)
		{
			const ::link_map& linkMap = reinterpret_cast<const ::link_map&>(extraInfo);
			// NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
			symbolicInfo.offset = reinterpret_cast<bzd::IntPointer>(address) - reinterpret_cast<bzd::IntPointer>(linkMap.l_ld);
		}
	}

	symbolicInfo.address = (info.dli_fbase == selfBaseAddress) ? address : reinterpret_cast<void*>(symbolicInfo.offset);
	symbolicInfo.path = (info.dli_fname) ? bzd::StringView{info.dli_fname} : bzd::StringView{""};
	symbolicInfo.symbol = (info.dli_sname) ? bzd::StringView{info.dli_sname} : bzd::StringView{""};

	return symbolicInfo;
}

void callStack() noexcept // NOLINT(bugprone-exception-escape)
{
	constexpr size_t maxStackLevel = 32;
	static void* addresses[maxStackLevel];

	const int nbLevels = ::backtrace(addresses, maxStackLevel);
	AsyncSignalSafeStream stream;

	for (int level = 0; level < nbLevels; ++level)
	{
		// Do not print the last stack trace, but ellipsis instead.
		if (level == maxStackLevel - 1)
		{
			toStream(stream, "...\n"_sv).sync();
			return;
		}

		auto address = addresses[level];
		const auto info = makeSymbolicInfo(address);
		bzd::StringView path{info.path};
		bzd::StringView symbol{info.symbol};

		// Look for improved function/source names with addr2line
		if (!path.empty())
		{
			bzd::String<1024> command;
			toString(command,
					 "exec 2>/dev/null; addr2line -f -e \"{}\" {:#x} {:#x}"_csv,
					 path,
					 reinterpret_cast<bzd::IntPointer>(info.address),
					 info.offset);

			{
				const auto result = exec(command.data());
				bzd::Size start = 0;
				const auto line1 = readLine(result, start);
				const auto line2 = readLine(result, start);
				const auto line3 = readLine(result, start);
				const auto line4 = readLine(result, start);
				symbol = (line1.size() && line1.at(0) != '?') ? line1 : symbol;
				symbol = (line3.size() && line3.at(0) != '?') ? line3 : symbol;
				path = (line2.size() && line2.at(0) != '?') ? line2 : path;
				path = (line4.size() && line4.at(0) != '?') ? line4 : path;
			}

			// Remove useless path prefix.
			for (const auto prefix : {"/proc/self/cwd/"_sv})
			{
				if (const auto index = path.find(prefix); index == 0)
				{
					path.removePrefix(prefix.size());
					break;
				}
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
		toStream(stream,
				 "#{:d} {:#x} in {}+{:#x} {}\n"_csv,
				 level,
				 reinterpret_cast<uint64_t>(address),
				 symbol.data(),
				 info.offset,
				 path.data())
			.sync();
	}
	toStream(stream, "---\n"_sv).sync();
}

void sigHandler(const int sig)
{
	// Ensure only a single instance is running at a time
	static volatile std::atomic_flag sigHandlerInProgress{};
	if (sigHandlerInProgress.test_and_set())
	{
		return;
	}

	AsyncSignalSafeStream stream;
	toStream(stream, "\nCaught signal: {} ({:d})\n"_csv, getSignalName(sig), sig).sync();

	callStack();
	std::exit(sig);
}
} // namespace

bool installStackTrace()
{
	// Specify that the signal handler will be allocated onto the alternate signal stack.
	static bzd::Array<bzd::UInt8, SIGSTKSZ> stack;

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

static bool isStackTraceInstalled = installStackTrace();
