/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macho.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abassibe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/17 15:01:55 by abassibe          #+#    #+#             */
/*   Updated: 2020/06/17 15:01:56 by abassibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ft_nm.h"

static int parse_lc(t_nm *data, int i)
{
    uint32_t cmd;

    cmd = swap_u32(data->is_endianess, data->lc->cmd);
    data->sc_64 = ((struct segment_command_64 *)(data->raw_data + sizeof(struct mach_header_64)));
}

int macho_file(t_nm *data)
{
    uint32_t ncmds;
    int i;

    i = 0;
    data->lc = (struct load_command *)(data->raw_data + ((data->is_64) ? sizeof(struct mach_header_64) : sizeof(struct mach_header)));
    if (is_overflow(data, data->lc))
        return (ft_error("Error while parsing file."));
    ncmds = swap_u32(data->is_endianess, (data->is_64)
                                             ? ((struct mach_header_64 *)(data->raw_data))->ncmds
                                             : ((struct mach_header *)(data->raw_data))->ncmds);
    while (i < ncmds)
    {
        if (is_overflow(data, data->lc + 1) || parse_lc(data, i++))
            return (ft_error("Error while parsing file."));
        data->lc = (void *)data->lc + swap_u32(data->is_endianess, data->lc->cmdsize);
    }
}
