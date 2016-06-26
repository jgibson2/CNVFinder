#ifndef __DNA_HASHER_H__
#define __DNA_HASHER_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t hashDNAstringtonum(char* , const uint32_t);

char* hashDNAnumtostring(uint64_t, const uint32_t);

#endif //__DNA_HASHER_H__
