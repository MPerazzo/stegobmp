#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "parameters.h"

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
}

void parse_options(int argc, char *argv[])
{
    options parameters = malloc(sizeof(*parameters));

    int c;

    static int mode_flag;

    while (1)
    {

        static struct option long_options[] =
            {
                {"extract", no_argument, &mode_flag, 1},
                {"embed", no_argument, &mode_flag, 0},
                {"in", required_argument, 0, 'i'},
                {"port", required_argument, 0, 'p'},
                {"out", required_argument, 0, 'o'},
                {"help", no_argument, 0, 'h'},
                {0, 0, 0, 0}};

        int option_index = 0;

        c = getopt_long_only(argc, argv, "hi:o:p:", long_options, &option_index);

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
            parameters->input_file = optarg;
            break;
        case 'o':
            parameters->output_file = optarg;
            break;
        case 'p':
            parameters->carrier_file = optarg;
            break;
        case '?':
            break;
        default:
            abort();
        }
    }

    if (parameters->input_file == NULL)
    {
        fprintf(stderr, "You must specify an input file.\n");
        exit(1);
    }

    if (parameters->carrier_file == NULL)
    {
        fprintf(stderr, "You must specify a carrier file.\n");
        exit(1);
    }

    if (mode_flag)
    {
        parameters->mode = extract;
    }
    else
    {
        parameters->mode = embed;
    }
}