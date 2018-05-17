/** Possible modes of execution */
typedef enum mode_t { EMBED, EXTRACT } Mode;

/** Stores all the parameters of the current execution */
typedef struct options_t
{
    Mode mode;
    char *input_file_name;
    char *carrier_file_name;
    char *output_file_name;
} Options;

/** Command line arguments parser */
Options * parse_options(int argc, char *argv[]);