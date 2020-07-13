/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   archive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abassibe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/10 09:27:19 by abassibe          #+#    #+#             */
/*   Updated: 2020/07/10 09:28:10 by abassibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ft_nm.h"

int		parse_archive(t_nm *data, struct ar_hdr *ar_header, uint64_t ar_size)
{
	void		*ptr;
	t_nm		tmp;
	uint64_t	name_size;

	ft_bzero(&tmp, sizeof(t_nm));
	ptr = (void *)ar_header + sizeof(struct ar_hdr);
	name_size = !ft_strncmp(ar_header->ar_name,
			AR_EFMT1, ft_strlen(AR_EFMT1))
		? ft_atoi(ar_header->ar_name + ft_strlen(AR_EFMT1))
		: 0;
	if (is_overflow(data, ptr + name_size))
		return (-1);
	tmp.size = ar_size - sizeof(ar_header);
	tmp.file_name = data->file_name;
	tmp.raw_data = ptr + name_size;
	tmp.otool = data->otool;
	tmp.path = data->path;
	tmp.archive = 1;
	if (!data->otool)
		ft_printf("\n");
	ft_printf("%s(%s):\n", data->path, ptr);
	init_data(&tmp);
	return (0);
}

int		archive(t_nm *data)
{
	struct ar_hdr	*ar_header;
	void			*ptr;
	uint64_t		ar_size;
	int				i;

	ptr = data->raw_data + SARMAG;
	i = 0;
	if (data->otool)
		ft_printf("Archive : %s\n", data->path);
	while (ptr < data->raw_data + data->size)
	{
		ar_header = (struct ar_hdr *)ptr;
		ptr += sizeof(struct ar_hdr);
		ar_size = ft_atoi(ar_header->ar_size);
		if (is_overflow(data, ptr) || ar_size == 0 ||
				is_overflow(data, ptr + ar_size))
			return (ft_error("Error while parsing file."));
		if (i++)
		{
			if (parse_archive(data, ar_header, ar_size) == -1)
				return (ft_error("Error while parsing file."));
		}
		ptr += ar_size;
	}
	return (0);
}
