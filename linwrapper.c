#define _GNU_SOURCE
#include <sys/types.h>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

static int (*real_open)(const char *pathname, int flags, int mode); 
/* static ssize_t (*real_write)(int fd, const void *buf, size_t count); */

__attribute__((constructor)) void init(void)
{
    /*
     * "dlsym()" returns a void*, which is an object pointer.
     * But it's actually pointing to a function.
     * On some platforms which this code doesn't run on,
     * object and function pointers are of different size
     * so can't be cast to each other.
     * Tell the compiler to not worry about it.
     */
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"

    real_open = dlsym(RTLD_NEXT, "open");
    /* real_write = dlsym(RTLD_NEXT, "write"); */
    
#   pragma GCC diagnostic pop
}

int open(const char *pathname, int flags, mode_t mode)
{
    int fd, ret;
    
    /*
     * "/dev/fd/N" doesn't exist, and its analog in /proc doesn't work.
     * So emulate equivalent behavior.
     */
    if (1 != sscanf(pathname, "/dev/fd/%d", &fd))
    {
        return real_open(pathname, flags, mode);
    }

    ret = dup(fd);
    
    if (ret == EBADF)
    {
        return ENOENT;
    }
    
    return ret;
}
/*
ssize_t write(int fd, const void *buf, size_t count)
{
    ssize_t ret = real_write(fd, buf, count);
    printf("write(%d, \"%s\", %llu) = %lld\n",
           fd,
           (const char*)buf,
           (long long unsigned)count,
           (long long int)ret);
    return ret;
}
*/