#include <stdint.h>
#include "FlashOS.h"

#define FLASH_MIN_WRITE_SIZE  (8U)

int Init(unsigned long adr, unsigned long clk, unsigned long fnc)
{
  // Unlock Flash Register
  *(volatile uint32_t *)0x400B1004UL = 0x45670123UL;                                // Flash->KEYR1
  __builtin_arm_dsb(0xFUL);
  *(volatile uint32_t *)0x400B1004UL = 0xCDEF89ABUL;                                // Flash->KEYR1
  __builtin_arm_dsb(0xFUL);

  (void)adr;
  (void)clk;
  (void)fnc;

  return 0;
}

int UnInit(unsigned long fnc)
{
  // Lock Flash Register
  *(volatile uint32_t *)0x400B1010UL |= (0x1UL << 7UL);                             // FLASH->CR1 : Bit 7 (LOCK)
  __builtin_arm_dsb(0xFUL);

  (void)fnc;

  return 0;
}

int EraseChip(void)
{
  // Clear "Erase Done" Flag
  *(volatile uint32_t *)0x400B100CUL |= (0x1UL << 5UL);                             // Flash->SR1 : Bit 5 (EOP)
  __builtin_arm_dsb(0xFUL);

  // Enable Erasing Bank1
  *(volatile uint32_t *)0x400B1010UL |= (0x1UL << 2UL);                             // FLASH->CR1 : Bit 2 (MER)
  __builtin_arm_dsb(0xFUL);

  // Erase
  *(volatile uint32_t *)0x400B1010UL |= (0x1UL << 6UL);                             // FLASH->CR1 : Bit 6 (STAT)

  // Wait For Erasing Done
  while ((*((volatile uint32_t *)0x400B1010UL) & (0x1UL << 6UL)) != 0x0UL) {        // FLASH->CR1 : Bit 6 (STAT)
    ;
  }

  // Disable Erase Bank 1
  *(volatile uint32_t *)0x400B1010UL &= ~(0x1UL << 2UL);                            // FLASH->CR1 : Bit 2 (MER)
  __builtin_arm_dsb(0xFUL);

  // Check "Erase/Write Done" Flag
  if (((*(volatile uint32_t *)0x400B100CUL) & (0x1UL << 5UL)) == (0x1UL << 5UL)) {  // Flash->SR1 : Bit 5 (EOP)
    // Clear "Erase/Write Done" Flag
    *(volatile uint32_t *)0x400B100CUL |= (0x1UL << 5UL);                           // Flash->SR1 : Bit 5 (EOP)
    return 0;
  } else {
    return 1;
  }
}

int EraseSector(unsigned long adr)
{
  // Clear "Erase Done" Flag
  *(volatile uint32_t *)0x400B100CUL |= (0x1UL << 5UL);                             // Flash->SR1 : Bit 5 (EOP)
  __builtin_arm_dsb(0xFUL);

  // Write Destination Address
  *(volatile uint32_t *)0x400B1014UL = adr;                                         // Flash->AR1
  __builtin_arm_dsb(0xFUL);

  // Enable Erase Sector of BANK 1
  *(volatile uint32_t *)0x400B1010UL |= (0x1UL << 1UL);                             // FLASH->CR1 : Bit 1 (PER)
  __builtin_arm_dsb(0xFUL);

  // Erase
  *(volatile uint32_t *)0x400B1010UL |= (0x1UL << 6UL);                             // FLASH->CR1 : Bit 6 (STAT)

  // Wait For Erasing Done
  while (((*(volatile uint32_t *)0x400B1010UL) & (0x1UL << 6UL)) != 0x0UL) {        // FLASH->CR1 : Bit 6 (STAT)
    ;
  }

  // Disable Erase Sector of BANK 1
  *(volatile uint32_t *)0x400B1010UL &= ~(0x1UL << 1UL);                            // FLASH->CR1 : Bit 1 (PER)
  __builtin_arm_dsb(0xFUL);

  // Check "Erase/Write Done" Flag
  if (((*(volatile uint32_t *)0x400B100CUL) & (0x1UL << 5UL)) == (0x1UL << 5UL)) {  // Flash->SR1 : Bit 5 (EOP)
    // Clear "Erase/Write Done" Flag
    *(volatile uint32_t *)0x400B100CUL |= (0x1UL << 5UL);                           // Flash->SR1 : Bit 5 (EOP)
    return 0;
  } else {
    return 1;
  }
}

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf)
{
  // Data Filling
  uint8_t sz_misalignment_bytes = sz & (FLASH_MIN_WRITE_SIZE - 1U);

  if (sz_misalignment_bytes != 0U) {
    for (uint8_t i = 0U; i < FLASH_MIN_WRITE_SIZE - sz_misalignment_bytes; ++i) {
      *(buf + sz) = 0xFFU;
      ++sz;
    }
  }

  // Enable Programming
  *(volatile uint32_t *)0x400B1010UL |= (0x1UL << 0UL);                             // FLASH->CR1 : Bit 0 (PG)
  __builtin_arm_dsb(0xFUL);

  // Program
  for (uint32_t i = 0UL; i < sz; i += 8UL) {
    *(volatile uint64_t *)(adr + i) = *(uint64_t *)(buf + i);
    __builtin_arm_dsb(0xFUL);
    while (((*(volatile uint32_t *)0x400B100CUL) & (0x1UL << 0UL)) != 0x0UL) {      // FLASH->SR1 : Bit 0 (BSY)
      ;
    }
  }

  // Disable Programming
  *(volatile uint32_t *)0x400B1010UL &= ~(0x1UL << 0U);                             // FLASH->CR1 : Bit 0 (PG)
  __builtin_arm_dsb(0xFUL);

  // Check "Erase/Write Done" Flag
  if (((*(volatile uint32_t *)0x400B100CUL) & (0x1UL << 5UL)) == (0x1UL << 5UL)) {  // Flash->SR1 : Bit 5 (EOP)
    // Clear "Erase/Write Done" Flag
    *(volatile uint32_t *)0x400B100CUL |= (0x1UL << 5UL);                           // Flash->SR1 : Bit 5 (EOP)
    return 0;
  } else {
    return 1;
  }
}

unsigned long Verify(unsigned long adr, unsigned long sz, unsigned char *buf)
{
  for (unsigned long i = 0UL; i < sz; ++i) {
    if (*(unsigned char *)(adr + i) != *(buf + i)) {
      return adr + i;
    }
  }

  return adr + sz;
}
