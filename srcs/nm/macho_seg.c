#include "../../includes/ft_nm.h"

static int create_new_sects(t_nm *data, void *sect)
{
    t_section new_sect;
    t_list *lst;

    new_sect.name = ((struct section *)sect)->sectname;
    new_sect.index = data->nsects;
    if (!(lst = ft_lstnew(&new_sect, sizeof(t_section))))
        return (-1);
    ft_lstadd(&data->sects, lst);
    return (1);
}

int macho_segment_init(t_nm *data, void **section, uint32_t *nsects)
{
    *section = data->lc + data->sc_size;
    if (is_overflow(data, *section))
        return (-1);
    *nsects = (data->is_64) ? ((struct segment_command_64 *)
                                   data->lc)
                                  ->nsects
                            : ((struct segment_command *)data->lc)->nsects;
    *nsects = swap_u32(data->is_endianess, *nsects);
    return (1);
}

int macho_segment(t_nm *data)
{
    uint32_t nsects;
    void *section;

    if (macho_segment_init(data, &section, &nsects) == -1)
        return (-1);
    while (nsects--)
    {
        if (is_overflow(data, section) || is_overflow(data, section + data->sc_size))
            return (-1);
        data->nsects++;
        if (create_new_sects(data, section) == -1)
            return (-1);
        section += data->sc_size;
    }
    return (1);
}
