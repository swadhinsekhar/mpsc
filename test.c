#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>   //mmap
#include <fcntl.h>
#include <sys/types.h>  //ftruncate - setting of length
#include <sys/stat.h>   //fstat - file status
#include <errno.h>

/*
 The POSIX shared memory API allows processes to communicate
       information by sharing a region of memory.

       The interfaces employed in the API are:

       shm_open(3)    Create and open a new object, or open an existing
                      object.  This is analogous to open(2).  The call
                      returns a file descriptor for use by the other
                      interfaces listed below.

       ftruncate(2)   Set the size of the shared memory object.  (A newly
                      created shared memory object has a length of zero.)

       mmap(2)        Map the shared memory object into the virtual address
                      space of the calling process.

       munmap(2)      Unmap the shared memory object from the virtual
                      address space of the calling process.

       shm_unlink(3)  Remove a shared memory object name.

       close(2)       Close the file descriptor allocated by shm_open(3)
                      when it is no longer needed.

       fstat(2)       Obtain a stat structure that describes the shared
                      memory object.  Among the information returned by this
                      call are the object's size (st_size), permissions
                      (st_mode), owner (st_uid), and group (st_gid).

       fchown(2)      To change the ownership of a shared memory object.

       fchmod(2)      To change the permissions of a shared memory object.
*/

#define print_obj_size(fd, st, str) do { \
        if(0 == fstat(fd, &st)) \
            printf("%s : has %ld bytes \n", str, (long) st.st_size); \
    } while(0);

int create_mem_pool_obj(const char *name, size_t nbytes)
{
    int         fd = -1;
    struct stat st;

    fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (-1 == fd) {
        printf("Error in creating object "
            "err: %d - (%s)\n",
            errno, strerror(errno));
        goto bailout;
    }

    print_obj_size(fd, st, "Object has");

    if(ftruncate(fd, (off_t) nbytes) < 0 ) {
        printf("Error in allocating memory size to the object "
            "err: %d - (%s)\n",
            errno, strerror(errno));
        goto bailout;
    }

    print_obj_size(fd, st, "ftruncate: Object has");

    void *addr = mmap(NULL, nbytes, PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == addr) {
        printf("Error in mmap "
            "err: %d - (%s)\n",
            errno, strerror(errno));
        goto bailout;
    }

    return 0;

bailout:
    if(fd) {
        if(shm_unlink(name) < 0) {
            printf("Error in shm_unlink "
                "err: %d - (%s)\n",
            errno, strerror(errno));
        }
        close(fd);
    }
    return -1;
}

int main()
{
    /* shmem name will be create in "/dev/shm/:" */
    const char *shmem = "mpsc";

    create_mem_pool_obj(shmem, 8);
    shm_unlink(shmem);
}
