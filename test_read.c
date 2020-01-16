#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "shmem.h"

#define snzprintf(dst, fmt, data...) do { \
        size_t sz = sizeof(dst); \
        memset(dst, 0, sz); \
        snprintf(dst, (sz), fmt, data); \
    } while(0)

int main(int argc, char **argv)
{
    shmem_open_param_t param;

    snzprintf(param.filename, "%s", "mpsc");
    param.access_mode = SHMEM_ACCESS_MODE_RD;
    param.mmap_size = 64;

    open_shmem_pool(&param);
}
