// Usage: INITIALIZER(initialize) { /*source code to be run on load */ }
// You can use atexit(...) hook to get a finalizer.

#ifdef _MSC_VER
// VisualStudio Version
#define CCALL __cdecl
#pragma section(".CRT$XCU",read)
#define INITIALIZER(f)                                             \
   static void __cdecl f(void);                                    \
   __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
   static void __cdecl f(void)
#elif defined(__GNUC__)
// GCC Version
#define CCALL
#define INITIALIZER(f)                                             \
   static void f(void) __attribute__((constructor));               \
   static void f(void)
#endif
