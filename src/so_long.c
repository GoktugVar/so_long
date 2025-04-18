#include "../inc/so_long.h"

# include "../minilibx/mlx.h"
# include <X11/keysym.h>
# include <X11/X.h>
# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <stdio.h>

#pragma region[Utils]

void	*ft_memset(void *b, int c, size_t len)
{
	char	*s;

	if (!b)
		return (NULL);
	s = (char *)b;
	while (0 < len--)
		s[len] = (char)c;
	return (s);
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	safe_free(void **buf)
{
	if (buf && *buf)
	{
		free(*buf);
		*buf = NULL;
	}
}

void	error_print(const char *func, const char *message)
{
	static int	first_error = -1;
	int			i;

	first_error++;
	if (first_error == 0)
	{
		write(2, "Error\n", 6);
		write(2, message, ft_strlen(message));
		write(2, "\n→ at [", 10);
	}
	else
	{
		i = first_error;
		while (i-- > 0)
			write(2, " ", 1);
		write(2, "→ by [", 9);
	}
	write(2, func, ft_strlen(func));
	write(2, "]\n", 2);
}

t_return	error_handle(const char *func, const char *message, int flag)
{
	static t_return	ret = {0};

	error_print(func, message);
	if (flag & 1)
		exit(1);
	return (ret);
}

#pragma endregion

#pragma region[Read File]

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*new_str;
	size_t	i;
	size_t	j;

	if (!s1 && !s2)
		return (NULL);
	new_str = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!new_str)
		return (error_handle("ft_strjoin",
				"Memory allocation failed", 0).p);
	i = 0;
	while (s1 && s1[i])
	{
		new_str[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2 && s2[j])
	{
		new_str[i + j] = s2[j];
		j++;
	}
	new_str[i + j] = '\0';
	return (new_str);
}

char	*ft_join_and_free(char *str1, char *str2, int flag)
{
	char	*new_str;

	new_str = ft_strjoin(str1, str2);
	if (flag & 1)
		safe_free((void **)&str1);
	if ((flag >> 1) & 1)
		safe_free((void **)&str2);
	if (!new_str)
		error_print("ft_join_and_free", "ft_strjoin returned NULL");
	return (new_str);
}

char	*read_content(int fd)
{
	ssize_t rlen;
	char	*result;
	char	buffer[BUFFER_SIZE + 1];

	result = NULL;
	while (1)
	{
		rlen = read(fd, buffer, BUFFER_SIZE);
		if (rlen == -1)
		{
			safe_free((void **)&result);
			return (error_handle("read_content",
					"Failed to read the file", 0).p);
		}
		if (rlen == 0)
			break;
		buffer[rlen] = '\0';
		result = ft_join_and_free(result, buffer, 1);
		if (!result)
			return(error_handle("read_content",
					"ft_join_and_free returned NULL", 0).p);
	}
	return (result);
}

int	check_extension(const char *path)
{
	size_t	len;
	const char	*file;

	if (!path)
		return (error_handle("check_extension", "Path is NULL", 0).i);
	len = ft_strlen(path);
	while (len--)
		if (path[len] == '/')
			break;
	file = &path[len + 1];
	len = ft_strlen(file);
	if (len <= 4)
		return (error_handle("check_extension", "Invalid file extension", 0).i);
	if (file[len - 4] == '.' &&
		file[len - 3] == 'b' &&
		file[len - 2] == 'e' &&
		file[len - 1] == 'r')
		return (1);
	return (error_handle("check_extension", "Invalid file extension", 0).i);
}

char	*read_file(const char *path)
{
	int		fd;
	char	*result;

	if (!check_extension(path))
		return (error_handle("read_file", "Invalid file extension", 0).p);
	fd = open(path, O_RDONLY);
	if (fd == -1)
		return(error_handle("read_file", "Failed to open the file", 0).p);
	result = read_content(fd);
	if (!result)
		error_print("read_file", "read_content returned NULL.");
	close(fd);
	return (result);
}

#pragma endregion

#pragma region[Check Path]

char	*ft_strdup(char *s1)
{
	char	*dst;
	int		i;

	if (!s1)
		return (NULL);
	dst = (char *)malloc(ft_strlen(s1) + 1);
	if (dst == NULL)
		return (error_handle("ft_strdup",
				"Memory allocation failed", 0).p);
	i = 0;
	while (s1[i])
	{
		dst[i] = s1[i];
		i++;
	}
	dst[i] = '\0';
	return (dst);
}

void	look_for_path(char *map, size_t pos, size_t width)
{
	if (map[pos] == '1' || map[pos] == '\n')
		return ;
	map[pos] = '1';
	look_for_path(map, pos - width - 1, width);
	look_for_path(map, pos - 1, width);
	look_for_path(map, pos + width + 1, width);
	look_for_path(map, pos + 1, width);
}

int	look_map(char *map)
{
	size_t	i;

	i = 0;
	while (map[i])
	{
		if (map[i] != '1' && map[i] != '0' && map[i] != '\n')
			return (error_handle("look_map", "Path not found", 0).i);
		i++;
	}
	return (1);
}

int	check_path(t_game *game)
{
	char	*map;

	map = ft_strdup(game->map.data);
	if (map == NULL)
		return (error_handle("check_path", "ft_strdup returned NULL", 0).i);
	look_for_path(map, game->player.pos, game->map.width);
	if (look_map(map) == 0)
	{
		free(map);
		return (error_handle("check_path", "No Path", 0).i);
	}
	free(map);
	return (1);
}

#pragma endregion

#pragma region[Check Map]

void	set_player_pos(t_game *game)
{
	char	*map;
	int		i;

	map = game->map.data;
	i = 0;
	while (map[i])
	{
		if (map[i] == 'P')
		{
			game->player.pos = i;
			return ;
		}
		i++;
	}
}

int	check_char(char c, t_game *game)
{
    static int	exit_count = 0;
    static int	player_count = 0;

	if (c == 'C')
		game->collectible.count++;
	else if (c == 'E' && ++exit_count > 1)
		return (error_handle("check_char", "Multiple exits detected", 0).i);
	else if (c == 'P' && ++player_count > 1)
		return (error_handle("check_char", "Multiple players detected", 0).i);
	else if (c == '\0')
	{
		if (exit_count == 0)
			return (error_handle("check_char", "No exit found", 0).i);
		if (player_count == 0)
			return (error_handle("check_char", "No player found", 0).i);
		if (game->collectible.count == 0)
			return (error_handle("check_char", "No collectible found", 0).i);
	}
	else if (c != '1' && c != '0' && c != 'E' && c != 'P')
		return (error_handle("check_char", "Invalid character in map", 0).i);
	return (1);
}

size_t check_line(char *line, int flag, t_game *game)
{
	size_t	i;

	i = 0;
	while (line[i] != '\n' && line[i] != '\0')
	{
		if (flag == 0 && line[i] != '1')
			return(error_handle("check_line", "Border must be '1'", 0).i);
		if (flag == 1 && check_char(line[i], game) == 0)
			return(error_handle("check_line", "Invalid char in map", 0).i);
		i++;
	}
	if (i == 0)
		return (error_handle("check_line", "Empty line", 0).i);
	if (line[0] != '1' || line[i - 1] != '1')
		return (error_handle("check_line", "Border must be '1'", 0).i);
	return (i);
}

int	check_map(t_game *game)
{
	char	*map;

	map = game->map.data;
	game->collectible.count = 0;
	game->map.width = check_line(map, 0, game);
	if (game->map.width == 0)
		return (error_handle("check_map", "Map Error", 0).i);
	game->map.height = 0;
	while (*map)
	{
		if (check_line(map, 1, game) != game->map.width)
			return (error_handle("check_map", "Inconsistent line length", 0).i);
		map += game->map.width;
		if (*map == '\n')
			map++;
		game->map.height++;
	}
	check_line(map - game->map.width, 0, game);
	if (check_char('\0', game) == 0)
		return (error_handle("check_map", "Map Error", 0).i);
	game->map.len = ft_strlen(game->map.data);
	set_player_pos(game);
	if (check_path(game) == 0)
		return (error_handle("check_map", "No Path", 0).i);
	return (1);
}

#pragma endregion

#pragma region[Image Nodes]

t_img	*create_node(void *img_ptr)
{
	t_img	*new_node;

	new_node = (t_img *)malloc(sizeof(t_img));
	if (!new_node)
		return (NULL);
	new_node->img_ptr = img_ptr;
	new_node->next = NULL;
	new_node->prev = NULL;
	return (new_node);
}

t_img	*add_node(t_img **head, t_img *new_node)
{
	t_img	*last;

	if (new_node == NULL)
		return (*head);
	if (*head == NULL)
	{
		*head = new_node;
		new_node->next = new_node;
		new_node->prev = new_node;
		return (new_node);
	}
	last = (*head)->prev;
	last->next = new_node;
	new_node->prev = last;
	new_node->next = *head;
	(*head)->prev = new_node;
	return (new_node);
}

void	destroy_images(t_game *game)
{
	t_img	*current;
	t_img	*to_free;

	if (game->img_list == NULL)
		return ;
	current = game->img_list->next;
	while (current != game->img_list)
	{
		to_free = current;
		current = current->next;
		mlx_destroy_image(game->mlx.mlx_ptr, to_free->img_ptr);
		free(to_free);
	}
	mlx_destroy_image(game->mlx.mlx_ptr, game->img_list->img_ptr);
	free(game->img_list);
	game->img_list = NULL;
}

#pragma endregion

#pragma region[Set Images]

int	init_image(void **img_ptr, char *path, t_game *game)
{
	int		width;
	int		height;
	void	*img;
	t_img	*node;

	img = mlx_xpm_file_to_image(game->mlx.mlx_ptr, path, &width, &height);
	if (img == NULL)
		return (error_handle("init_image", "Failed to load image", 0).i);
	node = create_node(img);
	if (node == NULL)
	{
		mlx_destroy_image(game->mlx.mlx_ptr, img);
		return (error_handle("init_image", "Failed to create image node", 0).i);
	}
	add_node(&game->img_list, node);
	*img_ptr = img;
	return (1);
}

int	init_walls(char *path, t_game *game, int phase)
{
	if (phase == 9)
	{
    	return init_image(
        	&game->map.img_w[path[9] - '0'][path[10] - '0'][path[11] - '0']
            	[path[12] - '0'][path[13] - '0'][path[14] - '0'][path[15] - '0']
				[path[16] - '0'][path[17] - '0'], path, game);
	}
	path[9 + phase] = '0';
	if (init_walls(path, game, phase + 1) == 0)
	{
		if (phase == 0)
			return (error_handle("init_walls", "Failed to load walls", 0).i);
		return (0);
	}
	path[9 + phase] = '1';
	if (init_walls(path, game ,phase + 1) == 0)
	{
		if (phase == 0)
			return (error_handle("init_walls", "Failed to load walls", 0).i);
		return (0);
	}
	return (1);
}

int	set_images(t_game *game)
{
	char *path;

	path = ft_strdup("textures/000000000.xpm");
	path = 
	if (path == NULL)
		return (error_handle("set_images", "ft_strdup returned NULL", 0).i);
	if (init_image(&game->map.img_f, "textures/f.xpm", game) == 0
		|| init_image(&game->player.img, "textures/p.xpm", game) == 0
		|| init_image(&game->collectible.img, "textures/c.xpm", game) == 0
		|| init_image(&game->exit.img_c, "textures/ec.xpm", game) == 0
		|| init_image(&game->exit.img_o, "textures/eo.xpm", game) == 0
		|| init_walls(path, game, 0) == 0)
	{
		free(path);
		return (error_handle("set_images", "Failed to set images", 0).i);
	}
	free(path);
	return (1);
}

#pragma endregion

#pragma region[Print Map]

void	*get_cell(t_game *game, int i)
{
	if (game->map.data[i] == '1')
		return (game->map.img_w[0][0][0][0][0][0][0][0][0]);
	if (game->map.data[i] == 'P')
		return (game->player.img);
	if (game->map.data[i] == 'C')
		return (game->collectible.img);
	if (game->map.data[i] == 'E')
		return (game->exit.img_c);
	else
		return (game->map.img_f);
}

void print_map(t_game *game)
{
	int		i;
	int		pos_x;
	int		pos_y;

	i = 0;
	pos_x = 0;
	pos_y = 0;
	while (game->map.data[i])
	{
		if (game->map.data[i] == '\n')
		{
			pos_x = 0;
			pos_y += PIXEL_SIZE;
			i++;
			continue ;
		}
		else
		{
			mlx_put_image_to_window(game->mlx.mlx_ptr,
				game->mlx.win_ptr, get_cell(game, i), pos_x, pos_y);
			pos_x += PIXEL_SIZE;
			if (game->map.data[i] == 'P')
				game->map.data[i] = '0';
		}
		i++;
	}
}

#pragma endregion

#pragma region[Key Hook]

void check_move(t_game *game, int dst)
{
	if (game->map.data[dst] != '1')
	{
		game->player.move_count++;
		printf("Move count: %zu\n", game->player.move_count);
		if (game->map.data[dst] == 'C')
		{
			game->map.data[dst] = '0';
			game->collectible.count--;
		}
		if (game->map.data[dst] == 'E' && game->collectible.count == 0)
		{
			printf("You win!\n");
			mlx_loop_end(game->mlx.mlx_ptr);
			return ;
		}
		mlx_put_image_to_window(game->mlx.mlx_ptr,
			game->mlx.win_ptr, get_cell(game, game->player.pos), 
			(game->player.pos % (game->map.width + 1)) * PIXEL_SIZE,
			(game->player.pos / (game->map.width + 1)) * PIXEL_SIZE);
		game->player.pos = dst;
		mlx_put_image_to_window(game->mlx.mlx_ptr,
			game->mlx.win_ptr, game->player.img, 
			(game->player.pos % (game->map.width + 1)) * PIXEL_SIZE,
			(game->player.pos / (game->map.width + 1)) * PIXEL_SIZE);
	}
}

int key_hook(int keycode, t_game *game)
{
	if (keycode == XK_Escape)
		mlx_loop_end(game->mlx.mlx_ptr);
	else if (keycode == 119)
		check_move(game, game->player.pos - game->map.width - 1);
	else if (keycode == 115)
		check_move(game, game->player.pos + game->map.width + 1);
	else if (keycode == 97)	
		check_move(game, game->player.pos - 1);
	else if (keycode == 100)
		check_move(game, game->player.pos + 1);
	return (0);
}

#pragma endregion

#pragma region[Start MLX]

void	set_events(t_game *game)
{
	mlx_hook(game->mlx.win_ptr, KeyPress, KeyPressMask, key_hook, game);
	mlx_hook(game->mlx.win_ptr, 17, 1L << 17, mlx_loop_end, game->mlx.mlx_ptr);
}

void	mlx_free(t_game *game)
{
	destroy_images(game);
	if (game->mlx.win_ptr)
		mlx_destroy_window(game->mlx.mlx_ptr, game->mlx.win_ptr);
	if (game->mlx.mlx_ptr)
	{
		mlx_destroy_display(game->mlx.mlx_ptr);
		free(game->mlx.mlx_ptr);
	}
}

int start_mlx(t_game *game)
{
    game->mlx.mlx_ptr = mlx_init();
    if (!game->mlx.mlx_ptr)
        return (error_handle("start_mlx", "Failed to initialize MLX", 0).i);
    if (!set_images(game))
    {
        mlx_free(game);
        return (error_handle("start_mlx", "Failed to set images", 0).i);
    }
	game->mlx.win_ptr = mlx_new_window(game->mlx.mlx_ptr,
		game->map.width * PIXEL_SIZE, game->map.height * PIXEL_SIZE, "So Long");
    if (!game->mlx.win_ptr)
    {
        mlx_free(game);
        return (error_handle("start_mlx", "Failed to create window", 0).i);
    }
    set_events(game);
    print_map(game);
    mlx_loop(game->mlx.mlx_ptr);
    mlx_free(game);
    return (1);
}

#pragma endregion

#pragma region[Main]

int	main(int ac, char **av)
{
	t_game	game;

	if (ac != 2)
		error_handle("main", "Invalid number of arguments", 1);
	ft_memset(&game, 0, sizeof(t_game));
	game.map.data = read_file(av[1]);
	if (game.map.data == NULL)
		error_handle("main", "read_file returned NULL", 1);
	if (check_map(&game) == 0)
	{
		safe_free((void **)&game.map.data);
		error_handle("main", "Invalid map", 1);
	}
	if (start_mlx(&game) == 0)
	{
		safe_free((void **)&game.map.data);
		error_handle("main", "Failed to start game", 1);
	}
	safe_free((void **)&game.map.data);
	return (0);
}

#pragma endregion