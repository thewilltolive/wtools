/*
 * =====================================================================================
 *
 *       Filename:  w_error.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  26/04/2012 08:38:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef  W_ERROR_H_
#define  W_ERROR_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *@addtogroup  [BASENAME]
 *@{
 */

typedef enum
{
    W_NO_ERROR,
    W_E_NOT_INITIALIZED,
    W_E_OPERATION_FAILED,
    W_E_AGAIN,
    W_E_NO_MEMORY,
    W_E_BAD_PARAMETER

}w_error_t;


/**
 *@}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef W_ERROR_H_  ----- */

