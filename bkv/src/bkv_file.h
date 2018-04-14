/**
** @file bkv_file.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_FILE_H_
#define  BKV_FILE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

int bkv_open_file_create_write(const char      *p_file,
                               int              mode,
                               int             *p_fd,
                               int             *p_size,
                               uint8_t        **p_ptr);
int bkv_open_file_readonly(const char      *p_file,
                           int             *p_fd,
                           int             *p_size,
                           uint8_t        **p_ptr);
int bkv_file_truncate(int fd, int new_size, uint8_t **pp_ptr);
int bkv_file_sync(int fd, uint8_t *ptr);
int bkv_file_close(int fd, uint8_t *ptr, int len);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_FILE_H_  ----- */

