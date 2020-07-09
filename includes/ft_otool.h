#ifndef FT_OTOOL_H
#define FT_OTOOL_H

#include "libft.h"
#include <mach-o/loader.h>
#include <ar.h>
#include <mach-o/fat.h>
#include <mach-o/nlist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>

typedef struct s_otool
{
    char *path;
    void *raw_data;
    uint32_t size;
    char *file_name;
    char is_64;
    char is_endianess;
    uint32_t header_size;
    uint32_t sc_size;
    uint32_t nlist_size;
    uint64_t nsects;
    t_list *sects;
    t_list *syms;
    struct load_command *lc;
} t_otool;

int ft_error(char *error);

#endif