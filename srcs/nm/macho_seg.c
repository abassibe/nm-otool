#include "../../includes/ft_nm.h"

static int create_new_sects(t_nm *data, void *sect)
{
    t_section new_sect;
    t_list *lst;

    new_sect.name = data->is_64 ? ((struct section_64 *)sect)->sectname : ((struct section *)sect)->sectname;
    new_sect.index = data->nsects;
    if (!(lst = ft_lstnew(&new_sect, sizeof(t_section))))
        return (-1);
    ft_lstadd(&data->sects, lst);
    return (1);
}

int macho_segment_init(t_nm *data, void **section, uint32_t *nsects)
{
    void *segment_command = data->lc;

    *section = segment_command + data->sc_size;
    if (is_overflow(data, *section))
        return (-1);
    *nsects = data->is_64 ? ((t_segment_command_64 *)
                                 segment_command)
                                ->nsects
                          : ((t_segment_command *)segment_command)->nsects;
    *nsects = swap_u32(data->is_endianess, *nsects);
    return (1);
}

static void init_mysect(t_nm *data, void *sect, t_my_sect *mysect)
{
    if (!data->is_64)
    {
        mysect->addr = swap_u32(data->is_endianess, ((struct section *)sect)->addr);
        mysect->size = swap_u32(data->is_endianess, ((struct section *)sect)->size);
        mysect->offset = swap_u32(data->is_endianess, ((struct section *)sect)->offset);
        mysect->name = ((struct section *)sect)->sectname;
    }
    else
    {
        mysect->addr = swap_u64(data->is_endianess, ((struct section_64 *)sect)->addr);
        mysect->size = swap_u64(data->is_endianess, ((struct section_64 *)sect)->size);
        mysect->offset = swap_u32(data->is_endianess, ((struct section_64 *)sect)->offset);
        mysect->name = ((struct section_64 *)sect)->sectname;
    }
}

static int print_section(t_nm *data, void *section)
{
    t_my_sect mysect;

    init_mysect(data, section, &mysect);
    if (!ft_strcmp(SECT_TEXT, mysect.name))
    {
        if (is_overflow(data, data->raw_data + mysect.offset + mysect.size))
            return (-1);
        ft_printf("Contents of (__TEXT,__text) section\n");
        ft_hexdump(data, data->raw_data + mysect.offset, mysect.size, mysect.addr);
    }
    return (0);
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
        if (data->otool)
        {
            if (print_section(data, section) == -1)
                return (-1);
        }
        if (create_new_sects(data, section) == -1)
            return (-1);
        section += data->sc_size;
    }
    return (1);
}
