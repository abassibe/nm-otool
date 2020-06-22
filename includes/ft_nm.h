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

#ifndef NM_H
#define NM_H

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

typedef struct s_debug_symbol
{
    char *symbol;
    uint16_t typevalue;
} t_debug_symbol;

typedef struct s_nm
{
    char *path;
    void *raw_data;
    struct stat buffer;
    char is_64;
    char is_endianess;
    uint64_t nsects;
    t_list *sects;
    t_list *syms;
    struct mach_header *mh;
    struct mach_header_64 *mh_64;
    struct segment_command *sc;
    struct segment_command_64 *sc_64;
    struct load_command *lc;
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

typedef struct s_mysect
{
    char *name;
    uint64_t index;
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
} t_mysect;

typedef struct s_mysym
{
    char *name;
    int namefailed;
    uint8_t type;
    char type_p;
    uint8_t sect;
    uint16_t desc;
    uint64_t value;
    char *debug_symbol;
} t_mysym;

int is_overflow(t_nm *data, void *ptr);
int ft_error(char *error);
uint32_t swap_u32(char endian, uint32_t x);
uint64_t swap_u64(char endian, uint64_t x);
char *ft_strdup_overflow(t_nm *data, char *src, char end_char, int *failed);

int macho_file(t_nm *data);

#endif
