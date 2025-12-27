#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <limits.h> 

// ============== Syscall Numbers ==============

#define SYS_exit          1
#define SYS_fork          2
#define SYS_read          3
#define SYS_write         4
#define SYS_open          5
#define SYS_close         6
#define SYS_waitpid       7
#define SYS_execve       11
#define SYS_pipe         42
#define SYS_dup2         63
#define SYS_mmap2       192

// ============== File Access Modes ==============

#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02

// ============== Mmap Flags ==============

#define PROT_READ	0x1		
#define PROT_WRITE	0x2		
#define PROT_EXEC	0x4	
#define PROT_NONE	0x0		
#define PROT_GROWSDOWN	0x01000000	
#define PROT_GROWSUP	0x02000000

#define MAP_SHARED	0x01		
#define MAP_PRIVATE	0x02		
#define MAP_SHARED_VALIDATE	0x03	
#define MAP_TYPE	0x0f	
	
#define MAP_ANONYMOUS	0x20

// ============== Macros and useful constants ==============

#define NULL 0

#define WEXITSTATUS(status) (((status) & 0xff00) >> 8)

// ============== Syscall Wrappers ==============

__attribute((noreturn)) void exit(int r);

pid_t fork(void);

ssize_t read(int fd, void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count);

int open(const char *pathname, int flags, int mode);

int close(int fd);

pid_t waitpid(pid_t pid, int *status, int options);

int execve(const char *pathname, char *const argv[], char *const envp[]);

int pipe(int pipefd[2]);

int dup2(int oldfd, int newfd);

void *mmap2(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

// ============== String Functions ==============

size_t strlen(const char *s);

void *memcpy(void *dest, const void *src, size_t len);

char *strcat(char *restrict dst, const char *restrict src);

int atoi(const char *nptr);

int strcmp(const char *s1, const char *s2);

char* itoa(int num, char* str, int base);

// ================== I/O ==================

int puts(const char *s);

// ============== Utility Functions ==============

int popen(char *command);

// ============== Start Main ==============

#ifndef __LIBC_SRC_

__attribute((used)) static void entrypoint(long *stack)
{
    int main(int, char **);
    int r = main(*stack, (char **)(stack+1));
    exit(r);
}

asm (
    ".globl _start\n"
    "_start:\n"
    "       push %esp\n"
    "       call entrypoint\n"
);

#endif