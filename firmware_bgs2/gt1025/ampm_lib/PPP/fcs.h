#ifndef _FCS_H_
#define _FCS_H_

#include "typedef.h"

U16 FCS_Get(U8 *cp, U32 len);
U16 FCS_Calculate(U8 val, U16 fcs);

#endif
