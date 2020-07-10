/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abassibe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/17 15:20:31 by abassibe          #+#    #+#             */
/*   Updated: 2020/07/10 09:50:14 by abassibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ft_nm.h"

int			ft_error(char *error)
{
	ft_putendl(error);
	return (-1);
}

uint32_t	swap_u32(char swap, uint32_t x)
{
	if (swap)
	{
		x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
		return ((x << 16) | (x >> 16));
	}
	else
		return (x);
}

uint64_t	swap_u64(char swap, uint64_t x)
{
	if (swap)
	{
		x = ((x << 8) & 0xFF00FF00FF00FF00ULL) |
			((x >> 8) & 0x00FF00FF00FF00FFULL);
		x = ((x << 16) & 0xFFFF0000FFFF0000ULL) |
			((x >> 16) & 0x0000FFFF0000FFFFULL);
		return ((x << 32) | (x >> 32));
	}
	else
		return (x);
}

int			check_overflow_wo_error(t_nm *data, void *ptr)
{
	if ((ptr > data->raw_data + data->size) || (ptr < data->raw_data))
		return (1);
	return (0);
}

char		*ft_strdup_overflow(t_nm *data, char *src, char end_char,
		int *failed)
{
	size_t	size;
	size_t	i;
	char	*str;

	i = 0;
	size = 0;
	while (!check_overflow_wo_error(data, src + size) && src[size] &&
			src[size] != end_char && ft_isprint(src[size]))
		size++;
	if (check_overflow_wo_error(data, src + size))
	{
		*failed = 1;
		return (ft_strdup("bad string index"));
	}
	if ((str = (char *)malloc(sizeof(*str) * (size + 1 + 1))) == NULL)
		return (NULL);
	while (i < size)
	{
		str[i] = src[i];
		i++;
	}
	if (src[size] == end_char)
		str[i++] = end_char;
	str[i] = 0;
	return (str);
}
