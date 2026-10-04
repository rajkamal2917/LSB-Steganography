#ifndef TYPES_H
#define TYPES_H
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define RESET   "\033[0m"

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_failure,
    e_success
} Status_e;

typedef enum
{
    d_failure,
    d_success
} Status_d;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
