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

int parse_macho_symtab(t_nm *data, t_symtab_command *stc)
{
	void *sym;
	void *strtab;
	uint64_t nsyms;
	t_list *lst;

	if (is_overflow(data, stc + 1))
		return (-1);
	strtab = (void *)data->raw_data + swap_u32(data->is_endianess, stc->stroff);
	sym = (void *)data->raw_data + swap_u32(data->is_endianess, stc->symoff);
	nsyms = swap_u32(data->is_endianess, stc->nsyms);
	while (nsyms--)
	{
		if (is_overflow(data, strtab) || is_overflow(data, sym))
			return (-1);
		if (!(lst = create_mysym(data, strtab, sym)))
			return (-1);
		ft_lstadd(&data->syms, lst);
		sym += data->nlist_size;
	}
	return (1);
}

static int parse_lc_error(t_nm *data)
{
	uint64_t cmd_size;
	void *end_cmds;

	cmd_size = data->lc->cmdsize;
	end_cmds = (t_load_command *)(data->raw_data + data->header_size) + swap_u32(data->is_endianess, ((t_mach_header *)(data->raw_data))->sizeofcmds);
	if ((void *)data->lc + (cmd_size || 1) > end_cmds)
		return (-1);
	return (1);
}

static int parse_lc(t_nm *data)
{
	uint32_t cmd;

	cmd = swap_u32(data->is_endianess, data->lc->cmd);
	if (parse_lc_error(data) == -1)
		return (-1);
	if (cmd == LC_SEGMENT || cmd == LC_SEGMENT_64)
		return (macho_segment(data));
	else if (!data->otool && cmd == LC_SYMTAB)
		return (parse_macho_symtab(data, (void *)data->lc));
	return (1);
}

int macho_file(t_nm *data)
{
	uint32_t ncmds;
	uint32_t i;

	i = 0;
	if (data->otool)
		ft_printf("%s:\n", data->path);
	data->lc = (t_load_command *)(data->raw_data + data->header_size);
	if (is_overflow(data, data->lc))
		return (-1);
	ncmds = swap_u32(data->is_endianess, ((t_mach_header *)(data->raw_data))->ncmds);
	while (i < ncmds)
	{
		if (is_overflow(data, data->lc + 1) || parse_lc(data) == -1)
			return (-1);
		data->lc = (void *)data->lc + swap_u32(data->is_endianess, data->lc->cmdsize);
		i++;
	}
	if (!data->otool)
		print_symbols(data);
	return (1);
}
