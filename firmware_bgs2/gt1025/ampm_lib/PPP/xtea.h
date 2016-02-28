#ifndef _XTEA_H_
#define _XTEA_H_

void XTEA_Decrypt(U16 num_rounds, U32 v[2], U32 const k[4]);
void XTEA_Encrypt(U16 num_rounds, U32 v[2], U32 const k[4]);

#endif
