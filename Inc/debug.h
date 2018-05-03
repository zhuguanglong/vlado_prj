#ifndef __DEBUG_H
#define __DEBUG_H
#ifdef __DEBUG__
  	#define DEBUG printf
	  #define ERROR_DEBUG(x) printf("Error [%d] at line [%d] in function [%s]  \n\r",x,__LINE__,__FUNCTION__)
#else
  	#define DEBUG(x,...)
	#define ERROR_DEBUG(x)
#endif

#endif

