#pragma region [Header]

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 64
# endif

typedef struct s_exit
{
	size_t	exit_x;
	size_t	exit_y;
	int		exit_status;
} t_exit;


typedef struct s_player
{
	size_t	move_count;
	size_t	player_x;
	size_t	player_y;
	int		direction;
} t_player;

typedef struct s_so_long
{
	char		**map;
	size_t		map_x;
	size_t		map_y;
	t_player	player;
	t_exit		exit;
} t_so_long;

size_t	ft_strlen(const char *s);

#pragma endregion

#pragma region [Exit]

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
	if(buf && *buf)
	{
		while (*buf && **buf)
		{
			safe_free(**buf);
			(**buf)++;
		}
		safe_free(*buf);
	}
}

void	error_exit(char *str)
{
	write(2, "Error\n", ft_strlen("Error\n"));
	write(2, str, ft_strlen(str));
	exit(1);
}

void	free_exit(char *str, void *buf)
{
	safe_free(&buf);
	error_exit(str);
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
	size_t	count;

	count = 0;
	while (str && *str)
		if (*(str++) != sep && (*(str + 1) == sep || !*(str + 1)))
			count++;
    return (count);
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

char	**ft_split_free(char *s, char c)
{
	char	**split;
	char	**start;

	if (!s && !*s)
		return (NULL);
	split = (char **)malloc(sizeof(char *) * (str_count(s, c) + 1));
	if (!split)
		free_exit("malloc_error on ft_split_free()", s);
	start = split;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s)
		{
			*split = str_parse(s, c);
			if (!*split)
				return (free_split(start), NULL); // TODO fix that norm and free
			split++;
		}
		while (*s && *s != c)
			s++;
	}
	*split = NULL;
	safe_free((void **)&s);
	return (start);
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

char	*ft_strjoin_free(char *s1, char *s2)
{
	char	*str;
	size_t	len;
	size_t	i;

	if (!s1 && !s2)
		return (NULL);
	len = ft_strlen(s1) + ft_strlen(s2);
	str = (char *)malloc(len + 1);
	if (!str)
	{
		safe_free((void **)&s1);
		return (NULL);
	}
	i = 0;
	if (s1)
		while (s1[i])
			*(str++) = (s1[i++]);
	i = 0;
	if (s2)
		while (s2[i])
			*(str++) = (s2[i++]);
	*str = '\0';
	safe_free((void **)&s1);
	return (str - len);
}

char	**read_map(char *path)
{
	int		fd;
	char	*map;
	int		byte_read;
	char	buffer[BUFFER_SIZE + 1];

	fd = open(path, O_RDONLY);
	if (fd == -1)
		error_exit("open_error on read_map()");
	map = NULL;
	while (map == NULL || byte_read > 0)
	{
		byte_read = read(fd, buffer, BUFFER_SIZE);
		if (byte_read == -1 || byte_read == 0)
			break;
		buffer[byte_read] = 0;
		map = ft_strjoin_free(map, buffer);
	}
	close(fd);
	if (byte_read == -1)
		free_exit("read_error on read_map()", map);
	if (!map && byte_read != 0)
		error_exit("malloc_error on read_map()");
	return (ft_split_free(map, '\n'));
}

#pragma endregion

#pragma region [Main]

int	main(int ac, char **av)
{
	t_so_long	so_long;

	if (ac != 2)
		error_exit("Invalid Input Count");
	so_long.map = read_map(av[1]);
	if (!so_long.map)
		error_exit("malloc_error on ft_split_free()");
	for (int i = 0; so_long.map[i]; i++)
	{
		printf("%s\n", so_long.map[i]);
		free(so_long.map[i]);
	}
	free(so_long.map);
}

#pragma endregion