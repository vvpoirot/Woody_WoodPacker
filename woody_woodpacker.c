#include "lib.h"

void ft_error(char* msg)
{
    write(2, "[ERROR] ", strlen("[ERROR] "));
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

void	ft_putnbr(int fd, int nb)
{
	if (nb == -2147483648)
	{
		ft_putnbr(fd, nb / 10);
		write(fd, "8", 1);
	}
	else if (nb < 0)
	{
        write(fd, "-", 1);
		ft_putnbr(fd, -nb);
	}
	else
	{
		if (nb > 9)
			ft_putnbr(fd, nb / 10);
        char c = (48 + nb % 10);
        write(fd, &c, 1);
	}
}

void validArg(char* arg)
{
    int fd = open(arg, O_RDONLY);
    if (fd < 0 || read(fd, 0, 0) == -1)
        ft_error("File can't be open\n");

    unsigned char header[6];
    header[6] = 0;
    if (read(fd, header, 5) == -1)
        ft_error("Can't take content of file\n");
    close(fd);

    if (header[0] != 0x7F || header[1] != 'E' || header[2] != 'L' || header[3] != 'F' || header[4] != 2)
        ft_error("File architecture not suported. x86_64 only.\n");
}

off_t getFileSize(int fd) {
    off_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return (size);
}

int main(int argc, char** argv) {
    if (argc != 2)
        ft_error("Wrong number of arg\n");
    // validArg(argv[1]);

    (void)argv;
    // int inputFD = open(argv[1], O_RDONLY);
    // int outputFD = open("woody", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    // huffman(inputFD, outputFD, 0);
    // printf("File Compressed\n");

    // close(inputFD);
    // close(outputFD);

    int woodyFD = open("res", O_RDONLY);
    int destFD = open("final", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    huffman(woodyFD, destFD, 1);
    printf("File Uncompressed\n");
    close(woodyFD);
    close(destFD);
    return (EXIT_SUCCESS);
}