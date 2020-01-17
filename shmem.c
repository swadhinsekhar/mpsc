#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>   //mmap, shm_open
#include <fcntl.h>      // O_* constant
#include <sys/types.h>  //ftruncate - setting of length
#include <sys/stat.h>   //fstat - file status, mode const
#include <errno.h>

#include "shmem.h"

void *open_shmem_pool(shmem_open_param_t *param, shmem_obj_t *shmem_obj)
{
    struct stat st;
    int         fd = -1, err, ac_mode = O_RDWR;
    bool        new = false;
    char        fname[128];

    if ((SHMEM_ACCESS_MODE_RD != param->access_mode) &&
        (SHMEM_ACCESS_MODE_WR != param->access_mode)) {
        printf("Access mode wrong!!\n");
        goto out;
    }

    snzprintf(fname, "/dev/shm/%s", param->filename);

    if (-1 == stat((const char *)fname, &st)) {
        err = errno;
        if ((ENOENT != err) || (SHMEM_ACCESS_MODE_RD == param->access_mode)) {
            printf("shared memory not found or not created "
                "(%s) stat failed. err (%d). (%s)\n",
                 param->filename, err, strerror(err));
            goto out;
        }
        new = true;
        ac_mode |= O_CREAT;
        printf("File not present: %s, Creating it\n", param->filename);
    }

    /* shmem name will be create in "/dev/shm/:" */
    fd = shm_open(param->filename, ac_mode, S_IRUSR | S_IWUSR);
    if (-1 == fd) {
        printf("Error in creating object "
            "err: %d - (%s)\n",
            errno, strerror(errno));
        goto out;
    }

    print_obj_size(fd, st, "Object has");

    if (new) {
        if (ftruncate(fd, (off_t) param->mmap_size) < 0 ) {
            printf("Error in allocating memory size to the object "
                    "err: %d - (%s)\n",
                    errno, strerror(errno));
            goto out;
        }
        print_obj_size(fd, st, "ftruncate: Object has");
    }

    void *addr = mmap(NULL, param->mmap_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if ((NULL == addr) && (addr == MAP_FAILED)) {
        printf("Error in mmap err: %d - (%s)\n",
            errno, strerror(errno));
        goto out;
    }

    if (NULL != shmem_obj) {
        shmem_obj->fd = fd;
        shmem_obj->mode = ac_mode;
        shmem_obj->mmap_sz = param->mmap_size;
        snzprintf(shmem_obj->fname, "%s", param->filename);
    }
    return addr;

out:
    if (fd) {
        close(fd);
    }

    return NULL;
}

int destroy_shmem_pool(shmem_obj_t *obj)
{
    printf("Deleting shmem object : %s\n", obj->fname);

    if (obj->fd > 0) {

        munmap(obj->objmem, obj->mmap_sz);

        if (shm_unlink(obj->fname) < 0) {
            printf("Error in shm_unlink "
                "err: (%d) - (%s)\n",
                errno, strerror(errno));
        }

        close(obj->fd);
        obj->objmem = NULL;
        obj->mmap_sz = 0;
        obj->fd = -1;
    }

    return 0;
}
