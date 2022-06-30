#include "example/format/format.hh"

#include "cc/bzd/core/logger.hh"
#include "cc/bzd/platform/clock.hh"
#include "cc/bzd/platform/stream.hh"
#include "cc/components/generic/executor/executor.hh"
#include "cc/components/linux/core/core.hh"
#include "cc/components/posix/proactor/proactor.hh"
#include "cc/components/std/clock/steady/steady.hh"
#include "cc/components/std/clock/system/system.hh"
#include "cc/components/std/stream/in/in.hh"
#include "cc/components/std/stream/out/out.hh"

// Backend platform symbols
namespace bzd::platform::backend {

bzd::OStream* out{nullptr};
bzd::IStream* in{nullptr};
bzd::Clock* steadyClock{nullptr};
bzd::Clock* systemClock{nullptr};

} // namespace bzd::platform::backend

// for deploy in deployments

namespace {

template <auto "string_", class T>
class Wrapper
{
public:
	Wrapper(T& obj) : obj_{obj} {}

	auto init();

	auto shutdown();

	auto& get() noexcept { return obj_; }

private:
	// mutex
	// refcount
	// status_
	T& obj_;
};

template <class T>
auto Wrapper<"in", T>::init()
{
	if (status_.load() == State::initialized || status_.load() == State::initializing)
	{
		auto scope = co_await mutex_.lock();
		if (status_.compareExchange(State::uninitialized, State::initializing))
		{
			co_await registry.proactorIO_.init();
			bzd::platform::backend::in = &registry.in_;
			state_.store(State::initialized);
		}
	}
}

/// Auto-generated application registry.
[[nodiscard]] auto makeRegistry() noexcept
{
	// Here is the dependency graph:
	// {'core1': set(), 'core2': set(), 'executor': {'core1', 'core2'}, 'proactorIO': set(), 'out': set(), 'in': {'proactorIO'},
	// 'steadyClock': set(), 'systemClock': set()}

	// Definition of all registry entries.
	static bzd::platform::std::Out out{};
	static bzd::platform::posix::proactor::Proactor proactorIO{};
	static bzd::platform::std::In in{/*proactor*/ proactorIO};
	static bzd::platform::std::clock::Steady steadyClock{};
	static bzd::platform::std::clock::System systemClock{};
	static bzd::platform::linux::Core</*stackSize*/ 100000> core1{/*priority*/ bzd::UInt8{0}, /*name*/ float{3.124}};
	static bzd::platform::linux::Core</*stackSize*/ 100000> core2{/*priority*/ bzd::UInt8{0}, /*name*/ float{3.124}};
	static bzd::platform::generic::Executor executor{core1, core2};
	static bzd::platform::generic::Obje obj{};

	// Declaration of the registry.
	struct Registry
	{
		Wrapper<"out", decltype(out)> out_;
		Wrapper<decltype(proactorIO)> proactorIO_;
		Wrapper<decltype(in)> in_;
		Wrapper<decltype(steadyClock)> steadyClock_;
		Wrapper<decltype(systemClock)> systemClock_;
		Wrapper<decltype(core1)> core1_;
		Wrapper<decltype(core2)> core2_;
		Wrapper<decltype(executor)> executor_;
		Wrapper<decltype(obj)> obj_;
	};
	return Registry{out, proactorIO, in, steadyClock, systemClock, core1, core2, executor, obj};
}

/// Helper to handle return code.
template <class Promise>
[[nodiscard]] constexpr bzd::Bool verifyReturnedPromise(Promise&& promise, const bzd::StringView taskName) noexcept
{
	if (!promise.isReady())
	{
		bzd::log::error("Task '{}' did not terminate correctly."_csv, taskName).sync();
		return false;
	}

	const auto result{promise.moveResultOut()};
	if (result->hasError())
	{
		bzd::log::error("Task '{}' terminated with an error."_csv, taskName).sync();
		bzd::log::error(result->error()).sync();
		return false;
	}

	return true;
}

} // namespace

enum class InitState
{
	uninitialized,
	initializing,
	initialized
};

auto initProactorIO()
{
	static Atomic<InitState> init{InitState::uninitialized};

	if (init.load() != InitState::initialized)
	{
		static Mutex mutex{};
		auto scope = co_await mutex.lock();
		if (init.compareExchange(InitState::uninitialized, InitState::initializing))
		{
			co_await registry.proactorIO_.init();
			init = InitState::initialized;
		}
	}
}

auto initIn()
{
	co_await initProactorIO();
	bzd::platform::backend::in = &registry.in_;
}

auto initializeInfrastructure()
{
	co_await in();
}

/// Initialize and compose the application.
bool execute() noexcept
{
	// Create registry.
	[[maybe_unused]] auto registry = makeRegistry();

	// Initialize infrastructure: initialize all special symbols
	initializeInfrastructure.sync();

	bzd::platform::backend::out = &registry.out_;
	registry.proactorIO_.init().sync();
	bzd::platform::backend::in = &registry.in_;
	bzd::platform::backend::steadyClock = &registry.steadyClock_;
	bzd::platform::backend::systemClock = &registry.systemClock_;
	bzd::log::info("Infrastructure initialized."_csv).sync();

	// Run everything per executor

	// Initialize executor named 'executor'
	struct executor
	{
		auto init() { return bzd::async::all(registry.core1_.init(), registry.core2_.init()).sync(); }

		auto shutdown() { return bzd::async::all(registry.core1_.shutdown(), registry.core2_.shutdown()).sync(); }
	};

	auto taskManager = [&]() {
		lock_guard scope{mutex};

		if (!intra.run_0)
		{
			if (init.obj_)
			{
				intra.run_0 = true;
				++refcounter.obj_;
				auto promise = Example::run(registry.obj_);
				promise.continuation([]() {
					--refcounter.obj_;
					taskManager();
				});
				registry.executor_.schedule(promise, bzd::async::Type::active);
			}
		}

		if (!intra.run_1)
		{
			if (init.obj_)
			{
				intra.run_1 = true;
				++refcounter.obj_;
				auto promise = Example::run(registry.obj_);
				promise.continuation([]() {
					--refcounter.obj_;
					taskManager();
				});
				registry.executor_.schedule(promise, bzd::async::Type::active);
			}
		}

		if (refcounter.obj_ == 0)
		{
			// shutdown
		}
	};

	// Let's assume that run() takes obj as argument and it needs to be initialized.
	auto promise0 = registry.proactorIO_.exec();
	promise0.continuation([]() { taskManager(); });
	registry.executor_.schedule(promise0, bzd::async::Type::service);

	auto promise1 = registry.obj_.init();
	promise1.continuation([]() {
		init.obj_ = true;
		taskManager();
	});
	registry.executor_.schedule(promise1, bzd::async::Type::active);

	// to their platform backend if applicable.
	bzd::platform::backend::out = &registry.out_;
	bzd::log::info("Initialization phase."_csv).sync();
	registry.proactorIO_.init();
	bzd::platform::backend::in = &registry.in_;
	bzd::platform::backend::steadyClock = &registry.steadyClock_;
	bzd::platform::backend::systemClock = &registry.systemClock_;
	registry.core1_.init();
	registry.core2_.init();

	// Compose.
	bzd::log::info("Composition phase."_csv).sync();
	auto promise0 = registry.proactorIO_.exec();
	registry.executor_.schedule(promise0, bzd::async::Type::service);
	auto promise1 = Example::run(registry.obj_);
	registry.executor_.schedule(promise1, bzd::async::Type::active);
	auto promise2 = Example::run(registry.obj_);
	registry.executor_.schedule(promise2, bzd::async::Type::active);
	auto promise3 = Example::run(registry.obj_);
	registry.executor_.schedule(promise3, bzd::async::Type::active);

	// Execute.
	bzd::log::info("Execution phase."_csv).sync();
	registry.executor_.start();

	// Wait until completion.
	registry.executor_.stop();

	bzd::log::info("Completion."_csv).sync();
	bool isSuccess{true};
	isSuccess &= verifyReturnedPromise(bzd::move(promise1), "Example::run"_sv);
	isSuccess &= verifyReturnedPromise(bzd::move(promise2), "Example::run"_sv);
	isSuccess &= verifyReturnedPromise(bzd::move(promise3), "Example::run"_sv);

	return isSuccess;
}
