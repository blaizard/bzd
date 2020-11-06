#pragma once

#include "bzd/platform/types.h"

namespace bzd::platform::interface {
/**
 * Application stack definition
 */
class Stack
{
public:
	/**
	 * Reset the stack for initial use.
	 * This function sets the entry point in the stack.
	 */
	void reset(const FctPtrType fct) noexcept;

	void contextSwitch(Stack& stack) noexcept;

	Stack() = default;

protected:
	Stack(UInt8Type* stack, const SizeType size) : stackBase_{stack}, size_{size} {}

	/**
	 * Return the last element of the stack casted to a certain type
	 */
	template <class T, unsigned int Align = 1>
	T* last()
	{
		return reinterpret_cast<T*>(&stackBase_[((size_ - sizeof(T)) / Align) * Align]);
	}

	enum class Direction : UInt8Type
	{
		/**
		 * The stack grows from lower address to higher
		 */
		UPWARD = 0,
		/**
		 * The stack grows from higher address to lower
		 */
		DOWNWARD = 1
	};

public:
	UInt8Type* const stackBase_{nullptr};
	const SizeType size_{0};
	UInt8Type* stack_{nullptr};
	static const Direction direction_;
};
} // namespace bzd::platform::interface

namespace bzd::platform {
template <const SizeType N>
class Stack : public interface::Stack
{
public:
	Stack() : interface::Stack(data_, N) {}

private:
	UInt8Type data_[N];
};
} // namespace bzd::platform
