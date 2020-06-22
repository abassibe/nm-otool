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

static t_debug_symbol g_debug_symbols[] =
	{
		{"GSYM", 0x20},
		{"FNAME", 0x22},
		{"FUN", 0x24},
		{"STSYM", 0x26},
		{"LCSYM", 0x28},
		{"BNSYM", 0x2e},
		{"OPT", 0x3c},
		{"RSYM", 0x40},
		{"SLINE", 0x44},
		{"ENSYM", 0x4e},
		{"SSYM", 0x60},
		{"SO", 0x64},
		{"OSO", 0x66},
		{"LSYM", 0x80},
		{"BINCL", 0x82},
		{"SOL", 0x84},
		{"PARAMS", 0x86},
		{"VERSION", 0x88},
		{"OLEVEL", 0x8A},
		{"PSYM", 0xa0},
		{"EINCL", 0xa2},
		{"ENTRY", 0xa4},
		{"LBRAC", 0xc0},
		{"EXCL", 0xc2},
		{"RBRAC", 0xe0},
		{"BCOMM", 0xe2},
		{"ECOMM", 0xe4},
		{"ECOML", 0xe8},
		{"LENG", 0xfe},
		{0, 0}};

static t_mysect *find_mysection(t_list *lst, uint8_t n_sect)
{
	while (lst && lst->content)
	{
		if (((t_mysect *)lst->content)->index == n_sect)
			return (lst->content);
		lst = lst->next;
	}
	return (NULL);
}

static void match_sym_section(t_list *mysect_lst, t_mysym *mysym)
{
	t_mysect *mysect;

	if ((mysect = find_mysection(mysect_lst, mysym->sect)))
	{
		if (!ft_strcmp(mysect->name, SECT_TEXT))
			mysym->type_p = 'T';
		else if (!ft_strcmp(mysect->name, SECT_DATA))
			mysym->type_p = 'D';
		else if (!ft_strcmp(mysect->name, SECT_BSS))
			mysym->type_p = 'B';
		else
			mysym->type_p = 'S';
		if (!(mysym->type & N_EXT))
			mysym->type_p -= 'A' - 'a';
	}
}

char *get_debug_symbol(uint16_t type)
{
	int i;

	i = 0;
	while (g_debug_symbols[i].symbol)
	{
		if (g_debug_symbols[i].typevalue == type)
			return (g_debug_symbols[i].symbol);
		i++;
	}
	return (NULL);
}

void fill_mysym(t_nm *data, t_mysym *mysym)
{
	if (N_STAB & mysym->type)
	{
		mysym->type_p = '-';
		mysym->debug_symbol = get_debug_symbol(mysym->type);
	}
	else if ((N_TYPE & mysym->type) == N_UNDF)
	{
		if (mysym->namefailed)
			mysym->type_p = 'C';
		else if (mysym->type & N_EXT)
			mysym->type_p = 'U';
		else
			mysym->type_p = '?';
	}
	else if ((N_TYPE & mysym->type) == N_SECT)
		match_sym_section(data->sects, mysym);
	else if ((N_TYPE & mysym->type) == N_ABS)
		mysym->type_p = 'A';
	else if ((N_TYPE & mysym->type) == N_INDR)
		mysym->type_p = 'I';
}

t_mysym *init_mysym(t_nm *data,
					t_mysym *mysym, char *symname, void *sym)
{
	int namefailed;

	ft_bzero(mysym, sizeof(t_mysym));
	namefailed = 0;
	mysym->type_p = ' ';
	mysym->name = ft_strdup_overflow(data, symname, '\n', &namefailed);
	if (namefailed)
		mysym->namefailed = TRUE;
	if (!mysym->name)
		return (NULL);
	if (!data->is_64)
	{
		mysym->type = ((struct nlist *)sym)->n_type;
		mysym->sect = ((struct nlist *)sym)->n_sect;
		mysym->desc = ((struct nlist *)sym)->n_desc;
		mysym->value = swap_u32(data->is_endianess, ((struct nlist *)sym)->n_value);
	}
	else
	{
		mysym->type = ((struct nlist_64 *)sym)->n_type;
		mysym->sect = ((struct nlist_64 *)sym)->n_sect;
		mysym->desc = ((struct nlist_64 *)sym)->n_desc;
		mysym->value = swap_u32(data->is_endianess, ((struct nlist_64 *)sym)->n_value);
	}
	return (mysym);
}

static t_list *create_mysym(t_nm *data, char *strtab, void *sym)
{
	t_mysym mysym;
	char *symname;

	symname = strtab + swap_u32(data->is_endianess, (data->is_endianess)
														? ((struct nlist_64 *)sym)->n_un.n_strx
														: ((struct nlist *)sym)->n_un.n_strx);
	if (!(init_mysym(data, &mysym, symname, (struct nlist *)sym)))
		return (NULL);
	fill_mysym(data, &mysym);
	return (ft_lstnew(&mysym, sizeof(t_mysym)));
}

int parse_macho_symtab(t_nm *data,
					   struct symtab_command *symtab_command)
{
	void *sym;
	uint64_t sym_size;
	void *strtab;
	uint64_t nsyms;
	t_list *lst;

	if (is_overflow(data, symtab_command + 1))
		return (-1);
	strtab = (void *)data->raw_data + swap_u32(data->is_endianess, symtab_command->stroff);
	sym = (void *)data->raw_data + swap_u32(data->is_endianess, symtab_command->symoff);
	nsyms = swap_u32(data->is_endianess, symtab_command->nsyms);
	sym_size = ((data->is_64) ? sizeof(struct nlist_64) : sizeof(struct nlist));
	while (nsyms--)
	{
		if (is_overflow(data, strtab) || is_overflow(data, sym))
			return (-1);
		if (!(lst = create_mysym(data, strtab, sym)))
			return (-1);
		ft_lstadd(&data->syms, lst);
		sym += sym_size;
	}
	return (1);
}

static int create_mysects(t_nm *data, void *sect)
{
	t_section new_sect;
	t_list *lst;

	new_sect.name = data->is_64 ? ((struct section_64 *)sect)->sectname
								: ((struct section *)sect)->sectname;
	new_sect.index = data->nsects;
	if (!(lst = ft_lstnew(&new_sect, sizeof(t_section))))
		return (-1);
	ft_lstadd(&data->sects, lst);
	return (1);
}

int macho_segment_init(t_nm *data, void **section, uint32_t *nsects, void *segment_command)
{
	*section = segment_command + ((data->is_64)
									  ? sizeof(struct segment_command_64)
									  : sizeof(struct segment_command));
	if (is_overflow(data, data->lc))
		return (-1);
	*nsects = (data->is_64) ? ((struct segment_command_64 *)
								   segment_command)
								  ->nsects
							: ((struct segment_command *)segment_command)->nsects;
	*nsects = swap_u32(data->is_endianess, *nsects);
	return (1);
}

int macho_segment(t_nm *data, void *segment_command)
{
	uint64_t section_size;
	uint32_t nsects;
	void *section;

	if (macho_segment_init(data, &section, &nsects, segment_command))
		return (-1);
	section_size = ((data->is_64) ? sizeof(struct section_64)
								  : sizeof(struct section));
	while (nsects--)
	{
		if (is_overflow(data, section) || is_overflow(data, section + section_size))
			return (-1);
		data->nsects++;
		if (create_mysects(data, section) == -1)
			return (-1);
		section += section_size;
	}
	return (1);
}

static int parse_lc_error(t_nm *data)
{
	uint64_t cmd_size;
	void *endofcmds;

	cmd_size = data->lc->cmdsize;
	endofcmds = (struct load_command *)(data->raw_data + ((data->is_64) ? sizeof(struct mach_header_64) : sizeof(struct mach_header))) + swap_u32(data->is_endianess, (data->is_64) ? ((struct mach_header_64 *)(data->raw_data))->sizeofcmds : ((struct mach_header *)(data->raw_data))->sizeofcmds);
	if ((void *)data->lc + (cmd_size || 1) > endofcmds)
		return (-1);
	return (1);
}

static int parse_lc(t_nm *data, int i)
{
	uint32_t cmd;

	cmd = swap_u32(data->is_endianess, data->lc->cmd);
	if (parse_lc_error(data) == -1)
		return (-1);
	if (cmd == LC_SEGMENT || cmd == LC_SEGMENT_64)
		return (macho_segment(data, data->lc));
	else if (cmd == LC_SYMTAB)
		return (parse_macho_symtab(data, (void *)data->lc));
	data->sc_64 = ((struct segment_command_64 *)(data->raw_data + sizeof(struct mach_header_64)));
	return (1);
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
		if (is_overflow(data, data->lc + 1) || parse_lc(data, i++) == -1)
			return (ft_error("Error while parsing file."));
		data->lc = (void *)data->lc + swap_u32(data->is_endianess, data->lc->cmdsize);
	}
	return (1);
}
