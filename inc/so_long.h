/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ivar <ivar@student.42kocaeli.com.tr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 18:03:22 by ivar              #+#    #+#             */
/*   Updated: 2025/04/19 18:10:37 by ivar             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_H
# define SO_LONG_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 64
# endif

# ifndef PIXEL_SIZE
#  define PIXEL_SIZE 128
# endif

# include <stddef.h>

typedef union u_return
{
	int		i;
	char	*p;
}	t_return;

typedef struct s_img
{
	void			*img_ptr;
	struct s_img	*prev;
	struct s_img	*next;
}	t_img;

typedef struct s_collectible
{
	size_t	count;
	void	*img;
}	t_collectible;

typedef struct s_player
{
	size_t	pos;
	size_t	move_count;
	void	*img;
}	t_player;

typedef struct s_exit
{
	size_t	pos;
	void	*img_o;
	void	*img_c;
}	t_exit;

typedef struct s_map
{
	char	*data;
	size_t	len;
	size_t	width;
	size_t	height;
	void	*img_f;
	void	*img_w[2][2][2][2][2][2][2][2];
}	t_map;

typedef struct s_mlx
{
	void	*mlx_ptr;
	void	*win_ptr;
}	t_mlx;
typedef struct s_game
{
	t_mlx			mlx;
	t_map			map;
	t_exit			exit;
	t_player		player;
	t_img			*img_list;
	t_collectible	collectible;
}	t_game;

void		safe_free(void **buf);
void		print_map(t_game *game);
void		destroy_images(t_game *game);
void		*get_cell(t_game *game, size_t i);
void		*ft_memset(void *b, int c, size_t len);
void		print_image(t_game *game, void *img, size_t pos);
void		error_print(const char *func, const char *message);

char		*ft_strdup(char *s1);
char		*read_file(const char *path);

int			check_map(t_game *game);
int			start_mlx(t_game *game);
int			set_images(t_game *game);
int			check_path(t_game *game);
int			key_hook(int keycode, t_game *game);

size_t		ft_strlen(const char *s);

t_img		*create_node(void *img_ptr);
t_img		*add_node(t_img **head, t_img *new_node);

t_return	error_handle(const char *func, const char *message, int flag);

#endif