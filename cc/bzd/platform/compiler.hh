#pragma once

// ---- Identify the compiler ----

#define COMPILER_GCC 0
#define COMPILER_CLANG 0

#if (defined(__GNUC__) && !defined(__clang__))

/// If the compiler is a GCC base compiler.
#undef COMPILER_GCC
#define COMPILER_GCC 1

#elif (defined(__clang__))

/// If the compiler is a CLANG based compiler.
#undef COMPILER_CLANG
#define COMPILER_CLANG 1

#endif
