#ifndef __FLASHOS_H__
#define __FLASHOS_H__

#define VERS            (0)
#define FLASH_DRV_VERS  (0x0100 + VERS)

#define UNKNOWN         (0)
#define ONCHIP          (1)
#define EXT8BIT         (2)
#define EXT16BIT        (3)
#define EXT32BIT        (4)
#define EXTSPI          (5)

#define SECTOR_NUM      (512)
#define PAGE_MAX        (65536)

#define SECTOR_END      {0xFFFFFFFFUL, 0xFFFFFFFFUL}

struct FlashSectors {
  unsigned long szSector;
  unsigned long AddrSector;
};

struct FlashDevice {
  unsigned short Vers;
  char DevName[128];
  unsigned short DevType;
  unsigned long DevAdr;
  unsigned long szDev;
  unsigned long szPage;
  unsigned long Res;
  unsigned char valEmpty;
  unsigned long toProg;
  unsigned long toErase;
  struct FlashSectors sectors[SECTOR_NUM];
};

extern int Init(unsigned long adr, unsigned long clk, unsigned long fnc);
extern int UnInit(unsigned long fnc);
extern int BlankCheck(unsigned long adr, unsigned long sz, unsigned char pat);
extern int EraseChip(void);
extern int EraseSector(unsigned long adr);
extern int ProgramPage(unsigned long adr, unsigned long sz, unsigned char *buf);
extern unsigned long Verify(unsigned long adr, unsigned long sz, unsigned char *buf);

#endif
