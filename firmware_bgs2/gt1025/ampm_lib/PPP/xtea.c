
#include "typedef.h"

void XTEA_Encrypt(U16 num_rounds, U32 v[2], U32 const k[4])
{
    U16 i;
    U32 v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
    
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}
 
void XTEA_Decrypt(U16 num_rounds, U32 v[2], U32 const k[4])
{
    U16 i;
    U32 v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;
    
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}
