#include "Exception.h"
#include <stdio.h>       //library contains NULL
#include <malloc.h>      //memory allocation
#include <stdarg.h>

Exception ex2;		     //static memory allocation

Exception *createException(char *msg, int errorCode){
	Exception *ex;

	ex=malloc(sizeof(Exception));   // dynamic memory allocation
	ex->msg=msg;
	ex->errorCode=errorCode;
	ex->data=NULL;
	return ex;
}

void freeException(Exception *e){
	if(e->data&&e->freeDataOnFreeingException){
		free(e->data);					//<--- Free memory pointed by pointers first;
	}
	if(e->msg){
		free(e->msg);
	}
	free(e);
}
void dumpException(Exception *e){
	printf("%s (error=0x%x)\n", e->msg, e->errorCode);
}

void throwException(int errorCode, void *data, int freeDataOnFreeingException, char *message,...){
	Exception *e;
	size_t len;
	char *buffer;
	
	va_list va;
	va_start(va, message);
	len=vsnprintf(NULL, 0, message, va);
	printf("Len: %d\n", len);
	buffer=malloc(len+1);
	vsprintf(buffer, message, va);
	va_end(va);
	e=createException(message, errorCode);
	e->data=data;
	e->freeDataOnFreeingException=freeDataOnFreeingException;
	e->msg=buffer;
	
	Throw(e);
}



