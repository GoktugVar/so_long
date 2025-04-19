/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_images.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:50 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:13:03 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include "mlx.h"
#include <stdlib.h>

int	init_image(void **img_ptr, char *path, t_game *game)
{
	int		width;
	int		height;
	void	*img;
	t_img	*node;

	img = mlx_xpm_file_to_image(game->mlx.mlx_ptr, path, &width, &height);
	if (img == NULL)
		return (error_handle("init_image", "Failed to load image", 0).i);
	node = create_node(img);
	if (node == NULL)
	{
		mlx_destroy_image(game->mlx.mlx_ptr, img);
		return (error_handle("init_image", "Failed to create image node", 0).i);
	}
	add_node(&game->img_list, node);
	*img_ptr = img;
	return (1);
}

int	init_walls(char *path, t_game *game, int phase)
{
	if (phase == 9)
		return (init_image(&game->map.img_w[path[15] - '0'][path[16] - '0']
			[path[17] - '0'][path[18] - '0'][path[20] - '0']
			[path[21] - '0'][path[22] - '0'][path[23] - '0'], path, game));
	path[15 + phase] = '0';
	if (phase != 4 && init_walls(path, game, phase + 1) == 0)
	{
		if (phase == 0)
			return (error_handle("init_walls", "Failed to load walls", 0).i);
		return (0);
	}
	path[15 + phase] = '1';
	if (init_walls(path, game, phase + 1) == 0)
	{
		if (phase == 0)
			return (error_handle("init_walls", "Failed to load walls", 0).i);
		return (0);
	}
	return (1);
}

int	set_images(t_game *game)
{
	char	*path;

	path = ft_strdup("textures/walls/000010000.xpm");
	if (path == NULL)
		return (error_handle("set_images", "ft_strdup returned NULL", 0).i);
	if (init_image(&game->map.img_f, "textures/f.xpm", game) == 0
		|| init_image(&game->player.img, "textures/p.xpm", game) == 0
		|| init_image(&game->collectible.img, "textures/c.xpm", game) == 0
		|| init_image(&game->exit.img_c, "textures/ec.xpm", game) == 0
		|| init_image(&game->exit.img_o, "textures/eo.xpm", game) == 0
		|| init_walls(path, game, 0) == 0)
	{
		free(path);
		return (error_handle("set_images", "Failed to set images", 0).i);
	}
	free(path);
	return (1);
}
