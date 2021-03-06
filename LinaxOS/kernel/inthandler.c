// Interrupt handler.

// These functions are called from asm_inthandlerXX, defined in naskfunc.s
// There are other `inthandler` functions in other source files, which are
// related to their own components (keyboard, mouse, timer).

#include "console.h"
#include "mtask.h"
#include "naskfunc.h"
#include "stdio.h"

// Interrupt 00 : Division by zero.
int* inthandler00() {
  TASK* task = task_now();
  CONSOLE* cons = task->cons;
  cons_putstr0(cons, "\nINT 00 :\n Division by zero.\n");
  return &task->tss.esp0;  // Abort
}

// Interrupt 07 : FPU exception.
int* inthandler07(int *esp) {
  (void)esp;
  TASK *now = task_now();
  io_cli();
  io_clts();
  if (taskctl->task_fpu != now) {
    if (taskctl->task_fpu != NULL)
      io_fnsave(taskctl->task_fpu->fpu);
    io_frstor(now->fpu);
    taskctl->task_fpu = now;
  }
  io_sti();
  return 0;
}

// IRQ-0c : Stack exception.
int* inthandler0c(int* esp) {
  // esp[ 0] = edi  : esp[0~7] are given from asm_inthandler, pushal
  // esp[ 1] = esi
  // esp[ 2] = ebp
  // esp[ 4] = ebx
  // esp[ 5] = edx
  // esp[ 6] = ecx
  // esp[ 7] = eax
  // esp[ 8] = ds   : esp[8~9] are given from asm_inthandler, push
  // esp[ 9] = es
  // esp[10] = error code (0)
  // esp[11] = eip
  // esp[12] = cs
  // esp[13] = eflags
  // esp[14] = esp  : esp for application
  // esp[15] = ss   : ss for application
  TASK* task = task_now();
  CONSOLE* cons = task->cons;
  cons_printf(cons,
              "\nINT 0C :\n Stack Exception.\n"
              "EIP = %08x\n",
              esp[11]);
  return &task->tss.esp0;  // Abort
}

// Interrupt 0d : General protected exception.
int* inthandler0d(int* esp) {
  TASK* task = task_now();
  CONSOLE* cons = task->cons;
  cons_printf(cons,
              "\nINT 0D :\n General Protected Exception.\n"
              "EIP = %08x\n",
              esp[11]);
  return &task->tss.esp0;  // Abort
}
