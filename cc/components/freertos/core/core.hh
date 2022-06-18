#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/bzd/core/error.hh"
#include "cc/components/freertos/core/interface.hh"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <iostream>

namespace bzd::platform::freertos {

template <SizeType stackSize>
class Core : public bzd::platform::Core<Core<stackSize>>
{
private:
	using Self = Core<stackSize>;
	using Parent = bzd::platform::Core<Self>;

public:
	constexpr Core(const bzd::StringView name) : name_{name} {}

	Result<void, bzd::Error> start(const bzd::platform::WorkloadType& workload) noexcept
	{
		// No need to taint the stack, this is already done by freertos with configCHECK_FOR_STACK_OVERFLOW = 2
		// stack_.taint(0xa5);
		workload_ = workload;
		// Use a binary semaphore instead of a mutex as it can be taken in the same thread.
		// Also it is by default taken, which doesn't require to call xSemaphoreTake here.
		if (!(mutex_ = xSemaphoreCreateBinaryStatic(&mutexBuffer_)))
		{
			return bzd::error(ErrorType::failure, "mutex creation"_csv);
		}
		// Create the task and make sure the operation is successful. This will immediatly run the task.
		handle_ = xTaskCreateStatic(workloadWrapper,
									name_.data(),
									stack_.size() / sizeof(StackType_t),
									static_cast<void*>(this),
									tskIDLE_PRIORITY,
									stack_.data(),
									&tcb_);
		if (!handle_)
		{
			return bzd::error(ErrorType::failure, "xTaskCreateStatic"_csv);
		}

		return bzd::nullresult;
	}

	Result<void, bzd::Error> stop() noexcept
	{
		// Wait indefinitely until the workload is completed.
		while (xSemaphoreTake(mutex_, portMAX_DELAY) == pdFALSE)
		{
		}
		vSemaphoreDelete(mutex_);

		::std::cout << "Stack usage: " << getStackUsage() << " / " << stackSize << ::std::endl;

		return bzd::nullresult;
	}

	StackSize getStackUsage() noexcept { return stack_.estimateMaxUsage(0xa5); }

	void getUsage() noexcept {}

	CoreId getId() noexcept { return 0; }

private:
	static void workloadWrapper(void* object)
	{
		auto core = reinterpret_cast<Self*>(object);
		// Run the workload.
		auto& workload = core->workload_.value();
		workload();
		// Notify the stop function that the workload is completed.
		xSemaphoreGive(core->mutex_);
		// Delete itself.
		vTaskDelete(nullptr);
	}

private:
	bzd::StringView name_;
	bzd::Stack<stackSize, alignof(StackType_t)> stack_{};
	bzd::Optional<bzd::platform::WorkloadType> workload_;
	TaskHandle_t handle_{};
	StaticTask_t tcb_{};
	StaticSemaphore_t mutexBuffer_{};
	SemaphoreHandle_t mutex_{nullptr};
};

} // namespace bzd::platform::freertos
