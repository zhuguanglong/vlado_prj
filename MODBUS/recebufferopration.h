#ifndef __RECEIVE_BUFFER_OPRATION_H__
#define __RECEIVE_BUFFER_OPRATION_H__
#include "stdio.h"

typedef struct BUFFERTYPE 
{  
    unsigned char *rxbuffer;
    int size;    
    int (*WRITEB)(struct BUFFERTYPE*, unsigned char);
    int (*CLEARB)(struct BUFFERTYPE*, int);
    int (*READB)(struct BUFFERTYPE*, unsigned char* );
    int (*READBUFFER)(struct BUFFERTYPE*, int , int );
    int (*WRITERBUFFER)(struct BUFFERTYPE*, unsigned char *, int);
    
}stBufferOpration;

extern stBufferOpration uart1buffop;

extern int clear(stBufferOpration* buffer, int len);
extern int write(stBufferOpration* buffer,unsigned char data);
#endif
