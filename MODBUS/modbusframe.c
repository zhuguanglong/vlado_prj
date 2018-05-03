#include "modbusframe.h"
#include "mbcrc.h"
#include "debug.h"
#include "usart.h"
#include  <string.h>
#include <stdlib.h> 

/************net_cmd_list-->func**************/
/***********when in the net_cmd_list add a new cmd,please add a mate func  *********/
signed short write_pin_output(unsigned char* FrameHeader,unsigned char len);
signed short write_value_out(unsigned char* FrameHeader,unsigned char len);
const rs485_command_t rs485_cmd_list_w[] = {{WRITE_PIN_OUTPUT,write_pin_output},
                                            {WRITE_VALUE_OUT,write_value_out}};
signed short read_station_light(unsigned char* FrameHeader,unsigned char len);
signed short read_station_pin_output(unsigned char* FrameHeader,unsigned char len);
signed short read_value_analog_out(unsigned char* FrameHeader,unsigned char len);																				
signed short read_value_analog_in(unsigned char* FrameHeader,unsigned char len);
signed short read_version_hardware(unsigned char* FrameHeader,unsigned char len);
signed short read_version_software(unsigned char* FrameHeader,unsigned char len);																						
const rs485_command_t rs485_cmd_list_r[] = {{READ_STATION_LIGHT,read_station_light},
                                            {READ_STATION_PIN_OUTPUT,read_station_pin_output},
                                            {READ_VALUE_ANALOG_OUT,read_value_analog_out},
                                            {READ_VALUE_ANALOG_IN,read_value_analog_in},
                                            {READ_VERSION_HARDWARE,read_version_hardware},
                                            {READ_VERSION_SOFTWARE,read_version_software}};
const unsigned char rs485_cmd_w_num = sizeof(rs485_cmd_list_w)/sizeof( rs485_cmd_list_w[0]);
const unsigned char rs485_cmd_r_num = sizeof(rs485_cmd_list_r)/sizeof( rs485_cmd_list_r[0]);
/************************************************************************************/


/****************** some declaration  about analysis func **************/
static short rs485_parse_command_r(unsigned short cmd);
static short rs485_parse_command_w(unsigned short cmd);
AnalyzRslt Analyzerespmodbus(stBufferOpration *BufferOp);
/****************** declaration end **************/

/************** modbus - rtu analyse ******************/
AnalyzRslt Analyzerespmodbus(stBufferOpration *BufferOp)
{
   static unsigned int length = 0;
   int i = 0;
	 int rt = 0;
   AnalyzRslt AnlySlt;
	 int rxnb = BufferOp->size;
	 unsigned char *databuf = BufferOp->rxbuffer;
	 unsigned int sum=0;
	 AnlySlt.FrameHeaderAddr = NULL;
	 if(rxnb<8)//the shortest frame is 8 bytes
	 {
			AnlySlt.AnalyzRtl = ERROR_NONE;
			return AnlySlt;
	 }	 
	 for(i=0;i<rxnb;i++)
	 { 
			if(databuf[i]==DEVICE_ADDRESS)
    	{
    	    if(databuf[i+1]==WRITE_MULTI_HOLDING_REGISTER)
					{
							length=databuf[i+2];
							if(length < modbus_rx_max)
							{   
									rt = i;
									if((rxnb-rt) >= (length+5))
									{
											sum = usMBCRC16((databuf+rt),(length+3));
										  //EndianConvertLToB((unsigned char*) &sum,2);
											if(memcmp(&databuf[length+rt+3],&sum,2)==0)
											{   
													AnlySlt.FrameHeaderAddr = databuf+rt;
													AnlySlt.FrameLen = length+5;
													AnlySlt.AnalyzRtl = CHECK_OK;
													return AnlySlt;
											}
											AnlySlt.FrameHeaderAddr = databuf+rt;
											AnlySlt.AnalyzRtl = CHECK_ERROR;
											return AnlySlt;
									}
									AnlySlt.AnalyzRtl = ERROR_NONE;
									return AnlySlt;
						 }
					}
					else
					{
						 rt = i;
						 if((rxnb-rt) >= 8)
						 {
								 sum = usMBCRC16((databuf+rt),6);
							   //EndianConvertLToB((unsigned char*) &sum,2);
								 if(memcmp(&databuf[rt+6],&sum,2)==0)
								 {	 
										 AnlySlt.FrameHeaderAddr = databuf+rt;
										 AnlySlt.FrameLen = 6;
										 AnlySlt.AnalyzRtl = CHECK_OK;
										 return AnlySlt;
								 }
								 AnlySlt.FrameHeaderAddr = databuf+rt;
								 AnlySlt.AnalyzRtl = CHECK_ERROR;
								 return AnlySlt;
							}
							AnlySlt.AnalyzRtl = ERROR_NONE;
							return AnlySlt;
						}
			}
	 }
	 AnlySlt.AnalyzRtl = UNKNOW_ERROR;
   return AnlySlt;	 
}

/*************** receive the data from the PC *************/
void rs485_rx_func(stBufferOpration* p)
{
	  short cmd_t;
	  short idx;
	  unsigned char len_t;
	  unsigned char tpye;
	  unsigned char *frame = NULL;
	  AnalyzRslt result;
	  stBufferOpration *pData;
	  pData = p; 
		result = Analyzerespmodbus(pData);
		if(result.AnalyzRtl == CHECK_OK)
		{   
				/*   fetch a message     */
				memcpy(&cmd_t,result.FrameHeaderAddr+2,2);
				EndianConvertLToB((unsigned char*) &cmd_t,2);
				tpye =  result.FrameHeaderAddr[1];
			  len_t = (unsigned char)result.FrameLen;
			  frame = result.FrameHeaderAddr;
				if(tpye == READ_DISCRETE_INPUT)
				{
					  idx = rs485_parse_command_r(cmd_t);
					  rs485_cmd_list_r[idx].func(frame,len_t);
						DEBUG("READ_DISCRETE_INPUT\r\n");
				}
				else if(tpye == READ_COIL_OUTPUT)
				{
					  idx = rs485_parse_command_r(cmd_t);
					  rs485_cmd_list_r[idx].func(frame,len_t);			
					  DEBUG("READ_COIL_OUTPUT\r\n");
				}
				else if(tpye == WRITE_SINGLE_COIL_OUTPUT)
				{
					  idx = rs485_parse_command_w(cmd_t);
					  rs485_cmd_list_w[idx].func(frame,len_t);					  
					  DEBUG("WRITE_SINGLE_COIL_OUTPUT\r\n");
				}
				else if(tpye == READ_INPUT_REGISTER)
				{
					  idx = rs485_parse_command_r(cmd_t);
					  rs485_cmd_list_r[idx].func(frame,len_t);
					  DEBUG("READ_INPUT_REGISTER\r\n");
				}
				else if(tpye == READ_MULTI_HOLDING_REGISTER)
				{
					  idx = rs485_parse_command_r(cmd_t);
					  rs485_cmd_list_r[idx].func(frame,len_t);
					  DEBUG("READ_MULTI_HOLDING_REGISTER\r\n");
				}	
				else if(tpye == WRITE_SINGLE_HOLDING_REGISTER)
				{
					  idx = rs485_parse_command_w(cmd_t);
					  rs485_cmd_list_w[idx].func(frame,len_t);					
					  DEBUG("WRITE_SINGLE_HOLDING_REGISTER\r\n");
				}
				else if(tpye == WRITE_MULTI_HOLDING_REGISTER)
				{
					  DEBUG("WRITE_MULTI_HOLDING_REGISTER\r\n");
				}								
		}
#ifdef TIMER
    pData->CLEARB(pData, pData->size);
#else
		if(result.AnalyzRtl != ERROR_NONE)
		{
				pData->CLEARB(pData, pData->size);
		}
#endif
}

/************ get the func  by cmd *************/
static short rs485_parse_command_r(unsigned short cmd)
{
    //Report("parse_command");
    short i;
    for(i=0;i<rs485_cmd_r_num;i++)
    {
        if(cmd == rs485_cmd_list_r[i].cmd)
        {
					  DEBUG("exist the fun\r\n ");
            return i;					
        }
    }
		DEBUG("not exist the fun\r\n");
    return -1;
}

static short rs485_parse_command_w(unsigned short cmd)
{
    //Report("parse_command");
    short i;
    for(i=0;i<rs485_cmd_w_num;i++)
    {
        if(cmd == rs485_cmd_list_w[i].cmd)
        {
					  DEBUG("exist the fun\r\n ");
            return i;					
        }
    }
		DEBUG("not exist the fun\r\n");
    return -1;
}

signed short write_pin_output(unsigned char* FrameHeader,unsigned char len)
{
	 unsigned short sum;
	 unsigned char *p = malloc(len+2); 
	 if(p == NULL)
	 {
	    return -1;
	 }
	 memcpy(p,FrameHeader,len);
	 sum = usMBCRC16(p,len);
	 memcpy(p+len,&sum,2);
	 LowLevelUart2Tx(p,len+2);
	 free(p);  
	 return 0;
}
signed short write_value_out(unsigned char* FrameHeader,unsigned char len)
{
	 unsigned short sum;
	 unsigned char *p = malloc(len+2); 
	 if(p == NULL)
	 {
	    return -1;
	 }
	 memcpy(p,FrameHeader,len);
	 sum = usMBCRC16(p,len);
	 memcpy(p+len,&sum,2);
	 LowLevelUart2Tx(p,len+2);
	 free(p);  
	 return 0;
}

signed short read_station_light(unsigned char* FrameHeader,unsigned char len)
{
	 unsigned short sum;
	 unsigned char num = (FrameHeader[4]*256 + FrameHeader[5])/8;
	 if(FrameHeader[5]%8)
	 {
	    num++;
	 }
	 unsigned char *p = malloc(num+5); 
	 if(p == NULL)
	 {
	    return -1;
	 }
	 memcpy(p,FrameHeader,2); 
	 p[2] = num;
	 /* get the data */
	 /* start */

   /* end */	 
	 sum = usMBCRC16(p,num+3);
	 memcpy(p+num+3,&sum,2);
	 LowLevelUart2Tx(p,num+5);
	 free(p);
	 return 0;
}
signed short read_station_pin_output(unsigned char* FrameHeader,unsigned char len)
{
	 unsigned short sum;
	 unsigned char num = (FrameHeader[4]*256 + FrameHeader[5])/8;
	 if(FrameHeader[5]%8)
	 {
	    num++;
	 }
	 unsigned char *p = malloc(num+5); 
	 if(p == NULL)
	 {
	    return -1;
	 }
	 memcpy(p,FrameHeader,2); 
	 p[2] = num;
	 /* get the data */
	 /* start */

   /* end */	 
	 sum = usMBCRC16(p,num+3);
	 memcpy(p+num+3,&sum,2);
	 LowLevelUart2Tx(p,num+5);
	 free(p);
	 return 0;
}
signed short read_value_analog_out(unsigned char* FrameHeader,unsigned char len)
{
	 unsigned short sum;
	 unsigned char num = FrameHeader[5]*2;
	 unsigned char *p = malloc(num+5); 
	 if(p == NULL)
	 {
	    return -1;
	 }
	 memcpy(p,FrameHeader,2); 
	 p[2] = num;
	 /* get the data */
	 /* start */

   /* end */	 
	 sum = usMBCRC16(p,num+3);
	 memcpy(p+num+3,&sum,2);
	 LowLevelUart2Tx(p,num+5);
	 free(p);
	 return 0;
}	
signed short read_value_analog_in(unsigned char* FrameHeader,unsigned char len)
{
	 unsigned short sum;
	 unsigned char num = FrameHeader[5]*2;
	 unsigned char *p = malloc(num+5); 
	 if(p == NULL)
	 {
	    return -1;
	 }
	 memcpy(p,FrameHeader,2); 
	 p[2] = num;
	 /* get the data */
	 /* start */

   /* end */	 
	 sum = usMBCRC16(p,num+3);
	 memcpy(p+num+3,&sum,2);
	 LowLevelUart2Tx(p,num+5);
	 free(p);
	 return 0;
}
signed short read_version_hardware(unsigned char* FrameHeader,unsigned char len)
{
	 unsigned short sum;
	 unsigned char num = FrameHeader[5]*2;
	 unsigned char *p = malloc(num+5); 
	 if(p == NULL)
	 {
	    return -1;
	 }
	 memcpy(p,FrameHeader,2); 
	 p[2] = num;
	 /* get the data */
	 /* start */
   memcpy(p+3,"AAAA",4);
   /* end */	 
	 sum = usMBCRC16(p,num+3);
	 memcpy(p+num+3,&sum,2);
	 LowLevelUart2Tx(p,num+5);
	 free(p);
	 return 0;
}
signed short read_version_software(unsigned char* FrameHeader,unsigned char len)
{
	 unsigned short sum;
	 unsigned char num = FrameHeader[5]*2;
	 unsigned char *p = malloc(num+5); 
	 if(p == NULL)
	 {
	    return -1;
	 }
	 memcpy(p,FrameHeader,2); 
	 p[2] = num;
	 /* get the data */
	 /* start */
     memcpy(p+3,"BBBB",4);
   /* end */	 
	 sum = usMBCRC16(p,num+3);
	 memcpy(p+num+3,&sum,2);
	 LowLevelUart2Tx(p,num+5);
	 free(p);
	 return 0;
}

