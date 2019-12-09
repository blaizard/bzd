#pragma once

#include "bzd/types.h"

namespace bzd
{
	namespace interface
	{
		/**
		 * Application stack definition
		 */
		class Stack
		{
		public:
			void reset(const FctPtrType fct, void* context);

		protected:
			Stack(UInt8Type* stack, const SizeType size)
				: stackBase_(stack), size_(size)
			{
			}

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

			friend class Task;

			UInt8Type* const stackBase_;
			const SizeType size_;
			UInt8Type* stack_;
			static const Direction direction_;
		};
	}
}
