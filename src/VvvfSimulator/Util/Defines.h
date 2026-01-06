#ifndef   VVVFSIMULATOR__UTIL_DEFINES_H
#define   VVVFSIMULATOR__UTIL_DEFINES_H

// Enable macro pushing and popping if supported by the compiler,
// falling back to regular define and undefine just in case it is not.
// Arguably, though, if we were to build this using such a compiler
// something's sure enough got to be wrong...
#if       defined(_MSC_VER) || defined(__clang__) || defined(__GNUC__)
    #define MAYBE_PUSH(name) _Pragma("push_macro(\"" #name "\")")
    #define MAYBE_POP(name)  _Pragma("pop_macro(\"" #name "\")")
#else
    #define MAYBE_PUSH(name) /* push not supported */
    #define MAYBE_POP(name)  /* pop not supported */
#endif // defined(_MSC_VER) || defined(__clang__) || defined(__GNUC__)

// Alternative implementation with more explicit compiler detection
#if   defined(_MSC_VER)
    // Microsoft Visual C++ (et al.)
    #define PUSH_MACRO(name) __pragma(push_macro(#name))
    #define POP_MACRO(name)  __pragma(pop_macro(#name))
#elif defined(__clang__) || defined(__GNUC__)
    // Clang or GCC
    #define PUSH_MACRO(name) _Pragma("push_macro(\"" #name "\")")
    #define POP_MACRO(name)  _Pragma("pop_macro(\"" #name "\")")
#else
    // Fallback for unsupported compilers
    #define PUSH_MACRO(name) /* push not supported */
    #define POP_MACRO(name)  /* pop not supported */
#endif

#endif // VVVFSIMULATOR__UTIL_DEFINES_H