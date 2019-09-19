#pragma once

#include "include/types.h"

namespace async
{
	namespace interface
	{
		/**
		 * Application stack definition
		 */
		class Stack
		{
		public:
			void reset(const fct_ptr_type fct, void* context);

		protected:
			Stack(byte_type* stack, const size_type size)
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

			enum class Direction : byte_type
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

			byte_type* const stackBase_;
			const size_type size_;
			byte_type* stack_;
			static const Direction direction_;
		};
	}
}
