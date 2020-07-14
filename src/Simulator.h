#ifndef SIMULATOR_H
#define SIMULATOR_H

#include  <stdint.h>

typedef struct  _21BitInteger _21BitInteger;
struct  _21BitInteger{
  int value:21;
};
#define wreg    fileRegisters[0xFE8]
#define bsr     fileRegisters[0xFE0]
#define status  fileRegisters[0xFD8]
#define pc      (((_21BitInteger *)&fileRegisters[0xff9])->value)

#define STATUS_N    (1<<4)
#define STATUS_OV   (1<<3)
#define STATUS_Z    (1<<2)
#define STATUS_DC   (1<<1)
#define STATUS_C    (1<<0)

extern  uint8_t codeMemory[];
typedef void  (*InstructionExecutor)(void);

int computeFileRegAddress(int addr, int b);
void  executeInstruction();
void  addwf();
void  incf();
void  clrf();
void  bcf();
void  bsf();
void  bn();
void  bnz();
void  movwf();
void  andwf();
void  rlcf();

#endif // SIMULATOR_H
