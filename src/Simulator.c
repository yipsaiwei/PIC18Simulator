#include<stdint.h>
#include<stdio.h>
#include "Simulator.h"



InstructionExecutor  pic18ExecutionTable[256]={
  [0x14] = andwf, andwf, andwf, andwf,
  [0x24] = addwf, addwf, addwf, addwf,
  [0x28] = incf, incf, incf, incf,
  [0x34] = rlcf, rlcf,
  [0x6A] = clrf, clrf,
  [0x6E] = movwf, movwf,
  [0x90] = bcf, bcf, bcf, bcf, bcf, bcf, bcf, bcf, 
           bcf, bcf, bcf, bcf, bcf, bcf, bcf, bcf, 
  [0x80] = bsf, bsf, bsf, bsf, bsf, bsf, bsf, bsf, 
           bsf, bsf, bsf, bsf, bsf, bsf, bsf, bsf, 
  [0xE0] = bz,
  [0xE1] = bnz,
  [0xE2] = bc,
  [0xE5] = bnov,
  [0xE6] = bn, 
};

//Range of file registers: 0x000-0xfff
int8_t fileRegisters[0x1000];
int8_t *sfr = &fileRegisters[0xf60];

//Max range of code memory : 0x000000 - 0x1fffff
//Range of code memory for this simulator: 0x0000 - 0xffff
uint8_t codeMemory[0x10000];

/*
          memory      address
          --------------------
           xx (low)   0x100
codeptr--->xx         0x101
           xx         0x102
           xx (high)  0x103
 
 
           
*/

int computeFileRegAddress(int addr, int a){
  if(a){
   // BANKED 
    addr+=(unsigned int)bsr<<8;
  }else{
    //ACCESS
    if(addr>0x5f)
      addr+=0xf00;
  }
  return  addr;
}

void  executeInstruction(){
  //uint8_t v1 = codePtr[0];
  //uint8_t v2 = codePtr[1];
  uint8_t *codeptr = &codeMemory[pc];
  InstructionExecutor  executor = pic18ExecutionTable[codeptr[1]];
  executor();
}


int add(int val1, int val2){
  int val3;
  val3 = val1 + val2;
  // Adjust the status flags C, DC, Z, OV, N accordingly
  if((uint8_t)val3==0)
    status |=STATUS_Z;
    if(val3>0xFF)
    status |= STATUS_C;
  if((val3&0x80)&&!(val3>0xFF))
    status |= STATUS_N;
  if((((uint8_t)val1&0x0F)+((uint8_t)val2&0x0F))>0x0F)
    status |= STATUS_DC;
  if(!((uint8_t)val1&0x80)&&!((uint8_t)val2&0x80)&&((uint8_t)val3&0x80))
    status |=STATUS_OV;   
  return val3;
}
/*
d (Direction)     :0==>WREG, 1==>bank
a (BANK)          :0==> ACCESS BANK, 1==>BANK
f (File register) : range from 0 to 255
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: addwf f, d, a
Opcode : 0010 01da ffff ffff
*/
void  addwf(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  int addr = (uint8_t) codeptr[0];
  int d = codeptr[1]&0x02;
  int a = codeptr[1]&0x01;
  addr = computeFileRegAddress(addr,  a);
  if(d){
    //d is 1 ==> file register
    fileRegisters[addr] = fileRegisters[addr] + wreg;
  }else{
    //d is 0
 wreg = fileRegisters[addr] + wreg;
  }
  pc+=2;
}


/*
d (Direction)     :0==>WREG, 1==>bank
a (BANK)          :0==> ACCESS BANK, 1==>BANK
f (File register) : range from 0 to 255
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: incf f, d, a
Opcode : 0010 10da ffff ffff
*/
void  incf(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  int addr = (uint8_t) codeptr[0];
  int d = codeptr[1]&0x02;
  int a = codeptr[1]&0x01;
  addr = computeFileRegAddress(addr,  a);
  int initialValue=fileRegisters[addr];
  int finalValue=fileRegisters[addr]+1;
  if((uint8_t)finalValue==0)
    status |=STATUS_Z;
  if((int)finalValue>0xFF)
    status |= STATUS_C;
  if((finalValue&0x80)&&!(finalValue>0xFF))
    status |= STATUS_N;
  if((((uint8_t)initialValue&0x0F)+0x01>0x0F))
    status |= STATUS_DC;
  if(!((uint8_t)initialValue&0x80)&&((uint8_t)finalValue&0x80))
    status |=STATUS_OV;
  if(d){
    //d is 1 ==> file register
    fileRegisters[addr] = finalValue;
  }else{
    //d is 0
    wreg = finalValue;
  }
  pc+=2;
}


/*
a (BANK)          :0==> ACCESS BANK, 1==>BANK
f (File register) : range from 0 to 255
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: clrf f, a
Opcode : 0110 101a ffff ffff
*/
void  clrf(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  int addr = (uint8_t) codeptr[0];
  int a = codeptr[1]&0x01;
  addr = computeFileRegAddress(addr, a);
  fileRegisters[addr]=0x00;
  status|=STATUS_Z;
  pc+=2;
}


/*
a (BANK)          :0==> ACCESS BANK, 1==>BANK
b (bit)           :Range from 0 to 7
f (File register) : range from 0 to 255
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: bcf f, b, a
Opcode : 1001 bbba ffff ffff
*/
void  bcf(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  int addr = (uint8_t) codeptr[0];
  int bitToClear;
  int a = codeptr[1]&0x01;
  addr = computeFileRegAddress(addr, a); 
  bitToClear=(codeptr[1]&0x0E)>>1;
  fileRegisters[addr]&=~(1<<bitToClear);
  pc+=2;
}


/*
b (bit)           :Range from 0 to 7
a (BANK)          :0==> ACCESS BANK, 1==>BANK
f (File register) : range from 0 to 255
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: bsf f, b, a
Opcode : 1000 bbba ffff ffff
*/
void  bsf(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  int addr = (uint8_t) codeptr[0];
  int bitToClear;
  int a = codeptr[1]&0x01;
  addr = computeFileRegAddress(addr, a);
  bitToClear=(codeptr[1]&0x0E)>>1;
  fileRegisters[addr]|=1<<bitToClear;
  pc+=2;
}

/*
n (value to branch)           :MSB=1, branch backward and vice versa
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: bn n
Opcode : 1110 0110 nnnn nnnn
*/
void  bn(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  uint8_t n;
  n=(uint8_t) codeptr[0];
  if(status&0x10)
  {
  if(n&0x80)
  {
    //branch backward
    n=~n;
    pc=pc-(n*2);
  }else{
  //branch forward
  pc=pc+(n*2)+2;
    }
  }else{
    pc+=2;
  }
}

/*
n (value to branch)           :MSB=1, branch backward and vice versa
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: bnz n
Opcode : 1110 0001 nnnn nnnn
*/
void  bnz(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  uint8_t n;
  n=(uint8_t) codeptr[0];
  if(status&0x04)
  {
    pc+=2;
  }else{
    if(n&0x80)
  {
    //branch backward
    n=~n;
    pc=pc-(n*2);
  }else{
  //branch forward
  pc=pc+(n*2)+2;
    }
  }
}

/*
n (value to branch)           :MSB=1, branch backward and vice versa
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: bc n
Opcode : 1110 0010 nnnn nnnn
*/
void  bc(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  uint8_t n;
  n=(uint8_t) codeptr[0];
  if(status&0x01)
  { 
  if(n&0x80)
  {
    //branch backward
    n=~n;
    pc=pc-(n*2);
  }else{
  //branch forward
  pc=pc+(n*2)+2;
    }
  }else
    pc+=2;
}

/*
n (value to branch)           :MSB=1, branch backward and vice versa
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: bnov n
Opcode : 1110 0101 nnnn nnnn
*/
void  bnov(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  uint8_t n;
  n=(uint8_t) codeptr[0];
  if(status&0x08)
  { 
    pc+=2;
  }else
    if(n&0x80)
  {
    //branch backward
    n=~n;
    pc=pc-(n*2);
  }else{
  //branch forward
  pc=pc+(n*2)+2;
    }
}

/*
n (value to branch)           :MSB=1, branch backward and vice versa
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: bz n
Opcode : 1110 0000 nnnn nnnn
*/
void  bz(){
  // execute the instruction
  uint8_t *codeptr = &codeMemory[pc];
  uint8_t n;
  n=(uint8_t) codeptr[0];
  if(status&0x04)
  { 
  if(n&0x80)
  {
    //branch backward
    n=~n;
    pc=pc-(n*2);
  }else{
  //branch forward
  pc=pc+(n*2)+2;
    }
  }else
    pc+=2;
}

/*
a (BANK)          :0==> ACCESS BANK, 1==>BANK
f (File register) : range from 0 to 255
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: movwf f, a
Opcode : 0110 111a ffff ffff
*/
void  movwf(){
  uint8_t *codeptr = &codeMemory[pc];
  int addr = (uint8_t) codeptr[0];
  int a = codeptr[1]&0x01;
  addr = computeFileRegAddress(addr, a);
  fileRegisters[addr]=wreg;
  pc+=2;
}

/*
d (Direction)     :0==>WREG, 1==>bank
a (BANK)          :0==> ACCESS BANK, 1==>BANK
f (File register) : range from 0 to 255
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: andwf f, d, a
Opcode : 0001 01da ffff ffff
*/
void  andwf(){
  uint8_t *codeptr = &codeMemory[pc];
  int addr = (uint8_t) codeptr[0];
  int a = codeptr[1]&0x01;
  int d = codeptr[1]&0x02;
  uint8_t finalValue;
  addr = computeFileRegAddress(addr, a);
  finalValue=fileRegisters[addr] & wreg;
  if(d){
  //d is 1 ==> file register
  fileRegisters[addr] = finalValue;
  }else{
  //d is 0
  wreg = finalValue;
  }
  pc+=2;
  if(finalValue==0)
    status |=STATUS_Z;
  if((finalValue&0x80)&&!(finalValue>0xFF))
    status |= STATUS_N;
}

/*
d (Direction)     :0==>WREG, 1==>bank
a (BANK)          :0==> ACCESS BANK, 1==>BANK
f (File register) : range from 0 to 255
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mnemonic: rlcf f, d, a
Opcode : 0011 01da ffff ffff
*/
void  rlcf(){
  uint8_t *codeptr = &codeMemory[pc];
  int addr = (uint8_t) codeptr[0];
  int a = codeptr[1]&0x01;
  int d = codeptr[1]&0x02; 
  int finalValue;
  addr = computeFileRegAddress(addr, a);
  if(status&0x01)
    finalValue=(fileRegisters[addr]<<1)+1;
  else
    finalValue=fileRegisters[addr]<<1;
  if(d){
  //d is 1 ==> file register
  fileRegisters[addr] = finalValue;
  }else{
  //d is 0
  wreg = finalValue;
  }
  pc+=2;
}  
