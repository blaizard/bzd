// std
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

namespace {
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

bool demangle(char* pBuffer, const size_t size, const char* const pSymbol) noexcept
{
	int status;
	try
	{
		const std::unique_ptr<char, decltype(&std::free)> demangled(abi::__cxa_demangle(pSymbol, 0, 0, &status), &std::free);
		std::strncpy(pBuffer, ((demangled && status == 0) ? demangled.get() : pSymbol), size);
	}
	catch (...)
	{
		// Ignore and return false
		return false;
	}
	return (size > 0 && pBuffer[0] != '\0');
}

void callStack(std::ostream& out) noexcept
{
	constexpr size_t MAX_STACK_LEVEL = 10; //< A larger number causes some hanging
	static void* addresses[MAX_STACK_LEVEL];

	const int nbLevels = ::backtrace(addresses, MAX_STACK_LEVEL);
	const std::unique_ptr<char*, decltype(&std::free)> symbols(::backtrace_symbols(addresses, nbLevels), &std::free);

	// Reset filters
	std::cout << std::dec << std::noshowbase;

	for (int level = 0; level < nbLevels; ++level)
	{
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
		out << "#" << std::dec << std::left << std::setfill(' ') << std::setw(3) << level << "0x" << std::setfill('0') << std::hex
			<< std::right << std::setw(16) << reinterpret_cast<uint64_t>(addresses[level]);

		// Look for improved function/source names with addr2line
		{
			char cmd[1024];
			if (pOffset)
			{
				snprintf(cmd, sizeof(cmd), "addr2line -f -e \"%s\" %s", pSourcePath, pOffset);
			}
			else
			{
				snprintf(cmd, sizeof(cmd), "addr2line -f -e \"%s\" 0x%lx", pSourcePath, reinterpret_cast<uint64_t>(addresses[level]));
			}

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

		static char pBuffer[1024];
		if (pFunction)
		{
			if (demangle(pBuffer, sizeof(pBuffer), pFunction))
			{
				out << " in " << pBuffer;
				if (pOffset)
				{
					out << "+" << pOffset;
				}
			}
		}

		out << " (" << pSourcePath << ")";
		out << std::endl;
	}
}

void sigHandler(const int sig)
{
	// Ensure only one instance is running at a time
	static volatile std::atomic_flag sigHandlerInProgress = ATOMIC_FLAG_INIT;
	if (sigHandlerInProgress.test_and_set())
	{
		return;
	}

	std::cout << "\nCaught signal: " << getSignalName(sig) << std::endl;
	callStack(std::cout);
	std::exit(sig);
}
} // namespace

bool installBootstrap()
{
	struct ::sigaction sa;
	for (const auto& signal : {SIGSEGV, SIGFPE, SIGILL, SIGSYS, SIGABRT, SIGBUS, SIGTERM, SIGINT, SIGHUP})
	{
		::memset(&sa, 0, sizeof(sa));
    	sa.sa_handler = sigHandler;
		::sigemptyset(&sa.sa_mask);
		::sigaction(signal, &sa, nullptr);
	}

	return true;
}

static bool isBootstrapInstalled = installBootstrap();
