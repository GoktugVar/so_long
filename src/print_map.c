/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:46 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:17:22 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include "mlx.h"

void	print_image(t_game *game, void *img, size_t pos)
{
	mlx_put_image_to_window(game->mlx.mlx_ptr, game->mlx.win_ptr, img,
		(pos % (game->map.width + 1)) * PIXEL_SIZE,
		(pos / (game->map.width + 1)) * PIXEL_SIZE);
}

int	is_wall(t_game *game, size_t i)
{
	if (i > game->map.len)
		return (1);
	if (game->map.data[i] == '1' || game->map.data[i] == '\n')
		return (1);
	return (0);
}

void	*find_tile(t_game *game, size_t i)
{
	return (game->map.img_w
		[is_wall(game, i - game->map.width - 2)]
		[is_wall(game, i - game->map.width - 1)]
		[is_wall(game, i - game->map.width)]
		[is_wall(game, i - 1)]
		[is_wall(game, i + 1)]
		[is_wall(game, i + game->map.width)]
		[is_wall(game, i + game->map.width + 1)]
		[is_wall(game, i + game->map.width + 2)]);
}

void	*get_cell(t_game *game, size_t i)
{
	if (game->map.data[i] == '1')
		return (find_tile(game, i));
	if (game->map.data[i] == 'P')
		return (game->player.img);
	if (game->map.data[i] == 'C')
		return (game->collectible.img);
	if (game->map.data[i] == 'E')
	{
		if (game->collectible.count == 0)
			return (game->exit.img_o);
		else
			return (game->exit.img_c);
	}
	else
		return (game->map.img_f);
}

void	print_map(t_game *game)
{
	int		i;
	int		pos_x;
	int		pos_y;

	i = -1;
	pos_x = 0;
	pos_y = 0;
	while (game->map.data[++i])
	{
		if (game->map.data[i] == '\n')
		{
			pos_x = 0;
			pos_y += PIXEL_SIZE;
		}
		else
		{
			mlx_put_image_to_window(game->mlx.mlx_ptr, game->mlx.win_ptr,
				get_cell(game, i), pos_x, pos_y);
			pos_x += PIXEL_SIZE;
			if (game->map.data[i] == 'P')
				game->map.data[i] = '0';
		}
	}
}
