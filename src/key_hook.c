/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_hook.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:44 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:05:16 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include "mlx.h"
#include <X11/keysym.h>
#include <unistd.h>

void	put_size_t(size_t n)
{
	char	c;

	if (n >= 10)
		put_size_t(n / 10);
	c = (n % 10) + '0';
	write(1, &c, 1);
}

void	check_move(t_game *game, int dst)
{
	if (game->map.data[dst] != '1')
	{
		game->player.move_count++;
		write(1, "Move Count: ", ft_strlen("Move Count: "));
		put_size_t(game->player.move_count);
		write(1, "\n", 1);
		if (game->map.data[dst] == 'C')
		{
			game->map.data[dst] = '0';
			game->collectible.count--;
			if (game->collectible.count == 0)
				print_image(game, game->exit.img_o, game->exit.pos);
		}
		if (game->map.data[dst] == 'E' && game->collectible.count == 0)
		{
			write(1, "You win!\n", ft_strlen("You win!\n"));
			mlx_loop_end(game->mlx.mlx_ptr);
			return ;
		}
		print_image(game, get_cell(game, game->player.pos), game->player.pos);
		game->player.pos = dst;
		print_image(game, game->player.img, game->player.pos);
	}
}

int	key_hook(int keycode, t_game *game)
{
	if (keycode == XK_Escape)
		mlx_loop_end(game->mlx.mlx_ptr);
	else if (keycode == 119)
		check_move(game, game->player.pos - game->map.width - 1);
	else if (keycode == 115)
		check_move(game, game->player.pos + game->map.width + 1);
	else if (keycode == 97)
		check_move(game, game->player.pos - 1);
	else if (keycode == 100)
		check_move(game, game->player.pos + 1);
	return (0);
}
