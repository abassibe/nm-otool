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
		lst_sort.c \
		print.c \
		archive.c \
		fat.c \
		bit_swap.c \
		utils.c

OBJS = $(SRCS:.c=.o)

SRCPATH = ./srcs/nm/

OBJSPATH = ./objs/

FLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): libft.a $(addprefix objs/, $(OBJS))
	@gcc $(FLAGS) $(addprefix objs/, $(OBJS)) -L. -lft -o $(NAME)

$(OBJSPATH)%.o: $(SRCPATH)%.c
	@mkdir -p objs
	@gcc $(FLAGS) -c $< -o $@ -I includes

libft.a:
	@make -C libft/
	@mv libft/libft.a .
	@make clean -C libft/

.PHONY: all clean fclean re

clean:
	@rm -rf $(OBJSPATH)
	@rm -f libft.a

fclean: clean
	@rm -f $(NAME)

re: fclean all

