// Optimization for badarg test: Fast validation of exec() arguments
// This should be integrated into your exec() implementation

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "defs.h"

// Fast validation: Check if a user address is obviously invalid
// This catches cases like 0xffffffff immediately without page table walks
static inline int
is_obviously_invalid_addr(uint64 uva)
{
  // Fast path: Check if address is >= MAXVA (most common invalid case)
  // For 0xffffffff, this will return 1 immediately
  if (uva >= MAXVA)
    return 1;
  
  // Also check for NULL (though exec might handle this separately)
  // and check for addresses in kernel space
  if (uva >= KERNBASE)
    return 1;
    
  return 0;
}

// Fast validation of argv array for exec()
// Returns 1 if valid, 0 if invalid
// This should be called BEFORE any expensive operations in exec()
static int
validate_exec_argv(struct proc *p, uint64 uargv)
{
  // First, fast check if argv pointer itself is invalid
  if (is_obviously_invalid_addr(uargv))
    return 0;
  
  // For badarg test: argv[0] = 0xffffffff will be caught here
  // We need to check if we can read the argv array
  // But we want to avoid expensive page table walks
  
  // Fast path: If argv is in an obviously invalid range, fail immediately
  // This catches the badarg test case (0xffffffff) instantly
  
  // Read argv[0] address (first element of argv array)
  // But first validate that uargv itself is readable
  if (!uaddr_in_range(p, uargv, sizeof(uint64)))
    return 0;
  
  uint64 arg0_addr;
  if (copyin(p->pagetable, (char*)&arg0_addr, uargv, sizeof(uint64)) < 0)
    return 0;
  
  // Fast check: If argv[0] is obviously invalid, fail immediately
  // This is the key optimization for badarg test
  if (is_obviously_invalid_addr(arg0_addr))
    return 0;
  
  // Continue with normal validation for other arguments...
  // (This is just the fast path - full validation continues after)
  
  return 1;
}

// OPTIMIZED exec() pattern:
// 
// int exec(char *path, char **argv) {
//   struct proc *p = myproc();
//   
//   // FAST PATH: Validate argv addresses BEFORE any expensive operations
//   uint64 uargv = (uint64)argv;
//   if (!validate_exec_argv(p, uargv)) {
//     return -1;  // Fail fast - no memory allocation, no page table walks
//   }
//   
//   // Only now do expensive operations:
//   // - Allocate memory
//   // - Copy arguments
//   // - Load program
//   // etc.
// }
