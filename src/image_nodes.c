/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_nodes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:42 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:04:56 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include <stdlib.h>
#include <mlx.h>

t_img	*create_node(void *img_ptr)
{
	t_img	*new_node;

	new_node = (t_img *)malloc(sizeof(t_img));
	if (!new_node)
		return (NULL);
	new_node->img_ptr = img_ptr;
	new_node->next = NULL;
	new_node->prev = NULL;
	return (new_node);
}

t_img	*add_node(t_img **head, t_img *new_node)
{
	t_img	*last;

	if (new_node == NULL)
		return (*head);
	if (*head == NULL)
	{
		*head = new_node;
		new_node->next = new_node;
		new_node->prev = new_node;
		return (new_node);
	}
	last = (*head)->prev;
	last->next = new_node;
	new_node->prev = last;
	new_node->next = *head;
	(*head)->prev = new_node;
	return (new_node);
}

void	destroy_images(t_game *game)
{
	t_img	*current;
	t_img	*to_free;

	if (game->img_list == NULL)
		return ;
	current = game->img_list->next;
	while (current != game->img_list)
	{
		to_free = current;
		current = current->next;
		mlx_destroy_image(game->mlx.mlx_ptr, to_free->img_ptr);
		free(to_free);
	}
	mlx_destroy_image(game->mlx.mlx_ptr, game->img_list->img_ptr);
	free(game->img_list);
	game->img_list = NULL;
}
