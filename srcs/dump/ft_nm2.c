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

#include "ft_nm.h"

void dispatch(t_nm *data);

int error(char *error)
{
    ft_putendl(error);
    return (0);
}

void dump_bin_macho(t_nm *data, void *raw_data)
{
    char *tmp;
    size_t size;
    struct load_command *lc;
    int nb_cmd;
    int nb_sections;
    struct section_64 *actual_section;
    struct nlist_64 *nl_64;
    struct nlist_64 *nl_64_tab[100];
    int nb_nl;
    int j;

    data->mh_64 = ((struct mach_header_64 *)(data->raw_data));
    size = data->mh_64->ncmds;
    tmp = raw_data;
    lc = ((struct load_command *)(data->raw_data + sizeof(struct mach_header_64)));
    nb_cmd = 0;
    nb_sections = 0;
    actual_section = NULL;
    nb_nl = 0;
    j = 0;

    printf("%p ", tmp);
    for (int i = 0; i < data->size; i++)
    {
        printf("\033[0;0m");
        if (i > 0 && i % 4 == 0)
            printf(" ");
        if (i > 0 && i % 32 == 0)
        {
            printf("\n");
            printf("\033[0;0m%p ", &tmp[i]);
        }
        if (i < sizeof(struct mach_header_64))
            printf("\033[0;31m");
        else if ((void *)&tmp[i] < (void *)lc + sizeof(struct segment_command_64) && lc->cmd == LC_SEGMENT_64)
        {
            if (nb_cmd % 2)
                printf("\033[0;32m");
            else
                printf("\033[1;32m");
            if ((void *)&tmp[i + 1] >= (void *)lc + sizeof(struct segment_command_64))
            {
                nb_cmd++;
                if (((struct segment_command_64 *)lc)->nsects > 0)
                {
                    nb_sections = ((struct segment_command_64 *)lc)->nsects;
                    actual_section = (void *)&tmp[i + 1];
                }
                lc = (void *)lc + sizeof(struct segment_command_64);
            }
        }
        else if (nb_sections > 0)
        {
            if (nb_sections % 2)
                printf("\033[0;33m");
            else
                printf("\033[1;33m");
            if ((void *)&tmp[i + 1] >= (void *)actual_section + sizeof(struct section_64))
            {
                nb_sections--;
                actual_section++;
                lc = (void *)&tmp[i + 1];
            }
        }
        else if ((void *)&tmp[i] < (void *)lc + sizeof(struct symtab_command) && lc->cmd == LC_SYMTAB)
        {
            nl_64 = (void *)(data->raw_data + ((struct symtab_command *)lc)->symoff);
            while (j < ((struct symtab_command *)lc)->nsyms)
            {
                nl_64_tab[nb_nl] = nl_64;
                nb_nl++;
                j++;
                nl_64++;
            }
            if (nb_cmd % 2)
                printf("\033[0;32m");
            else
                printf("\033[1;32m");
            if ((void *)&tmp[i + 1] >= (void *)lc + sizeof(struct symtab_command))
            {
                nb_cmd++;
                lc = (void *)lc + sizeof(struct symtab_command);
            }
        }
        else if ((void *)&tmp[i] >= (void *)nl_64_tab[0] && (void *)&tmp[i] < (void *)(nl_64_tab[nb_nl - 1]) + sizeof(struct nlist_64))
        {
            static int save = 0;
            if (save < sizeof(struct nlist_64))
                printf("\033[0;35m");
            else
                printf("\033[1;35m");
            save++;
            if (save == sizeof(struct nlist_64) * 2)
                save = 0;
        }
        else if ((void *)&tmp[i] < (void *)lc + sizeof(struct dyld_info_command) && (lc->cmd == LC_DYLD_INFO || lc->cmd == LC_DYLD_INFO_ONLY))
        {
            printf("\033[0;34m");
            if ((void *)&tmp[i + 1] >= (void *)lc + sizeof(struct dyld_info_command))
                lc = (void *)lc + sizeof(struct dyld_info_command);
        }
        if (ft_isalnum(tmp[i]) || tmp[i] == '_' || tmp[i] == '/' || tmp[i] == '.')
            printf(" %c ", tmp[i]);
        else
            printf("%.2hhx ", tmp[i]);
    }
    printf("\n");
}

void archive(t_nm *data)
{
    struct ar_hdr *ar_header;
    void *ptr;
    t_nm tmp;

    ptr = data->raw_data + SARMAG;
    ar_header = (struct ar_hdr *)ptr;
    ptr += sizeof(struct ar_hdr);
    tmp.size = ft_atoi(ar_header->ar_size);
    printf("%s(%s):\n", data->path, ptr);
    ptr += tmp.size;
    while (ptr < data->raw_data + data->size)
    {
        ar_header = (struct ar_hdr *)ptr;
        ptr += sizeof(struct ar_hdr);
        tmp.size = ft_atoi(ar_header->ar_size);
        int name_size = !ft_strncmp(ar_header->ar_name,
                                    AR_EFMT1, ft_strlen(AR_EFMT1))
                            ? ft_atoi(ar_header->ar_name + ft_strlen(AR_EFMT1))
                            : 0;
        printf("%s(%s):\n", data->path, ptr);
        tmp.raw_data = ptr + name_size;
        dispatch(&tmp);
        ptr += tmp.size;
    }
}

void dispatch(t_nm *data)
{
    uint32_t magic;

    magic = *(uint32_t *)(data->raw_data);
    if (magic == MH_MAGIC_64 || magic == MH_CIGAM_64 || magic == MH_MAGIC || magic == MH_CIGAM)
    {
        data->is_64 = (magic == MH_MAGIC_64 || magic == MH_CIGAM_64);
        data->is_endianess = (magic == MH_CIGAM_64 || magic == MH_CIGAM);
        dump_bin_macho(data, data->raw_data);
    }
    else if (!ft_strncmp(data->raw_data, ARMAG, SARMAG))
        archive(data);
}

static int get_args(t_nm *data, char *path)
{
    void *file;
    int fd;
    struct stat buffer;

    if ((fd = open(path, O_RDONLY)) < 0)
        return (error("No such file."));
    if (fstat(fd, &buffer) < 0)
        return (error("No valid file."));
    if (buffer.st_size == 0)
        return (error("No valid file."));
    if ((data->raw_data = mmap(NULL, buffer.st_size, VM_PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        return (error("Mapping error."));
    data->size = buffer.st_size;
    data->path = path;
    return (1);
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
