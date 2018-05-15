enum mode
{
    embed,
    extract
};

struct options
{
    enum mode mode;
    char *input_file;
    char *carrier_file;
    char *output_file;
};

typedef struct options *options;

void parse_options(int argc, char *argv[]);

extern options parameters;