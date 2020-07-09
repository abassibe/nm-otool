#include "../../includes/ft_nm.h"

static int fat_init(t_nm *data, struct fat_arch *fat, uint32_t *offset, cpu_type_t *cpu_type)
{
    if (is_overflow(data, fat + 1))
        return (-1);
    *cpu_type = data->is_endianess ? bswap_int32(fat->cputype) : fat->cputype;
    *offset = data->is_endianess ? bswap_uint32(fat->offset) : fat->offset;
    if (*offset == 0)
        return (-1);
    return (0);
}

static int parse_fat(t_nm *data, struct fat_arch *fat)
{
    t_nm tmp;
    uint32_t offset;
    cpu_type_t cpu_type;
    cpu_type_t cpu_subtype;

    if (fat_init(data, fat, &offset, &cpu_type) == -1)
        return (-1);
    cpu_subtype = data->is_endianess ? bswap_int32(fat->cpusubtype) : fat->cpusubtype;
    ft_bzero(&tmp, sizeof(tmp));
    if (cpu_type == CPU_TYPE_X86_64)
    {
        tmp.size = data->is_endianess ? bswap_uint32(fat->size) : fat->size;
        tmp.file_name = data->file_name;
        tmp.raw_data = data->raw_data + offset;
        return (init_data(&tmp));
    }
    return (0);
}

static int fat_section(t_nm *data, struct fat_arch *fat, unsigned long nfat_arch)
{
    unsigned long i;
    int ret;

    i = 0;
    ret = 0;
    while (i < nfat_arch)
    {
        if (parse_fat(data, fat) == -1)
            return (ft_error("Error while parsing file."));
        fat = (void *)fat + sizeof(struct fat_arch);
        i++;
    }
    return (0);
}

int fat(t_nm *data)
{
    unsigned long nfat_arch;
    struct fat_arch *fat;

    nfat_arch = ((struct fat_header *)data->raw_data)->nfat_arch;
    nfat_arch = swap_u32(data->is_endianess, nfat_arch);
    if (is_overflow(data, data->raw_data + sizeof(struct fat_header)))
        return (ft_error("Error while parsing file."));
    fat = data->raw_data + sizeof(struct fat_header);
    return (fat_section(data, fat, nfat_arch));
}
