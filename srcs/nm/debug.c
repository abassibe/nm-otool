#include "../../includes/ft_nm.h"

static t_debug_symbol g_debug_symbols[] =
    {
        {"GSYM", 0x20},
        {"FNAME", 0x22},
        {"FUN", 0x24},
        {"STSYM", 0x26},
        {"LCSYM", 0x28},
        {"BNSYM", 0x2e},
        {"OPT", 0x3c},
        {"RSYM", 0x40},
        {"SLINE", 0x44},
        {"ENSYM", 0x4e},
        {"SSYM", 0x60},
        {"SO", 0x64},
        {"OSO", 0x66},
        {"LSYM", 0x80},
        {"BINCL", 0x82},
        {"SOL", 0x84},
        {"PARAMS", 0x86},
        {"VERSION", 0x88},
        {"OLEVEL", 0x8A},
        {"PSYM", 0xa0},
        {"EINCL", 0xa2},
        {"ENTRY", 0xa4},
        {"LBRAC", 0xc0},
        {"EXCL", 0xc2},
        {"RBRAC", 0xe0},
        {"BCOMM", 0xe2},
        {"ECOMM", 0xe4},
        {"ECOML", 0xe8},
        {"LENG", 0xfe},
        {0, 0}};

char *get_debug_symbol(uint16_t type)
{
    int i;

    i = 0;
    while (g_debug_symbols[i].symbol)
    {
        if (g_debug_symbols[i].typevalue == type)
            return (g_debug_symbols[i].symbol);
        i++;
    }
    return (NULL);
}
