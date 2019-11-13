
#ifndef __RSA_2048_BASE64_H__
#define __RSA_2048_BASE64_H__
#include "hal.h"
unsigned int str_hex(unsigned char *str,unsigned char *hex) ;
int base64_encode(const unsigned char * sourcedata, char * base64);
int base64_decode(const char * base64, unsigned char * dedata);

#endif //RSA_2048_BASE64_H


