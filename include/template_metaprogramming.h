#pragma once

#include "include/types.h"
#include "include/utility.h"

namespace bzd
{
	namespace tmp
	{
		// Type

		template <class T>
		struct Type { using type = T; };

		// ChooseNth
		// Choose N-th element in list <Type<T>...>

		namespace impl
		{
			template <SizeType N, class... Ts>
			struct ChooseNth;

			template <SizeType N, class H, class... Ts>
			struct ChooseNth<N, H, Ts...> : ChooseNth<N - 1, Ts...> {};

			template <class H, class... Ts>
			struct ChooseNth<0, H, Ts...> : Type<H> {};
		}

		template <SizeType N, class... Ts>
		using ChooseNth = typename impl::ChooseNth<N, Ts...>::type;

		// Contains

		namespace impl
		{
			template <class T, class U, class... Ts>
			struct Contains
			{
				static constexpr const bool value = Contains<T, U>::value || Contains<T, Ts...>::value;
			};

			template <class T, class U>
			struct Contains<T, U>
			{
				static constexpr const bool value = bzd::typeTraits::isSame<T, U>::value;
			};
		}

		template <class T, class... Ts>
		using Contains = typename impl::Contains<T, Ts...>;

		// Find
		// Return the index of the element or -1 if none

		namespace impl
		{
			template <SizeType N, class T, class U, class... Ts>
			struct Find
			{
				static constexpr const int value = (Find<N, T, U>::value >= 0) ? Find<N, T, U>::value : Find<N + 1, T, Ts...>::value;
			};

			template <SizeType N, class T, class U>
			struct Find<N, T, U>
			{
				static constexpr const int value = (bzd::typeTraits::isSame<T, U>::value) ? static_cast<int>(N) : -1;
			};
		}

		template <class T, class... Ts>
		using Find = typename impl::Find<0, T, Ts...>;

		// TypeList

		template <class... Ts>
		struct TypeList
		{
			using type = TypeList;
			static constexpr const SizeType size = sizeof...(Ts);

			template <SizeType N>
			using ChooseNth = typename impl::ChooseNth<N, Ts...>::type;

			template <class T>
			using Contains = typename impl::Contains<T, Ts...>;

			template <class T>
			using Find = typename impl::Find<0, T, Ts...>;
		};

		// Union

		namespace impl
		{
			template <class T, class... Ts>
			union UnionConstexpr
			{
			public:
				// By default initialize the dummy element only, a constexpr constructor must initialize something
				constexpr UnionConstexpr() : next_{} {}

				template <class U, typename typeTraits::enableIf<!bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr UnionConstexpr(U&& value) : next_{bzd::forward<U>(value)} {}
				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr UnionConstexpr(const U& value) : value_{value} {}

				template <class U, typename typeTraits::enableIf<!bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline U& get() { return next_.template get<U>(); }

				template <class U, typename typeTraits::enableIf<!bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline const U& get() const { return next_.template get<U>(); }

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline U& get() { return value_; }

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline const U& get() const { return value_; }

			protected:
				T value_;
				UnionConstexpr<Ts...> next_;
			};

			template <class T, class... Ts>
			union Union
			{
			public:
				// By default initialize the dummy element only, a constexpr constructor must initialize something
				constexpr Union() : next_{} {}
				~Union() {}; // This is the only difference with a constexpr Union

				template <class U, typename typeTraits::enableIf<!bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr Union(U&& value) : next_{bzd::forward<U>(value)} {}
				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr Union(const U& value) : value_{value} {}

				template <class U, typename typeTraits::enableIf<!bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline U& get() { return next_.template get<U>(); }

				template <class U, typename typeTraits::enableIf<!bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline const U& get() const { return next_.template get<U>(); }

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline U& get() { return value_; }

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline const U& get() const { return value_; }

			protected:
				T value_;
				Union<Ts...> next_;
			};

			template <class T>
			union UnionConstexpr<T>
			{
			public:
				constexpr UnionConstexpr() : dummy_{} {}

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr UnionConstexpr(const U& value) : value_{value} {}

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline U& get() { return value_; }

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline const U& get() const { return value_; }

			protected:
				T value_;
				struct {} dummy_;
			};

			template <class T>
			union Union<T>
			{
			public:
				constexpr Union() : dummy_{} {}
				~Union() {}

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr Union(const U& value) : value_{value} {}

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline U& get() { return value_; }

				template <class U, typename typeTraits::enableIf<bzd::typeTraits::isSame<T, U>::value>::type* = nullptr>
				constexpr inline const U& get() const { return value_; }

			protected:
				T value_;
				struct {} dummy_;
			};
		}

		template <class... Ts>
		using Union = impl::Union<Ts...>;
		template <class... Ts>
		using UnionConstexpr = impl::UnionConstexpr<Ts...>;
	}
}
