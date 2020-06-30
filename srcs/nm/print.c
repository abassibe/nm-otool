#include "../../includes/ft_nm.h"

static void print_mysyms_line(uint8_t left_padding, t_my_symbol *sym)
{
    if (sym->type_p == '-')
        ft_printf("%0*llx %c %02d %5s %s\n", left_padding, sym->value,
                  sym->type_p, sym->sect, sym->debug_symbol, sym->name);
    else if (sym->type_p == 'I')
        ft_printf("%*c %c %s (indirect for %s)\n", left_padding, ' ',
                  sym->type_p, sym->name, sym->name);
    else if (sym->type_p != 'U')
        ft_printf("%0*llx %c %s\n", left_padding, sym->value, sym->type_p,
                  sym->name);
    else
        ft_printf("%*c %c %s\n", left_padding, ' ', sym->type_p, sym->name);
}

// void print_mysym(t_nm *data, t_my_symbol *sym)
// {
//     if ((!(data->flags & FLAG_A) && sym->type_p == '-') || (data->flags & FLAG_U && sym->type_p != 'U') || (data->flags & FLAG_UU && sym->type_p == 'U') || (data->flags & FLAG_G && (sym->type_p < 'A' || sym->type_p > 'Z')) || (!(data->flags & FLAG_A) && sym->type_p == '-'))
//         ;
//     else
//         print_mysyms_line((data->is_64) ? 16 : 8, sym);
// }

void print_symbols(t_nm *data)
{
    t_list *symlst;
    t_my_symbol *sym;
    int reverse;

    symlst = data->syms;
    ft_lstsort(symlst);
    while (symlst)
    {
        sym = symlst->content;
        // print_mysym(data, sym);
        print_mysyms_line((data->is_64) ? 16 : 8, sym);
        symlst = symlst->next;
    }
}