#pragma once

#include "bzd/utility/singleton.h"
#include "bzd/container/map.h"
#include "bzd/container/string_stream.h"
#include "bzd/core/assert.h"
#include "bzd/types.h"

namespace bzd
{
	namespace impl
	{
		template <class T>
		class Registry
		{
		public:
			using KeyType = bzd::StringView;
			using MapType = bzd::interface::Map<KeyType, T>;

		protected:
			constexpr Registry(MapType& registry)
			{
				get(&registry);
			}

		public:
			static constexpr MapType& get()
			{
				return get(nullptr);
			}

		private:
			static MapType& get(MapType* registry)
			{
				static MapType* instance = registry;
				bzd::assert::isTrue(instance != nullptr, "Registry was not initialized");
				return *instance;
			}
		};
	}

	namespace interface
	{
		template <class T>
		using Registry = impl::Registry<T>;
	}

	template <class T>
	class Registry
	{
	protected:
		using KeyType = typename interface::Registry<T>::KeyType;

	public:
		template <class... Args>
		constexpr Registry(const bzd::StringView& str, Args&&... args)
		{
			interface::Registry<T>::get().insert(str, T{bzd::forward<Args>(args)...});
		}

		static constexpr T& get(const KeyType& key)
		{
			return interface::Registry<T>::get()[key];
		}
	
		/**
		 * \brief Fixed-size object registry.
		 */
		template <SizeType Capacity>
		class Declare : public interface::Registry<T>
		{
		public:
			constexpr Declare()
					: interface::Registry<T>(registry_)
			{
			}

		protected:
			bzd::Map<KeyType, T, Capacity> registry_;
		};
	};
}
