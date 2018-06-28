#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"

#define NOT_FOUND ((unsigned int)-1)

/**
 * Prints the help
 */
void print_help()
{
    printf("Usage: stegobmp [OPTIONS]\n");
    printf("Options:\n");
    printf("%-30s", "\t-i input-file");
    printf("path to the input file to be hidden.\n");
    printf("%-30s", "\t-h");
    printf("prints the help and quits.\n");
    printf("%-30s", "\t-o output-file");
    printf("path to the output file.\n");
    printf("%-30s", "\t-p bitmap-file");
    printf("path to the carrier BMP file.\n");
    printf("%-30s", "\t-steg steg-alg");
    printf("steganography algorithm.\n");
    printf("%-30s", "\t-a alg");
    printf("encryption algorithm.\n");
    printf("%-30s", "\t-m mode");
    printf("encryption mode.\n");
    printf("%-30s", "\t-pass password");
    printf("encryption password.\n");
}

StegAlgorithm steg_algorithm(char * steg_algorithm){
    char * steg_algorithms[] = {"LSB1", "LSB4", "LSBE"};
    int size = sizeof(steg_algorithms)/sizeof(char*);

    for (int i = 0; i < size; i++){
        if (strcmp(steg_algorithm, steg_algorithms[i]) == 0){
            return i;
        }
    }

    return NOT_FOUND;
}

EncryptionAlgorithm check_encryption_algorithm(char * encryption_algorithm){
    char * encryption_algorithms[] = {"aes128", "aes192", "aes256", "des"};
    int size = sizeof(encryption_algorithms)/sizeof(char*);

    for (int i = 0; i < size; i++){
        if (strcmp(encryption_algorithm, encryption_algorithms[i]) == 0){
            return i;
        }
    }

    return NOT_FOUND;
}

EncryptionMode check_encryption_mode(char * encryption_mode){
    char * encryption_modes[] = {"ecb", "cfb", "ofb", "cbc"};
    int size = sizeof(encryption_modes)/sizeof(char*);

    for (int i = 0; i < size; i++){
        if (strcmp(encryption_mode, encryption_modes[i]) == 0){
            return i;
        }
    }

    return NOT_FOUND;
}

Options * parse_options(int argc, char *argv[])
{
    Options * parameters = calloc(1, sizeof(Options));
    // Default values
    char * encryption_algorithm = "aes128";
    char * encryption_mode = "cbc";
    parameters->steg_algorithm = LSB1;

    int c;
    static int mode_flag = -1;

    while (1)
    {
        static struct option long_options[] =
            {
                {"extract", no_argument, &mode_flag, EXTRACT},
                {"embed", no_argument, &mode_flag, EMBED},
                {"in", required_argument, 0, 'i'},
                {"port", required_argument, 0, 'p'},
                {"out", required_argument, 0, 'o'},
                {"help", no_argument, 0, 'h'},
                {"steg", required_argument, 0, 's'},
                {"alg", required_argument, 0, 'a'},
                {"mode", required_argument, 0, 'm'},
                {"pass", required_argument, 0, 'k'},
                {0, 0, 0, 0}};

        int option_index = 0;

        c = getopt_long_only(argc, argv, "hi:o:p:s:a:m:k:", long_options, &option_index);

        /* End of the options */
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf(" with arg %s", optarg);
                printf("\n");
                break;
            /* Print help and quit */
            case 'h':
                print_help();
                exit(0);
                break;
            case 'i':
                parameters->input_file_name = optarg;
                break;
            case 'o':
                parameters->output_file_name = optarg;
                break;
            case 'p':
                parameters->carrier_file_name = optarg;
                break;
            case 's':
                parameters->steg_algorithm = steg_algorithm(optarg);

                if (parameters->steg_algorithm == NOT_FOUND){
                    free(parameters);
                    exit(1);
                }
                break;
            case 'a':
                encryption_algorithm = optarg;
                break;
            case 'm':
                encryption_mode = optarg;
                break;
            case 'k':
                parameters->password = optarg;
                break;
            case '?':
                break;
            default:
                abort();
        }
    }

    if (parameters->carrier_file_name == NULL)
    {
        fprintf(stderr, "You must specify a carrier file.\n");
        free(parameters);
        exit(1);
    }

    if (parameters->output_file_name == NULL)
    {
        fprintf(stderr, "You must specify an output file.\n");
        free(parameters);
        exit(1);
    }

    parameters->mode = mode_flag;

    if (!(parameters->mode == EMBED || parameters->mode == EXTRACT))
    {
        fprintf(stderr, "You must specify a valid execution mode.\n");
        free(parameters);
        exit(1);
    }

    if (parameters->input_file_name == NULL && parameters->mode == EMBED)
    {
        fprintf(stderr, "You must specify an input file.\n");
        free(parameters);
        exit(1);
    }

    parameters->encryption_algorithm = check_encryption_algorithm(encryption_algorithm);

    if (parameters->encryption_algorithm == NOT_FOUND)
    {
        fprintf(stderr, "Invalid encryption algorithm.\n");
        free(parameters);
        exit(1);
    }

    parameters->encryption_mode = check_encryption_mode(encryption_mode);

    if (parameters->encryption_mode == NOT_FOUND)
    {
        fprintf(stderr, "Invalid encryption mode.\n");
        free(parameters);
        exit(1);
    }

    if (parameters->password == NULL)
    {
        parameters->encryption_algorithm = ECHO;
        parameters->encryption_function = ECHO_FUNCTION;
    }
    else
    {
        parameters->encryption_function = parameters->encryption_algorithm*4 + parameters->encryption_mode;
    }

    return parameters;
}