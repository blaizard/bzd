#pragma once

#include "bzd/platform/types.h"

#include <iostream>

namespace bzd::platform::interface {

/**
 * A stack object refers to an application or a native stack.
 *
 * It only holds the stack pointer and will allow for basic
 * operation such as context switching.
 */
class Stack
{
public: // Type.
	/**
	 * Describes the direction to which the stack grows.
	 */
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

public: // Constructor.
	Stack() noexcept = default;

public: // Exposed functions.
	/**
	 * Switch a the task passed into argument.
	 * The current task should get its task pointer updated.
	 *
	 * \param stack The New stack to be used.
	 */
	void shift(Stack& stack) noexcept;

protected: // Variables.
	/**
	 * The stack pointer.
	 */
	bzd::PtrType stack_{nullptr};

	/**
	 * The direction to which the stack grows.
	 */
	static const Direction direction_;
};

/**
 * A user stack is a custom stack, contained at a specific
 * address and with a specific size.
 */
class StackUser : public Stack
{
public: // Exposed functions.
	/**
	 * Reset the stack for initial use.
	 * This function sets the entry point in the stack.
	 *
	 * \param fct The entry point to be set.
	 */
	void reset(const FctPtrType fct) noexcept;

	/**
	 * Taint the stack
	 */
	void taint(UInt8Type pattern = 0xff)
	{
		for (int i = 0; i < size_; ++i)
		{
			stackBase_[i] = pattern;
		}
	}

protected: // Constructor.
	StackUser(UInt8Type* stack, const SizeType size) noexcept : Stack{}, stackBase_{stack}, size_{size} {}

protected: // Internal functions.
	/**
	 * Return the last element of the stack casted to a certain type.
	 */
	template <class T, unsigned int Align = 1>
	T* last()
	{
		return reinterpret_cast<T*>(&stackBase_[((size_ - sizeof(T)) / Align) * Align]);
	}

private: // Variables.
	UInt8Type* const stackBase_{nullptr};
	const SizeType size_{0};
};

} // namespace bzd::platform::interface

namespace bzd::platform {
/**
 * Define a user stack of a specific size.
 */
template <const SizeType N>
class Stack : public interface::StackUser
{
public: // Constructor.
	Stack() noexcept : interface::StackUser{data_, N} {}

private: // Variables.
	UInt8Type data_[N];
};
} // namespace bzd::platform
