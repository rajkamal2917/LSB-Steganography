#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include <string.h>
#include "types.h"

typedef struct  _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;

    /* Secret file info */
    uint secret_file_ext_size;
    uint secret_file_size; 
    char check_magic_string[5];
    char secret_file_ext[4];
    char magic_string[5]; //store the magic string read from the user

    /* Output Info */
    char output_fname[100];
    FILE *fptr_output; 

}DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *DecInfo);

/* Open the files */
Status_d open_file(DecodeInfo *decInfo);

/* Perform the decoding operation */
Status_d Do_decoding(DecodeInfo *decInfo);

/* Decode magic string */
Status_d decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extension size */
Status_d decode_secret_file_ext_size(DecodeInfo *decInfo);

/* Decode secret file extension */
Status_d decode_secret_file_ext(DecodeInfo *decInfo);

/* Decode the secret file size */
Status_d decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status_d decode_secret_file_data(DecodeInfo *decInfo);

/* decode the byte from the lsb */
Status_d decode_byte_from_lsb(char *image_buffer,char *data);

/* decode the size from the lsb */
Status_d decode_size_from_lsb(char *image_buffer,uint *size);


#endif

