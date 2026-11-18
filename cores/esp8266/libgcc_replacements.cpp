/* replaces libgcc weak functions with stubs when exceptions are disabled
 * ref. gcc-gnu/libgcc/crtstuff.c
 *
 * in case something calls them w/ -fno-exceptions
 */

#if !defined(__cpp_exceptions)
#include <cstdlib>  // std::abort()
#endif

extern "C" {

#if !defined(__cpp_exceptions)
void __register_frame_info (const void*, struct object*) {
    std::abort();
}

void __register_frame_info_bases (const void*, struct object*, void*, void*) {
    std::abort();
}

void* __deregister_frame_info (const void*) {
    std::abort();
}

void* __deregister_frame_info_bases (const void*) {
    std::abort();
}
#endif

}  // extern "C"
