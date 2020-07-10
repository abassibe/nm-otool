/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lst_sort.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abassibe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/10 09:37:12 by abassibe          #+#    #+#             */
/*   Updated: 2020/07/10 09:37:52 by abassibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ft_nm.h"

static int		sort_by_name(t_list *lst1, t_list *lst2)
{
	t_my_symbol		*sym1;
	t_my_symbol		*sym2;

	if (!lst1 || !lst2 || !lst1->content || !lst2->content)
		return (0);
	sym1 = lst1->content;
	sym2 = lst2->content;
	if (!ft_strcmp(sym1->name, sym2->name))
		return (sym1->value >= sym2->value);
	return (ft_strcmp(sym1->name, sym2->name) > 0);
}

static void		lst_swap(t_list *lst1, t_list *lst2)
{
	void	*content_tmp;
	size_t	content_size_tmp;

	content_tmp = lst1->content;
	content_size_tmp = lst1->content_size;
	lst1->content = lst2->content;
	lst1->content_size = lst2->content_size;
	lst2->content = content_tmp;
	lst2->content_size = content_size_tmp;
}

void			ft_lstsort(t_list *lst)
{
	t_list	*to_replace;
	t_list	*el;
	t_list	*min;

	to_replace = lst;
	while (to_replace && to_replace->next)
	{
		min = NULL;
		el = to_replace;
		while (el)
		{
			if (!min || sort_by_name(min, el))
				min = el;
			el = el->next;
		}
		if (min && to_replace != min)
			lst_swap(to_replace, min);
		to_replace = to_replace->next;
	}
}
