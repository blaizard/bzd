#pragma once

namespace bzd
{
	namespace typeTraits
	{
		// integralConstant

		template <class T, T v>
		struct integralConstant
		{
			static constexpr T value = v;
			typedef T valueType;
			typedef integralConstant type;
			constexpr operator valueType() const noexcept
			{
				return value;
			}
			constexpr valueType operator()() const noexcept
			{
				return value;
			}
		};

		// trueType

		typedef integralConstant<bool, true> trueType;

		// falseType

		typedef integralConstant<bool, false> falseType;
	}
}
