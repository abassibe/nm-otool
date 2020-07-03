#include "../../includes/ft_nm.h"

int archive(t_nm *data)
{
    struct ar_hdr *ar_header;
    void *ptr;
    t_nm tmp;

    ft_bzero(&tmp, sizeof(t_nm));
    tmp.is_endianess = 0;
    ptr = data->raw_data + SARMAG;
    ar_header = (struct ar_hdr *)ptr;
    ptr += sizeof(struct ar_hdr);
    tmp.size = ft_atoi(ar_header->ar_size);
    ptr += tmp.size;
    while (ptr < data->raw_data + data->size)
    {
        ft_bzero(&tmp, sizeof(t_nm));
        ar_header = (struct ar_hdr *)ptr;
        ptr += sizeof(struct ar_hdr);
        tmp.size = ft_atoi(ar_header->ar_size);
        int name_size = !ft_strncmp(ar_header->ar_name,
                                    AR_EFMT1, ft_strlen(AR_EFMT1))
                            ? ft_atoi(ar_header->ar_name + ft_strlen(AR_EFMT1))
                            : 0;
        printf("%s(%s):\n", data->path, ptr);
        tmp.raw_data = ptr + name_size;
        tmp.size -= sizeof(ar_header);
        init_data(&tmp);
        ptr += tmp.size + sizeof(ar_header);
    }
    return (0);
}
