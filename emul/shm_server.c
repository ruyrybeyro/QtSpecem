#if !defined(_WIN32)
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "../h/env.h"

#define SHMSZ     65536
#define SHMVARS   (sizeof(struct Z80vars) + (sizeof(union Z80Regs) * 2) + sizeof(struct CPU_flags) + \
                  sizeof(union Z80IX) + sizeof(union Z80IY) + 1)

#define SHM_RAM_KEY  5678
#define SHM_VARS_KEY 1234

#if defined(_WIN32) 

unsigned char * alloc_speccy_shared_ram(void)
{
    unsigned char *mem = (unsigned char *)calloc(SHMSZ, 1);
    if (!mem) {
        perror("calloc failed for RAM");
    }
    return mem;
}

unsigned char * alloc_speccy_shared_vars(void)
{
    unsigned char *vars = (unsigned char *)calloc(SHMVARS, 1);
    if (!vars) {
        perror("calloc failed for Vars");
    }
    return vars;
}

void dealloc_shared(unsigned char *mem, unsigned char *vars)
{
    // Nothing to do on Windows since we use heap allocation
    (void)mem;
    (void)vars;
}

#else  // Unix (Linux, macOS, BSD, etc.)

unsigned char * alloc_speccy_shared_ram(void)
{
    int shmid = shmget(SHM_RAM_KEY, SHMSZ, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed for RAM");
        return NULL;
    }

    void *shm = shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat failed for RAM");
        return NULL;
    }

    return (unsigned char *)shm;
}

unsigned char * alloc_speccy_shared_vars(void)
{
    int shmid = shmget(SHM_VARS_KEY, SHMVARS, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed for Vars");
        return NULL;
    }

    void *shm = shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat failed for Vars");
        return NULL;
    }

    // Increase shared memory reference count
    (*(unsigned char *)(shm + SHMVARS - 1))++;
    return (unsigned char *)shm;
}

void dealloc_shared(unsigned char *mem, unsigned char *vars)
{
    if (!vars) return; // Avoid null pointer access

    if ((--(*(vars + SHMVARS - 1))) == 0) {
        shmdt(vars);
        shmdt(mem);
    }
}

#endif

