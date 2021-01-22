//==============================================================================
#include "os_type.h"
#include "ramtron.h"
//#include "gpio16.h"
#include "include/spi_interface.h"
#include "eagle_soc.h"
//#include "osapi.h"
#include <esp8266.h>
//==============================================================================
union U_RAMTRON_STAT_REG RAMTRON_STAT_REG;
//unsigned int rmtr_byte_cnt;
//==============================================================================
#define NBYTES (13)
#define HSPI_CS_PIN (1 << 5) // GPIO5
//#define SPIWrite(a) {hspi_send_uint8(a);}
//#define SPIRead(a) {hspi_send_uint8(a);}
//==============================================================================
void RamtronInit(void) {
	os_printf("hspi_init \r\n");


	//set_gpio_mode(HSPI_CS_PIN, GPIO_PULLUP, GPIO_OUTPUT);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);

//	gpio_write(HSPI_CS_PIN, 1);
	gpio_output_set(HSPI_CS_PIN, 0, HSPI_CS_PIN, 0);
	os_printf("hspi_init end \r\n");
}
//==============================================================================
//PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
//PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
//gpio_output_set(0, 0, (1<<LEDGPIO), (1<<BTNGPIO));
void CSMem(unsigned char aMem, char aCS) {
	switch (aMem) {
	case 1:
		if (aCS)
			gpio_output_set(HSPI_CS_PIN, 0, HSPI_CS_PIN, 0);
//			gpio_write(HSPI_CS_PIN, 1);      // OUT(MEM1_PT) |= CSMEM1;
		else
			gpio_output_set(0, HSPI_CS_PIN, 0, HSPI_CS_PIN);
//			gpio_write(HSPI_CS_PIN, 0);          //OUT(MEM1_PT) &= ~CSMEM1;
		break;
	}
}
//==============================================================================
void WRENMem(unsigned char aMem) {

	CSMem(aMem, 0);
	SpiData spiData;

	spiData.cmd = WREN;
	spiData.cmdLen = 1;
	spiData.addr = 0;
	spiData.addrLen = 0;
	spiData.data = 0;
	spiData.dataLen = 0; //32;
	SPIMasterSendData(SpiNum_HSPI, &spiData);

	CSMem(aMem, 1);
}
//==============================================================================
void RamtronWRDis(unsigned char aMem) {
	CSMem(aMem, 0);
	SpiData spiData;

	spiData.cmd = WRDI;
	spiData.cmdLen = 1;
	spiData.addr = 0;
	spiData.addrLen = 0;
	spiData.data = 0;
	spiData.dataLen = 0; //32;
	SPIMasterSendData(SpiNum_HSPI, &spiData);
	CSMem(aMem, 1);
}

//==============================================================================
void RamtronCMD(unsigned char aMem, uint16_t aCmd) {
	CSMem(aMem, 0);
	SpiData spiData;
	spiData.cmd =  aCmd;
	spiData.cmdLen = 1;
	spiData.addr = 0;
	spiData.addrLen = 0;
	spiData.data = 0;
	spiData.dataLen = 0; //32;
	SPIMasterSendData(SpiNum_HSPI, &spiData);
	CSMem(aMem, 1);
}
//==============================================================================
//unsigned char ReadStatRegMem(unsigned char aMem) {
//	unsigned char tmp;
//	CSMem(aMem, 0);
//	SPIWrite(RDSR);
//	SPIRead(tmp);
//	CSMem(aMem, 1);
//	return tmp;
//}
//==============================================================================
void WriteMem(unsigned char aMem, unsigned long aAddr, unsigned char *aData,
		unsigned int aLen) {
	RamtronCMD(aMem, WREN); //WRENMem(aMem);
	CSMem(aMem, 0);
	SpiData spiData;
	spiData.cmd = WRITE;
	spiData.cmdLen = 1;
	spiData.addr = (unsigned int*) &aAddr;
	spiData.addrLen = 2;
	spiData.data = (uint32_t*) aData;
	spiData.dataLen = aLen; //32;
	SPIMasterSendData(SpiNum_HSPI, &spiData);
	CSMem(aMem, 1);
	RamtronCMD(aMem, WRDI); //RamtronWRDis(1);
}

//==============================================================================
void ReadMem(unsigned char aMem, unsigned long aAddr,
		uint32_t/*unsigned char*/*aData, unsigned int aLen) {

	CSMem(aMem, 0);
	SpiData spiData;
	spiData.cmd = READ;
	spiData.cmdLen = 1;
	spiData.addr = (unsigned int*) &aAddr;
	spiData.addrLen = 2;
	spiData.data = aData;
	spiData.dataLen = aLen;
	SPIMasterRecvData(SpiNum_HSPI, &spiData);
	CSMem(aMem, 1);
}

//==============================================================================
//void ReadMemNmsg(unsigned char aMem, unsigned long aAddr, unsigned char *aData,
//		unsigned int aMSGstart, unsigned int nMsg) {
//	unsigned int cnt;
//	cnt = 0;
//	CSMem(aMem, 0);
//	SPIWrite(READ);
//	SPIWrite(1, ((unsigned char*) (&aAddr))[2]);
//	SPIWrite(1, ((unsigned char*) (&aAddr))[1]);
//	SPIWrite(1, ((unsigned char*) (&aAddr))[0]);
////    SPIWrite((unsigned char) ((aAddr >> 8) & 0x00ff));
////    SPIWrite((unsigned char) (aAddr & 0x00ff));
//	unsigned int i, j;
//	for (j = 0; j < nMsg; j++)
//		for (i = 0; i < NBYTES; i++) {
//			SPIRead(1, aData[aMSGstart * NBYTES + cnt]);
//			cnt++;
//		}
//	CSMem(aMem, 1);
//
//}
//==============================================================================
//void RMTRclear(void) {
//	WRENMem(1);
//	CSMem(1, 0);
//	SPIWrite(WRITE);
//	SPIWrite(0x0000);
//	SPIWrite(0x0000);
//	unsigned int i;
//	for (i = 0; i < 0x7fff; i++)
//		SPIWrite(0x00);
//	CSMem(1, 1);
//	RamtronWRDis(1);
//	//ReadMemNmsg(1, 0, RTRbuf0.BYTE, 0, 150);
//}
//==============================================================================
void ICACHE_FLASH_ATTR spi_master_init_my() {
	SpiAttr hSpiAttr;
	hSpiAttr.bitOrder = SpiBitOrder_MSBFirst;
	hSpiAttr.speed = SpiSpeed_10MHz;
	hSpiAttr.mode = SpiMode_Master;
	hSpiAttr.subMode = SpiSubMode_0;

	// Init HSPI GPIO
	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2); //configure io to spi mode
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2); //configure io to spi mode
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2); //configure io to spi mode
	//PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);//configure io to spi mode CS

	SPIInit(SpiNum_HSPI, &hSpiAttr);
//	uint32_t value = 0x00000001;
//	uint32_t sendData[8] = { 0 };
//	SpiData spiData;

	os_printf("\r\n =============   spi init master   ============= \r\n");
}

//==============================================================================
void ICACHE_FLASH_ATTR spi_master_test() {

	uint32_t value = 0x00000001;
	uint32_t sendData[8] = { 0 };
	SpiData spiData;

//  Test 8266 slave.Communication format: 1byte command + 1bytes address + x bytes Data.
	os_printf("\r\n Master send 32 bytes data to slave(8266)\r\n");
	os_memset(sendData, 0, sizeof(sendData));
	sendData[0] = 0x55565758;
//    sendData[1] = 0x595a5b5c;
//    sendData[2] = 0x5d5e5f60;
//    sendData[3] = 0x61626364;
//    sendData[4] = 0x65666768;
//    sendData[5] = 0x696a6b6c;
//    sendData[6] = 0x6d6e6f70;
//    sendData[7] = 0x71727374;
	spiData.cmd = MASTER_WRITE_DATA_TO_SLAVE_CMD;
	spiData.cmdLen = 1;
	spiData.addr = &value;
	spiData.addrLen = 2;
	spiData.data = sendData;
	spiData.dataLen = 4; //32;
	SPIMasterSendData(SpiNum_HSPI, &spiData);
}


