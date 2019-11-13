#include "public.h"
#include "keys.h"

int public_encrypt(uint8_t input[501],char basee[4096])
{
	uint8_t  output[512];
	uint32_t outputLen;
	uint8_t  inputLen;
	rsa_pk_t pk = {0};
  const unsigned char *sourcedata;
	pk.bits = KEY_M_BITS;
	memcpyX(&pk.modulus         [RSA_MAX_MODULUS_LEN-sizeof(key_m)],  key_m,  sizeof(key_m));
	memcpyX(&pk.exponent        [RSA_MAX_MODULUS_LEN-sizeof(key_e)],  key_e,  sizeof(key_e));



	inputLen = strlen((const char *)input);


    // public key encrypt
	rsa_public_encrypt(output, &outputLen, input, inputLen, &pk);


    // base64 encode
	unsigned char buffer[4096];
  unsigned char temp[10];
  memsetX (buffer, 0x0, 4096);
	for(int i = 0; i<outputLen; i++) {
    //Print (L"%02X", output[i]);
    memsetX (temp, 0x0, 10);
		AsciiSPrint((CHAR8 *)temp, 10, "%02X", (unsigned char) output[i]);
    AsciiStringCat ((CHAR8 *)buffer, (CHAR8 *)temp);
	}
  //Print (L"\n%a\n",buffer);
	sourcedata = buffer ;
	base64_encode(sourcedata, basee);// encode
	

	return 1;
}

int public_decrypt(char base64[4096],unsigned char msg [512])
{
	rsa_pk_t pk = {0};
  uint32_t msg_len;
	pk.bits = KEY_M_BITS;
	memcpyX(&pk.modulus         [RSA_MAX_MODULUS_LEN-sizeof(key_m)],  key_m,  sizeof(key_m));
	memcpyX(&pk.exponent        [RSA_MAX_MODULUS_LEN-sizeof(key_e)],  key_e,  sizeof(key_e));
	
	

    

    // public key decrypt

    // base64 decode
	char dedata[4096];
	base64_decode(base64, (unsigned char*)dedata);// decode

	uint8_t str1[512];
	str_hex((unsigned char *)dedata,str1);

    // public key decrypt
	rsa_public_decrypt(msg, &msg_len, str1, sizeof(str1), &pk);
	//print_array("Public_key_decrypt", msg, msg_len);

	return 1;
}

