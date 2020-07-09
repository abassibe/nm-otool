/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abassibe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 02:20:28 by abassibe          #+#    #+#             */
/*   Updated: 2020/03/01 02:20:44 by abassibe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
#define FT_NM_H

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

typedef union u_header {
    struct mach_header *mh;
    struct mach_header_64 *mh_64;
} t_header;

typedef union u_seg_command {
    struct segment_command *sc;
    struct segment_command_64 *sc_64;
} t_seg_command;

typedef struct s_nm
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
    t_header header;
    t_seg_command *sc;
    struct load_command *lc;
    int otool;
} t_nm;

typedef struct s_symlist
{
    char *name;
    char *padded;
    struct s_symlist *next;
    struct s_symlist *prev;
} t_symlist;

typedef struct s_section
{
    char *name;
    uint64_t index;
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
} t_section;

typedef struct s_my_sect
{
    char *name;
    uint64_t index;
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
} t_my_sect;

typedef struct s_my_sym
{
    char *name;
    int namefailed;
    uint8_t type;
    char type_p;
    uint8_t sect;
    uint16_t desc;
    uint64_t value;
} t_my_sym;

typedef struct s_debug_symbol
{
    char *symbol;
    uint16_t typevalue;
} t_debug_symbol;

typedef struct s_my_symbol
{
    char *name;
    int namefailed;
    uint8_t type;
    char type_p;
    uint8_t sect;
    uint16_t desc;
    uint64_t value;
    char *debug_symbol;
} t_my_symbol;

typedef struct mach_header_64 t_mach_header_64;
typedef struct mach_header t_mach_header;
typedef struct segment_command_64 t_segment_command_64;
typedef struct segment_command t_segment_command;
typedef struct nlist_64 t_nlist_64;
typedef struct nlist t_nlist;
typedef struct load_command t_load_command;
typedef struct symtab_command t_symtab_command;

int is_overflow(t_nm *data, void *ptr);
int ft_error(char *error);
uint32_t swap_u32(char endian, uint32_t x);
uint64_t swap_u64(char endian, uint64_t x);
int32_t bswap_int32(int32_t x);
uint32_t bswap_uint32(uint32_t x);
uint64_t bswap_uint64(uint64_t x);
char *ft_strdup_overflow(t_nm *data, char *src, char end_char, int *failed);

int macho_file(t_nm *data);
int macho_segment(t_nm *data);
t_list *create_mysym(t_nm *data, char *strtab, void *sym);

void print_symbols(t_nm *data);
void ft_hexdump(t_nm *data, void *start, int64_t size, int64_t start_address);

void ft_lstsort(t_list *lst);

int archive(t_nm *data);
int init_data(t_nm *data);

int fat(t_nm *data);

#endif
