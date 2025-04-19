/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_path.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:39 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:04:37 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include <stdlib.h>

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
