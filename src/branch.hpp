#ifndef BRANCH_H
#define BRANCH_H

// Use these macros wisely, they give a power that can result in disaster or
// total success

#ifdef __GNUC__
// These macros will tell gcc to optimize for these statements that may occour
// many or few times. clang is also smart and supports these builtins too
#	define likely(x)		__builtin_expect(x, 1)
#	define unlikely(x)		__builtin_expect(x, 0)
#else
// Other compilers do not have the intristrics so we just throw in the raw
// expression and expect that they are nuts
#	define likely(x)		x
#	define unlikely(x)		x
#endif

#endif