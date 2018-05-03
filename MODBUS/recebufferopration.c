#include  "recebufferopration.h"

unsigned char uart1buffer[64];
int clear(stBufferOpration* buffer, int len);
int read(stBufferOpration* buffer,unsigned char* data); 
int write(stBufferOpration* buffer,unsigned char data);
int readbuffer(stBufferOpration* buffer,int wnb, int rlen);
int writebuffer(stBufferOpration* buffer,unsigned char *wbuf, int wlen);


stBufferOpration uart1buffop = {uart1buffer,0,write,clear,read,readbuffer,writebuffer};

/******* remove len byte data from rnd ****************/
/******* Returns the number of purged bytes ***********/
int clear(stBufferOpration* buffer, int len)
{
	  int i;
    buffer->size = 0;
    for(i=0;i<len;i++)
      buffer->rxbuffer[i] = 0;
    return len;
}

/******* read the data from the buffer where rnd is located *****/ 
/******* Returns the number of bytes to read ********************/
int read(stBufferOpration* buffer,unsigned char* data)
{
    return 0;
}


/******* write the data in the buffer to wnd, and move the wnd *****/
/******* returns the number of written bytes  ********************/
int write(stBufferOpration* buffer,unsigned char data)
{   
    int size = buffer->size;
    buffer->rxbuffer[size]= data;
    size++;
    size = size%64;
    buffer->size = size;
    return size;
}


/******* starting from the RND position in the buffer **********/
/******* read the rlen length bytes to the rbuf  **********/
/******* return the number of bytes to read  **********/
int readbuffer(stBufferOpration* buffer,int wnb, int rlen) 
{  
    int i = 0;
    for(i = wnb;i>(wnb-rlen);i--)
        buffer[wnb-i] = buffer[i];
    return (wnb-i);
}


/******* start in the WND position in the buffer  **********/
/******* write the bytes of wlen length to the number of bytes written **********/
int writebuffer(stBufferOpration* buffer,unsigned char* wbuf, int wlen) 
{
		return 0;
}

