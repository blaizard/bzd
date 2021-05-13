// std
#include "bzd/container/array.h"
#include "bzd/container/optional.h"
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
#include <execinfo.h>
#include <iomanip>
#include <iostream>
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

char* exec(const char* cmd)
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
	return result;
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

void callStack(std::ostream& out) noexcept
{
	constexpr size_t MAX_STACK_LEVEL = 32;
	static void* addresses[MAX_STACK_LEVEL];

	const int nbLevels = ::backtrace(addresses, MAX_STACK_LEVEL);
	const std::unique_ptr<char*, decltype(&std::free)> symbols(::backtrace_symbols(addresses, nbLevels), &std::free);

	AsyncSignalSafeChannel channel;

	// Reset filters
	std::cout << std::dec << std::noshowbase;

	for (int level = 0; level < nbLevels; ++level)
	{
		// Do not print the last stack trace, but ellipsis instead.
		if (level == MAX_STACK_LEVEL - 1)
		{
			bzd::format::toString(channel, CSTR("...\n"));
			return;
		}

		char* pSymbol = symbols.get()[level];
		const char* pSourcePath = nullptr;
		const char* pFunction = nullptr;
		const char* pOffset = nullptr;

		// Look for the source path
		{
			const auto pEnd = std::strchr(pSymbol, '(');
			if (pEnd)
			{
				*pEnd = '\0';
				pSourcePath = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Look for the function
		if (pSourcePath)
		{
			const auto pEnd = std::strchr(pSymbol, '+');
			if (pEnd)
			{
				*pEnd = '\0';
				pFunction = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Look for the offset
		if (pFunction)
		{
			const auto pEnd = std::strchr(pSymbol, ')');
			if (pEnd)
			{
				*pEnd = '\0';
				pOffset = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Print stack trace number and memory address
		bzd::format::toString(channel, CSTR("#{:d} {:#x}"), level, reinterpret_cast<uint64_t>(addresses[level]));

		// Look for improved function/source names with addr2line
		{
			static char cmd[1024];
			if (pOffset)
			{
				snprintf(cmd, sizeof(cmd), "addr2line -f -e \"%s\" %s", pSourcePath, pOffset);
			}
			else if (pSourcePath)
			{
				snprintf(cmd, sizeof(cmd), "addr2line -f -e \"%s\" 0x%lx", pSourcePath, reinterpret_cast<uint64_t>(addresses[level]));
			}

			if (pOffset || pSourcePath)
			{
				pSymbol = exec(cmd);
				if (pSymbol)
				{
					// Function
					if (pSymbol[0] != '?')
					{
						const auto pEnd = std::strchr(pSymbol, '\n');
						if (pEnd)
						{
							*pEnd = '\0';
							pFunction = pSymbol;
							pSymbol = pEnd + 1;
						}
					}
					// Source path
					if (pSymbol[0] != '?')
					{
						const auto pEnd = std::strchr(pSymbol, '\n');
						if (pEnd)
						{
							*pEnd = '\0';
							pSourcePath = pSymbol;
							pSymbol = pEnd + 1;
						}
					}
				}
			}
		}

		if (pFunction)
		{
			const auto maybeDemangled = tryDemangle(pFunction);
			if (maybeDemangled)
			{
				out << " in ";
				out.write(maybeDemangled->data(), maybeDemangled->size());
				if (pOffset)
				{
					out << "+" << pOffset;
				}
			}
			else
			{
				out << pFunction;
			}
		}

		if (pSourcePath)
		{
			out << " (" << pSourcePath << ")";
		}
		else
		{
			out << " (<unknown>)";
		}
		out << std::endl;
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

	callStack(std::cout);
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
