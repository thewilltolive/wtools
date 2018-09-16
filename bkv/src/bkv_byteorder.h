/**
** @file bkv_byteorder.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_BYTEORDER_H_
#define  BKV_BYTEORDER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

int le16_to_cpu(uint8_t *m, uint16_t *v, int *offset);
int cpu_to_le16(int16_t v, uint8_t *m, int *offset);
int le32_to_cpu(uint8_t *m, uint32_t *v, int *offset);
int cpu_to_le32(int32_t v, uint8_t *m, int *offset);
int le64_to_cpu(uint8_t *m, int64_t *v, int *offset);
int cpu_to_le64(int64_t v, uint8_t *m, int *offset);
int lef_to_cpu(uint8_t *m, float *v, int *offset);
int cpu_to_lef(float v, uint8_t *m, int *offset);
int le_double_to_cpu(uint8_t *m, double *v, int *offset);
int cpu_to_le_double(double v, uint8_t *m, int *offset);

 
    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_BYTEORDER_H_  ----- */

