// Optimized version of uaddr_in_range for faster badarg test execution
// Replace the existing uaddr_in_range function with this optimized version

static inline int
uaddr_in_range(struct proc *p, uint64 uva, uint64 len)
{
  // OPTIMIZATION 1: Check MAXVA FIRST (most common invalid case)
  // This catches 0xffffffff immediately without any other checks
  // Reordered to be the very first check for maximum speed
  if (uva >= MAXVA)
    return 0;
  
  // OPTIMIZATION 2: Check overflow before doing addition
  // This prevents potential issues and is still fast
  if (len > 0) {
    if (uva + len < uva)  // Overflow check
      return 0;
    if (uva + len > MAXVA)  // Range check
      return 0;
  }
  
  // OPTIMIZATION 3: Fast kernel space check (if KERNBASE < MAXVA)
  // This catches kernel addresses quickly
  if (uva >= KERNBASE)
    return 0;
  
  // For addresses in valid user range, let copyin/copyout do full validation
  // This is safe because they perform complete page table checks
  return 1;
}

// Additional optimization: Ultra-fast check for single invalid addresses
// Use this in exec() BEFORE calling uaddr_in_range for even faster rejection
static inline int
is_invalid_addr_fast(uint64 uva)
{
  // Single comparison - fastest possible check
  // Catches 0xffffffff and other >= MAXVA addresses instantly
  return uva >= MAXVA || uva >= KERNBASE;
}

// Usage in exec() should be:
// 
// int exec(char *path, char **argv) {
//   // ULTRA-FAST PATH: Check argv pointer immediately
//   uint64 uargv = (uint64)argv;
//   if (is_invalid_addr_fast(uargv))
//     return -1;
//   
//   // Read argv[0] address
//   uint64 arg0;
//   if (copyin(p->pagetable, (char*)&arg0, uargv, sizeof(uint64)) < 0)
//     return -1;
//   
//   // ULTRA-FAST PATH: Check argv[0] immediately (catches badarg test)
//   if (is_invalid_addr_fast(arg0))
//     return -1;
//   
//   // Only now proceed with expensive operations...
// }
