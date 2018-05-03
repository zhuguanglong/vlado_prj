#include "modbusframe.h"
#include  "recebufferopration.h"
#include "mbcrc.h"
#include "debug.h"
#include "usart.h"
#include  <string.h>
#include <stdlib.h> 

const unsigned char ASCII[17]="0123456789ABCDEF";
unsigned char lrc (unsigned char *nData, unsigned char wLength);
unsigned char chartoint(unsigned char *src,unsigned char *dst,unsigned char len);
/************** modbus - ASCII analyse ******************/
AnalyzRslt find_frame(stBufferOpration *BufferOp)
{
   unsigned char i = 0;
	 unsigned char j;
   AnalyzRslt AnlySlt;
	 int rxnb = BufferOp->size;
	 unsigned char *databuf = BufferOp->rxbuffer;
	 AnlySlt.FrameHeaderAddr = NULL;
	 if(rxnb<14)//the shortest frame is 14 bytes
	 {
			AnlySlt.AnalyzRtl = FRAME_INCOMPLETE;
			return AnlySlt;
	 }	 
	 for(i=0;i<rxnb;i++)
	 { 
		  if(databuf[i]==':')
    	{
				 for(j=i;j<rxnb-1;j++)
				 {
				    if(databuf[j]==0x0a && databuf[j+1]==0x0d)
					  {
						   //analyse
//							 rt = i+1;//databuf[i]==':'
//							 str_len = j-i-1;
//							 unsigned char *src_lrc = malloc(len_lrc);
//							 lrc_len = chartoint(databuf+i+1,src_lrc,str_len);//contain 2byte lrc
//							 sum = lrc(src_lrc,lrc_len-1);
//							 if(sum == src_lrc[lrc_len-1])
//							 {
//									AnlySlt.FrameHeaderAddr = databuf+rt;
//									AnlySlt.FrameLen = Length+5;
//									AnlySlt.AnalyzRtl = CHECK_OK;
//									return AnlySlt;
//							 }
							 AnlySlt.FrameHeaderAddr = databuf+i;
							 AnlySlt.FrameLen = j - i + 2;
							 AnlySlt.AnalyzRtl = FRAME_COMPLETE;
						}
				 }
			   AnlySlt.AnalyzRtl = FRAME_INCOMPLETE;
			   return AnlySlt;				 
			}
	 }
	 AnlySlt.AnalyzRtl = UNKNOW_ERROR;
   return AnlySlt;	 
}

void check_analyse_frame()
{
	 short cmd_t;
	 short idx;
	 unsigned char len_t;
	 unsigned char len_hex;
	 unsigned char sum;
	 unsigned char tpye;
	 unsigned char *frame = NULL;
	 AnalyzRslt rst;
	 stBufferOpration *pData;
	 pData = &uart1buffop; 
	 rst = find_frame(pData);
	 len_t = rst.FrameLen-3;//':',\r,\n
 	 if(rst.AnalyzRtl == FRAME_COMPLETE)	 
	 {
			 unsigned char *src_lrc = malloc(len_t);
   		 len_hex = chartoint(rst.FrameHeaderAddr+1,src_lrc,len_t);//contain 2byte lrc
			 sum = lrc(src_lrc,len_hex-1);
			 if(sum == src_lrc[len_hex-1])
			 {
				  
			 }		
	 }
}
unsigned char lrc (unsigned char *nData, unsigned char wLength)
{
	unsigned char nLRC = 0 ; // LRC char initialized

	for (int i = 0; i < wLength; i++)
	nLRC += *nData++;

	return (256-nLRC);

} // End: LRC

unsigned char chartoint(unsigned char *src,unsigned char *dst,unsigned char len)
{
	 unsigned char i;
	 unsigned char j;
	 char *p = malloc(len);
	 for(i=0;i<len;i++)
	 {
		  if(src[i]>64)
			{
			   p[i] = src[i] - 55;
			}
			else
			{
				 p[i] = src[i] - 48;
			}						
	 }
	 for(i=0;i<len;i+=2)
	 {
	    dst[j++] = p[i]*16 + p[i+1];
	 }
	 free(p);
	 return j;
}

void inttochar(unsigned char *src,unsigned char *dst,unsigned char len)
{
   
}