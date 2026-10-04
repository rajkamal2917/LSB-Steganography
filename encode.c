#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    // return secret file size
    fseek(fptr, 0, SEEK_END);
    return (uint)ftell(fptr);
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status_e read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /* 
     *check argv[2] is having .bmp or not
        *yes -> store the file name into encInfo -> src_image_fname = argv[2]
        *no - > return e_failure
    */
    char *ext = strstr(argv[2], ".");
    if (ext != NULL && strcmp(ext, ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
        return e_failure;

    /*
     *check argv[3] is having extn there or not
        *yes - > store the file name into encInfo -> secret_fname = argv[3]
        *no -> return e_failure
    */
    ext = strstr(argv[3], ".");
    if (ext != NULL && strcmp(ext, ".txt") == 0)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, ".txt");
    }
    else
        return e_failure;
    /*
     * check argv[4] is NULL or not
        * not -> check argv[4] is having .bmp or not
            * yes-> store the file name into encInfo -> stego_image_fname = argv[4]
            * no-> return e_failure
        * NULL-> store default name the encInfo -> stego_image_fname = "stego.bmp"
    */

    if (argv[4] != NULL)
    {
        ext = strstr(argv[4], ".");
        if (ext != NULL && strstr(ext, ".bmp") != NULL)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
            return e_failure;
    }
    else
        encInfo->stego_image_fname = "stego.bmp";

    return e_success;
}

Status_e open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,RED "ERROR: Unable to open file %s\n"RESET, encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED"ERROR: Unable to open file %s\n"RESET, encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED"ERROR: Unable to open file %s\n"RESET, encInfo->stego_image_fname);

        return e_failure;
    }
    return e_success;
}

Status_e check_capacity(EncodeInfo *encInfo)
{
    // store image size into encInfo->image_capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //printf("size of source file:%u\n", encInfo->image_capacity);

    // store secret file size into encInfo->size_secret_file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    //printf("size of secret file: %lu\n", encInfo->size_secret_file);

    // check the image capacity
    if (encInfo->image_capacity >= (strlen(encInfo->magic_string) * 8) + 32 + (strlen(encInfo->extn_secret_file) * 8) + 32 + (encInfo->size_secret_file * 8))
        return e_success;
    else
        return e_failure;
}

Status_e copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    /* 
     *rewind source file pointer
     * read 54 bytes from source file
     * write 54 bytes to dest file
     * return e_success
     */

    char header[54];
    rewind(fptr_src_image);
    fread(header, 54, 1, fptr_src_image);
    fwrite(header, 54, 1, fptr_dest_image);
    return e_success;
}
Status_e encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    /*
     * char buffer[8]
     *read 8 bytes from source file
     *call encode_byte_to_lsb(magic_string[i], buffer)
     *write the buffer into dest file
     *repeat this for strlen(magic_string) times from step 1
     */

    char buffer[8];
    int len = strlen(magic_string);
    for (int i = 0; i < len; i++)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
}

Status_e encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    /*
     * create char buffer[32]
     *read 32 bytes from source file
     *call encode_size_to_lsb(size, buffer)
     *write the buffer into dest file
     */

    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
 
}

Status_e encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    /* create char buffer[8]
     * read 8 bytes from source file
     *call encode_byte_to_lsb(file_extn[i], buffer)
     *write the buffer into dest file
     *repeat this for strlen(file_extn) times from step 1
     */

    int size = strlen(file_extn);
    char buffer[8];
    for (int i = 0; i < size; i++)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
}

Status_e encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    /* create char buffer[32]
     * read 32 bytes from source file
     *call encode_size_to_lsb(file_size, buffer)
     *write the buffer into dest file
     */

    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
}

Status_e encode_secret_file_data(EncodeInfo *encInfo)
{
    /*
     * read secret data from file and store it into encInfo -> secret_data
     * create char buffer[8]
     * read 8 bytes from source file
     * call encode_byte_to_lsb(encInfo -> secret_data[i], buffer)
     * write the buffer into dest file
     * repeat this for strlen(encInfo -> size_secret_file) times from step 2
     */
    rewind(encInfo->fptr_secret);
    char ch;
    char buffer[8];
    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(&ch,1,1,encInfo->fptr_secret);
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
}

Status_e copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    /*
     * copy the remaining data 
     */
    char ch;
    while (fread(&ch, 1, 1, fptr_src) == 1)
        fwrite(&ch, 1, 1, fptr_dest);

    if (ftell(fptr_src) == ftell(fptr_dest))
        return e_success;
    else
        return e_failure;
}

Status_e encode_byte_to_lsb(char data, char *image_buffer)
{
    /* 
     * ((data >> i)&1)-->get the bit from LSB to MSB.
     * (image_buffer[i]&0XFE)--> clear the LSB bit from every byte.
     * Using the OR (|) we add the get bit into LSB to every byte.
     * repeat the process for 8 times because of we have to modify the LSB bit for every byte.
     */

    //encode byte to lsb
    for (int i = 0; i < 8; i++) 
    {
        image_buffer[i] = ((data >> i) & 1) | (image_buffer[i] & 0XFE);
    }
}

Status_e encode_size_to_lsb(int size, char *imageBuffer)
{
    /* 
     * ((ext_size >> i) & 1) get the bit from lsb to msb
     * (imageBuffer[i] & 0XFE) clear the lsb bit to every byte in image buffer
     * (((ext_size >> i) & 1) | (imageBuffer[i] & 0XFE)) add both using (|) and store into their respective image buffer
     */

    unsigned int ext_size = (unsigned int)size;
    for (int i = 0; i < 32; i++)
    {
        imageBuffer[i] = (((ext_size >> i) & 1) | (imageBuffer[i] & 0XFE));
    }
}

Status_e do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo))
    {
        printf(GREEN"Open the files done successfully\n"RESET);

        /* read the magic string from the user */
        printf("Enter the Magic String:");
        scanf(" %[^\n]", encInfo->magic_string);

        if (check_capacity(encInfo))
        {
            printf(GREEN"Check capacity done successfully \n"RESET);

            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image))
            {
                printf(GREEN"BMP header copied successfully\n"RESET);

                if (encode_magic_string(encInfo->magic_string, encInfo))
                {
                    printf(GREEN"Magic string encoded successfully\n"RESET);

                    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo))
                    {
                        printf(GREEN"Secret file extension size encoded successfully\n"RESET);

                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo))
                        {
                            printf(GREEN"Secret file extension encoded successfully\n"RESET);

                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo))
                            {
                                printf(GREEN"Secret file size encoded successfully\n"RESET);

                                if (encode_secret_file_data(encInfo))
                                {
                                    printf(GREEN"Secret file data encoded successfully\n"RESET);

                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image))
                                    {
                                        printf(GREEN"Copy remaining image data done successfully\n"RESET);
                                    }
                                    else
                                    {
                                        printf(RED"-->Error message : do_encoding failed...\n"RESET);
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf(RED"-->Error message : encoding secret file data failed\n"RESET);
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf(RED"-->Error message : encoding secret file size failed\n"RESET);
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf(RED"-->Error message : encoding secret file extension failed\n"RESET);
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf(RED"-->Error message : encoding secret file extension size failed\n"RESET);
                        return e_failure;
                    }
                }
                else
                {
                    printf(RED"-->Error message : encoding magic string failed\n"RESET);
                    return e_failure;
                }
            }
            else
            {
                printf(RED"-->Error message : copy bmp header failed\n"RESET);
                return e_failure;
            }
        }
        else
        {
            printf(RED"-->Error message : check capacity failed\n"RESET);
            return e_failure;
        }
    }
    else
    {
        printf(RED"-->Error message: open files failed\n"RESET);
        return e_failure;
    }

    return e_success;
}