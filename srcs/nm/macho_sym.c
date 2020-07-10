/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macho_sym.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abassibe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/10 09:42:15 by abassibe          #+#    #+#             */
/*   Updated: 2020/07/10 09:46:30 by abassibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ft_nm.h"

static t_my_sect	*find_mysection(t_list *lst, uint8_t n_sect)
{
	int i = 0;
	while (lst && lst->content)
	{
		if (((t_my_sect *)lst->content)->index == n_sect)
			return (lst->content);
		lst = lst->next;
		i++;
	}
	return (NULL);
}

static void			match_sym_section(t_list *mysect_lst, t_my_sym *my_sym)
{
	t_my_sect	*mysect;

	if ((mysect = find_mysection(mysect_lst, my_sym->sect)))
	{
		if (!ft_strcmp(mysect->name, SECT_TEXT))
			my_sym->type_p = 'T';
		else if (!ft_strcmp(mysect->name, SECT_DATA))
			my_sym->type_p = 'D';
		else if (!ft_strcmp(mysect->name, SECT_BSS))
			my_sym->type_p = 'B';
		else
			my_sym->type_p = 'S';
		if (!(my_sym->type & N_EXT))
			my_sym->type_p -= 'A' - 'a';
	}
}

static void			fill_mysym(t_nm *data, t_my_sym *my_sym)
{
	if (N_STAB & my_sym->type)
		my_sym->type_p = '-';
	else if ((N_TYPE & my_sym->type) == N_UNDF)
	{
		if (my_sym->namefailed)
			my_sym->type_p = 'C';
		else if (my_sym->type & N_EXT)
			my_sym->type_p = 'U';
		else
			my_sym->type_p = '?';
	}
	else if ((N_TYPE & my_sym->type) == N_SECT)
		match_sym_section(data->sects, my_sym);
	else if ((N_TYPE & my_sym->type) == N_ABS)
		my_sym->type_p = 'A';
	else if ((N_TYPE & my_sym->type) == N_INDR)
		my_sym->type_p = 'I';
}

static t_my_sym		*init_my_sym(t_nm *data, t_my_sym *my_sym,
		char *symname, void *sym)
{
	int		namefailed;

	namefailed = 0;
	my_sym->type_p = ' ';
	my_sym->name = ft_strdup_overflow(data, symname, '\n', &namefailed);
	if (namefailed)
		my_sym->namefailed = TRUE;
	if (!my_sym->name)
		return (NULL);
	if (!data->is_64)
	{
		my_sym->type = ((t_nlist *)sym)->n_type;
		my_sym->sect = ((t_nlist *)sym)->n_sect;
		my_sym->desc = ((t_nlist *)sym)->n_desc;
		my_sym->value = swap_u32(data->is_endianess, ((t_nlist *)sym)->n_value);
	}
	else
	{
		my_sym->type = ((t_nlist_64 *)sym)->n_type;
		my_sym->sect = ((t_nlist_64 *)sym)->n_sect;
		my_sym->desc = ((t_nlist_64 *)sym)->n_desc;
		my_sym->value = swap_u64(data->is_endianess,
				((t_nlist_64 *)sym)->n_value);
	}
	return (my_sym);
}

t_list				*create_mysym(t_nm *data, char *strtab, void *sym)
{
	t_my_sym	my_sym;
	t_list		*to_replace;
	char		*symname;

	ft_bzero(&my_sym, sizeof(t_my_sym));
	symname = strtab + swap_u32(data->is_endianess, (data->is_64) ?
			((t_nlist_64 *)sym)->n_un.n_strx : ((t_nlist *)sym)->n_un.n_strx);
	if (!(init_my_sym(data, &my_sym, symname, (t_nlist *)sym)))
		return (NULL);
	fill_mysym(data, &my_sym);
	to_replace = ft_lstnew(&my_sym, sizeof(t_my_sym));
	to_replace->next = NULL;
	return (to_replace);
}
