#include "../../includes/ft_nm.h"

static int free_data(t_nm *data)
{
    if (munmap(data->raw_data, data->size) < 0)
        return (0);
    return (1);
}

static int get_args(t_nm *data, char *path)
{
    int fd;
    struct stat buffer;

    ft_bzero(data, sizeof(t_nm));
    data->path = path;
    data->otool = 1;
    if ((fd = open(data->path, O_RDONLY)) < 0)
        return (ft_error("No such file."));
    if (fstat(fd, &buffer) < 0)
        return (ft_error("Fstat error."));
    if (buffer.st_size == 0)
        return (ft_error("No valid file."));
    if ((data->raw_data = mmap(NULL, buffer.st_size, VM_PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        return (ft_error("Mapping error."));
    if (close(fd) == -1)
        return (ft_error("Error while closing file."));
    data->size = buffer.st_size;
    return (1);
}

int main(int ac, char **av)
{
    t_nm data;
    int i;

    i = 0;
    if (ac < 2)
    {
        ft_printf("At least one file must be specified\n");
        return (-1);
    }
    while (++i < ac)
    {
        if (get_args(&data, av[i]) == -1)
            continue;
        if (init_data(&data) == -1)
            return (-1);
        if (!free_data(&data))
            return (-1);
        if (i + 1 < ac)
            write(1, "\n", 1);
    }
    return (0);
}
