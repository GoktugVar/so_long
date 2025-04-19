/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:52 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:03:53 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

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
