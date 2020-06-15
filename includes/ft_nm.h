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

typedef struct s_nm
{
    char *path;
    void *raw_data;
    void *end;
    struct stat buffer;
    char is_64;
    char is_endianess;
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

#endif
