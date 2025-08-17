/*

This Sketch is more for testing "new" and "delete" operators defined in
cores/esp8266/heap.cpp for capturing caller debug context than an example.

Covers C++ Exceptions "enable" and "disable" for the "new" and "delete" operators.

This test sketch assumes memalign enabled.


Notes,

From https://stackoverflow.com/a/4005664
  A std::nothrow_t deallocation function exists, but you cannot call it with a
  delete expression.

  The deallocation function is there for completeness. If a new expression fails
  because of an exception, the compiler needs to free the memory it allocated via
  operator new with a matching call to operator delete. So there needs to be an
  operator delete that accepts a std::nothrow_t, to allow this.

  (That is, in general, a new expression with the form new (args...) T will
  allocate memory with a call to operator new(sizeof(T), args...). To "match"
  means to call operator delete with the same arguments, except the first.)

  Note you can call the operator directly: operator delete(memory, std::nothrow);.
  However, a delete expression never calls a global deallocation function with
  additional parameters.
  ...
  Note that for C++14 onward there is an issue about symmetry:
  LWG 2458(https://wg21.cmeerw.net/lwg/issue2458) . – FrankHB CommentedAug 1 at 3:32

Exercise variations on new and delete operators listed in
  https://en.cppreference.com/w/cpp/memory/new/operator_new
    * Replaceable allocation functions
    * Replaceable non-throwing allocation functions
  https://en.cppreference.com/w/cpp/memory/new/operator_delete
    * replaceable usual deallocation functions
    * replaceable placement deallocation functions

If you see the linker error message:
  ".../gcc-gnu/libstdc++-v3/libsupc++/new_opa.cc:86: undefined reference to `memalign'"
your Arduino Sketch is using an operation that requires an aligned allocation.
You need to add line'-DUMM_ENABLE_MEMALIGN=1' to the build options.

*/

#include <umm_malloc/umm_malloc_cfg.h>
#include <umm_malloc/umm_heap_select.h>
#include <stdlib.h>  // aligned_alloc

#define PRINTF(fmt, ...) ets_uart_printf(pstrcpy(PSTR(fmt)), ##__VA_ARGS__)
#define PRINTF_LN(fmt, ...) ets_uart_printf(pstrcpy(PSTR((fmt "\r\n"))), ##__VA_ARGS__)
#define PRINTF_P(a, ...) ets_uart_printf(pstrcpy(a), ##__VA_ARGS__)

constexpr size_t buf_sz = 256u;
char bigbuf1[buf_sz];
char bigbuf2[buf_sz];

inline const char* pstrcpy(const char* p) {
  strcpy_P(bigbuf1, p);
  return bigbuf1;
}

inline const char* pstrcpy2(const char* p) {
  strcpy_P(bigbuf2, p);
  return bigbuf2;
}


#ifdef DEV_DEBUG_ABI_CPP
extern bool abi_new_print;
#else
bool abi_new_print;
#endif

#define POISON_BYTE (0xa5)
// Heap safe printf - does not need heap space.
// #define PRINTF ets_uart_printf

#ifndef _NOINLINE
#define _NOINLINE __attribute__((noinline))
#endif

constexpr size_t oom_sz = 128000u;
constexpr size_t ok_sz = 300u;

struct NEW_OOM {
  char buf[oom_sz];
};

struct NEW_OK {
  char buf[ok_sz];
};


void printSeparator(const char* str = NULL) {
  PRINTF_LN("\r\n==============================================================================");
  if (str) {
    PRINTF_P(str);
    PRINTF_LN();
  }
}
#define PRINT_SEPARATOR(a) printSeparator(__PRETTY_FUNCTION__)

void print_pass_fail(int err_count) {
  PRINTF_LN();
  PRINTF_LN("====================================");
  PRINTF_LN("====================================");
  PRINTF_LN("==  %-29s ==", "");
  if (err_count) {
    PRINTF_LN("== %5u %-24s ==", err_count, "Error(s) see '*' ");
  } else {
    PRINTF_LN("==  %-29s ==", "All Tests Passed");
  }
  PRINTF_LN("==  %-29s ==", "");
  PRINTF_LN("====================================");
  PRINTF_LN("====================================");
}

////////////////////////////////////////////////////////////////////////////////////////
// Test "new" and "delete" Operators
// Note, all array values are even. This detail is used later in the test functions.
//
// Delete enumeration values based on note numbers at
//   https://en.cppreference.com/w/cpp/memory/new/operator_delete
enum NewDelFn {
  del = 1,
  del_array = 2,
  del_align = 3,
  del_align_array = 4,
  del_sz = 5,
  del_sz_array = 6,
  del_sz_align = 7,
  del_sz_align_array = 8,
  del_nothrow = 9,
  del_nothrow_array = 10,
  del_nothrow_align = 11,
  del_nothrow_align_array = 12
};

void delete_new(void* new_ptr, size_t sz, size_t al, size_t delete_idx, bool array) {
  std::align_val_t alignment = std::align_val_t(al);
  if (new_ptr) {
    if (array && (delete_idx & 1u)) delete_idx++;
    abi_new_print = true;
    switch (delete_idx) {
      // ...
      case NewDelFn::del:
        ::operator delete(new_ptr);
        break;
      case NewDelFn::del_array:
        ::operator delete[](new_ptr);
        break;
      case NewDelFn::del_align:
        ::operator delete(new_ptr, alignment);
        break;
      case NewDelFn::del_align_array:
        ::operator delete[](new_ptr, alignment);
        break;
      // ... size
      case NewDelFn::del_sz:
        ::operator delete(new_ptr, sz);
        break;
      case NewDelFn::del_sz_array:
        ::operator delete[](new_ptr, sz);
        break;
      case NewDelFn::del_sz_align:
        ::operator delete(new_ptr, sz, alignment);
        break;
      case NewDelFn::del_sz_align_array:
        ::operator delete[](new_ptr, sz, alignment);
        break;
      // ... nothrow
      case NewDelFn::del_nothrow:
        ::operator delete(new_ptr, std::nothrow);
        break;
      case NewDelFn::del_nothrow_array:
        ::operator delete[](new_ptr, std::nothrow);
        break;
      case NewDelFn::del_nothrow_align:
        ::operator delete(new_ptr, alignment, std::nothrow);
        break;
      case NewDelFn::del_nothrow_align_array:
        ::operator delete[](new_ptr, alignment, std::nothrow);
        break;
      default:
        panic();
        break;
    }
    abi_new_print = false;
  }
}

// This works for C++ Exceptions enabled or disabled
void _NOINLINE test_new_nothrow(int& err_count, size_t sz, size_t alignment, int delete_idx, bool expect_success) {
  bool array = (0 == (delete_idx & 1u)) ? true : false;
  PRINT_SEPARATOR();

  void* new_ptr;
  abi_new_print = true;
  if (array) {
    if (alignment) {
      new_ptr = new (std::align_val_t(alignment), std::nothrow) char[sz];
    } else {
      new_ptr = new (std::nothrow) char[sz];
    }
  } else {
    if (alignment) {
      if (oom_sz == sz) {
        new_ptr = new (std::align_val_t(alignment), std::nothrow) struct NEW_OOM;
      } else {
        new_ptr = new (std::align_val_t(alignment), std::nothrow) struct NEW_OK;
      }
    } else {
      if (oom_sz == sz) {
        new_ptr = new (std::nothrow) struct NEW_OOM;
      } else {
        new_ptr = new (std::nothrow) struct NEW_OK;
      }
    }
  }
  abi_new_print = false;

  if (new_ptr) {
    // Success
    if (!expect_success) err_count++;
    if (alignment) {
      PRINTF_LN("%c %p, 'new (std::align_val_t(%u), std::nothrow)' worked, size %u", (expect_success) ? ' ' : '*', new_ptr, alignment, sz);
      if (0 != ((uintptr_t)new_ptr & (alignment - 1))) {
        err_count++;
        PRINTF_LN("* 'new (std::align_val_t(%u), std::nothrow)' allocation not aligned", alignment);
      }
    } else {
      PRINTF_LN("%c %p, 'new (std::nothrow)' worked, size %u", (expect_success) ? ' ' : '*', new_ptr, sz);
      if (0 != ((uintptr_t)new_ptr & (__STDCPP_DEFAULT_NEW_ALIGNMENT__ - 1))) {
        err_count++;
        PRINTF_LN("* 'new (std::nothrow)' allocation not aligned, default alignment %u", __STDCPP_DEFAULT_NEW_ALIGNMENT__);
      }
    }
  } else {
    if (expect_success) err_count++;
    if (alignment) {
      PRINTF_LN("%c NULL, 'new (std::align_val_t(%u), std::nothrow)' failed, size %u", (expect_success) ? '*' : ' ', alignment, sz);
    } else {
      PRINTF_LN("%c NULL, 'new (std::nothrow)' failed, size %u", (expect_success) ? '*' : ' ', sz);
    }
  }
  delete_new(new_ptr, sz, alignment, delete_idx, array);
}

// This only works for C++ Exceptions enabled
void _NOINLINE test_new_try_catch(int& err_count, size_t sz, size_t alignment, int delete_idx, bool expect_success) {
  bool array = (0 == (delete_idx & 1u)) ? true : false;
  PRINT_SEPARATOR();

#if defined(__cpp_exceptions)
  void* new_ptr = NULL;

  try {
    abi_new_print = true;
    if (array) {
      if (alignment) {
        new_ptr = new (std::align_val_t(alignment)) char[sz];
      } else {
        new_ptr = new char[sz];
      }
    } else {
      if (alignment) {
        if (oom_sz == sz) {
          new_ptr = new (std::align_val_t(alignment)) struct NEW_OOM;
        } else {
          new_ptr = new (std::align_val_t(alignment)) struct NEW_OK;
        }
      } else {
        if (oom_sz == sz) {
          new_ptr = new struct NEW_OOM;
        } else {
          new_ptr = new struct NEW_OK;
        }
      }
    }
  } catch (const std::exception& e) {
    abi_new_print = false;
    PRINTF_LN("  e.what() string address %p", e.what());
    String str = String(e.what());
    PRINTF_LN("  Catch Exception: \"%s\"", pstrcpy2(e.what()));
  } catch (...) {
    abi_new_print = false;
    PRINTF_LN("  Catch (...)");
  }
  abi_new_print = false;

  if (new_ptr) {
    if (!expect_success) err_count++;
    if (alignment) {
      PRINTF_LN("%c %p, 'new (std::align_val_t(%u))' worked, size %u", (expect_success) ? ' ' : '*', new_ptr, alignment, sz);
      if (0 != ((uintptr_t)new_ptr & (alignment - 1))) {
        err_count++;
        PRINTF_LN("* 'new (std::align_val_t(%u))' allocation not aligned", alignment);
      }
    } else {
      PRINTF_LN("%c %p, 'new' worked, size %u", (expect_success) ? ' ' : '*', new_ptr, sz);
      if (0 != ((uintptr_t)new_ptr & (__STDCPP_DEFAULT_NEW_ALIGNMENT__ - 1))) {
        err_count++;
        PRINTF_LN("* 'new' allocation not aligned, default alignment %u", __STDCPP_DEFAULT_NEW_ALIGNMENT__);
      }
    }
  } else {
    if (expect_success) err_count++;
    if (alignment) {
      PRINTF_LN("%c NULL, 'new (std::align_val_t(%u))' failed, size %u", (expect_success) ? '*' : ' ', alignment, sz);
    } else {
      PRINTF_LN("%c NULL, 'new' failed, size %u", (expect_success) ? '*' : ' ', sz);
    }
  }
  delete_new(new_ptr, sz, alignment, delete_idx, array);

#else
  (void)err_count, (void)sz, (void)expect_success, (void)alignment, (void)array, (void)delete_idx;
  PRINTF_LN("  Requires build option: C++ Exceptions: 'enabled'");
#endif
}


void print_build_options() {
#if DEV_DEBUG_ABI_CPP
  PRINTF_LN("  Built with DEV_DEBUG_ABI_CPP");
#if defined(__cpp_exceptions) && !(defined(DEBUG_ESP_OOM) || defined(DEBUG_ESP_PORT) || defined(DEBUG_ESP_WITHINISR) || defined(MIN_ESP_OOM))
  PRINTF_LN("*   This build used the LIBC's default operator \"new\"(/\"delete\") handler for C++ Exceptions, which bypassed");
  PRINTF_LN("*   our monitoring logic. The goal is to confirm the coverage of our added Heap debugging logic.");
  PRINTF_LN("*   To use the \"new\"/\"delete\" monitoring logic, define a debug option like MIN_ESP_OOM, DEBUG_ESP_PORT, etc.");
#endif
#else
  PRINTF_LN("* For complete operation, the test build requires option -DDEV_DEBUG_ABI_CPP=1");
#endif

#if defined(__cpp_exceptions)
  PRINTF_LN("  Built with C++ Exceptions: 'enabled'");
#else
  PRINTF_LN("  Built with C++ Exceptions: 'disabled'");
#endif

#if defined(UMM_POISON_CHECK_LITE)
  PRINTF_LN("  Built with UMM_POISON_CHECK_LITE");
#elif defined(UMM_POISON_CHECK)
  PRINTF_LN("  Built with UMM_POISON_CHECK");
#elif defined(UMM_POISON_NONE)
  PRINTF_LN("  Built with UMM_POISON_NONE");
#else
  PRINTF_LN("  No POISON options");
#endif

#if UMM_ENABLE_MEMALIGN
  PRINTF_LN("  Built with UMM_ENABLE_MEMALIGN, i.e. memalign present");
#endif
}
//
// test_new_nothrow
// test_new_try_catch
//
void run_new_delete_tests() {
  int err_count = 0;
  printSeparator();
  PRINTF_LN("Build Option Highlights:");
  print_build_options();

  //                    counter    size   align by   use deallocate fn           expect success
  test_new_nothrow(err_count, oom_sz, 0u, NewDelFn::del_nothrow, false);
  test_new_nothrow(err_count, oom_sz, 0u, NewDelFn::del_nothrow_array, false);
  test_new_nothrow(err_count, oom_sz, 128u, NewDelFn::del_nothrow_align, false);
  test_new_nothrow(err_count, oom_sz, 128u, NewDelFn::del_nothrow_align_array, false);
  test_new_nothrow(err_count, ok_sz, 0u, NewDelFn::del_nothrow, true);
  test_new_nothrow(err_count, ok_sz, 0u, NewDelFn::del_nothrow_array, true);
  test_new_nothrow(err_count, ok_sz, 128u, NewDelFn::del_nothrow_align, true);
  test_new_nothrow(err_count, ok_sz, 128u, NewDelFn::del_nothrow_align_array, true);

#if defined(__cpp_exceptions)
  test_new_try_catch(err_count, oom_sz, 0u, NewDelFn::del, false);
  test_new_try_catch(err_count, oom_sz, 0u, NewDelFn::del_array, false);
  test_new_try_catch(err_count, oom_sz, 128u, NewDelFn::del_align, false);
  test_new_try_catch(err_count, oom_sz, 128u, NewDelFn::del_align_array, false);
  test_new_try_catch(err_count, ok_sz, 0u, NewDelFn::del, true);
  test_new_try_catch(err_count, ok_sz, 0u, NewDelFn::del_array, true);
  test_new_try_catch(err_count, ok_sz, 128u, NewDelFn::del_align, true);
  test_new_try_catch(err_count, ok_sz, 128u, NewDelFn::del_align_array, true);

  test_new_try_catch(err_count, oom_sz, 0u, NewDelFn::del_sz, false);
  test_new_try_catch(err_count, oom_sz, 0u, NewDelFn::del_sz_array, false);
  test_new_try_catch(err_count, oom_sz, 128u, NewDelFn::del_sz_align, false);
  test_new_try_catch(err_count, oom_sz, 128u, NewDelFn::del_sz_align_array, false);
  test_new_try_catch(err_count, ok_sz, 0u, NewDelFn::del_sz, true);
  test_new_try_catch(err_count, ok_sz, 0u, NewDelFn::del_sz_array, true);
  test_new_try_catch(err_count, ok_sz, 128u, NewDelFn::del_sz_align, true);
  test_new_try_catch(err_count, ok_sz, 128u, NewDelFn::del_sz_align_array, true);
#endif
  print_pass_fail(err_count);
}


void setup() {
  Serial.begin(115200);
  delay(100);
  PRINTF_LN("\r\n\n\nOperator \"new\" / \"delete\" test");

  PRINTF_LN("  __STDCPP_DEFAULT_NEW_ALIGNMENT__: %u", __STDCPP_DEFAULT_NEW_ALIGNMENT__);

  print_build_options();
#if UMM_ENABLE_MEMALIGN
  void* new_ptr = aligned_alloc(128, 32);
  if (new_ptr) {
    PRINTF_LN("  %p, function \"aligned_alloc()\" worked", new_ptr);
    free(new_ptr);
  }
#endif
  processKey('?');
}

void loop(void) {
  if (Serial.available() > 0) {
    int hotKey = Serial.read();
    processKey(hotKey);
  }
}

void processKey(int hotKey) {
  switch (hotKey) {
    case 'r':
      run_new_delete_tests();
      break;

    case 'u':
      {
        HeapSelectDram ephemeral;
        umm_info(NULL, true);
        break;
      }
    //
    case 'A':
      abort();
      break;

    case 'R':
      PRINTF("Restart, ESP.restart(); ...\r\n");
      ESP.restart();
      break;

    case 'P':
      panic();
      break;

    case '\r':
      PRINTF_LN();

    case '\n':
      break;

    case '?':
      PRINTF_LN();
      PRINTF_LN("Press a key + <enter>");
      PRINTF_LN("  u    - umm_info for DRAM Heap");
      PRINTF_LN("  r    - Run \"new\"/\"delete\" tests");
      PRINTF_LN("  ?    - Print Help");
      PRINTF_LN();
      PRINTF_LN("Crash with:");
      PRINTF_LN("  A    - Abort");
      PRINTF_LN("  P    - Panic");
      PRINTF_LN("  R    - Restart, ESP.restart();");
      PRINTF_LN();
      break;

    default:
      PRINTF_LN("\"%c\" - Not an option?  / ? - help", hotKey);
      processKey('?');
      break;
  }
}
