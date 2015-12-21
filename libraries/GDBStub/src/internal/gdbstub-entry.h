#ifndef GDBSTUB_ENTRY_H
#define GDBSTUB_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

void gdbstub_init_debug_entry();
void gdbstub_do_break();
void gdbstub_icount_ena_single_step();
void gdbstub_save_extra_sfrs_for_exception();
void gdbstub_uart_entry();

int gdbstub_set_hw_breakpoint(int addr, int len);
int gdbstub_set_hw_watchpoint(int addr, int len, int type);
int gdbstub_del_hw_breakpoint(int addr);
int gdbstub_del_hw_watchpoint(int addr);

extern void* gdbstub_do_break_breakpoint_addr;

#ifdef __cplusplus
{
#endif

#endif