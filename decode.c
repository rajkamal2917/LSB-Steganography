#include <stdio.h>
#include "decode.h"
#include "types.h"
#define SECRET_FILE_EXT ".txt"

Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /*
     *check argv[2] is having .bmp or not
     *yes -> store the file name into decInfo->src_image_fname = argv[2]
     *no - > return e_failure
     */
    char *ext = strstr(argv[2], ".");
    if (ext != NULL && strcmp(ext, ".bmp") == 0)
    {
        decInfo->src_image_fname = argv[2];
    }
    else
        return d_failure;

    /*
     * if user gave the name for output file then store the argv[3] into decInfo->output_fname
     *if argv[3] is NULL ,create a file in the name of output and store into decInfo->output_fname
     */
    if (argv[3] != NULL)
        strcpy(decInfo->output_fname, argv[3]);
    else
    {
        strcpy(decInfo->output_fname, "output");
    }
    // append the ".txt" ext into decInfo->output_fname
    strcat(decInfo->output_fname, SECRET_FILE_EXT);
    return d_success;
}
Status_d open_file(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,RED "ERROR: Unable to open file %s\n"RESET, decInfo->src_image_fname);

        return d_failure;
    }

    // Open the output file in write mode
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    // Do error handling
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED"ERROR: Unable to open file %s\n"RESET, decInfo->output_fname);

        return e_failure;
    }
    return d_success;
}
Status_d decode_byte_from_lsb(char *image_buffer, char *data)
{
    /*
     * decode the lsb bit from every byte
     * repeat for 8 times
     * store the result into data
     */
    // decode byte from lsb
    uint result = 0;
    for (int i = 0; i < 8; i++)
    {
        result = result | ((image_buffer[i] & 1) << i);
    }
    *data = result;
    return d_success;
}
Status_d decode_size_from_lsb(char *image_buffer, uint *size)
{
    /*
     * decode the lsb bit from every byte
     * repeat for 32 times
     * store the result into size
     */
    uint result = 0;
    for (int i = 0; i < 32; i++)
    {
        result = result | ((image_buffer[i] & 1) << i);
    }
    *size = result;
    return d_success;
}
Status_d decode_magic_string(DecodeInfo *decInfo)
{
    /*
     *Skip the header (54 bytes) from the src_image file by using fseek
     *create buffer[8];
     *read 8 bytes from source file
     *call decode_byte_from_lsb(buffer,&decInfo->check_magic_string[i])
     *repeat this for strlen(decInfo->magic_string) times
     */

    fseek(decInfo->fptr_src_image, 54, SEEK_CUR);
    int len = strlen(decInfo->magic_string);
    char buffer[8];
    int i;
    for (i = 0; i < len; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_src_image);
        decode_byte_from_lsb(buffer, &decInfo->check_magic_string[i]);
    }
    decInfo->check_magic_string[i] = '\0'; // store '\0' at last

    /*
     * compare the decoded magic string by strcmp function
     * if yes return d_success
     * if no return d_failure
     */
    if (strcmp(decInfo->magic_string, decInfo->check_magic_string) == 0)
        return d_success;
    else
        return d_failure;
}
Status_d decode_secret_file_ext_size(DecodeInfo *decInfo)
{
    /*
     * create buffer[32]
     * read 32 bytes from source file
     * call decode_size_from_lsb(buffer,&decInfo->secret_file_ext_size)
     * return d_success
     */
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_src_image);
    decode_size_from_lsb(buffer, &decInfo->secret_file_ext_size);
    return d_success;
}
Status_d decode_secret_file_ext(DecodeInfo *decInfo)
{
    /*
     * create buffer[8]
     * read 8 bytes from source file
     *call decode_byte_from_lsb(buffer,&decInfo->secret_file_ext[i])
     *repeat this for decInfo->secret_file_ext_size times
     */

    char buffer[8];
    int i;
    for (i = 0; i < decInfo->secret_file_ext_size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_src_image);
        decode_byte_from_lsb(buffer, &decInfo->secret_file_ext[i]);
    }
    decInfo->secret_file_ext[i] = '\0'; // store '\0' at last

    /*
     * compare the decoded secret file ext by strcmp function
     * if yes return d_success
     * if no return d_failure
     */
    if (strcmp(SECRET_FILE_EXT, decInfo->secret_file_ext) == 0)
        return d_success;
    else
        return d_failure;
}
Status_d decode_secret_file_size(DecodeInfo *decInfo)
{
    /*
     * create buffer[32]
     * read 32 bytes from source file
     * call decode_size_from_lsb(buffer,&decInfo->secret_file_size)
     * return d_success
     */
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_src_image);
    decode_size_from_lsb(buffer, &decInfo->secret_file_size);
    return d_success;
}
Status_d decode_secret_file_data(DecodeInfo *decInfo)
{
    /*
     * create buffer[8]
     * var -> to store the decoded data
     * read 8 bytes from src file
     * call decode byte from lsb(buffer,&var)
     * write the decoded byte to the output file
     * repeat for decInfo->secret_file_size times
     */

    char buffer[8];
    char var;
    for (int i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_src_image);
        decode_byte_from_lsb(buffer, &var);
        fputc(var, decInfo->fptr_output);
    }
    return d_success;
}

Status_d Do_decoding(DecodeInfo *decInfo)
{
    if (open_file(decInfo))
    {
        printf(GREEN"Open the files done successfully\n"RESET);

        /* get the magic string from the user*/
        printf("Enter the Magic String : ");
        scanf(" %[^\n]", decInfo->magic_string);

        if(decode_magic_string(decInfo))
        {
            printf(GREEN"Magic string is matched successfully\n"RESET);

            if (decode_secret_file_ext_size(decInfo))
            {
                printf(GREEN"Secret file ext size is decoded successfully\n"RESET);

                if (decode_secret_file_ext(decInfo))
                {
                    printf(GREEN"Secret file ext decoded successfully\n"RESET);

                    if (decode_secret_file_size(decInfo))
                    {
                        printf(GREEN"Secret file size is decoded successfully\n"RESET);

                        if (decode_secret_file_data(decInfo))
                        {
                            printf(GREEN"Secret file data decoded successfully\n"RESET);
                        }
                        else
                        {
                            printf(RED"-->Error message: decoding secret file data failed\n"RESET);
                            return d_failure;
                        }
                    }
                    else
                    {
                        printf(RED"-->Error message: decoding secret file size failed\n"RESET);
                        return d_failure;
                    }
                }
                else
                {
                    printf(RED"-->Error message: secret file extension doesn't match\n"RESET);
                    return d_failure;
                }
            }
            else
            {
                printf(RED"-->Error message: decoding secret file extension size failed\n"RESET);
                return d_failure;
            }
        }
        else
        {
            printf(RED"-->Error message: magic string doesn't match\n"RESET);
            return d_failure;
        }
    }
    else
    {
        printf(RED"-->Error message: failed to open the files\n"RESET);
        return d_failure;
    }
    return d_success;
}