/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:48 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:05:51 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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
	ssize_t	rlen;
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
			break ;
		buffer[rlen] = '\0';
		result = ft_join_and_free(result, buffer, 1);
		if (!result)
			return (error_handle("read_content",
					"ft_join_and_free returned NULL", 0).p);
	}
	return (result);
}

int	check_extension(const char *path)
{
	size_t		len;
	const char	*file;

	if (!path)
		return (error_handle("check_extension", "Path is NULL", 0).i);
	len = ft_strlen(path);
	while (len--)
		if (path[len] == '/')
			break ;
	file = &path[len + 1];
	len = ft_strlen(file);
	if (len <= 4)
		return (error_handle("check_extension", "Invalid file extension", 0).i);
	if (file[len - 4] == '.' && file[len - 3] == 'b'
		&& file[len - 2] == 'e' && file[len - 1] == 'r')
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
		return (error_handle("read_file", "Failed to open the file", 0).p);
	result = read_content(fd);
	if (!result)
		error_print("read_file", "read_content returned NULL.");
	close(fd);
	return (result);
}
