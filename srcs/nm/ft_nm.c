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
    if (munmap(data->raw_data, data->size) < 0)
        return (0);
    return (1);
}

static int get_args(t_nm *data, char *path)
{
    int fd;
    struct stat buffer;

    ft_bzero(data, sizeof(t_nm));
    data->path = path;
    if ((fd = open(data->path, O_RDONLY)) < 0)
        return (ft_error("No such file."));
    if (fstat(fd, &buffer) < 0)
        return (ft_error("Fstat error."));
    if (buffer.st_size == 0)
        return (ft_error("No valid file."));
    if ((data->raw_data = mmap(NULL, buffer.st_size, VM_PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        return (ft_error("Mapping error."));
    if (close(fd) == -1)
        return (ft_error("Error while closing file."));
    data->size = buffer.st_size;
    return (1);
}

int init_data(t_nm *data)
{
    uint32_t magic;

    if (is_overflow(data, data->raw_data + sizeof(uint32_t)))
        return (ft_error("Error while parsing file."));
    magic = *(uint32_t *)(data->raw_data);
    if (magic == MH_MAGIC_64 || magic == MH_CIGAM_64 || magic == MH_MAGIC || magic == MH_CIGAM)
    {
        data->is_64 = (magic == MH_MAGIC_64 || magic == MH_CIGAM_64) ? 1 : 0;
        data->is_endianess = (magic == MH_CIGAM_64 || magic == MH_CIGAM) ? 1 : 0;
        data->header_size = data->is_64 ? sizeof(t_mach_header_64) : sizeof(t_mach_header);
        data->sc_size = data->is_64 ? sizeof(t_segment_command_64) : sizeof(t_segment_command);
        data->nlist_size = data->is_64 ? sizeof(t_nlist_64) : sizeof(t_nlist);
        if (macho_file(data) == -1)
            return (-1);
        print_symbols(data);
    }
    else if (!ft_strncmp(data->raw_data, ARMAG, SARMAG))
        return (archive(data));
    else if (magic == FAT_MAGIC || magic == FAT_CIGAM)
    {
        data->is_endianess = magic == FAT_CIGAM ? 1 : 0;
        return (fat(data));
    }
    else
        ft_printf("%s The file was not recognized as a valid object file\n", data->path);
    return (0);
}

int main(int ac, char **av)
{
    t_nm data;
    int i;

    i = 0;
    if (ac < 2 && get_args(&data, "a.out"))
    {
        if (init_data(&data) == -1)
            return (-1);
    }
    while (++i < ac)
    {
        if (!get_args(&data, av[i]))
            continue;
        if (init_data(&data) == -1)
            return (-1);
        if (!free_data(&data))
            return (-1);
        if (i + 1 < ac)
            write(1, "\n", 1);
    }
    return (0);
}
