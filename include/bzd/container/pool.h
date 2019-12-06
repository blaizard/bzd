#pragma once

#include "bzd/types.h"
#include "bzd/utility/container_of.h"
#include "bzd/container/span.h"

#include <iostream>

namespace bzd
{
	namespace impl
	{
		template <class T>
		class Pool
		{
		protected:
			struct Element
			{
				T container_;
				Element* next_;
			};

		public:
			constexpr Pool(const bzd::Span<Element>& data) noexcept
					: data_(data)
			{
				// Build the free list
				Element** curFree = &free_;
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
			constexpr void release(T* container) noexcept
			{
				// assert(T >= &data_[0]);
				// assert(T < &data_[N]);

				Element* item = containerOf(container, &Element::container_);
				item->next_ = free_;
				free_ = item;
			}

			/**
			 * Reserve an element from the free list (if any)
			 */
			constexpr T* reserve() noexcept
			{
				if (free_)
				{
					Element* item = free_;
					free_ = free_->next_;
					return &item->container_;
				}

				return nullptr;
			}

			void toStream(std::ostream& os)
			{
				os << "free=";
				Element* curFree = free_;
				while (curFree)
				{
					os << (curFree - &data_[0]) << ",";
					curFree = curFree->next_;
				}
			}

		private:
			bzd::Span<Element> data_;
			Element* free_ = nullptr;
		};
	}

	namespace interface
	{
		template <class T>
		using Pool = impl::Pool<T>;
	}

	/**
	 * A Pool is a fixed memory buffer containing fixed size elements that can be taken or released.
	 */
	template <class T, SizeType N>
	class Pool : public interface::Pool<T>
	{
	private:
		using typename interface::Pool<T>::Element;

	public:
		constexpr Pool()
				: interface::Pool<T>(bzd::Span<Element>(data_, N))
		{
		}

	private:
		Element data_[N];
	};
}
