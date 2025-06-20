# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror
NAME = woody_woodpacker
SRC = woody_woodpacker.c \
		huffman.c \
		tea.c
SRC2 = woody_unpacker.c \
		huffman.c \
		tea.c
STUBSRC = stub.c \
		huffman.c \
		tea.c
OBJ = $(SRC:.c=.o)
OBJ2 = $(SRC2:.c=.o)
STUBOBJ = $(STUBSRC:.c=.o)

CLR_RMV		:= \033[0m
RED		    := \033[1;31m
GREEN		:= \033[1;32m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
CYAN 		:= \033[1;36m
RM		    := rm -f

all: $(NAME) unpacker stub

$(NAME): $(OBJ)
	@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}$(NAME) ${CLR_RMV}..."
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lssl -lcrypto -lz
	@echo "$(GREEN)$(NAME) created[0m ✔️"

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

stub: $(STUBOBJ)
	@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}stub ${CLR_RMV}..."
	@$(CC) $(CFLAGS) -o stub $(STUBOBJ) -static
	@echo "$(GREEN)stub created[0m ✔️"

unpacker: $(OBJ2)
	@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}woody_unpacker ${CLR_RMV}..."
	@$(CC) $(CFLAGS) -o woody_unpacker $(OBJ2) -lssl -lcrypto -lz
	@echo "$(GREEN)woody_unpacker created[0m ✔️"

clean:
	@rm -f $(OBJ)
	@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs ✔️"

fclean: clean
	@rm -f $(NAME)
	@rm -f woody_unpacker woody_unpacker.o
	@rm -f compressed
	@rm -f encrypted
	@rm -f decrypted
	@rm -f decompressed
	@rm -f stub stub.o
	@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)binary ✔️"

re: fclean all

.PHONY: all clean fclean re