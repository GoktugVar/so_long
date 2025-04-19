/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_mlx.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:55 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:07:16 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include "mlx.h"
#include <X11/X.h>
#include <stdlib.h>

void	set_events(t_game *game)
{
	mlx_hook(game->mlx.win_ptr, KeyPress, KeyPressMask, key_hook, game);
	mlx_hook(game->mlx.win_ptr, DestroyNotify, StructureNotifyMask,
		mlx_loop_end, game->mlx.mlx_ptr);
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

int	start_mlx(t_game *game)
{
	game->mlx.mlx_ptr = mlx_init();
	if (!game->mlx.mlx_ptr)
		return (error_handle("start_mlx", "Failed to initialize MLX", 0).i);
	if (!set_images(game))
	{
		mlx_free(game);
		return (error_handle("start_mlx", "Failed to set images", 0).i);
	}
	game->mlx.win_ptr = mlx_new_window(game->mlx.mlx_ptr, game->map.width
			* PIXEL_SIZE, game->map.height * PIXEL_SIZE, "So Long");
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
