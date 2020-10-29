#pragma once

#include "bzd/container/map.h"
#include "bzd/container/result.h"
#include "bzd/container/string_view.h"
#include "bzd/core/assert.h"
#include "bzd/types.h"
#include "bzd/utility/singleton.h"

namespace bzd::impl {
template <class T>
class Registry
{
public:
	using KeyType = bzd::StringView;
	using MapType = bzd::interface::Map<KeyType, T*>;

protected:
	constexpr Registry(MapType& registry) { get(&registry); }

public:
	static constexpr bzd::Result<MapType&, const char*> get() { return get(nullptr); }

private:
	static bzd::Result<MapType&, const char*> get(MapType* registry)
	{
		static MapType* instance = registry;
		if (instance == nullptr)
		{
			return bzd::makeError("Registry was not initialized");
		}
		return *instance;
	}
};
} // namespace bzd::impl

namespace bzd::interface {
template <class T>
using Registry = impl::Registry<T>;
}

namespace bzd {
/**
 * \brief Fixed-size registry object.
 *
 * A registry is a singleton object acting as a key-value store for object
 * instances. The size of the registry must be defined before populating the
 * object, to do so, it is preferable to do it at startup during dynamic
 * variable initialization.
 *
 * \code
 * // Declares a registry which can contain up to 3 doubles
 * bzd::Registry<double>::Declare<3> registry_;
 * \endcode
 *
 * The following snipet shows how to register objects within this registry.
 * Note that no more of N objects (number specified during registry declaration)
 * can be added.
 *
 * \code
 * // Creates and insert 3 objects within the registry
 * bzd::Registry<double>::Register<> registry_{"A", 42};
 * bzd::Registry<double>::Register<> registry_{"B", -1};
 * bzd::Registry<double>::Register<> registry_{"C", 3.14};
 * \endcode
 *
 * Note, this initialization scheme can be imediatly followed by object
 * registrations, as order of global variables in a single translation unit
 * (source file) are initialized in the order in which they are defined.
 */
template <class Interface>
class Registry
{
protected:
	using KeyType = typename interface::Registry<Interface>::KeyType;

public:
	/**
	 * \brief Declaration object for a fixed-size registry object.
	 */
	template <SizeType Capacity>
	class Declare : public interface::Registry<Interface>
	{
	public:
		constexpr Declare() : interface::Registry<Interface>(registry_) {}

	protected:
		bzd::Map<KeyType, Interface*, Capacity> registry_;
	};

	/**
	 *  \brief Register a new object to its registery previously delcared.
	 */
	template <class T = Interface>
	class Register
	{
	public:
		template <class... Args>
		constexpr Register(const KeyType& str, Args&&... args) : object_{bzd::forward<Args>(args)...}
		{
			auto result = interface::Registry<Interface>::get();
			bzd::assert::isTrue(result);
			(*result).insert(str, static_cast<Interface*>(&object_));
		}

	private:
		T object_;
	};

	/**
	 * \brief Registry accessor.
	 */
	static constexpr Interface& get(const KeyType& key)
	{
		auto result = interface::Registry<Interface>::get();
		bzd::assert::isTrue(result);
		return *(*result)[key];
	}

	/**
	 * \brief Get a ressource or create one if it does not exists.
	 * In order to be created, the ressource must have a default constructor. This is a limitation
	 * to ensure reproductability.
	 */
	template <class T = Interface>
	static Interface& getOrCreate(const KeyType& key)
	{
		auto result = interface::Registry<Interface>::get();
		if (!result || !(*result).contains(key))
		{
			static T defaultObject{};
			return static_cast<Interface&>(defaultObject);
		}

		return *(*result)[key];
	}
};
} // namespace bzd
