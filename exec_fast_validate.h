// Fast validation helpers for exec() to speed up badarg test
// Include this in your sysfile.c or wherever exec() is implemented

#ifndef EXEC_FAST_VALIDATE_H
#define EXEC_FAST_VALIDATE_H

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"

// Ultra-fast check: Returns 1 if address is obviously invalid, 0 if might be valid
// This is a single comparison - fastest possible check
// Catches 0xffffffff (badarg test) instantly
static inline int
is_addr_obviously_invalid(uint64 uva)
{
  // Single comparison - this catches >= MAXVA addresses immediately
  // For badarg test: 0xffffffff >= MAXVA, so returns 1 instantly
  return uva >= MAXVA;
}

// Fast validation of argv array pointer and first argument
// Returns -1 if invalid, 0 if valid
// This should be called at the VERY START of exec(), before ANY expensive operations
static int
fast_validate_exec_argv(struct proc *p, uint64 uargv)
{
  // Step 1: Ultra-fast check of argv pointer itself
  if (is_addr_obviously_invalid(uargv))
    return -1;
  
  // Step 2: Validate we can read the argv array (but use fast path)
  if (!uaddr_in_range(p, uargv, sizeof(uint64)))
    return -1;
  
  // Step 3: Read argv[0] address
  uint64 arg0_addr;
  if (copyin(p->pagetable, (char*)&arg0_addr, uargv, sizeof(uint64)) < 0)
    return -1;
  
  // Step 4: ULTRA-FAST check of argv[0] - THIS IS THE KEY OPTIMIZATION
  // For badarg test: arg0_addr = 0xffffffff, so this returns -1 immediately
  // This avoids all subsequent expensive operations
  if (is_addr_obviously_invalid(arg0_addr))
    return -1;
  
  return 0;  // Passed fast validation, continue with normal checks
}

#endif // EXEC_FAST_VALIDATE_H

// INTEGRATION INSTRUCTIONS:
//
// In your exec() function (likely in sysfile.c), add this at the VERY BEGINNING:
//
// int exec(char *path, char **argv) {
//   struct proc *p = myproc();
//   uint64 uargv = (uint64)argv;
//   
//   // FAST PATH: Validate argv before ANY expensive operations
//   if (fast_validate_exec_argv(p, uargv) < 0) {
//     return -1;  // Fail immediately - no memory allocation, no page walks
//   }
//   
//   // Only now proceed with expensive operations:
//   // - Memory allocation
//   // - Copying arguments
//   // - Loading program
//   // etc.
// }
