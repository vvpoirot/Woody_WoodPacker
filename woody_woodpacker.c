#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

void ft_error(char* msg)
{
    write(2, "[ERROR] ", strlen("[ERROR] "));
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

void validArg(char* arg)
{
    FILE *file = fopen(arg, "rb");
    if (!file)
        ft_error("File can't be open\n");

    unsigned char header[5];
    fread(header, 1, 5, file);
    fclose(file);

    if (header[0] != 0x7F || header[1] != 'E' || header[2] != 'L' || header[3] != 'F' || header[4] != 2)
        ft_error("File architecture not suported. x86_64 only.\n");
}

int main(int argc, char** argv) {
    if (argc != 2)
        ft_error("Wrong number of arg\n");
    validArg(argv[1]);

    return (EXIT_SUCCESS);
}