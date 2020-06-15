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

int error(char *error)
{
    ft_putendl(error);
    return (0);
}

void dump_bin(t_nm *data, void *raw_data)
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
    for (int i = 0; i < data->buffer.st_size; i++)
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
                //           printf("%p\n", nl_64_tab[nb_nl]);
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

static int get_args(t_nm *data, char *path)
{
    void *file;
    int fd;

    data->path = path;
    if ((fd = open(data->path, O_RDONLY)) < 0)
        return (error("No such file."));
    if (fstat(fd, &data->buffer) < 0)
        return (error("No valid file."));
    if (data->buffer.st_size == 0)
        return (error("No valid file."));
    if ((data->raw_data = mmap(NULL, data->buffer.st_size, VM_PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        return (error("Mapping error."));

    // dump_bin(data, data->raw_data);
    return (1);
}

void parse_lc(t_nm *data)
{
    data->sc_64 = ((struct segment_command_64 *)(data->raw_data + sizeof(struct mach_header_64)));
}

t_symlist *create_node(t_symlist *prev)
{
    t_symlist *new;

    new = (t_symlist *)malloc(sizeof(t_symlist));
    new->padded = ft_strnew(20);
    new->name = NULL;
    if (!prev)
        new->prev = NULL;
    else
    {
        new->prev = prev;
        prev->next = new;
    }
    new->next = NULL;
    return (new);
}

int is_higher(char *first, char *second)
{
    int i;

    i = 0;
    while (first[i] && second[i])
    {
        if (first[i] > second[i])
            return (1);
        if (first[i] < second[i])
            return (0);
        i++;
    }
    return (0);
}

void sort_list(t_symlist *list)
{
    t_symlist *tmp;
    t_symlist *save;
    t_symlist *start;
    char *tmp1;
    char *tmp2;

    save = list;
    start = list;
    while (start)
    {
        list = start->prev;
        while (list)
        {
            if (!is_higher(start->name, list->name))
            {
                tmp1 = list->name;
                tmp2 = list->padded;
                list->name = start->name;
                list->padded = start->padded;
                start->name = tmp1;
                start->padded = tmp2;
            }
            list = list->prev;
        }
        start = start->prev;
    }
    while (save->prev)
        save = save->prev;
    while (save)
    {
        printf("%s%s\n", save->padded, save->name);
        save = save->next;
    }
}

void macho_file(t_nm *data)
{
    size_t size;
    size_t offset;
    struct symtab_command *sc;
    struct nlist_64 *nl_64;
    struct section_64 *sec_64;
    t_symlist *symlist;

    if (data->is_64)
    {
        symlist = NULL;
        data->mh_64 = ((struct mach_header_64 *)(data->raw_data));
        offset = sizeof(struct mach_header_64);
        data->lc = ((struct load_command *)(data->raw_data + sizeof(struct mach_header_64)));
        size = data->mh_64->ncmds;
        parse_lc(data);
        for (int i = 0; i < size; i++)
        {
            if (data->lc->cmd == LC_SYMTAB)
            {
                sc = (struct symtab_command *)data->lc;
                int i = 0;
                void *k = (void *)(data->raw_data + sc->symoff);
                while (i < sc->nsyms)
                {
                    symlist = create_node(symlist);
                    nl_64 = (struct nlist_64 *)k;
                    if ((nl_64->n_type & N_TYPE) == N_UNDF)
                        nl_64->n_type &N_EXT ? sprintf(symlist->padded, "% 16.0d U ", 0) : sprintf(symlist->padded, "% 16.0d u ", 0);
                    else if ((nl_64->n_type & N_TYPE) == N_SECT)
                        nl_64->n_type &N_EXT ? sprintf(symlist->padded, "%016llx T ", nl_64->n_value) : sprintf(symlist->padded, "%016llx t ", nl_64->n_value);
                    else if ((nl_64->n_type & N_TYPE) == N_ABS)
                        nl_64->n_type &N_EXT ? sprintf(symlist->padded, "%016llx A ", nl_64->n_value) : sprintf(symlist->padded, "%016llx a ", nl_64->n_value);
                    else if ((nl_64->n_type & N_TYPE) == N_INDR)
                        nl_64->n_type &N_EXT ? sprintf(symlist->padded, "% 16.0d I ", 0) : sprintf(symlist->padded, "% 16.0d i ", 0);

                    i++;
                    k += sizeof(struct nlist_64);
                    char *c = (char *)(data->raw_data + sc->stroff);
                    symlist->name = &c[nl_64->n_un.n_strx];
                }
            }
            if (data->lc->cmd == LC_SEGMENT_64)
            {
                data->sc_64 = (struct segment_command_64 *)data->lc;
                // printf("adr = %p => %s\n", data->sc_64->segname, data->sc_64->segname);
                offset += sizeof(struct segment_command_64);
                for (int f = 0; f < data->sc_64->nsects; f++)
                {
                    sec_64 = (struct section_64 *)(data->raw_data + offset);
                    // printf("%p =  SECTION = [%s]  ", sec_64, sec_64->sectname);
                }
                // printf("\n");
            }
            offset += data->lc->cmdsize;
            data->lc = (void *)data->lc + data->lc->cmdsize;
        }
        sort_list(symlist);
    }
}

void dispatch(t_nm *data)
{
    uint32_t magic;

    magic = *(uint32_t *)(data->raw_data);
    data->is_64 = (magic == MH_MAGIC_64 || magic == MH_CIGAM_64);
    data->is_endianess = (magic == MH_CIGAM_64 || magic == MH_CIGAM);
    if (magic == MH_MAGIC || magic == MH_CIGAM || magic == MH_MAGIC_64 || magic == MH_CIGAM_64)
        macho_file(data);
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
