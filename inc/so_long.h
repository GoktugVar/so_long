
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
	void	*img_w[2][2][2][2][2][2][2][2][2];
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

#endif