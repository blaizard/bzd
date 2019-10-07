#pragma once

#include "include/utility.h"

namespace bzd
{
	namespace Log
	{
		enum class Level
		{
			CRITICAL = 0,
			ERROR = 1,
			WARNING = 2,
			INFO = 3,
			DEBUG = 4
		};

		namespace impl
		{
			class Buffer : public SingletonImpl<Buffer>
			{
			public:

			};
		}

		void print() noexcept
		{

		}
	}
}
