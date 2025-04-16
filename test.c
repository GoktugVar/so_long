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
	int		founded;
}	t_player;

typedef struct s_exit
{
	void	*img_o;
	void	*img_c;
	int		founded;
}	t_exit;

typedef struct s_map
{
	char	*data;
	size_t	height;
	size_t	width;
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

#pragma region[Check Map]

int	check_char(char c, t_game *game)
{
	if (c == 'C')
		game->collectible.count++;
	else if (c == 'P')
	{
		if (game->player.founded != 0)
			return(error_handle("check_char", "Multiple players", 0).i);
		game->player.founded = 1;
	}
	else if (c == 'E')
	{
		if (game->exit.founded != 0)
			return(error_handle("check_char", "Multiple exits", 0).i);
		game->exit.founded = 1;
	}
	else if (c != '1' && c != '0')
		return(error_handle("check_char", "Invalid char", 0).i);
	return (1);
}

size_t check_line(char *map, int flag, t_game *game)
{
	static size_t   i = 0;

	while (map[i] != '-' && map[i] != '\0')
	{
		if (flag == 0 && map[i] != '1')
			return(error_handle("check_line", "Border must be '1'", 0).i);
		if (flag == 1 && check_char(map[i], game) == 0)
			return(error_handle("check_line", "Invalid char in line", 0).i);
		i++;
	}
	if (map[0] != '1' || map[i - 1] != '1')
		return (error_handle("check_line",
			"Line must start and end with '1'", 0).i);
	return (i);
}

int check_map(t_game *game)
{
    char *map = game->map.data;

	game->collectible.count = 0;
    game->map.width = check_line(map, 0, game);
    if (game->map.width == 0)
        return (error_handle("check_map", "Map border missing", 0).i);
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
	if (game->player.founded == 0)
		return (error_handle("check_map", "Player not found", 0).i);
	if (game->exit.founded == 0)
		return (error_handle("check_map", "Exit not found", 0).i);
	if (game->collectible.count == 0)
		return (error_handle("check_map", "No Collectible", 0).i);
	if (check_path(game) == 0)
		return (error_handle("check_map", "No Path", 0).i);
    return (1);
}

#pragma endregion
