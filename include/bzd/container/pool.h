#pragma once

#include "bzd/types.h"
#include "bzd/utility/container_of.h"
#include "bzd/container/span.h"
#include "bzd/assert.h"

#include <iostream>

namespace bzd
{
	namespace impl
	{
		template <class T>
		class Pool
		{
		protected:
			static constexpr const SizeType npos = static_cast<SizeType>(-1);
			struct Element
			{
				T container_;
				SizeType next_;
			};

		public:
			constexpr Pool(const bzd::Span<Element>& data) noexcept
					: data_(data), free_(0)
			{
				// Build the free list
				for (SizeType i = 0; i < data_.size() - 1; ++i)
				{
					data_[i].next_ = i + 1;
				}
				data_.back().next_ = npos;
			}

			constexpr SizeType capacity() const noexcept
			{
				return data_.size();
			}

			constexpr bool empty() const noexcept
			{
				return (free_ == npos);
			}

			/**
			 * Release an element from the pool
			 */
			constexpr void release(T& container) noexcept
			{
				bzd::assert::isTrue(reinterpret_cast<bzd::IntPtrType>(&container) >= reinterpret_cast<bzd::IntPtrType>(data_.data()));
				bzd::assert::isTrue(reinterpret_cast<bzd::IntPtrType>(&container) < reinterpret_cast<bzd::IntPtrType>(data_.data() + data_.size()));

				Element* item = containerOf(&container, &Element::container_);
				item->next_ = free_;
				free_ = (item - data_.data());
			}

			/**
			 * Reserve an element from the free list (if any)
			 */
			constexpr T& reserve() noexcept
			{
				bzd::assert::isTrue(!empty()); //CONSTEXPR_STRING_VIEW("Pool empty, capacity: {}"), data_.size());

				Element& item = data_[free_];
				free_ = item.next_;
				return item.container_;
			}

			void toStream(std::ostream& os)
			{
				os << "free=";
				SizeType current = free_;
				while (current != npos)
				{
					os << current << ",";
					current = data_[current].next_;
				}
			}

		private:
			bzd::Span<Element> data_;
			SizeType free_;
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
