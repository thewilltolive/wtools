#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "bkv_byteorder.h"

union float_long {
    float f;
    long l;
};

#if 0
static float bkv_frexpf(const float x, int *pw2){
    union float_long fl;
    long int i;

    fl.f=x;
    i = (fl.l >> 23) & 0xff;
    i -= 0x7e;
   *pw2=i;
    fl.l &= 0x807fffff;
    fl.l |= 0x3f000000;
    return(fl.f);
}
#endif

int le16_to_cpu(uint8_t *m, uint16_t *v, int *offset){
    *v=(m[0]<<8)+m[1];
    if (offset)
        *offset+=2;
    return(0);
}

int cpu_to_le16(int16_t v,uint8_t *m, int *offset){
    m[0]=(v>>8)&0xFF;
    m[1]=(v)&0xFF;
    if (offset)
        *offset+=sizeof(int16_t);
    return(0);
}

int le32_to_cpu(uint8_t *m, uint32_t *v, int *offset){
    *v=(m[0]<<24)+(m[1]<<16)+(m[2]<<8)+m[3];
    if (offset)
        *offset+=sizeof(uint32_t);
    return(0);
}

int le64_to_cpu(uint8_t *m, int64_t *v, int *offset){
    *v=((int64_t)m[0]<<56)+
        ((uint64_t)m[1]<<48)+
        ((uint64_t)m[2]<<40)+
        ((uint64_t)m[3]<<32)+
        ((uint64_t)m[4]<<24)+
        ((uint64_t)m[5]<<16)+
        ((uint64_t)m[6]<<8)+
        m[7];
    if (offset)
        *offset+=sizeof(int64_t);
    return(0);
}

int cpu_to_le32(int32_t v,uint8_t *m, int *offset){
    m[0]=(v>>24)&0xFF;
    m[1]=(v>>16)&0xFF;
    m[2]=(v>>8)&0xFF;
    m[3]=(v)&0xFF;
    if (offset)
        *offset+=sizeof(int32_t);
    return(0);
}

int cpu_to_le64(int64_t v,uint8_t *m, int *offset){
    m[0]=(v>>56)&0xFF;
    m[1]=(v>>48)&0xFF;
    m[2]=(v>>40)&0xFF;
    m[3]=(v>>32)&0xFF;
    m[4]=(v>>24)&0xFF;
    m[5]=(v>>16)&0xFF;
    m[6]=(v>>8)&0xFF;
    m[7]=(v)&0xFF;
    if (offset)
        *offset+=sizeof(int64_t);
    return(0);
}
int lef_to_cpu(uint8_t *m, float *v, int *offset){
    int32_t ivalue;
    le32_to_cpu(m,&ivalue,offset);
    ((uint8_t*)v)[3]= ivalue >> 24;
    ((uint8_t*)v)[2]= (ivalue >> 16) & 0xff;
    ((uint8_t*)v)[1]= (ivalue >> 8) & 0xff;
    ((uint8_t*)v)[0]= ivalue & 0xff;
    return(0);
}

int cpu_to_lef(float v, uint8_t *m, int *offset){
    int32_t tmp= (*((int32_t*)&v) & 0xFF000000)|(*((int32_t*)&v) & 0x07800000) | (*((int32_t*)&v) & 0x007fffff);
    cpu_to_le32(tmp,m,offset);
    return(0);
}

int cpu_to_le_double(double v, uint8_t *m, int *offset){
    int32_t tmp= (*((int32_t*)&v) & 0xFF000000)|(*((int32_t*)&v) & 0x07800000) | (*((int32_t*)&v) & 0x007fffff);
    cpu_to_le64(tmp,m,offset);
    return(0);
}

int le_double_to_cpu(uint8_t *m, double *v, int *offset){
    int64_t ivalue;
    le64_to_cpu(m,&ivalue,offset);
    ((uint8_t*)v)[7]= (ivalue >> 56) & 0xff;
    ((uint8_t*)v)[6]= (ivalue >> 48) & 0xff;
    ((uint8_t*)v)[5]= (ivalue >> 40) & 0xff;
    ((uint8_t*)v)[4]= (ivalue >> 32) & 0xff;
    ((uint8_t*)v)[3]= (ivalue >> 24) & 0xff;
    ((uint8_t*)v)[2]= (ivalue >> 16) & 0xff;
    ((uint8_t*)v)[1]= (ivalue >> 8) & 0xff;
    ((uint8_t*)v)[0]= ivalue & 0xff;

    if (offset)
        *offset+=sizeof(double);
    return(0);
}
