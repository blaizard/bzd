#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/bzd/core/error.hh"
#include "cc/components/freertos/core/interface.hh"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
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
		// Use a binary semaphore instead of a mutex as it can be taken in the same thread.
		// Also it is by default taken, which doesn't require to call xSemaphoreTake here.
		if (!(mutex_ = xSemaphoreCreateBinaryStatic(&mutexBuffer_)))
		{
			return bzd::error(ErrorType::failure, "mutex creation"_csv);
		}
		handle_ =
			xTaskCreateStatic(workloadWrapper, "Core", stack_.size(), static_cast<void*>(this), tskIDLE_PRIORITY, stack_.data(), &tcb_);
		if (!handle_)
		{
			return bzd::error(ErrorType::failure, "xTaskCreateStatic"_csv);
		}

		// Need to wait until the mutex is taken.

		return bzd::nullresult;
	}

	Result<void, bzd::Error> stop() noexcept
	{
		// Wait on the semaphore
		while (xSemaphoreTake(mutex_, portMAX_DELAY) == pdFALSE)
		{
		}
		vTaskDelete(handle_);
		vSemaphoreDelete(mutex_);
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
		xSemaphoreGive(core->mutex_);
		vTaskSuspend(nullptr);
	}

private:
	bzd::Stack<stackSize> stack_{};
	bzd::Optional<bzd::platform::WorkloadType> workload_;
	TaskHandle_t handle_{};
	StaticTask_t tcb_{};
	StaticSemaphore_t mutexBuffer_{};
	SemaphoreHandle_t mutex_{nullptr};
};

} // namespace bzd::platform::freertos
