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
		protected:
			static bzd::interface::Map<int, T>& get(bzd::interface::Map<int, T>* registry = nullptr)
			{
				static bool isInitialized = false;
				static bzd::interface::Map<int, T>* instance = registry;
				bzd::assert::isTrue(instance != nullptr);
				bzd::assert::isTrue(!isInitialized || registry == nullptr, "Registry has already been declared!");
				isInitialized = true;
				return *instance;
			}

			constexpr Registry(bzd::interface::Map<int, T>& registry)
			{
				get(&registry);
			}
		};
	}

	namespace interface
	{
		template <class T>
		using Registry = impl::Registry<T>;
	}

	template <class T, SizeType Capacity>
	class Registry : public interface::Registry<T>
	{
	public:
		constexpr Registry()
				: interface::Registry<T>(registry_)
		{
		}

	protected:
		bzd::Map<int, T, Capacity> registry_;
	};

	template <class T>
	class Registry
	{
	public:
		constexpr Registry(const bzd::StringView str)
		{
			interface::Registry<T>::get().insert(45, 12);
		}
	};

/*
	template <class T, class Derived>
	class Registry : public Registry<T>
	{
	public:
		Registry(const bzd::StringView& name)
		{
			auto& registry = getInstance().registry_;
			const auto result = registry.find(name);
			bzd::assert::isTrue(!result);
			registry.insert(name, 12);
		}
	};
*/
}
