//================== Commands ====================================================
#define OK					(0xff)
#define BAD					(0x00)

#define CMD_SET_COUNTER		(0xA0)
#define CMD_IDENTIFY    	(0xA1)

//================================================================================
typedef union __packed {
	uint8 byte[7];
	struct __packed {
		uint8 dataLength;
		uint8 cmd;
		uint32 wCounter;
		uint8 cSum;
	};
} s_CMD_SET_COUNTER;
//================================================================================

void ICACHE_FLASH_ATTR UDP_Init();
void ICACHE_FLASH_ATTR UDP_Init_client();
void UDP_Recieved(void *arg, char *pusrdata, unsigned short length);
void mergeAnswerWith(char tPtr[2][24][4]);
void ICACHE_FLASH_ATTR sendUDPbroadcast(uint8* abuf, uint16 aLen);
//void ICACHE_FLASH_ATTR addValueToArray(char * tPtr, sint16 *arPtr, char aRot);

