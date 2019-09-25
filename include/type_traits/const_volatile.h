#pragma once

namespace bzd
{
	namespace typeTraits
	{

		// removeConst

		template <class T> struct removeConst { typedef T type; };
		template <class T> struct removeConst<const T> { typedef T type; };
 
		// removeVolatile

		template <class T> struct removeVolatile { typedef T type; };
		template <class T> struct removeVolatile<volatile T> { typedef T type; };

		// removeCV

		template <class T>
		struct removeCV
		{
			typedef typename removeVolatile<typename removeConst<T>::type>::type type;
		};
	}
}
