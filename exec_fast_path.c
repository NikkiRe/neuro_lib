// CONCRETE CODE TO ADD TO YOUR exec() FUNCTION
// Add this code at the VERY BEGINNING of your exec() function, before ANY other operations

// This is the exact code that will speed up the badarg test
// Place this right after: int exec(char *path, char **argv) {
//                         struct proc *p = myproc();

  // ========== FAST PATH VALIDATION - ADD THIS FIRST ==========
  uint64 uargv = (uint64)argv;
  
  // Fast check 1: Is argv pointer itself invalid? (catches badarg test early)
  // Single comparison - extremely fast
  if (uargv >= MAXVA) {
    return -1;  // Fail immediately, no expensive operations
  }
  
  // Fast check 2: Can we read the argv array?
  // Use uaddr_in_range for this (it already has fast path)
  if (!uaddr_in_range(p, uargv, sizeof(uint64))) {
    return -1;
  }
  
  // Fast check 3: Read argv[0] address
  uint64 arg0_addr;
  if (copyin(p->pagetable, (char*)&arg0_addr, uargv, sizeof(uint64)) < 0) {
    return -1;
  }
  
  // Fast check 4: Is argv[0] invalid? THIS IS THE KEY OPTIMIZATION
  // For badarg test: arg0_addr = 0xffffffff, so this catches it here
  // Single comparison - no page table walks, no memory allocation
  if (arg0_addr >= MAXVA) {
    return -1;  // Fail immediately - this is where badarg test fails fast
  }
  
  // Optional: Also check other common invalid addresses
  if (arg0_addr >= KERNBASE) {
    return -1;
  }
  // ========== END OF FAST PATH VALIDATION ==========
  
  // Only if we pass all fast checks, continue with expensive operations:
  // - Memory allocation
  // - Copying path and arguments
  // - Loading program
  // - etc.

// INTEGRATION EXAMPLE:
//
// int exec(char *path, char **argv) {
//   struct proc *p = myproc();
//   
//   // PASTE THE FAST PATH CODE ABOVE HERE
//   
//   // Your existing exec() code continues here...
//   // (memory allocation, copying, loading, etc.)
// }
