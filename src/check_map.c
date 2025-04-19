/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:37 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:04:24 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	set_unit_pos(t_game *game)
{
	char	*map;
	int		i;

	map = game->map.data;
	i = 0;
	while (map[i])
	{
		if (map[i] == 'P')
			game->player.pos = i;
		else if (map[i] == 'E')
			game->exit.pos = i;
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

size_t	check_line(char *line, int flag, t_game *game)
{
	size_t	i;

	i = 0;
	while (line[i] != '\n' && line[i] != '\0')
	{
		if (flag == 0 && line[i] != '1')
			return (error_handle("check_line", "Border must be '1'", 0).i);
		if (flag == 1 && check_char(line[i], game) == 0)
			return (error_handle("check_line", "Invalid char in map", 0).i);
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
	set_unit_pos(game);
	if (check_path(game) == 0)
		return (error_handle("check_map", "No Path", 0).i);
	return (1);
}
