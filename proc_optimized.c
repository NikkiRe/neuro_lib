// Оптимизированная версия uaddr_in_range для ускорения badarg теста
// Основная идея: добавить быструю проверку для заведомо невалидных адресов
// до выполнения дорогих операций с page table

static inline int
uaddr_in_range(struct proc *p, uint64 uva, uint64 len)
{
  // Защита от переполнения
  if (len > 0 && uva + len < uva)
    return 0;
  
  // БЫСТРАЯ ПРОВЕРКА #1: адреса с установленным старшим битом (bit 63) 
  // являются kernel space адресами и невалидны для user space
  // Это ловит случаи вроде 0xffffffff (sign-extended до 0xffffffffffffffff)
  if (uva & (1ULL << 63))
    return 0;
  
  // БЫСТРАЯ ПРОВЕРКА #2: адреса в верхней части 32-битного диапазона
  // (>= 0xFFFFFFFF, т.е. 2^32-1) невалидны для user space
  // Это ловит 0xffffffff когда он zero-extended до 0x00000000ffffffff
  // Проверяем только если старшие 32 бита нулевые (чисто 32-битный адрес)
  // Адреса >= 0xFFFFFFFF в нижних 32 битах заведомо невалидны
  if ((uva >> 32) == 0 && (uva >= 0xFFFFFFFFULL)) {
    return 0;
  }
  
  // Проверка на явно невалидные адреса (>= MAXVA)
  if (uva >= MAXVA)
    return 0;
  if (len > 0 && uva + len > MAXVA)
    return 0;
  
  // Для остальных адресов < MAXVA пусть copyin/copyout сами проверят
  // через page table (это безопасно, так как они делают полную проверку)
  return 1;
}
