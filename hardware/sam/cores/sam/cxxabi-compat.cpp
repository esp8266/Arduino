/* We compile with nodefaultlibs, so we need to provide an error
 * handler for an empty pure virtual function */
extern "C" void __cxa_pure_virtual(void) {
    while(1)
        ;
}
