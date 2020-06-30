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

static int free_data(t_nm *data)
{
    if (munmap(data->raw_data, data->buffer.st_size) < 0)
        return (0);
    return (1);
}

static int get_args(t_nm *data, char *path)
{
    void *file;
    int fd;

    ft_bzero(data, sizeof(t_nm));
    data->path = path;
    if ((fd = open(data->path, O_RDONLY)) < 0)
        return (ft_error("No such file."));
    if (fstat(fd, &data->buffer) < 0)
        return (ft_error("Fstat error."));
    if (data->buffer.st_size == 0)
        return (ft_error("No valid file."));
    if ((data->raw_data = mmap(NULL, data->buffer.st_size, VM_PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        return (ft_error("Mapping error."));
    return (1);
}

void dispatch(t_nm *data)
{
    if (macho_file(data) == -1)
        return;
    print_symbols(data);
}

void init_data(t_nm *data)
{
    uint32_t magic;

    if (is_overflow(data, data->raw_data + sizeof(uint32_t)))
    {
        ft_error("Error while parsing file.");
        return;
    }
    magic = *(uint32_t *)(data->raw_data);
    data->is_64 = (magic == MH_MAGIC_64 || magic == MH_CIGAM_64) ? 1 : 0;
    data->is_endianess = (magic == MH_CIGAM_64 || magic == MH_CIGAM) ? 1 : 0;
    data->header_size = data->is_64 ? sizeof(struct mach_header_64) : sizeof(struct mach_header);
    data->sc_size = data->is_64 ? sizeof(struct segment_command_64) : sizeof(struct segment_command);
    data->nlist_size = data->is_64 ? sizeof(struct nlist_64) : sizeof(struct nlist);
}

int main(int ac, char **av)
{
    t_nm data;
    int i;

    i = 0;
    if (ac < 2 && get_args(&data, "a.out"))
    {
        init_data(&data);
        dispatch(&data);
    }
    while (++i < ac)
    {
        if (!get_args(&data, av[i]))
            continue;
        init_data(&data);
        dispatch(&data);
        if (!free_data(&data))
            return (-1);
    }
    return (0);
}
