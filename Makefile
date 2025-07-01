# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g
NAME = woody_woodpacker
SRC = woody_woodpacker.c \
		srcs/huffman.c \
		srcs/tea.c
SRC2 = woody_unpacker.c \
		srcs/huffman.c \
		srcs/tea.c
SRCM = srcs/memory_huffman.c
OBJ = $(SRC:.c=.o)
OBJ2 = $(SRC2:.c=.o)
OBJM = $(SRCM:.c=.o)

CLR_RMV		:= \033[0m
RED		    := \033[1;31m
GREEN		:= \033[1;32m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
CYAN 		:= \033[1;36m
RM		    := rm -f

all: $(NAME) unpacker memory

$(NAME): $(OBJ)
	@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}$(NAME) ${CLR_RMV}..."
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lssl -lcrypto -lz
	@echo "$(GREEN)$(NAME) created[0m ✔️"

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@


unpacker: $(OBJ2)
	@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}woody_unpacker ${CLR_RMV}..."
	@$(CC) $(CFLAGS) -o woody_unpacker $(OBJ2) -lssl -lcrypto -lz
	@echo "$(GREEN)woody_unpacker created[0m ✔️"

memory: $(OBJM)
	@echo "$(GREEN)Compilation ${CLR_RMV}of ${YELLOW}memory ${CLR_RMV}..."
	@$(CC) $(CFLAGS) -o memory $(OBJM) -lssl -lcrypto -lz
	@echo "$(GREEN)memory created[0m ✔️"

clean:
	@rm -f $(OBJ)
	@rm -f $(OBJ2)
	@rm -f $(OBJM)
	@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs ✔️"

fclean: clean
	@rm -f $(NAME)
	@rm -f woody_unpacker
	@rm -f compressed
	@rm -f encrypted
	@rm -f decrypted
	@rm -f decompressed
	@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)binary ✔️"

re: fclean all

.PHONY: all clean fclean re