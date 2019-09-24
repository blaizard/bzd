#pragma once

#include "include/types.h"
#include "include/utils.h"
#include "include/container/array.h"

#include <iostream>

namespace bzd
{
	/**
	 * A Pool is a fixed memory buffer containing fixed size elements that can be taken or released.
	 */
	template <class T, SizeType N>
	class Pool
	{
	public:
		struct ItemType
		{
			T container_;
			ItemType* next_;
		};

	public:
		Pool() noexcept
		{
			// Build the free list
			ItemType** curFree = &free_;
			for (auto& item : data_)
			{
				*curFree = &item;
				curFree = &((*curFree)->next_);
			}
			*curFree = nullptr;
		}

		/**
		 * Release an element from the pool
		 */
		void release(T* container) noexcept
		{
			// assert(T >= &data_[0]);
			// assert(T < &data_[N]);

			ItemType* item = containerOf(container, &ItemType::container_);
			item->next_ = free_;
			free_ = item;
		}

		/**
		 * Reserve an element from the free list (if any)
		 */
		T* reserve() noexcept
		{
			if (free_)
			{
				ItemType* item = free_;
				free_ = free_->next_;
				return &item->container_;
			}

			return nullptr;
		}

		void toStream(std::ostream& os)
		{
			os << "free=";
			ItemType* curFree = free_;
			while (curFree)
			{
				os << (curFree - &data_[0]) << ",";
				curFree = curFree->next_;
			}
		}

	private:
		bzd::Array<ItemType, N> data_;
		ItemType* free_;
	};
}
