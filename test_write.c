#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shmem.h"

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

int main(int argc, char *argv[])
{
    shmem_open_param_t  param;
    shmem_obj_t         shmem_obj;

    snzprintf(param.filename, "%s", "mpsc");
    param.access_mode = SHMEM_ACCESS_MODE_WR;
    param.mmap_size = 64;

    shmem_obj.objmem = open_shmem_pool(&param, &shmem_obj);

    if(argc == 2) {
        destroy_shmem_pool(&shmem_obj);
    }
}
