/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abassibe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 02:20:55 by abassibe          #+#    #+#             */
/*   Updated: 2020/03/01 02:20:56 by abassibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ft_nm.h"

static int get_args(t_nm *data, char *path)
{
    void *file;
    int fd;

    ft_bzero(data, sizeof(t_nm));
    data->path = path;
    if ((fd = open(data->path, O_RDONLY)) < 0)
        return (ft_error("No such file."));
    if (fstat(fd, &data->buffer) < 0)
        return (ft_error("No valid file."));
    if (data->buffer.st_size == 0)
        return (ft_error("No valid file."));
    if ((data->raw_data = mmap(NULL, data->buffer.st_size, VM_PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        return (ft_error("Mapping error."));
    return (1);
}

void dispatch(t_nm *data)
{
    uint32_t magic;

    if (is_overflow(data, data->raw_data + sizeof(uint32_t)))
    {
        ft_error("Error while parsing file.");
        return;
    }
    magic = *(uint32_t *)(data->raw_data);
    if (magic == MH_MAGIC || magic == MH_CIGAM || magic == MH_MAGIC_64 || magic == MH_CIGAM_64)
    {
        data->is_64 = (magic == MH_MAGIC_64 || magic == MH_CIGAM_64);
        data->is_endianess = (magic == MH_CIGAM_64 || magic == MH_CIGAM);
        macho_file(data);
    }
}

int main(int ac, char **av)
{
    t_nm data;
    int i;

    i = 0;
    if (ac < 2 && get_args(&data, "a.out"))
        dispatch(&data);
    while (++i < ac)
    {
        if (!get_args(&data, av[i]))
            continue;
        dispatch(&data);
    }
    return (0);
}
