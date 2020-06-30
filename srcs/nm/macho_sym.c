#include "../../includes/ft_nm.h"

static t_my_sect *find_mysection(t_list *lst, uint8_t n_sect)
{
    while (lst && lst->content)
    {
        if (((t_my_sect *)lst->content)->index == n_sect)
            return (lst->content);
        lst = lst->next;
    }
    return (NULL);
}

static void match_sym_section(t_list *mysect_lst, t_my_sym *my_sym)
{
    t_my_sect *mysect;

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

void fill_mysym(t_nm *data, t_my_sym *my_sym)
{
    if (N_STAB & my_sym->type)
    {
        my_sym->type_p = '-';
        my_sym->debug_symbol = get_debug_symbol(my_sym->type);
    }
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

t_my_sym *init_my_sym(t_nm *data,
                      t_my_sym *my_sym, char *symname, void *sym)
{
    int namefailed;

    ft_bzero(my_sym, sizeof(t_my_sym));
    namefailed = 0;
    my_sym->type_p = ' ';
    my_sym->name = ft_strdup_overflow(data, symname, '\n', &namefailed);
    if (namefailed)
        my_sym->namefailed = TRUE;
    if (!my_sym->name)
        return (NULL);
    if (!data->is_64)
    {
        my_sym->type = ((struct nlist *)sym)->n_type;
        my_sym->sect = ((struct nlist *)sym)->n_sect;
        my_sym->desc = ((struct nlist *)sym)->n_desc;
        my_sym->value = swap_u32(data->is_endianess, ((struct nlist *)sym)->n_value);
    }
    else
    {
        my_sym->type = ((struct nlist_64 *)sym)->n_type;
        my_sym->sect = ((struct nlist_64 *)sym)->n_sect;
        my_sym->desc = ((struct nlist_64 *)sym)->n_desc;
        my_sym->value = swap_u32(data->is_endianess, ((struct nlist_64 *)sym)->n_value);
    }
    return (my_sym);
}

t_list *create_mysym(t_nm *data, char *strtab, void *sym)
{
    t_my_sym my_sym;
    char *symname;

    symname = strtab + swap_u32(data->is_endianess, (data->is_endianess)
                                                        ? ((struct nlist_64 *)sym)->n_un.n_strx
                                                        : ((struct nlist *)sym)->n_un.n_strx);
    if (!(init_my_sym(data, &my_sym, symname, (struct nlist *)sym)))
        return (NULL);
    fill_mysym(data, &my_sym);
    return (ft_lstnew(&my_sym, sizeof(t_my_sym)));
}