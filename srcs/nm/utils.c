/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abassibe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/17 15:20:31 by abassibe          #+#    #+#             */
/*   Updated: 2020/06/17 15:20:33 by abassibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ft_nm.h"

int is_overflow(t_nm *data, void *ptr)
{
    if ((ptr > data->raw_data + data->buffer.st_size) || (ptr < data->raw_data))
        return (1);
    return (0);
}

int ft_error(char *error)
{
    ft_putendl(error);
    return (-1);
}

uint32_t swap_u32(char endian, uint32_t x)
{
    if (endian)
    {
        x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
        return ((x << 16) | (x >> 16));
    }
    else
        return (x);
}

uint64_t swap_u64(char endian, uint64_t x)
{
    if (endian)
    {
        x = ((x << 8) & 0xFF00FF00FF00FF00ULL) | ((x >> 8) & 0x00FF00FF00FF00FFULL);
        x = ((x << 16) & 0xFFFF0000FFFF0000ULL) | ((x >> 16) & 0x0000FFFF0000FFFFULL);
        return ((x << 32) | (x >> 32));
    }
    else
        return (x);
}
