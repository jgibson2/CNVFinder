#ifndef __DNA_HASHER_H__
#define __DNA_HASHER_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

uint64_t hashDNAstringtonum(std::string);

std::string hashDNAnumtostring(uint64_t, const uint8_t);

std::string dnaReverseComplement(std::string);


#endif //__DNA_HASHER_H__
