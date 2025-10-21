#include "FlashOS.h"

struct FlashDevice const FlashDevice = {
  FLASH_DRV_VERS,
  "CHIPNAME Flash",
  ONCHIP,
  0x00400000UL,
  0x00080000UL,
  1024UL,
  0UL,
  0xFFU,
  100UL,
  500UL,
  {
    {0x00000400UL, 0x00000000UL},
    SECTOR_END
  }
};
