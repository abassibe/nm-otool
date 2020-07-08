# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abassibe <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/11/09 05:21:00 by abassibe          #+#    #+#              #
#    Updated: 2020/07/08 11:04:55 by abassibe         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_nm

SRCS =	ft_nm.c \
		macho.c \
		macho_seg.c \
		macho_sym.c \
		debug.c \
		lst_sort.c \
		print.c \
		archive.c \
		fat.c \
		bit_swap.c \
		utils.c

OBJS = $(SRCS:.c=.o)

SRCPATH = ./srcs/nm/

FLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): libft.a $(OBJS) $(addprefix $(SRCPATH), $(SRCS))
	@gcc $(FLAGS) $(OBJS) -L. -lft -o $(NAME)

libft.a:
	@make -C libft/
	@mv libft/libft.a .
	@make clean -C libft/

%.o: $(SRCPATH)%.c
	@gcc $(FLAGS) -c $< -I includes

.PHONY: all clean fclean re

clean:
	@rm -f $(OBJS)
	@rm -f libft.a

fclean: clean
	@rm -f $(NAME)

re: fclean all

