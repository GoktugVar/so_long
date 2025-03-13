#pragma region [Header]

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 64
#endif

typedef struct s_collectible
{
	size_t	count;
	//void	image;
} t_collectible;

typedef struct s_player
{
	size_t	x;
	size_t	y;
	size_t	move_count;
	//void	image;
} t_player;

typedef struct s_exit
{
	size_t	x;
	size_t	y;
	//void	image_open;
	//void	image_close;
} t_exit;

typedef struct s_map
{
	char	**grid;
	size_t	height;
	size_t	width;
	//void	wall_image;
	//void	floor_image;
} t_map;

typedef struct s_game
{
	t_collectible	collectible;
	t_player		player;
	t_exit			exit;
	t_map			map;
} t_game;

size_t	ft_strlen(const char *s);

#pragma endregion

#pragma region [Safe Free]

void	safe_free(void **buf)
{
	if(buf && *buf)
	{
		free(*buf);
		*buf = NULL;
	}
}

void	safe_free_2d(void ***buf)
{
	int		i;

	i = 0;
	if (buf && *buf)
	{
		while ((*buf)[i])
			safe_free(&(*buf)[i++]);
		free(*buf);
		*buf = NULL;
	}
}

void    safe_close(int *fd)
{
    if (*fd != -1)
        close(*fd);
    *fd = -1;
}

#pragma endregion

#pragma region [Error Exit]

void	error_exit(char *message, char *context)
{
    write(2, "Error\n", 6);
    write(2, context, ft_strlen(context));
    write(2, " - ", 3);
    write(2, message, ft_strlen(message));
    write(2, "\n", 1);
    exit(1);
}

void	close_exit(int fd, char *message, char *context)
{
	safe_close(&fd);
	error_exit(message, context);
}

void	free_exit(void **buf, char *message, char *context)
{
    safe_free(buf);
    error_exit(message, context);
}

void	free_2d_exit(void ***buf, char *message, char *context)
{
    safe_free_2d(buf);
    error_exit(message, context);
}

#pragma endregion

#pragma region [Split]

static void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	while (n--)
		((unsigned char *)dst)[n] = ((unsigned char *)src)[n];
	return (dst);
}

static size_t	str_count(char const *str, char sep)
{
    size_t count;
    
	count = 0;
    while (str && *str)
    {
        if (*str != sep && (*(str + 1) == sep || !*(str + 1)))
            count++;
        str++;
    }
    return count;
}

static char	*str_parse(const char *str, char sep)
{
	int		len;
	char	*s;

	len = 0;
	while (str[len] && str[len] != sep)
		len++;
	s = (char *)malloc(sizeof(char) * (len + 1));
	if (!s)
		return (NULL);
	ft_memcpy(s, str, len);
	s[len] = '\0';
	return (s);
}

static int	fill_split_array(char **split, char *s, char c)
{
    while (*s)
    {
        while (*s && *s == c)
            s++;
        if (*s)
        {
            *split = str_parse(s, c);
            if (!*split)
				return (1);
            split++;
        }
        while (*s && *s != c)
            s++;
    }
    *split = NULL;
	return (0);
}

char	**split_with_free(char *s, char c)
{
    char	**split;

	if (!s)
		return (NULL);
    split = (char **)malloc(sizeof(char *) * (str_count(s, c) + 1));
    if (!split)
        free_exit((void **)&s, "Memory allocation failed", "split_with_free");
    if (fill_split_array(split, s, c) == 1)
	{
		safe_free((void **)&s);
		free_2d_exit((void ***)&split, "Memory allocation failed", "str_parse");
	}
    safe_free((void **)&s);
    return (split);
}

#pragma endregion

#pragma region [Read Map]

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

char	*strjoin_with_free(char *s1, char *s2)
{
    char	*str;
    size_t	len;
    size_t	i;
	size_t	j;

    len = ft_strlen(s1) + ft_strlen(s2);
    str = (char *)malloc(len + 1);
    if (!str)
	{
		safe_free((void **)&s1);
		return (NULL);
	}
    i = 0;
	j = 0;
    if (s1)
        while (s1[i])
            str[j++] = s1[i++];
    i = 0;
    if (s2)
        while (s2[i])
            str[j++] = s2[i++];
    str[j] = '\0';
    safe_free((void **)&s1);
    return (str);
}

void	read_map_file(char *path, t_game *game)
{
    int		fd;
    char	*map;
    int		byte_read;
    char	buffer[BUFFER_SIZE + 1];

    fd = open(path, O_RDONLY);
    if (fd == -1)
        error_exit("Failed to open file", "read_map_file");
    map = NULL;
    while (1)
    {
        byte_read = read(fd, buffer, BUFFER_SIZE);
        if (byte_read == -1 || byte_read == 0)
            break;
        buffer[byte_read] = '\0';
        map = strjoin_with_free(map, buffer);
		if (!map)
			close_exit(fd, "Memory allocation failed", "strjoin_with_free");
    }
    close(fd);
    if (byte_read == -1)
        free_exit((void **)&map, "Failed to read file", "read_map_file");
    game->map.grid = split_with_free(map, '\n');
	if (!game->map.grid)
		error_exit("Empty file", "split_with_free");
}

#pragma endregion

#pragma region [Init Game]

void    check_cell(t_game *game, char cell, size_t x, size_t y)
{
    static int    player_found = 0;
    static int    exit_found = 0;

    if (cell == 'P')
    {
        if (player_found == 1)
            free_2d_exit((void ***)&game->map.grid,
            "Invalid map", "unit_check");
        player_found = 1;
        game->player.x = x;
        game->player.y = y;
    }
    else if (cell == 'E')
    {
        if (exit_found == 1)
            free_2d_exit((void ***)&game->map.grid,
            "Invalid map", "unit_check");
        exit_found = 1;
        game->exit.x = x;
        game->exit.y = y;
    }
    else if (cell == 'C')
        game->collectible.count++;
    else if (cell != '1' && cell != '0')
        free_2d_exit((void ***)&game->map.grid, "Invalid map", "unit_check");
}

void    unit_check(t_game *game)
{
    size_t  x;
    size_t  y;

    y = 0;
    while (y < game->map.height)
    {
        x = 0;
        while (x < game->map.width)
        {
            check_cell(game, game->map.grid[y][x], x, y);
            x++;
        }
        y++;
    }
}

void    wall_check(t_game *game)
{
    size_t  x;
    size_t  y;

    x = 0;
    while (x < game->map.width)
    {
        if (game->map.grid[0][x] != '1'
            || game->map.grid[game->map.height - 1][x] != '1')
            free_2d_exit((void ***)&game->map.grid ,"Invalid map", "wall_check");
        x++;
    }
    y = 0;
    while (y < game->map.height)
    {
        if (game->map.grid[y][0] != '1'
            || game->map.grid[y][game->map.width - 1] != '1')
            free_2d_exit((void ***)&game->map.grid ,"Invalid map", "wall_check");
        y++;
    }
}

void    init_game(t_game *game)
{
    size_t height;

    if (game->map.grid[0] == NULL)
        free_2d_exit((void ***)&game->map.grid ,"Empty map", "check_map");
    height = 0;
    game->map.width = ft_strlen(game->map.grid[0]);
    while (game->map.grid[height])
    {
        if (game->map.width != ft_strlen(game->map.grid[height]))
            free_2d_exit((void ***)&game->map.grid ,"Invalid map", "check_map");
        height++;
    }
    game->map.height = height;
    wall_check(game);
    unit_check(game);
    // TODO map check
}

#pragma endregion

#pragma region [Main]

int	main(int ac, char **av)
{
    t_game	game;

    if (ac != 2)
        error_exit("Invalid number of arguments", "main");
    read_map_file(av[1], &game);
    init_game(&game);

    safe_free_2d((void ***)&(game.map.grid));
    return (0);
}

#pragma endregion