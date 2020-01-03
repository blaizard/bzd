#pragma once

#include "bzd/container/map.h"
#include "bzd/container/string_stream.h"
#include "bzd/core/assert.h"
#include "bzd/types.h"
#include "bzd/utility/singleton.h"

namespace bzd {
namespace impl {
template <class T>
class Registry
{
  public:
	using KeyType = bzd::StringView;
	using MapType = bzd::interface::Map<KeyType, T *>;

  protected:
	constexpr Registry(MapType &registry) { get(&registry); }

  public:
	static constexpr MapType &get() { return get(nullptr); }

  private:
	static MapType &get(MapType *registry)
	{
		static MapType *instance = registry;
		bzd::assert::isTrue(instance != nullptr, "Registry was not initialized");
		return *instance;
	}
};
}  // namespace impl

namespace interface {
template <class T>
using Registry = impl::Registry<T>;
}

namespace declare {
/**
 * \brief Declaration object for a fixed-size registry object.
 */
template <class T, SizeType Capacity>
class Registry : public interface::Registry<T>
{
  protected:
	using typename interface::Registry<T>::KeyType;

  public:
	constexpr Registry() : interface::Registry<T>(registry_) {}

  protected:
	bzd::Map<KeyType, T *, Capacity> registry_;
};
}  // namespace declare

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
 * bzd::declare::Registry<double, 3> registry_;
 * \endcode
 *
 * Note, this initialization scheme can be imediatly followed by object
 * registrations, as order of global variables in a single translation unit
 * (source file) are initialized in the order in which they are defined.
 */
template <class Interface, class T = Interface>
class Registry
{
  protected:
	using KeyType = typename interface::Registry<Interface>::KeyType;

  public:
	template <class... Args>
	constexpr Registry(const KeyType &str, Args &&... args) : object_{bzd::forward<Args>(args)...}
	{
		interface::Registry<Interface>::get().insert(str, &object_);
	}

	/**
	 * \brief Registry accessor.
	 */
	static constexpr Interface &get(const KeyType &key) { return *interface::Registry<Interface>::get()[key]; }

  private:
	T object_;
};
}  // namespace bzd
