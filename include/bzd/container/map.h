#pragma once

#include "bzd/types.h"
#include "bzd/container/vector.h"
#include "bzd/container/expected.h"

namespace bzd
{
	namespace impl
	{
		/**
		 * \brief Flat map implementation.
		 */
		template <class K, class V>
		class Map
		{
		protected:
			struct Element
			{
				K key_;
				V value_;
			};

		public:
			constexpr explicit Map(bzd::interface::Vector<Element>& data) noexcept
					: data_(data)
			{
			}

			void insert(const K& key, const V& /*value*/)
			{
			}

		protected:
			bzd::interface::Vector<Element>& data_;
		};
	}

	namespace interface
	{
		template <class K, class V>
		using Map = impl::Map<K, V>;
	}

	template <class K, class V, SizeType N>
	class Map : public interface::Map<K, V>
	{
	private:
		using typename interface::Map<K, V>::Element;

	public:
		constexpr Map()
				: interface::Map<K, V>(data_)
		{
		}

	protected:
		bzd::Vector<Element, N> data_;
	};
}
