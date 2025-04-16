#pragma region[Header]

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 64
#endif

typedef union u_return
{
	int		i;
	char	*p;
}	t_return;

typedef struct s_collectible
{
	size_t	count;
	void	*img;
}	t_collectible;

typedef struct s_player
{
	size_t	pos;
	size_t	move_count;
	void	*img;
}	t_player;

typedef struct s_exit
{
	void	*img_o;
	void	*img_c;
}	t_exit;

typedef struct s_map
{
	char	*data;
	size_t	len;
	size_t	width;
	size_t	height;
	void	*img_w;
	void	*img_f;
}	t_map;

typedef struct s_game
{
	t_collectible	collectible;
	t_player		player;
	t_exit			exit;
	t_map			map;
}	t_game;

#pragma endregion

#pragma region[Utils]

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

char	*read_file(const char *path)
{
	int		fd;
	char	*result;

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
	size_t	dst;
	
	map[pos] = '1';
	dst = pos - width - 1;
	if (map[dst] != '1')
		look_for_path(map, dst, width);
	dst = pos - 1;
	if (map[dst] != '1')
		look_for_path(map, dst, width);
	dst = pos + width + 1;
	if (map[dst] != '1')
		look_for_path(map, dst, width);
	dst = pos + 1;
	if (map[dst] != '1')
		look_for_path(map, dst, width);
}

int	look_map(char *map)
{
	size_t	i;

	i = 0;
	while (map[i])
	{
		if (map[i] != '1' && map[i] == '0' && map[i] != '-') // TODO fix - to new line
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
	while (line[i] != '-' && line[i] != '\0') // TODO fix - to new line
	{
		if (flag == 0 && line[i] != '1')
			return(error_handle("check_line", "Border must be '1'", 0).i);
		if (flag == 1 && check_char(line[i], game) == 0)
			return(error_handle("check_line", "Invalid char in map", 0).i);
		i++;
	}
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
		if (*map == '-') // TODO fix - to new line
			map++;
		game->map.height++;
	}
	if (check_char('\0', game) == 0)
		return (error_handle("check_map", "Map Error", 0).i);
	game->map.len = ft_strlen(game->map.data);
	set_player_pos(game);
	if (check_path(game) == 0)
		return (error_handle("check_map", "No Path", 0).i);
	return (1);
}

#pragma endregion

#pragma region[Main]

int	main(int ac, char **av)
{
	t_game	game;

	if (ac != 2)
		error_handle("main", "Invalid number of arguments", 1);
	game.map.data = read_file(av[1]);
	if (game.map.data == NULL)
		error_handle("main", "read_file returned NULL", 1);
	if (check_map(&game) == 0)
	{
		safe_free((void **)&game.map.data);
		error_handle("main", "Invalid map", 1);
	}
	// Start Game
	safe_free((void **)&game.map.data);
	return (0);
}

#pragma endregion