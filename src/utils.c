/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:57 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:08:01 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include <unistd.h>
#include <stdlib.h>

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
