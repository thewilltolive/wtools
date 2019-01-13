#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "bkv_file.h"
#include "lg.h"

static const char *s_file="bkv_file.c";

int bkv_file_create_and_mmap(int fd, int new_size, uint8_t **pp_ptr){
    int      l_ret=BKV_INV_ARG;
    uint8_t *l_ptr;
#ifdef __USE_XOPEN_EXTENDED
    if (-1 == (l_ret = truncate(fd,new_size))){
    }
#else
    char c=0;
    if ((new_size-1) != lseek(fd,new_size-1,SEEK_CUR)){
    }
    else if (1 != write(fd,&c,1)){
    }
#endif
    else if (MAP_FAILED == (l_ptr = mmap(NULL,new_size,PROT_WRITE,MAP_SHARED,fd,0))){

    }
    else {
        *pp_ptr = l_ptr;
        l_ret=BKV_OK;
    }
    return(l_ret);
}

bkv_error_t bkv_file_truncate(int fd,
                              int size){
    int l_ret = BKV_KO;
    int l_posix_ret=-1;
    if (-1 != fd){
        if (-1 == (l_posix_ret = ftruncate(fd,size))){
            lg_print(LG_LEVEL_ERROR,__FUNCTION__,__LINE__,
                     "Failed to truncate fd");
        }
    }
    if (0 == l_posix_ret){
        l_ret=BKV_OK;
    }
    return(l_ret);
}


bkv_error_t bkv_open_file_update(const char      *p_file,
                                       int              mode,
                                       int             *p_fd,
                                       int             *p_size,
                                       uint8_t        **p_ptr){
    int          l_fd;
    int          l_ret=BKV_INV_ARG;
    uint8_t     *l_ptr=NULL;
    struct stat  l_statbuf;
    if (-1 == (l_fd = open(p_file,O_RDWR,mode))){
        lg_print(LG_LEVEL_ERROR,s_file,__LINE__,"Failed to create file %s. errno=%d",p_file,errno);
    }
    else if (-1 == (l_ret = fstat(l_fd,&l_statbuf))){
    }
    else if (MAP_FAILED == (l_ptr = mmap(NULL,l_statbuf.st_size,PROT_WRITE|PROT_READ,MAP_PRIVATE,l_fd,0))){
    }
    else {
        *p_fd=l_fd;
        *p_size=l_statbuf.st_size;
        *p_ptr=l_ptr;
        l_ret=BKV_OK;
    }
    if (BKV_OK != l_ret){
        if (-1 != l_fd){
             close(l_fd);
        }
    }
    return(l_ret);
}

bkv_error_t bkv_open_file_create_write(const char      *p_file,
                                       int              mode,
                                       int             *p_fd,
                                       int             *p_size,
                                       uint8_t        **p_ptr){
    int          l_fd;
    int          l_ret=BKV_INV_ARG;
    /* if file has mode 0, it will not be accessible unless you're root. So error. */
    if (0 == mode){
    }
    else if (-1 == (l_fd = open(p_file,O_CREAT|O_TRUNC|O_RDWR,mode))){
        lg_print(LG_LEVEL_ERROR,s_file,__LINE__,"Failed to create file %s. errno=%d",p_file,errno);
    }
    else {
        *p_fd=l_fd;
        *p_size=0;
        *p_ptr=NULL;
        l_ret=BKV_OK;
    }
    if (BKV_OK != l_ret){
        if (-1 != l_fd){
             close(l_fd);
        }
    }
    return(l_ret);
}

bkv_error_t bkv_open_file_readonly(const char      *p_file,
                                   int             *p_fd,
                                   int             *p_size,
                                   uint8_t        **p_ptr){
    int          l_fd;
    struct stat  l_statbuf;
    int          l_ret=BKV_INV_ARG;
    uint8_t     *l_ptr;
    if (-1 == (l_fd = open(p_file,O_RDONLY))){
        lg_print(LG_LEVEL_ERROR,s_file,__LINE__,"Failed to create file %s. errno=%d",p_file,errno);
    }
    else if (-1 == fstat(l_fd,&l_statbuf)){
    }
    else if (MAP_FAILED == (l_ptr = mmap(NULL,l_statbuf.st_size,PROT_READ,MAP_SHARED,l_fd,0))){
    }
    else {
        *p_fd=l_fd;
        *p_size=l_statbuf.st_size;
        *p_ptr=l_ptr;
        l_ret=BKV_OK;
    }
    if (-1 == l_ret){
        if (-1 != l_fd){
             close(l_fd);
        }
        if (NULL != l_ptr){
            munmap(l_ptr,l_statbuf.st_size);
        }
    }
    return(l_ret);
}

int bkv_file_close(int fd, uint8_t *p_ptr, int len){
    if (-1 == munmap(p_ptr,len)){
        lg_print(LG_LEVEL_ERROR,s_file,__LINE__,"failed to munmap (%p,%d)",p_ptr,len);
    }
    if (-1 == close(fd)){
        lg_print(LG_LEVEL_ERROR,s_file,__LINE__,"failed to close fd (%d)",fd);
    }
    return(0);
}

int bkv_file_sync(int fd, uint8_t *ptr){
    (void)ptr;
#ifdef __USE_XOPEN_EXTENDED
    sync(fd);
#else
    (void)fd;
#endif
    return(0);
}

