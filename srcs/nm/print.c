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