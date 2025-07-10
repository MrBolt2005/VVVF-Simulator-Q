#ifndef   VVVFSIMULATOR__UTIL_DEFINES_H
#define   VVVFSIMULATOR__UTIL_DEFINES_H

// Enable macro pushing and popping if supported by the compiler,
// falling back to regular define and undefine just in case it is not.
// Arguably, though, if we were to build this using such a compiler
// something's sure enough got to be wrong...
#if       defined(_MSC_VER) || defined(__clang__) || defined(__GNUC__)
	#define MAYBE_PUSH(name) _Pragma("push_macro(\"" #name "\")")
	#define MAYBE_POP(name) _Pragma("pop_macro(\""" #name "\")")
#else
	#define MAYBE_PUSH(name)
	#define MAYBE_POP(name)
#endif // defined(_MSC_VER) || defined(__clang__) || defined(__GNUC__)

#endif // VVVFSIMULATOR__UTIL_DEFINES_H
