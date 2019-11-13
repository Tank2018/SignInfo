#ifndef __RSA_2048_PUBLIC_H__
#define __RSA_2048_PUBLIC_H__
#include "rsa.h"

#include "base64.h"
#include "hal.h"
int public_encrypt(uint8_t input[501],char base64[4096]);
int public_decrypt(char base64[4096],unsigned char msg [512]);

#endif //RSA_2048_PUBLIC_H


