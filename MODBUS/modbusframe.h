#ifndef MODBUSFRAME_H
#define MODBUSFRAME_H
#include  "recebufferopration.h"
#define DEVICE_ADDRESS  (unsigned char)3
	
/* read the station of input */
#define READ_DISCRETE_INPUT  (unsigned char)0x02 
/* read the station of coil output */	
#define READ_COIL_OUTPUT  (unsigned char)0x01
/* write the single coil */	
#define WRITE_SINGLE_COIL_OUTPUT  (unsigned char)0x05
	

/* read the value of input register */
#define READ_INPUT_REGISTER  (unsigned char)0x04
/* read the multiple value of holding register */	
#define READ_MULTI_HOLDING_REGISTER  (unsigned char)0x03
/* write the single holding register */	
#define WRITE_SINGLE_HOLDING_REGISTER  (unsigned char)0x06
/* write the multiple holding register */	
#define WRITE_MULTI_HOLDING_REGISTER  (unsigned char)0x10
	
#define READ_STATION_LIGHT 0x0000
#define READ_STATION_PIN_OUTPUT 0x0064
#define READ_VALUE_ANALOG_OUT 0x012c

#define READ_VALUE_ANALOG_IN 0x00f0
#define READ_VERSION_HARDWARE 0x00c8
#define READ_VERSION_SOFTWARE 0x00ca


#define WRITE_PIN_OUTPUT  0x0064
#define WRITE_VALUE_OUT  0x012c




#ifdef MODBUS_RTU
#define ERROR_NONE       (unsigned char)0x00
#define CHECK_OK         (unsigned char)0x01
#define CHECK_ERROR         (unsigned char)0x02
#define UNKNOW_ERROR     (unsigned char)0x03
#else
#define FRAME_COMPLETE  (unsigned char)0x00
#define FRAME_INCOMPLETE  (unsigned char)0x01
#define UNKNOW_ERROR     (unsigned char)0x02
#endif

	
#define GET_VALUE_CMD  (unsigned int)0x1000

#define modbus_rx_max   50
typedef struct AnalyzResult
{
    int            FrameLen;
    unsigned char* FrameHeaderAddr;
    unsigned char  AnalyzRtl;
}AnalyzRslt;

typedef signed short (*cmd_callback)(unsigned char* FrameHeader,unsigned char len);
typedef struct 
{
    unsigned short cmd;
    cmd_callback func;
}rs485_command_t;

#ifdef MODBUS_RTU
void rs485_rx_func(stBufferOpration* p);
#else
void check_analyse_frame(void);
#endif

#endif

