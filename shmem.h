#if !defined(__SHMEM_H__)
#define __SHMEM_H__

#include <stdint.h>

#define SHMEM_ACCESS_MODE_RD    0x01
#define SHMEM_ACCESS_MODE_WR    0x02
#define SHMEM_ACCESS_MODE_RDWR  0x03

#define snzprintf(dst, fmt, data...) do { \
        size_t sz = sizeof(dst); \
        memset(dst, 0, sz); \
        snprintf(dst, (sz), fmt, data); \
    } while(0)

#define print_obj_size(fd, st, str) do { \
        if(0 == fstat(fd, &st)) \
            printf("%s : has %ld bytes \n", str, (long) st.st_size); \
    } while(0);

typedef struct shmem_open_param {
    char        filename[64];
    int32_t     access_mode;
    uint32_t    mmap_size;

} shmem_open_param_t;

void *open_shmem_pool(shmem_open_param_t *param);

#endif
