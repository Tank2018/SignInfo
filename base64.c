#include "base64.h"


const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char   padding_char = '=';

unsigned int str_hex(UINT8 *str,UINT8 *hex) {
    UINT8 ctmp, ctmp1,half;
    UINT32 num=0;
    do{
        do{
            half = 0;
            ctmp = *str;
            if(!ctmp) break;
            str++;
        }while((ctmp == 0x20)||(ctmp == 0x2c)||(ctmp == '\t'));
        if(!ctmp) break;
        if(ctmp>='a') ctmp = ctmp -'a' + 10;
        else if(ctmp>='A') ctmp = ctmp -'A'+ 10;
        else ctmp=ctmp-'0';
        ctmp=ctmp<<4;
        half = 1;
        ctmp1 = *str;
        if(!ctmp1) break;
        str++;
        if((ctmp1 == 0x20)||(ctmp1 == 0x2c)||(ctmp1 == '\t'))
        {
            ctmp = ctmp>>4;
            ctmp1 = 0;
        }
        else if(ctmp1>='a') ctmp1 = ctmp1 - 'a' + 10;
        else if(ctmp1>='A') ctmp1 = ctmp1 - 'A' + 10;
        else ctmp1 = ctmp1 - '0';
        ctmp += ctmp1;
        *hex = ctmp;
        hex++;
        num++;
    }while(1);
    if(half)
    {
        ctmp = ctmp>>4;
        *hex = ctmp;
        num++;
    }
    return(num);
}

/**
* 编码代码
* const unsigned char * sourcedata， 源数组
* char * base64 ，码字保存
*/
int base64_encode(const unsigned char * sourcedata, char * base64) {
    int i=0, j=0;
    unsigned char trans_index=0; // 索引是8位，但是高两位都为0
    const int datalength = strlen((const char*)sourcedata);
    for (; i < datalength; i += 3){
        // 每三个一组，进行编码
        // 要编码的数字的第一个
        trans_index = ((sourcedata[i] >> 2) & 0x3f);
        base64[j++] = base64char[(int)trans_index];
        // 第二个
        trans_index = ((sourcedata[i] << 4) & 0x30);
        if (i + 1 < datalength){
            trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
            base64[j++] = base64char[(int)trans_index];
        }else{
            base64[j++] = base64char[(int)trans_index];
            base64[j++] = padding_char;
            base64[j++] = padding_char;
            break; // 超出总长度，可以直接break
        }
        // 第三个
        trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
        if (i + 2 < datalength){
            // 有的话需要编码2个
            trans_index |= ((sourcedata[i + 2] >> 6) & 0x03);
            base64[j++] = base64char[(int)trans_index];
            trans_index = sourcedata[i + 2] & 0x3f;
            base64[j++] = base64char[(int)trans_index];
        } else{
            base64[j++] = base64char[(int)trans_index];
            base64[j++] = padding_char;
            break;
        }
    }
    base64[j] = '\0';
    return 0;
}


/** 在字符串中查询特定字符位置索引
* const char *str ，字符串
* char c，要查找的字符
*/
int num_strchr(const char *str, char c) {
    const char *pindex = strchr(str, c);
    if (NULL == pindex){
        return -1;
    }
    return pindex - str;
}

/**
* 解码
* const char * base64 码字
* unsigned char * dedata， 解码恢复的数据
*/
int base64_decode(const char * base64, unsigned char * dedata) {
    int i = 0, j=0;
    int trans[4] = {0,0,0,0};
    for (;base64[i]!='\0';i+=4){
        // 每四个一组，译码成三个字符
        trans[0] = num_strchr(base64char, base64[i]);
        trans[1] = num_strchr(base64char, base64[i+1]);
        // 1/3
        dedata[j++] = ((trans[0] << 2) & 0xfc) | ((trans[1]>>4) & 0x03);
        if (base64[i+2] == '='){
            continue;
        } else{
            trans[2] = num_strchr(base64char, base64[i + 2]);
        }
        // 2/3
        dedata[j++] = ((trans[1] << 4) & 0xf0) | ((trans[2] >> 2) & 0x0f);
        if (base64[i + 3] == '='){
            continue;
        } else{
            trans[3] = num_strchr(base64char, base64[i + 3]);
        }
        // 3/3
        dedata[j++] = ((trans[2] << 6) & 0xc0) | (trans[3] & 0x3f);
    } dedata[j] = '\0';
    return 0;
}