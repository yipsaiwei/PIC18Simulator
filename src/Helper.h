#ifndef HELPER_H
#define HELPER_H

//The srcArray must be an array of char and codeAddress is the effect
//of PIC18 code memory space
#define copyCodeToCodeMemory(srcArray, codeAddress)                             \
            do{                                                                 \
              int i;                                                            \
              unsigned char *dst = (unsigned char *)&codeMemory[codeAddress];   \
              for (i = 0; i < sizeof(srcArray); i++){                           \
                dst[i] = (srcArray)[i];                                         \
              }                                                                 \
            }while(0)

#endif // HELPER_H
