#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/bzd/core/error.hh"
#include "cc/components/freertos/core/interface.hh"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace bzd::platform::freertos {

template <SizeType stackSize>
class Core : public bzd::platform::Core<Core<stackSize>>
{
private:
	using Self = Core<stackSize>;
	using Parent = bzd::platform::Core<Self>;

public:
	Result<void, bzd::Error> start(const bzd::platform::WorkloadType& workload) noexcept
	{
		stack_.taint();
		workload_ = workload;
		handle_ =
			xTaskCreateStatic(workloadWrapper, "Core", stack_.size(), static_cast<void*>(this), tskIDLE_PRIORITY, stack_.data(), &tcb_);
		if (!handle_)
		{
			return bzd::error(ErrorType::failure, "xTaskCreateStatic"_csv);
		}

		return bzd::nullresult;
	}

	Result<void, bzd::Error> stop() noexcept
	{
		while (terminated_.load() == false)
		{
			vTaskDelay(1000);
		}
		return bzd::nullresult;
	}

	StackSize getStackUsage() noexcept { return stack_.estimateMaxUsage(); }

	void getUsage() noexcept {}

	CoreId getId() noexcept { return 0; }

private:
	static void workloadWrapper(void* object)
	{
		auto core = reinterpret_cast<Self*>(object);
		auto& workload = core->workload_.value();
		workload();
		core->terminated_.store(true);
		vTaskDelete(core->handle_);
	}

private:
	bzd::Stack<stackSize> stack_{};
	bzd::Optional<bzd::platform::WorkloadType> workload_;
	TaskHandle_t handle_{};
	StaticTask_t tcb_{};
	Atomic<BoolType> terminated_{false};
};

} // namespace bzd::platform::freertos
