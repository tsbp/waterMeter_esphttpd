#ifndef ramtron_h
#define ramtron_h
//==============================================================================
//#include "init.h"
//#include "hspi.h"
//==============================================================================
//old #define RMTR_SIZE       (32760)   // память 32768 => 2520 сообщений * 13 байт = 32760  
#define RMTR_SIZE       (131066)   // память 131072 => 10082 сообщений * 13 байт = 131066  
#define MEM1_PT		P3
#define MEM2_PT		P3
#define CSMEM1		BIT4
#define CSMEM2		BIT5
#define HOLD            BIT0
//==============================================================================
#define WEL		BIT1
#define BP0		BIT2
#define BP1		BIT3
#define WPEN		BIT7
//==============================================================================
extern unsigned char ReadStatRegMem(unsigned char aMem);
extern void RamtronInit(void);
extern void WRENMem (unsigned char aMem);
extern void WRDIMem (unsigned char aMem);
extern void RamtronWRDis(unsigned char aMem);
extern void WriteMem(unsigned char aMem, unsigned long aAddr, unsigned char *aData, unsigned int aLen);
extern void ReadMem (unsigned char aMem, unsigned long aAddr, uint32_t/*unsigned char*/ *aData, unsigned int aLen);
void ReadMemNmsg (unsigned char aMem, unsigned long aAddr, unsigned char *aData, unsigned int aMSGstart, unsigned int nMsg);
extern void CSMem(unsigned char aMem, char aCS);
void ICACHE_FLASH_ATTR spi_master_init_my();
//==============================================================================
union U_RAMTRON_STAT_REG
{
    unsigned char byte;
    struct
    {
	unsigned char 		:1;
	unsigned char WEL_	:1;
	unsigned char BP0_	:1;
	unsigned char BP1_	:1;
	unsigned char		:1;
	unsigned char		:1;
	unsigned char		:1;
	unsigned char WPEN_	:1;
    };
};
extern union U_RAMTRON_STAT_REG RAMTRON_STAT_REG;
//==============================================================================
// RAMTRON OP-CODES
//==============================================================================
#define WREN		(0x06)
#define WRDI		(0x04)
#define RDSR		(0x05)
#define WRSR		(0x01)
#define READ		(0x03)
#define WRITE		(0x02)

//==============================================================================
#endif

