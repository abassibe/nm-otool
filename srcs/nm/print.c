#include "../../includes/ft_nm.h"

static void print_mysyms_line(uint8_t left_padding, t_my_symbol *sym)
{
    if (sym->type_p == '-')
        ;
    else if (sym->type_p == 'I')
        ft_printf("%*c %c %s (indirect for %s)\n", left_padding, ' ',
                  sym->type_p, sym->name, sym->name);
    else if (sym->type_p != 'U')
        ft_printf("%0*llx %c %s\n", left_padding, sym->value, sym->type_p,
                  sym->name);
    else
        ft_printf("%*c %c %s\n", left_padding, ' ', sym->type_p, sym->name);
}

void print_symbols(t_nm *data)
{
    t_list *symlst;
    t_my_symbol *sym;

    symlst = data->syms;
    ft_lstsort(symlst);
    while (symlst)
    {
        sym = symlst->content;
        print_mysyms_line((data->is_64) ? 16 : 8, sym);
        symlst = symlst->next;
    }
}

void ft_hexdump_line_values(t_nm *data, char *start, int64_t len)
{
    int64_t i;

    i = 0;
    while (i < len)
    {
        if (data->is_endianess)
        {
            if (i + 3 - 2 * (i % 4) < len)
            {
                ft_printf("%02x", (uint8_t)start[i + 3 - 2 * (i % 4)]);
                if (i % 4 == 3 && data->is_64)
                    ft_printf(" ");
                else
                    ft_printf(" ");
            }
        }
        else
            ft_printf("%02x ", (uint8_t)start[i]);
        i++;
    }
}

void ft_hexdump(t_nm *data, void *start, int64_t size,
                int64_t start_address)
{
    int64_t len;

    len = size > 16 ? 16 : size;
    while (len > 0)
    {
        ft_printf("%0*llx%s", (data->is_64) ? 16 : 8, start_address, "\t");
        ft_hexdump_line_values(data, start, len);
        ft_printf("\n");
        start += 16;
        start_address += 16;
        size -= 16;
        len = size > 16 ? 16 : size;
    }
}
