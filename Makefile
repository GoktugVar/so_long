NAME		= so_long

CC			= cc

INC_DIR		= inc
SRC_DIR		= src
BUILD_DIR	= build

MLX_DIR		= minilibx
MLX			= $(MLX_DIR)/libmlx.a

SRCS		= check_map.c check_path.c image_nodes.c key_hook.c \
				print_map.c read_file.c set_images.c so_long.c \
				start_mlx.c utils.c 


VPATH		= src

OBJS		= $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))

CFLAGS		= -Wall -Wextra -Werror -I$(INC_DIR) -I$(MLX_DIR)
LDFLAGS		= -L$(MLX_DIR)
LDLIBS		= -lmlx -lX11 -lXext
RM			= rm -rf

all: $(NAME)

$(MLX):
	make -C $(MLX_DIR)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(NAME): $(OBJS) $(MLX)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(NAME)

clean:
	$(RM) $(BUILD_DIR)
	make -C $(MLX_DIR) clean

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)

re: fclean all

.PHONY: all bonus clean fclean re