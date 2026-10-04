/*
DOCUMENTATION

NAME        :Rajkamal V
ID          :26012_111
START DATE  :01/09/2026
END DATE    :18/09/2026

SAMPLE INPUT:
-------------------------------------------------------------
                encoding enabled 
-------------------------------------------------------------

read and validate encode arguments are successfully done 
Open the files done successfully
Enter the Magic String:@#
Check capacity done successfully 
BMP header copied successfully
Magic string encoded successfully
Secret file extension size encoded successfully
Secret file extension encoded successfully
Secret file size encoded successfully
Secret file data encoded successfully
Copy remaining image data done successfully

-->>Encoding successfully done

SAMPLE OUTPUT:
-------------------------------------------------------------
                decoding enabled
-------------------------------------------------------------

read and validation decode arguments are done successfully
Open the files done successfully
Enter the Magic String : @#
Magic string is matched successfully
Secret file ext size is decoded successfully
Secret file ext decoded successfully
Secret file size is decoded successfully
Secret file data decoded successfully

-->>Decoding done successfully

*/

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[]) // .aout -e <source_file> <secret_data_file> <optional>
{
    //step 1 -> check check_operation_type(argv[1]) is returning e_encode or not
        // yes -> EncodeInfo encInfo;
            // check read_and_validate_encode_args(argv, &encInfo) is returning e_success or e_failure
                // failure -> print error msg and stop
                // success -> print success msg 
                            // check do_encoding(&encInfo) is returning e_success or e_failure
                                    // failure -> print error msg and stop
                                    // success -> print success msg and stop
    if(argc<2)
    {
        printf(RED "-->Error message : necessary arguments are missing\n" RESET);
        return 0;
    }                                
    if(check_operation_type(argv[1])==e_encode)
    {
        if(argc<4)
        {
            printf(RED"-->Error message: necessary arguments are missing\n"RESET);
            return 0;
        }
        printf(BLUE "\n-------------------------------------------------------------\n"RESET);
        printf(BLUE"\t\tencoding enabled \n"RESET);
        printf(BLUE "-------------------------------------------------------------\n\n"RESET);
        EncodeInfo encInfo;  // structure variable declaration.

        if(read_and_validate_encode_args(argv,&encInfo))
        {
            printf(GREEN"read and validate encode arguments are successfully done \n"RESET);

            if(do_encoding(&encInfo))   //function call 
            {   
                printf("\n");
                printf("-->>"BLUE"Encoding successfully done\n"RESET);
                
            }

            else
            {
                printf(RED"-->Error message: encoding failed\n"RESET);
                return 0;
            }
        }
        else 
            printf(RED"-->Error message: read and validate encode args is failed \n"RESET);
    }

    //step 2 -> check check_operation_type(argv[1]) is returning e_decode or not
        // yes -> DecodeInfo decInfo;
            // check read_and_validate_decode_args(argv,&decInfo) is returning e_success or e_failure
                // failure -> print error msg and stop
                // success -> print success msg 
                            // check Do_decoding(&decInfo) is returning e_success or e_failure
                                    // failure -> print error msg and stop
                                    // success -> print success msg and stop
    else if(check_operation_type(argv[1])==e_decode)
    {
        if(argc<3)
        {
            printf(RED"-->Error message: necessary arguments are missing\n"RESET);
            return 0;
        }
        printf(BLUE"\n-------------------------------------------------------------\n"RESET);
        printf(BLUE"\t\tdecoding enabled\n"RESET);
        printf(BLUE"-------------------------------------------------------------\n\n"RESET);
        DecodeInfo decInfo;
        if(read_and_validate_decode_args(argv,&decInfo))
        {
            printf(GREEN"read and validation decode arguments are done successfully\n"RESET);
            if(Do_decoding(&decInfo))
            {
                printf("\n");
                printf("-->>"BLUE"Decoding done successfully\n"RESET);
            }   
            else
            {
                printf(RED"-->Error message : decoding failed\n"RESET);
            }
        }
        else
            printf(RED"-->Error message : source file doesn't contain '.bmp'\n"RESET);
    }

    
    else
        printf(RED"-->Error message : '-e' '-d' argument is missing \n"RESET);
    return 0;
        
}

OperationType check_operation_type(char *symbol)
{
    //step 1 -> check symbol is -e or not
        // yes -> return e_encode
    if(strcmp(symbol,"-e")==0)
    {
        return e_encode;
    }

    // step 2 -> check symbol is -d or not
        // yes -> return e_decode
    else if(strcmp(symbol,"-d")==0)
        return e_decode;

    // return e_unsupported
    else    
        return e_unsupported;
}
