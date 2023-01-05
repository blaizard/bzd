#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/bzd/core/error.hh"
#include "cc/components/xtensa/core/interface.hh"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <iostream>

namespace bzd::platform::esp32 {

template <class Config>
class Core : public bzd::platform::Core
{
private:
	using Self = Core<Config>;
	static constexpr bzd::Byte freertosStackTaintingByte{0xa5};

public:
	constexpr Core(const Config& config) noexcept : config_{config} {}

	Result<void, bzd::Error> start(const bzd::FunctionRef<void(bzd::platform::Core&)> workload) noexcept override
	{
		// No need to taint the stack, this is already done by freertos with configCHECK_FOR_STACK_OVERFLOW = 2
		// stack_.taint(freertosStackTaintingByte);
		workload_ = workload;
		// Use a binary semaphore instead of a mutex as it can be taken in the same thread.
		// Also it is by default taken, which doesn't require to call xSemaphoreTake here.
		if (!(mutex_ = xSemaphoreCreateBinaryStatic(&mutexBuffer_)))
		{
			return bzd::error::Failure("xSemaphoreCreateBinaryStatic"_csv);
		}
		// Create the task and make sure the operation is successful. This will immediatly run the task.
		handle_ = xTaskCreateStaticPinnedToCore(workloadWrapper,
												Config::name.data(),
												stack_.size() / sizeof(StackType_t),
												static_cast<void*>(this),
												getFreeRTOSPriority(),
												reinterpret_cast<StackType_t*>(stack_.data()),
												&tcb_,
												Config::cpu);
		if (!handle_)
		{
			return bzd::error::Failure("xTaskCreateStatic"_csv);
		}

		return bzd::nullresult;
	}

	Result<void, bzd::Error> stop() noexcept override
	{
		// Wait indefinitely until the workload is completed.
		while (xSemaphoreTake(mutex_, portMAX_DELAY) == pdFALSE)
		{
		}
		vSemaphoreDelete(mutex_);

		::std::cout << "Stack usage: " << getStackUsage() << " / " << Config::stackSize << ::std::endl;

		return bzd::nullresult;
	}

	StackSize getStackUsage() noexcept override { return stack_.estimateMaxUsage(freertosStackTaintingByte); }

	void getUsage() noexcept {}

	CoreId getId() noexcept override { return 0; }

private:
	constexpr UBaseType_t getFreeRTOSPriority() const noexcept
	{
		// In freeRTOS context:
		// - 0 is the lowest priority.
		// - configMAX_PRIORITIES - 1 is the highest priority.
		return (configMAX_PRIORITIES - 1) * Config::priority / 100;
	}

	static void workloadWrapper(void* object)
	{
		auto core = reinterpret_cast<Self*>(object);
		// Run the workload.
		auto& workload = core->workload_.value();
		workload(*core);
		// Notify the stop function that the workload is completed.
		xSemaphoreGive(core->mutex_);
		// Delete itself.
		vTaskDelete(nullptr);
	}

private:
	const Config& config_;
	bzd::Stack<Config::stackSize, alignof(StackType_t)> stack_{};
	bzd::Optional<bzd::FunctionRef<void(bzd::platform::Core&)>> workload_{};
	TaskHandle_t handle_{};
	StaticTask_t tcb_{};
	StaticSemaphore_t mutexBuffer_{};
	SemaphoreHandle_t mutex_{nullptr};
};

} // namespace bzd::platform::esp32
