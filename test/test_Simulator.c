#include "unity.h"
#include "Simulator.h"
#include "Helper.h"

extern  uint8_t fileRegisters[];
void setUp(void)
{
}

void tearDown(void)
{
}

void test_pic18ExecutionTable_given_0x2545_expect_addwf_called()
{
  uint8_t machineCode[]={0x45, 0x26, 0x00, 0xff};
  wreg=0x23;
  fileRegisters[0x45]=0x56;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_HEX8(0x79, fileRegisters[0x45]);
  TEST_ASSERT_EQUAL_HEX8(0x23, wreg);
}

void  test_add_given_0x1F_and_0xE1_expect_N0_OVx_Z1_DC1_C1(){
  status = 0x00;
  int value = add(0x1F, 0xE1);
  TEST_ASSERT_EQUAL_HEX8(0x1F+0xE1, value);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z|STATUS_DC|STATUS_C, status);
}

void  test_add_given_0x7F_and_0x01_expect_N1_OV1_Zx_DC1_Cx(){
  status = 0x00;
  int value = add(0x7F, 0x01);
  TEST_ASSERT_EQUAL_HEX8(0x7F+0x01, value);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N|STATUS_OV|STATUS_DC, status);
}


//incf 0x03, w, a   0010 1000 0000 0011(0x2803)
void  test_incf_given_0x2803_expect_N1_OV1_Zx_DC1_Cx(){
  status = 0x00;
  uint8_t machineCode[]={0x03, 0x28, 0x00, 0xff};
  fileRegisters[0x03]=0x7F;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_HEX8(0x7F, fileRegisters[0x03]);
  TEST_ASSERT_EQUAL_HEX8(0x80, wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N|STATUS_OV|STATUS_DC, status);
}

void test_executeInstruction_given_0x2645_expect_addwf_called(void)
{
  //Setup the text fixture
  uint8_t machineCode[]={0x45, 0x26, 0x00, 0xff};    //Little endian
  //Set WREG
  wreg = 0x23;
  // Set content of target file register
  fileRegisters[0x45] = 0x56;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x79,fileRegisters[0x45]);
  TEST_ASSERT_EQUAL_HEX8(0x23,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}


/*
Mnemonic: bcf f, b, a
Opcode : 1001 bbba ffff ffff

bcf   0x31, 5, ACCESS
      1001 1010 0011 0001(0x9A31)
*/


//addwf 0x23, w, ACCESS ==> 0010 0100 0010 0011 (0x2413)
void test_executeInstruction_given_0x2413_expect_addwf_called(void)
{
  //Setup the text fixture
  uint8_t machineCode[]={0x13, 0x24, 0x00, 0xff};    //Little endian
  //Set WREG
  wreg = 0xA3;
  // Set content of target file register
  fileRegisters[0x13] = 0x10;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x10,fileRegisters[0x13]);
  TEST_ASSERT_EQUAL_HEX8(0xB3,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//addwf 0x67, w, ACCESS ==> 0010 0100 0010 0011 (0x2467)
void test_executeInstruction_given_0x2467_expect_addwf_called(void)
{
  //Setup the text fixture
  uint8_t machineCode[]={0x67, 0x24, 0x00, 0xff};    //Little endian
  //Set WREG
  wreg = 0x56;
  // Set content of target file register
    fileRegisters[0x67] = 0x00;
    fileRegisters[0xf67] = 0x43;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x43,fileRegisters[0xf67]);
  TEST_ASSERT_EQUAL_HEX8(0x99,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}


//addwf 0x8A,w,BANKED
void test_executeInstruction_given_0x258A_expect_addwf_called(void)
{
  //Setup the text fixture
  uint8_t machineCode[]={0x8A, 0x25, 0x00, 0xff};    //Little endian
  //Set WREG
  wreg = 0x44;
  //Set BSR
  bsr = 3;
  // Set content of target file register
    fileRegisters[0x8A] = 0x00;
    fileRegisters[0x38A] = 0x41;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x41,fileRegisters[0x38A]);
  TEST_ASSERT_EQUAL_HEX8(0x85,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//clrf 0x35, a   0110 1010 0011 0101(0x6A35)
void  test_clrf_given_0x2803_access_expect_Nx_OVx_Zx_DC0_Cx(){
  status = 0x00;
  uint8_t machineCode[]={0x35, 0x6A, 0x00, 0xff};
  fileRegisters[0x35]=0x71;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0x35]);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//clrf 0x17, b   0110 1011 0001 0111(0x6B17)
void  test_clrf_given_0x6B17_banked_expect_Nx_OVx_Zx_DC0_Cx(){
  status = 0x00;
  uint8_t machineCode[]={0x17, 0x6B, 0x00, 0xff};
  bsr=4;
  fileRegisters[0x17]=0x11;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0x417]);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//bcf 0x23, 3, a    1001 0110 0010 0011(0x9623)
void  test_bcf_given_0x6B17_access_expect_0xF7(){
  uint8_t machineCode[]={0x23, 0x96, 0x00, 0xff};
  fileRegisters[0x23]=0xFF;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_HEX8(0xF7, fileRegisters[0x23]);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//bcf 0x23, 6, b    1001 1101 0010 0011(0x9D23)
void  test_bcf_given_0x9D23_access_expect_0xBF(){
  uint8_t machineCode[]={0x23, 0x9D, 0x00, 0xff};
  bsr=8;
  fileRegisters[0x823]=0xFF;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_HEX8(0xBF, fileRegisters[0x823]);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//bcf 0x98, 6, a    1001 1100 1001 1000(0x9C98)
void  test_bcf_given_0x9C98_access_expect_0xBF(){
  uint8_t machineCode[]={0x98, 0x9C, 0x00, 0xff};
  bsr=8;
  fileRegisters[0xf98]=0xFF;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_HEX8(0xBF, fileRegisters[0xf98]);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}


/*
ADDRESS
~~~~~~~~
0x00       0x9C56
0x02       0x8201<== bn 0x01 (1000 0010 0000 0001)
0x04       0xFF00
0x06       0x1255
0x08       0x3657
*/

//bn 0x01   1110 0110 0000 0001(0xE601)
void  test_bn_given_0x8201_expect_pc_equals_0xABD0(){
  status=0x10;
  uint8_t machineCode[]={0x01, 0xE6};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABD2, pc);
}

//bn 0x23   1110 0110 0010 0011(0xE623)
void  test_bn_given_0xE623_expect_pc_equals_0xAC16(){
  status=0x10;
  uint8_t machineCode[]={0x23, 0xE6};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xAC16, pc);
}

//bn 0xFF   1110 0110 1111 1100(0xE6FC)
void  test_bn_given_0xE6FC_expect_pc_equals_0xABCB(){
  status=0x10;
  uint8_t machineCode[]={0xFC, 0xE6};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABC8, pc);
}

//bn 0xF6   1110 0110 1111 0110(0xE6F6)
void  test_bn_given_0xE6F6_expect_pc_equals_0xABBC(){
  status=0x10;
  uint8_t machineCode[]={0xF6, 0xE6};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABBC, pc);
}

//bn 0xF6   1110 0110 1111 0110(0xE6F6)
void  test_bn_given_0xE6F6_status_not_negative_expect_pc_equals_0xABD0(){
  status=0x00;
  uint8_t machineCode[]={0xF6, 0xE6};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//bn 0xF6   1110 0001 1111 0110(0xE1F6)
void  test_bnz_given_0xE1F6_status_not_zero_expect_pc_equals_0xABBC(){
  status=0x00;
  uint8_t machineCode[]={0xF6, 0xE1};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABBC, pc);
}

//bn 0xF6   1110 0001 1111 0110(0xE1F6)
void  test_bnz_given_0xE1F6_status_zero_expect_pc_equals_0xABBC(){
  status=0x04;
  uint8_t machineCode[]={0xF6, 0xE1};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//bc 0x34   1110 0010 0011 0100(0xE234)
void  test_bc_given_0xE234_status_carry_expect_pc_equals_0xAC04(){
  status=0x01;
  uint8_t machineCode[]={0x34, 0xE2};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xAC38, pc);
}

//bc 0x34   1110 0010 0011 0100(0xE234)
void  test_bc_given_0xE234_status_xcarry_expect_pc_equals_0xABCE(){
  status=0x00;
  uint8_t machineCode[]={0x34, 0xE2};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//bc 0xF0   1110 0010 1111 0000(0xE2F0)
void  test_bc_given_0xE2F0_status_carry_expect_pc_equals_0xABB0(){
  status=0x01;
  uint8_t machineCode[]={0xF0, 0xE2};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABB0, pc);
}

//bc 0x12   1110 0101 0001 0010(0xE512)
void  test_bc_given_0xE512_status_ov_expect_pc_equals_0xABF4(){
  status=0x08;
  uint8_t machineCode[]={0x12, 0xE5};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABF4, pc);
}

//bc 0x12   1110 0101 0001 0010(0xE512)
void  test_bc_given_0xE512_status_xov_expect_pc_equals_0xABF4(){
  status=0x00;
  uint8_t machineCode[]={0x12, 0xE5};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//bc 0xE9   1110 0101 1110 1001(0xE5E9)
void  test_bc_given_0xE512_status_ov_expect_pc_equals_0xABA2(){
  status=0x08;
  uint8_t machineCode[]={0xE9, 0xE5};
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
  TEST_ASSERT_EQUAL_PTR(0xABA2, pc);
}

//movwf 0x37, a    0110 1110 0011 0111(0x6E37)
void test_movwf_given_0x0x6E37_expect_fileRegisters0x37_equals_0x46(void)
{
  //Setup the text fixture
  uint8_t machineCode[]={0x37, 0x6E, 0x00, 0xff};    //Little endian
  //Set WREG
  wreg = 0x46;
  // Set content of target file register
    fileRegisters[0x37] = 0x00;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x46,fileRegisters[0x37]);
  TEST_ASSERT_EQUAL_HEX8(0x46,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//movwf 0x8A, a    0110 1110 1000 1010(0x6E8A)
void test_movwf_given_0x0x6E8A_expect_fileRegisters0xF37_equal_0x46(void)
{
  //Setup the text fixture
  uint8_t machineCode[]={0x8A, 0x6E, 0x00, 0xff};    //Little endian
  //Set WREG
  wreg = 0x46;
  // Set content of target file register
    fileRegisters[0x8A] = 0x10;
    fileRegisters[0xF8A] = 0x10;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x46,fileRegisters[0xF8A]);
  TEST_ASSERT_EQUAL_HEX8(0x10,fileRegisters[0x8A]);
  TEST_ASSERT_EQUAL_HEX8(0x46,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

//movwf 0x37, b    0110 1111 0011 0111(0x6F37)
void test_movwf_given_0x0x6F37_expect_fileRegisters0x37_equals_0x46(void)
{
  //Setup the text fixture
  uint8_t machineCode[]={0x37, 0x6F, 0x00, 0xff};    //Little endian
  //Set WREG
  wreg = 0x46;
  //Set BSR
  bsr=6;
  // Set content of target file register
    fileRegisters[0x37] = 0x00;
    fileRegisters[0x637] = 0x00;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x46,fileRegisters[0x637]);
  TEST_ASSERT_EQUAL_HEX8(0x00,fileRegisters[0x37]);
  TEST_ASSERT_EQUAL_HEX8(0x46,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

/*
    01000110
    00110110
   ----------
    00000110
*/

//andwf 0x78, w, a    0001 0100 0100 1000(0x1448)
void test_andwf_given_0x0x1448_expect_fileRegisters0x37_equals_0x46(void){
  //Setup the text fixture
  uint8_t machineCode[]={0x48, 0x14, 0x00, 0xff};    //Little endian
  //Set WREG
  status = 0x00;
  wreg = 0x46;
  // Set content of target file register
    fileRegisters[0x48] = 0x36;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x36,fileRegisters[0x48]);
  TEST_ASSERT_EQUAL_HEX8(0x06,wreg);
    TEST_ASSERT_EQUAL_HEX8(0x00, status);
    TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

/*
    11110000
    00001111
   ----------
    00000000
*/


//andwf 0x70, f, b    0001 0111 0111 0000(0x1770)
void test_andwf_given_0x1770_expect_fileRegisters0x37_equals_0x46(void){
  //Setup the text fixture
  uint8_t machineCode[]={0x70, 0x17, 0x00, 0xff};    //Little endian
  //Set WREG
  status = 0x00;
  wreg = 0xF0;
  bsr=8;
  // Set content of target file register
    fileRegisters[0x870] = 0x0F;
    fileRegisters[0x70] = 0x11;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00,fileRegisters[0x870]);
  TEST_ASSERT_EQUAL_HEX8(0x11,fileRegisters[0x70]);
  TEST_ASSERT_EQUAL_HEX8(0xF0,wreg);
    TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
    TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

/*
    11110000
    00011111
   ----------
    00000000
*/


//andwf 0x70, f, a    0001 0110 0111 0000(0x1670)
void test_andwf_given_0x1670_expect_fileRegisters0x70_equals_0x10(void){
  //Setup the text fixture
  uint8_t machineCode[]={0x70, 0x16, 0x00, 0xff};    //Little endian
  //Set WREG
  status = 0x00;
  wreg = 0xF0;
  // Set content of target file register
    fileRegisters[0x70] = 0x00;
    fileRegisters[0xF70] = 0x1F;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00,fileRegisters[0x70]);
  TEST_ASSERT_EQUAL_HEX8(0x10,fileRegisters[0xF70]);
  TEST_ASSERT_EQUAL_HEX8(0xF0,wreg);
    TEST_ASSERT_EQUAL_HEX8(0x00, status);
    TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
}

// rlcf 0x36, w, a     0011 0100 0011 0110(0x3436)
void test_rlcf_given_0x3436_no_carry_expect_fileRegisters0x70_equals_0x10(void){
  //Setup the text fixture
  uint8_t machineCode[]={0x36, 0x34, 0x00, 0xff};    //Little endian
  //Set WREG
  status = 0x00;
  wreg = 0x0F;
  // Set content of target file register
    fileRegisters[0x36] = 0x0E;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x0E,fileRegisters[0x36]);
  TEST_ASSERT_EQUAL_HEX8(0x1C,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
   // TEST_ASSERT_EQUAL_HEX8(0x00, status);
}

// rlcf 0x36, w, a     0011 0100 0011 0110(0x3436)
void test_rlcf_given_0x3436_with_carry_expect_fileRegisters0x70_equals_0x10(void){
  //Setup the text fixture
  uint8_t machineCode[]={0x36, 0x34, 0x00, 0xff};    //Little endian
  //Set WREG
  status = 0x01;
  wreg = 0x0F;
  // Set content of target file register
    fileRegisters[0x36] = 0x0E;
  //Copy instruction to code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Run the code under test
  executeInstruction();
 // Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x0E,fileRegisters[0x36]);
  TEST_ASSERT_EQUAL_HEX8(0x1D,wreg);
  TEST_ASSERT_EQUAL_PTR(0xABD0, pc);
   // TEST_ASSERT_EQUAL_HEX8(0x00, status);
}

